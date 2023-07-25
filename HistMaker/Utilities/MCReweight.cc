#ifndef MCREWEIGHT_CC
#define MCREWEIGHT_CC
// Create and read from MC reweighting files

#include <vector>
#include <string>
#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "DrawDataFormat.cc"

class MCReweight {
public:
  MCReweight(string s_) {
    SourceRegion = s_;
    OtherMCHists.clear();
    ttbarHists.clear();
    SetVariations(rm.Variations);
  };

  void SetVariations(vector<string> vars) {
    Variations = vars;
    VarSize = Variations.size();
    OtherMCHists.resize(VarSize);
    ttbarHists.resize(VarSize);
  }

  void AddData(TH1F* h_) {
    if (h_ == nullptr) return;
    if (DataHist == nullptr) {
      DataHist = (TH1F*) h_->Clone();
      DataHist->SetDirectory(0);
    }
    else DataHist->Add(h_);
    nbins = h_->GetNbinsX();
  }

  void AddMC(TH1F* h_, int iv = 0) {
    if (h_ == nullptr) return;
    if (OtherMCHists[iv] == nullptr) {
      OtherMCHists[iv] = (TH1F*) h_->Clone();
      OtherMCHists[iv]->SetDirectory(0);
    }
    else OtherMCHists[iv]->Add(h_);
  }

  void Addttbar(TH1F* h_, int iv = 0) {
    if (h_ == nullptr) return;
    ttbarHists[iv] = (TH1F*) h_->Clone();
    ttbarHists[iv]->SetDirectory(0);
  }

  // void StatError(TH1F* hcentral, vector<double>& errup, vector<double>& errlow) {
  //   if (hcentral == nullptr) return;
  //   bool doreport = false;
  //   bool reported = false;
  //   hcentral->SetBinErrorOption(TH1::kPoisson);
  //   for (unsigned i = 0; i < nbins; ++i) {
  //     if (hcentral->GetBinErrorUp(i + 1) != hcentral->GetBinErrorUp(i + 1)) {
  //       if (!reported && doreport) {
  //         cout << "Stat has val of nan for " << hcentral->GetName() << endl;
  //         reported = true;
  //       }
  //       continue;
  //     }
  //     errup[i] = errup[i] + hcentral->GetBinErrorUp(i + 1) * hcentral->GetBinErrorUp(i + 1);
  //     errlow[i] = errlow[i] + hcentral->GetBinErrorLow(i + 1) * hcentral->GetBinErrorLow(i + 1);
  //   }
  // }

  // void SystError(TH1F* hcentral, TH1F* hvarup, TH1F* hvarlow, vector<double>& errup, vector<double>& errlow) {
  //   if (hcentral == nullptr) return;
  //   bool doreport = false;
  //   bool reportedup = false;
  //   bool reportedlow = false;
  //   // cout << errup[5] <<"," << errlow[5] <<endl;
  //   for (unsigned i = 0; i < nbins; ++i) {
  //     double diffup(0), difflow(0);
  //     if (hvarup != nullptr) diffup = hvarup->GetBinContent(i + 1) - hcentral->GetBinContent(i + 1);
  //     if (hvarlow != nullptr) difflow = hvarlow->GetBinContent(i + 1) - hcentral->GetBinContent(i + 1);
  //     if (diffup != diffup) {
  //       diffup = 0;
  //       if (!reportedup && doreport) {
  //         cout << "Diff up has  val of nan for " << hvarup->GetName() << " and " << hcentral->GetName() << endl;
  //         reportedup = true;
  //       }
  //     }
  //     if (difflow != difflow) {
  //       difflow = 0;
  //       if (!reportedlow && doreport) {
  //         cout << "Diff low has  val of nan for " << hvarlow->GetName() << " and " << hcentral->GetName() << endl;
  //         reportedlow = true;
  //       }
  //     }
  //     double eu = max(max(diffup,difflow),0.0);
  //     double el = min(min(diffup,difflow),0.0);
  //     // if (i == 5) cout << "diffup = " << diffup << ", difflow = " << difflow <<endl;
  //     // if (i == 5) cout << "eu = " << eu << ", el = " << el <<endl;
  //     errup[i] = errup[i] + eu * eu;
  //     errlow[i] = errlow[i] + el * el;
  //   }
  // }

  // void CreateOutputFile() {

  // }

  void CreateSF1DPlot() {
    SF1D = (TH1F*) DataHist->Clone();
    SF1D->SetDirectory(0);
    SF1D->Add(OtherMCHists[0], -1.);
    SF1D->Divide(ttbarHists[0]);
    SF1D->SetName(("ttbarReweightSFFrom" + SourceRegion).c_str());
    SF1D->SetTitle(("ttbar Reweighting Scale Factor From " + SourceRegion).c_str());
    TString FuncName = "MCRFunc" + SourceRegion;
    SF1DF = new TF1(FuncName,"[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    SF1D->Fit(SF1DF,"RM");
    // cout << "SF Value at 685.592GeV from" << SourceRegion << " is " << SF1D->GetBinContent(SF1D->FindBin(685.592)) << endl;
  }

  // void CreateSF2DPlot() {

  // }

  void SetSaveDirectory(TFile *f) {
    if (SF1D != nullptr) SF1D->SetDirectory(f);
    if (SF2D != nullptr) SF2D->SetDirectory(f);
  }

  float GetSF1D(double wpmass, int type = 0) {
    return SF1D->GetBinContent(SF1D->FindBin(wpmass));
  }
  float GetSF1DF(double wpmass, int type = 0) {
    float sf = SF1DF->Eval(wpmass);
    if (sf > 2.0 || sf < 0.5) cout << SF1DF->GetName() << " at " << wpmass << " has extreme value of " << sf << endl;
    return SF1DF->Eval(wpmass);
    // return SF1D->GetBinContent(SF1D->FindBin(wpmass));
  }

  // float GetSF2D(float wpmassfl, float wpmassll) {
    
  // }

  string SourceRegion;
  unsigned nbins;
  TH1F* DataHist = nullptr;
  vector<string> Variations;
  int VarSize;
  vector<TH1F*> OtherMCHists, ttbarHists;// [iVariation]
  TH1F* SF1D;
  TF1* SF1DF;
  TH2F* SF2D;
};


#endif