#ifndef DRAWDATAFORMAT_CC
#define DRAWDATAFORMAT_CC

#include <vector>
#include <map>
#include <string>

#include "TString.h"
#include "TH1.h"
#include "TFile.h"

#include "../../Utilities/DataFormat.cc"
#include "../../Utilities/Dataset.cc"

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
    string out = Form("%i-%i",b1,b2);
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
    SplitInit();
  };
  vector<RegionIdRange> Ranges;
  vector<string> StringRanges;
  vector<TString> LatexRanges;
  // Variations is not very comfortable to be placed inside RegionManager.
  // But rest here at the momemt, as RegionManager is assured to be widely included.
  vector<string> Variations = {"central" // 0
  , "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown" // 1-8
  , "EleSFup", "EleSFdown", "MuonTriggerWup", "MuonTriggerWdown", "MuonIdWup", "MuonIdWdown", "MuonIsoWup", "MuonIsoWdown" // 9-16
  , "BjetTagSFup", "BjetTagSFdown", "PUIDSFup", "PUIDSFdown", "L1PreFiringSFup", "L1PreFiringSFdown" // 17-22
  , "PUreweightSFup", "PUreweightSFdown", "PDFWup", "PDFWdown", "LHEScaleWup", "LHEScaleWdown" // 23 - 28
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
    Add(1161,1162);
    Add(1163,1163,true);
    Add(1164,1164,true);
    Add(1165,1165,true);
    Add(1166,1166,true);
    Add(1260);
    Add(1261,1262);
    Add(1263,1263,true);
    Add(1264,1264,true);
    Add(1265,1265,true);
    Add(1266,1266,true);
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

class Histograms{
public:
  Histograms() {
    Init();
  };

  void Init() {
    Hists.clear();
    // NameFormat = "=SampleType=_=Observable=_=RegionRange=_=Variation=";
    SampleTypes = dlib.DatasetNames;
    Variations = rm.Variations;
    Regions = rm.StringRanges;
  }

  void SetSampleTypes(vector<string> sts) {SampleTypes = sts;}
  void SetVariations(vector<string> vars) {Variations = vars;}

  // TString GetHistName(int io, int ist, int iv, int ir) {
  //   TString histname = StandardNames::HistName;
  //   histname.ReplaceAll("=SampleType=", SampleTypes[ist]);
  //   histname.ReplaceAll("=Variation=", Variations[iv]);
  //   histname.ReplaceAll("=RegionRange=", Regions[ir]);
  //   histname.ReplaceAll("=Observable=", Observables[io]);
  //   return histname;
  // }

  // Creating Histograms
  void AddObservable(string ob, int nbins_, double xlow_, double xup_) {
    Observables.push_back(ob);
    nbins.push_back(nbins_);
    xlow.push_back(xlow_);
    xup.push_back(xup_);
  }

  void CreateHistograms(string path, string prefix) {
    Hists.clear();
    Hists.resize(Observables.size());
    for (unsigned io = 0; io < Observables.size(); ++io) {
      Hists[io].resize(SampleTypes.size());
      TString fn = StandardNames::HistFileName(path, prefix, Observables[io]);
      fouts.push_back(new TFile(fn, "RECREATE"));
      fouts[io]->cd();
      for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
        Hists[io][ist].resize(Variations.size());
        for (unsigned iv = 0; iv < Variations.size(); ++iv) {
          Hists[io][ist][iv].resize(Regions.size());
          for (unsigned ir = 0; ir < Regions.size(); ++ir) {
            // TString histname = GetHistName(io, ist, iv, ir);
            TString histname = StandardNames::HistName(SampleTypes[ist], Observables[io], Regions[ir], Variations[iv]);
            Hists[io][ist][iv][ir] = new TH1F(histname, histname, nbins[io], xlow[io], xup[io]);
          }
        }
      }
    }
  }

  int Fill(string ob, int ist, int iv, int rid, float x, float w) {
    if (x !=x ) {
      cout << Form("Skipping filling nan value to ob %s, ist %i, iv %i, region %i",ob.c_str(), ist, iv, rid) <<endl;
      return 1;
    }
    if (w != w) {
      cout << Form("Skipping filling nan weight to ob %s, ist %i, iv %i, region %i",ob.c_str(), ist, iv, rid) <<endl;
      return 2;
    }
    int io = -1;
    for (unsigned iob_ = 0; iob_ < Observables.size(); ++iob_) {
      if (Observables[iob_] == ob) {
        io = iob_;
        break;
      }
    }
    if (io < 0) {
      string msg = "Observable not found to fill: " + ob;
      cout << msg << endl;
      throw runtime_error(msg);
    }
    int ir = rm.GetRangeIndex(rid);
    if (ir < 0) return 3;
    Hists[io][ist][iv][ir]->Fill(x,w);
    return 0;
  }

  void SetCurrentFill(int ist, int iv, int rid, float w) {
    tmp_ist = ist;
    tmp_iv = iv;
    tmp_rid = rid;
    tmp_w = w;
  }

  int Fill(string ob, float x) {
    return Fill(ob, tmp_ist, tmp_iv, tmp_rid, x, tmp_w);
  }

  void PostProcess() {
    for (unsigned i = 0; i < Observables.size(); ++i) {
      fouts[i]->Write();
      fouts[i]->Save();
    }
  }

  vector<TFile*> fouts;
  vector< vector< vector< vector<TH1F*> > > > Hists; // Hists[iobservable][isampletype][ivariation][irange]
  vector<string> SampleTypes, Variations, Regions, Observables;
  vector<int> nbins; // [nObservables]
  vector<double> xlow, xup; // [nObservables]
  TString NameFormat;

  int tmp_ist, tmp_iv, tmp_rid;
  float tmp_w;
};

#endif