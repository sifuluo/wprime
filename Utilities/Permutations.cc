#ifndef PERMUTATIONS_CC
#define PERMUTATIONS_CC
#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TMath.h"

#include "Configs.cc"
#include "DataFormat.cc"
#include "Hypothesis.cc"

class Permutations {
public:
  Permutations(Configs *conf_) {
    conf = conf_;
    InitPtPerms();
    GetFileName();
    if (conf->AuxHistCreation && conf->Type == 2) CreatePermutationFile();
    else {
      ReadPermutationFile();
    }
  };

  Permutations() {
    InitPtPerms();
    cout << "PtPerms are as following: " << endl;
    PrintPtPerms();
  }

  void InitPtPerms() { // Ordered as LJ0, LJ1, Hadb, Lepb, WPb
    PtPerms.clear();
    // sequence of ix affects the order of permutations
    // Taking i0 to be the first because WPb is most likely to be the leading jet.
    // So all such cases will end up together in the first part of distribution.
    for (unsigned i0 = 0; i0 < 5; ++i0) { // WPb
      for (unsigned i1 = 0; i1 < 5; ++i1) { // Hadb
        if (i1 == i0) continue;
        for (unsigned i2 = 0; i2 < 5; ++i2) { // Lepb
          if (i2 == i1 || i2 == i0) continue;
          for (unsigned i3 = 0; i3 < 5; ++i3) { // LJ0
            if (i3 == i2 || i3 == i1 || i3 == i0) continue;
            for (unsigned i4 = i3 + 1; i4 < 5; ++i4) { // LJ1 is always lower in pT than LJ0
              if (i4 == i2 || i4 == i1 || i4 == i0) continue;
              int ThisPerm = i3 * 10000 + i4 * 1000 + i1 * 100 + i2 * 10 + i0 * 1; // (Order is LJ0, LJ1, Hadb, Lepb, WPb)
              PtPerms.push_back(ThisPerm);
              // PtPerms.push_back(ThisPerm % 10000);
            }
          }
        }
      }
    }
  }

  void PrintPtPerms() {
    for (unsigned i = 0; i < PtPerms.size(); ++i) cout << "PtPerm Index: " << i << " : " << Form("%05i",PtPerms[i]) << endl;
  }

  int GetPtPerm(vector<Jet>& js) { // Input as in default Jets order: LJ0, LJ1, Hadb, Lepb, WPb
    if (js.size() != 5) cout << "Perm Index is designed only for 5 jets" <<endl;
    vector<double> jspt;
    for (unsigned i = 0; i < 5; ++i) {
      jspt.push_back(js[i].Pt());
    }
    return GetPtPerm(jspt);
  }
  int GetPtPerm(vector<TLorentzVector>& jslv) {
    if (jslv.size() != 5) cout << "Perm Index is designed only for 5 jets" <<endl;
    vector<double> jspt;
    for (unsigned i = 0; i < 5; ++i) {
      jspt.push_back(jslv[i].Pt());
    }
    return GetPtPerm(jspt);
  }
  int GetPtPerm(vector<double> &jspt) {
    if (jspt.size() != 5) cout << "Perm Index is designed only for 5 jets" <<endl;
    vector<pair<int, double> > ipts;
    if (jspt[1] > jspt[0]) {
      double t = jspt[0];
      jspt[0] = jspt[1];
      jspt[1] = t;
    }
    for (unsigned i = 0; i < 5; ++i) {
      ipts.push_back(pair<int,double>(i, jspt[i]));
    }
    sort(ipts.begin(), ipts.end(), [](pair<int, double> a1, pair<int, double> a2){return (a1.second > a2.second);});
    // Now jet indices are ordered by Jet pt from high to low
    int p = 0;
    for (unsigned i = 0; i < 5; ++i) {
      p += i * pow(10, 4 - ipts[i].first); // eg. if Lepb is the third leading: p += 2 * pow(10, 1);
    }
    return p;
  }
  int GetPtPermIndex(int p) {
    for (unsigned i = 0; i < PtPerms.size(); ++i) {
      if (PtPerms[i] % 1000 == p % 1000) return i; // The last three is already enough to determine the two light jets.
    }
    cout << "PtPerm = " << p << " not found" <<endl;
    return -1;
  }

  int GetPtPermIndex(vector<Jet>& js) {
    return GetPtPermIndex(GetPtPerm(js));
  }

  int GetPtPermIndex(vector<TLorentzVector>& js) {
    return GetPtPermIndex(GetPtPerm(js));
  }

  // [0]:LJ1, [1]:LJ2, [2]:Hadb, [3]:Lepb, [4]: WPb
  vector<int> ConvertPermToVector(int perm) {
    vector<int> pv = {perm / 10000 %10, perm / 1000 % 10, perm / 100 % 10, perm / 10 % 10, perm % 10};
    return pv;
  }
  int ComparePerm(int perm, int trueperm) {
    // Not necessarily be perm of 5 members; Should work with any event with less than 10 jets (Which is the case for 5/6 jets region)
    if (perm < 1234 || trueperm < 1234) return -1; // Not enough jets in the permutation.
    vector<int> pv = ConvertPermToVector(perm);
    vector<int> tpv = ConvertPermToVector(trueperm);
    return ComparePerm(pv, tpv);
  }

  int ComparePerm(vector<int> pv, vector<int> tpv) {
    cout << "Start ComparePerm" <<endl;
    if (pv == tpv) return 0;
    int out = 0;
    vector<bool> WithinTrueJets = vector<bool>(5,false); // Check if the permutation digits are the same ones.
    if (pv.size() < 5 || tpv.size() < 5) return -1;
    cout << Form("pv = %i, %i, %i, %i, %i, tpv = %i, %i, %i, %i, %i",pv[0],pv[1],pv[2],pv[3],pv[4],tpv[0],tpv[1],tpv[2],tpv[3],tpv[4]) << endl;
    for (unsigned i = 0; i < 5; ++i) {
      if (pv[i] == -1) return -1;
      for (unsigned j = 0; j < 5; ++j) {
        if (tpv[j] == -1) return -1;
        if (pv[i] == tpv[j]) WithinTrueJets[i] = true;
      }
    }
    for (unsigned i = 0; i < 5; ++i) { // A jet is taking a jet not from the hypothesis
      if (!WithinTrueJets[i]) out += -1 * pow(2, (4 - i)); // Binary is enough to hold the information
    }
    // if (out < 0) return out; // No need to proceed since the permutations digits are not all the same.
    if (out < 0) return -1;

    bool WPbCorrect = tpv[4] == pv[4];
    bool WPbToHadb = tpv[4] == pv[2];
    bool WPbToLepb = tpv[4] == pv[3];
    bool WPbToLJ = tpv[4] == pv[0] || tpv[4] == pv[1];
    bool HadbCorrect = tpv[2] == pv[2];
    bool HadbToLepb = tpv[2] == pv[3];
    bool HadbToWPb = tpv[2] == pv[4];
    bool HadbToLJ = tpv[2] == pv[0] || tpv[2] == pv[1];
    bool LepbCorrect = tpv[3] == pv[3];
    bool LepbToHadb = tpv[3] == pv[2];
    bool LepbToWPb = tpv[3] == pv[4];
    bool LepbToLJ = tpv[3] == pv[0] || tpv[2] == pv[1];

    bool WPbToTopb = WPbToHadb || WPbToLepb;
    // Either topb is ...
    bool TopbCorrect = HadbCorrect || LepbCorrect;
    bool TopbSwap = HadbToLepb || LepbToHadb;
    bool TopbToWPb = HadbToWPb || LepbToWPb;
    bool TopbToLJ = HadbToLJ || LepbToLJ;
    
    // Code is already human readable. No need for explanation for return codes.
    if (WPbCorrect && HadbToLepb && LepbToHadb) return 1;
    if (WPbCorrect && TopbToLJ && TopbCorrect) return 2;
    if (WPbCorrect && TopbToLJ && TopbSwap) return 2;
    if (WPbCorrect && HadbToLJ && LepbToLJ) return 2;

    if (WPbToTopb && TopbToWPb && TopbCorrect) return 3;
    if (WPbToTopb && TopbToWPb && TopbSwap) return 3;    
    if (WPbToTopb && TopbToLJ && TopbCorrect) return 4;
    if (WPbToTopb && TopbToLJ && TopbSwap) return 4;
    if (WPbToTopb && HadbToLJ && LepbToLJ) return 4;

    if (WPbToLJ && HadbCorrect && LepbCorrect) return 5;
    if (WPbToLJ && HadbToLepb && LepbToHadb) return 5;
    if (WPbToLJ && TopbToLJ && TopbCorrect) return 6;
    if (WPbToLJ && TopbToLJ && TopbSwap) return 6;
    return 9; // Other undefined circumstances
  }

  int GetbTagPermIndex(vector<Jet>& js) { // Input as in default Jets order: LJ0, LJ1, Hadb, Lepb, WPb
    if (js.size() != 5) cout << "Perm Index is designed only for 5 jets" <<endl;
    int idx = 0;
    for (unsigned i = 0; i < 5; ++i) {
      if (js[i].bTagPasses[conf->bTagWP]) {
        idx += pow(2, 4 - i);
      }
    }
    return idx;
  }

  int GetbTagPermIndex(vector<bool>& jsb) {
    if (jsb.size() != 5) cout << "Perm Index is designed only for 5 jets" <<endl;
    int idx = 0;
    for (unsigned i = 0; i < 5; ++i) {
      if (jsb[i]) {
        idx += pow(2, 4 - i);
      }
    }
    return idx;
  }

  void GetFileName() {
    string sampletype = conf->SampleType;
    if (!conf->AuxHistCreation) sampletype = "Merged";
    string filename = "Permutations_" + conf->SampleYear + "_" + sampletype + ".root";
    FileName = conf->AuxHistBasePath + filename;
  }

  void CreatePermutationFile() {
    PermFile = new TFile(FileName,"RECREATE");
    TString PtPermHistName = "PtPerm_" + conf->SampleYear + "_" + conf->SampleType;
    TString bTagPermHistName = "bTagPerm_" + conf->SampleYear + "_" + conf->SampleType;
    // TString WPrimedRHistName = "WPrimedR_" + conf->SampleYear + "_" + conf->SampleType;
    TString WPrimedRHistName = "WPrimedR";
    PtPermHist = new TH1F(PtPermHistName, PtPermHistName, 60,0,60);
    bTagPermHist = new TH1F(bTagPermHistName, bTagPermHistName, 40,0,40);
    WPrimedRHist = new TH1F(WPrimedRHistName, WPrimedRHistName, 100,0,10);
  }

  void FillJets(vector<Jet>& js, double ew = 1) {
    PtPermHist->Fill(GetPtPermIndex(js), ew);
    bTagPermHist->Fill(GetbTagPermIndex(js), ew);
  }
  
  void FillHypo(Hypothesis& h, vector<Jet>& js, double ew = 1) {
    PtPermHist->Fill(GetPtPermIndex(h.Jets), ew);
    bTagPermHist->Fill(GetbTagPermIndex(js), ew);
    if (conf->WPType == 0) WPrimedRHist->Fill(h.WPb().DeltaR(h.HadT()));
    else if (conf->WPType == 1) WPrimedRHist->Fill(h.WPb().DeltaR(h.LepT()));
  }
  
  void PostProcess() {
    PermFile->Write();
    PermFile->Save();
  }

  void Clear() {
    PtPermHists.clear();
    bTagPermHists.clear();
  }

  void ReadPermutationFile() {
    if (conf->AuxHistCreation) return;
    vector<string> PermSamples;
    for (unsigned i = 0; i < 9; ++i) {
      PermSamples.push_back(Form("FL%i", (i + 3) * 100));
    }
    for (unsigned i = 0; i < 9; ++i) {
      PermSamples.push_back(Form("LL%i", (i + 3) * 100));
    }
    PermFile = new TFile(FileName, "READ");
    for (unsigned i = 0; i < 18; ++i) {
      TString PtPermHistName = "PtPerm_" + conf->SampleYear + "_" + PermSamples[i];
      TString bTagPermHistName = "bTagPerm_" + conf->SampleYear + "_" + PermSamples[i];
      TH1F* h1 = (TH1F*) PermFile->Get(PtPermHistName);
      TH1F* h2 = (TH1F*) PermFile->Get(bTagPermHistName);
      h1->SetDirectory(0);
      h2->SetDirectory(0);
      h1->Scale(1. / h1->GetMaximum());
      h2->Scale(1. / h2->GetMaximum());
      PtPermHists.push_back(h1);
      bTagPermHists.push_back(h2);
    }
    TString WPrimedRFileName = conf->AuxHistBasePath + "WPrimedR_2018_FL500.root";
    WPrimedRFile = new TFile(WPrimedRFileName,"READ");
    WPrimedRHist = (TH1F*) WPrimedRFile->Get("WPrimedR");
    WPrimedRHist->SetDirectory(0);
    WPrimedRHist->Scale(1./WPrimedRHist->GetMaximum());
  }

  int LocateHist(double mass, int WPType) {
    int im = floor(mass / 100.);
    if ((mass - im * 100.) >= 50.) im++; // Round up mass to int on the hundred digit
    if (im < 3) im = 3;
    if (im > 11) im = 11; // Limit to the mass range
    im -= 3; // Solve into index
    if (WPType == 1) im += 9; // "LL"
    return im; 
  }

  double GetPtPermLikelihood(vector<Jet>& js, double mass, int WPType) {
    return PtPermHists[LocateHist(mass, WPType)]->GetBinContent(PtPermHists[LocateHist(mass, WPType)]->FindBin(GetPtPermIndex(js)));
  }

  double GetPtPermLikelihood(vector<TLorentzVector>& AllJets, vector<int> ThisPerm, double mass, int WPType) {
    vector<TLorentzVector> js = vector<TLorentzVector>(ThisPerm.size());
    for (unsigned i = 0; i < ThisPerm.size(); ++i) js[i] = AllJets[ThisPerm[i]];
    return PtPermHists[LocateHist(mass, WPType)]->GetBinContent(PtPermHists[LocateHist(mass, WPType)]->FindBin(GetPtPermIndex(js)));
  }

  double GetbTagPermLikelihood(vector<Jet>& js, double mass, int WPType) {
    return bTagPermHists[LocateHist(mass, WPType)]->GetBinContent(PtPermHists[LocateHist(mass, WPType)]->FindBin(GetbTagPermIndex(js)));
  }
  
  double GetbTagPermLikelihood(vector<bool>& AllbTags, vector<int> ThisPerm, double mass, int WPType) {
    vector<bool> js = vector<bool> (ThisPerm.size());
    for (unsigned i = 0; i < ThisPerm.size(); ++i) js[i] = AllbTags[ThisPerm[i]];
    return bTagPermHists[LocateHist(mass, WPType)]->GetBinContent(PtPermHists[LocateHist(mass, WPType)]->FindBin(GetbTagPermIndex(js)));
  }

  double GetWPrimedRLikelihood(TLorentzVector t, TLorentzVector b) {
    return WPrimedRHist->GetBinContent(WPrimedRHist->FindBin(t.DeltaR(b)));
  }

  Configs* conf;
  string SampleType;
  TString FileName;
  TFile* PermFile;
  TFile* WPrimedRFile;
  TH1F *PtPermHist, *bTagPermHist, *WPrimedRHist;
  vector<TH1F*> PtPermHists, bTagPermHists;
  vector<int> PtPerms;
};


#endif