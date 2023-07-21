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
    // SplitInit();
    TestInit();
  };
  vector<RegionIdRange> Ranges;
  vector<string> StringRanges;
  vector<TString> LatexRanges;
  // Variations is not very comfortable to be placed inside RegionManager.
  // But rest here at the momemt, as RegionManager is assured to be widely included.
  vector<string> Variations = {"central" // 0
  , "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown" // 1-8
  , "EleSFup", "EleSFdown", "MuonTriggerWup", "MuonTriggerWdown", "MuonIdWup", "MuonIdWdown", "MuonIsoWup", "MuonIsoWdown" // 9-16
  , "BjetTagCorrup", "BjetTagCorrdown", "BjetTagUncorrup", "BjetTagUncorrdown", "PUIDSFup", "PUIDSFdown", "L1PreFiringSFup", "L1PreFiringSFdown" // 17-24
  , "PUreweightSFup", "PUreweightSFdown", "PDFWup", "PDFWdown", "LHEScaleWup", "LHEScaleWdown" // 25 - 30
  };

  // Temperary working version below Take the version above next time. FIXME
  // Variations have to be ordered as central followed by Up variations and Down variations.
  // Histmanager will determine the variation type based on 
  // vector<string> Variations = {"central", "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown", "SFup", "SFdown"};

  void DefaultInit() {
    Reset();
    Add(1150);
    Add(1151);
    Add(1152,1155,true);
    Add(1250);
    Add(1251);
    Add(1252,1255,true);
    Add(1160);
    Add(1161,1162);
    Add(1163,1166,true);
    Add(1260);
    Add(1261,1262);
    Add(1263,1266,true);
  }

  void SplitInit() {
    Reset();
    Add(1150);
    Add(1151);
    Add(1152,1152,true);
    Add(1153,1153,true);
    Add(1154,1154,true);
    Add(1155,1155,true);
    Add(1250);
    Add(1251);
    Add(1252,1252,true);
    Add(1253,1253,true);
    Add(1254,1254,true);
    Add(1255,1255,true);
    Add(1160);
    Add(1161,1161);
    Add(1162,1162);
    Add(1163,1163,true);
    Add(1164,1164,true);
    Add(1165,1165,true);
    Add(1166,1166,true);
    Add(1260);
    Add(1261,1261);
    Add(1262,1262);
    Add(1263,1263,true);
    Add(1264,1264,true);
    Add(1265,1265,true);
    Add(1266,1266,true);
  }

  void TightOnlyInit(int type = 0) {
    Reset();
    if (type == 0 || type == 1) {
      Add(1150);
      Add(1151);
      Add(1152);
      Add(1153,1153,true);
      Add(1154,1154,true);
      Add(1155,1155,true);
      Add(1160);
      Add(1161);
      Add(1162);
      Add(1163,1163,true);
      Add(1164,1164,true);
      Add(1165,1165,true);
      Add(1166,1166,true);
    }
    if (type == 0 || type == 2) {
      Add(2150);
      Add(2151);
      Add(2152);
      Add(2153,2153,true);
      Add(2154,2154,true);
      Add(2155,2155,true);
      Add(2160);
      Add(2161);
      Add(2162);
      Add(2163,2163,true);
      Add(2164,2164,true);
      Add(2165,2165,true);
      Add(2166,2166,true);
    }
  }

  void TestInit() {
    Reset();
    Add(1121);
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