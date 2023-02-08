#ifndef DRAWDATAFORMAT_CC
#define DRAWDATAFORMAT_CC

#include <vector>
#include <map>
#include <string>

#include "TString.h"
#include "TH1.h"

#include "../Utilities/DataFormat.cc"

TString Replacement(TString in, TString tr, TString n) {
  TString out = in.ReplaceAll(tr, n);
  return out;
}

class RegionIdRange {
public:
  RegionIdRange(int b1_, int b2_, bool IsSR_ = false) {
    b1 = b1_;
    b2 = b2_;
    IsSR = IsSR_;
  };
  int b1 = 0;
  int b2 = 0;
  bool IsSR;

  string GetString() {
    string out = Form("%i-%i",b1,b2);
    return out;
  }

  bool InRange(int x_, int digit) {
    x_ = x_ / digit % 10;
    int b1_ = b1 / digit % 10;
    int b2_ = b2 / digit % 10;
    return (x_ - b1_) * (x_ - b2_) <= 0;
  }
  bool PassTrigger(int id) {
    return InRange(id, 1000);
  }
  bool PassLepton(int id) {
    return InRange(id, 100);
  }
  bool PassnJet(int id) {
    return InRange(id, 10);
  }
  bool PassbTag(int id) {
    return InRange(id, 1);
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
    DefaultInit();
  };
  vector<RegionIdRange> Ranges;
  vector<string> StringRanges;

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

  void Reset() {Ranges.clear();}

  void Add(int b1, int b2 = -1, bool IsSR_ = false) {
    if (b2 == -1) b2 = b1;
    if (CheckOverlap(b1)) cout << b1 << " overlaps with previous ranges" << endl;
    if (CheckOverlap(b2)) cout << b2 << " overlaps with previous ranges" << endl;
    Ranges.push_back(RegionIdRange(b1,b2, IsSR_));
    StringRanges.push_back(Ranges.back().GetString());
  }

  bool CheckOverlap(int b){
    for (RegionIdRange r : Ranges) {
      if (r.Pass(b)) return true;
    }
    return false;
  }

  int GetRangeIndex(int id) {
    for (unsigned i = 0; i < Ranges.size(); ++i) {
      if (Ranges[i].Pass(id)) return i;
    }
    return -1;
  }

};

RegionManager rm;

struct TH1ByRegions { // TH1 of same observable same sample but different region number
  TH1ByRegions() {
    
  };

  void CreateHistograms(TString hn, TString ht, int nbins, double xlow, double xup) {
    histos.clear();
    for (unsigned i = 0; i < rm.Ranges.size(); ++i) {
      TString reg = rm.Ranges[i].GetString();
      hname = Replacement(hn,"=RegionRange=", reg);
      htitle = Replacement(ht,"=RegionRange=", reg);
      histos.push_back(new TH1F(hname, htitle, nbins, xlow, xup));
    }
  }

  void Fill(double x, double w, int id) {
    int idx = rm.GetRangeIndex(id);
    if (idx < 0 || idx >= rm.Ranges.size()) return;
    histos[idx]->Fill(x,w);
  }
  
  TH1F* GetHisto(int id) {
    int idx = rm.GetRangeIndex(id);
    if (idx == -1) return nullptr;
    return histos[idx];
  }

  vector<TH1F*> histos;
  TString hname, htitle;
  int nbins;
  double xlow, xup;
};

struct RegionHistsByVariables{ // Collections of TH1 of same sample, different variables
  RegionHistsByVariables(TString n_template, TString t_template = ""){
    NameTemplate = n_template;
    if (t_template == "") TitleTemplate = NameTemplate;
    else TitleTemplate = t_template;
    histos.clear();
  };

  void AddVariable(TString var, int nbins_, double xlow_, double xup_) {
    TString hn = Replacement(NameTemplate,"=Variable=",var);
    TString ht = Replacement(TitleTemplate,"=Variable=",var);
    histos[var] = TH1ByRegions();
    histos[var].CreateHistograms(hn, ht, nbins_, xlow_, xup_);
  }

  void Fill(TString var, double x, double w, int id) {
    histos[var].Fill(x, w, id);
  }

  map<TString, TH1ByRegions> histos;
  TString NameTemplate;
  TString TitleTemplate;
};

class TH1Collection{
public:
  TH1Collection() {
  };

  void Init(vector<string> vars) {
    Variables = vars;
    SampleTypes = dlib.DatasetNames;
    SampleValid.resize(SampleTypes.size(), true);
    Ranges = rm.StringRanges;
  }

  void ReadHistograms(TString n_template, TFile *f) {
    histos.clear();
    histos.resize(SampleTypes.size());
    for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
      histos[ist].resize(Variables.size());
      bool validsample = true;
      for (unsigned iv = 0; iv < Variables.size(); ++iv) {
        if (!validsample) break;
        int missinghist = 0;
        histos[ist][iv].resize(Ranges.size());
        for (unsigned ir = 0; ir < Ranges.size(); ++ir) {
          TString hn = Replacement(n_template, "=SampleType=", SampleTypes[ist]);
          hn.ReplaceAll("=Variable=",Variables[iv]);
          hn.ReplaceAll("=RegionRange=",Ranges[ir]);
          histos[ist][iv][ir] = (TH1F*) f->Get(hn);
          if (histos[ist][iv][ir] == nullptr) ++missinghist;
        }
        if (missinghist == Ranges.size()) validsample = false; // If one variable can't find any regions plots, meaning the entire sample is missing.
      }
      if (!validsample) SampleValid[ist] = false;
    }
  }
  vector< vector< vector<TH1F*> > > histos; // histos[isampletype][ivariable][irange]
  vector<string> SampleTypes, Variables, Ranges;
  vector<bool> SampleValid;
};

#endif