#ifndef REGIONS_CC
#define REGIONS_CC

#include <vector>
#include <map>
#include <string>

#include "TString.h"
#include "TH1.h"
#include "TFile.h"

#include "../../Utilities/DataFormat.cc"

class RegionIdRange {
public:
  RegionIdRange(int b1_, int b2_, bool IsSR_ = false) {
    b1 = b1_;
    b2 = b2_;
    IsSR = IsSR_;
    Sort();
  };
  RegionIdRange(int b1_, bool IsSR_ = false) {
    b2 = b1 = b1_;
    IsSR = IsSR_;
    Sort();
  };

  int b1 = 0;
  int b2 = 0;
  vector<int> B1, B2;
  bool IsSR;

  void Sort() {
    B1 = {GetDigit(b1,0), GetDigit(b1,1), GetDigit(b1,2), GetDigit(b1,3)};
    B2 = {GetDigit(b2,0), GetDigit(b2,1), GetDigit(b2,2), GetDigit(b2,3)};
  }

  int GetDigit(int x, int pos) {
    return x / int(pow(10, pos)) % 10;
  }

  string GetString() {
    string out = Form("%ito%i",b1,b2);
    if (b1 == b2) out = Form("%i",b1);
    return out;
  }

  TString GetLatex() {
    TString l0,l1,l2,l3;
    if (B1[3] == 1 && B2[3] == 1) l3 = " #font[12]{#mu},";
    else if (B1[3] == 2 && B2[3] == 2) l3 = " #font[12]{e},";
    else l3 = "#font[12]{{#mu}/e},";

    if (B1[2] == 1 && B2[2] == 1) l2 = "Sig. Like";
    else if (B1[2] == 2 && B2[2] == 2) l2 = "Bkg. Est.";
    else l2 = "Inclusive";

    if (B1[1] == B2[1]) l1 = Form(" %d Jets,",B1[1]);
    else l1 = Form(" %d-%d Jets,",B1[1],B2[1]);

    if (B1[0] == B2[0]) l0 = Form(" %d bTags",B1[0]);
    else l0 = Form(" %d-%d bTags",B1[0],B2[0]);

    return l2 + l3 + l1 + l0;
  }

  bool InRange(int x_, int digit) {
    x_ = GetDigit(x_,digit);
    return (x_ - B1[digit]) * (x_ - B2[digit]) <= 0;
  }
  bool PassTrigger(int id) {
    return InRange(id, 3);
  }
  bool PassLepton(int id) {
    return InRange(id, 2);
  }
  bool PassnJet(int id) {
    return InRange(id, 1);
  }
  bool PassbTag(int id) {
    return InRange(id, 0);
  }
  bool Pass(int id) {
    bool p = PassTrigger(id) && PassLepton(id) && PassnJet(id) && PassbTag(id);
    return p;
  }

  vector<bool> Pass(RegionID id) {
    vector<bool> p;
    for (unsigned i = 0; i < id.RegionCount; ++i) p.push_back(Pass(id.Regions[i]));
    return p;
  }
  bool PassAny(RegionID id) {
    bool p = false;
    for (unsigned i = 0; i < id.RegionCount; ++i) p = p || Pass(id.Regions[i]);
    return p;
  }
};

class RegionManager{
public:
  RegionManager() {
  };
  vector<RegionIdRange> Ranges;
  vector<string> StringRanges;
  vector<TString> LatexRanges;
  // Variations is not very comfortable to be placed inside RegionManager.
  // But rest here at the momemt, as RegionManager is assured to be widely included.
  vector<string> Variations = {"" // 0
  , "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESUp", "JESDown", "JERUp", "JERDown" // 1-8
  , "EleSFUp", "EleSFDown", "MuonTriggerUp", "MuonTriggerDown", "MuonIdUp", "MuonIdDown", "MuonIsoUp", "MuonIsoDown" // 9-16
  , "BjetTagCorrUp", "BjetTagCorrDown", "BjetTagUncorrUp", "BjetTagUncorrDown", "PUIDSFUp", "PUIDSFDown", "L1PreFiringSFUp", "L1PreFiringSFDown" // 17-24
  , "PUreweightSFUp", "PUreweightSFDown", "PDFUp", "PDFDown", "LHEScaleUp", "LHEScaleDown" // 25 - 30
  };

  // Temperary working version below Take the version above next time. FIXME
  // Variations have to be ordered as central followed by Up variations and Down variations.
  // Histmanager will determine the variation type based on 
  // vector<string> Variations = {"central", "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown", "SFup", "SFdown"};
  void AcceptRegions(vector<int> a, vector<int> b, vector<int> c, vector<int> d) {
    Reset();
    for (int aa : a) for (int bb : b) for (int cc : c) for (int dd: d) if (dd <= cc) {
      int r = aa * 1000 + bb * 100 + cc * 10 + dd;
      if (dd < 3) Add(r);
      else Add(r,r,true);
    }
  }

  void InclusiveInit() {
    Reset();
    Add(1150,2266,true);
  }

  void Reset() {
    Ranges.clear();
    StringRanges.clear();
    LatexRanges.clear();
  }

  void Add(int b1, int b2 = -1, bool IsSR_ = false) {
    if (b2 == -1) b2 = b1;
    if (CheckOverlap(b1)) cout << b1 << " overlaps with previous ranges" << endl;
    if (CheckOverlap(b2)) cout << b2 << " overlaps with previous ranges" << endl;
    Ranges.push_back(RegionIdRange(b1,b2, IsSR_));
    StringRanges.push_back(Ranges.back().GetString());
    LatexRanges.push_back(Ranges.back().GetLatex());
  }

  bool CheckOverlap(int b){
    for (RegionIdRange r : Ranges) {
      if (r.Pass(b)) return true;
    }
    return false;
  }

  int GetRangeIndex(int id) {
    if (id < 0) return -1;
    if (id > 1300 && id < 1400) id -= 100;
    if (id > 2300 && id < 2400) id -= 100; // Merging two background est region. Remove if needed.
    for (unsigned i = 0; i < Ranges.size(); ++i) {
      if (Ranges[i].Pass(id)) return i;
    }
    return -1;
  }

};

RegionManager rm;

#endif