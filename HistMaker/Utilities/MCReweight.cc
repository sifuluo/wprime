#ifndef MCREWEIGHT_CC
#define MCREWEIGHT_CC
// Create and read from MC reweighting files

#include <vector>
#include <string>
#include <iostream>

#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include "TString.h"
#include "DrawDataFormat.cc"

class MCReweight {
public:
  MCReweight(int r_) {
    SourceRegionInt = r_;
    SourceRegion = Form("%d",r_);
    StringRange = rm.StringRanges[rm.GetRangeIndex(r_)];
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
      cout << "init Data in region" << SourceRegion << " with hist: " << h_->GetName() << endl;
    }
    else {
      DataHist->Add(h_);
      cout << "Adding Data in region" << SourceRegion << " with hist: " << h_->GetName() << endl;
    }
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

  void ReadFromFiles(string path, string prefix, string obs) { // Reads histograms of observables to make reweight
    Observable = obs;
    for (unsigned i = 0; i < dlib.DatasetNames.size(); ++i) {
      string ds = dlib.DatasetNames[i];
      string thisprefix = prefix;
      if (ds == "ttbar") thisprefix += "_NRW";
      TString fn = StandardNames::HistFileName(path, thisprefix, obs, ds);
      TFile *f = new TFile(fn,"READ");
      if (!f || !(f->IsOpen())) continue;
      // cout << "Calculating MC reweighting from file: " << fn << endl;
      // cout << "ds = " << ds << ", Region = " << SourceRegionInt << endl;
      TString histname = StandardNames::HistName(ds, obs, StringRange, rm.Variations[0]);
      TH1F* h_ = (TH1F*) f->Get(histname);
      if ((ds == "SingleMuon" && SourceRegionInt / 1000 == 1) || (ds == "SingleElectron" && SourceRegionInt / 1000 == 2)) AddData(h_);
      else if (ds == "ttbar") Addttbar(h_);
      else if (dlib.Datasets[ds].Type == 1) AddMC(h_);
      f->Close();
      delete f;
    }
  }

  bool ReadFromFile(TFile *f, string obs) { // Read the reweight file for later application
    Observable = obs;
    TString hn = "ttbarReweightSF_" + Observable + "_" + SourceRegion;
    TString fn = "MCRFunc" + SourceRegion;
    TH1F* htmp = (TH1F*)f->Get(hn);
    cout << "Trying to access SF plot: " << hn << endl;
    if (htmp == nullptr) return false;
    SF1D = (TH1F*)f->Get(hn)->Clone();
    SF1D->SetDirectory(0);
    SF1DF = SF1D->GetFunction(fn);
    nbins = SF1D->GetNbinsX();
    StartBin = EndBin = -1;
    for (unsigned ib = 1; ib <=nbins; ++ib) {
      if (SF1D->GetBinContent(ib) == 0) continue;
      if (StartBin == -1) StartBin = ib;
      EndBin = ib;
    }
    xmin = SF1D->GetBinLowEdge(StartBin);
    xmax = SF1D->GetBinLowEdge(EndBin + 1);
    cout << "SF Value at 685.592GeV from" << SourceRegion << " is " << SF1D->GetBinContent(SF1D->FindBin(685.592)) << endl;
    cout << "SF Evaled value is " << SF1DF->Eval(685.592) << endl;
    return true;
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

  void CreateSF1DPlot() {
    SF1D = (TH1F*) DataHist->Clone();
    SF1D->SetDirectory(0);
    SF1D->Add(OtherMCHists[0], -1.);
    SF1D->Divide(ttbarHists[0]);
    SF1D->SetName(("ttbarReweightSF_" + Observable + "_" + SourceRegion).c_str());
    TString SFTitle = Observable + " in " + StringRange +  "; (Data - OtherMC) / ttbar for " + Observable + ";";
    SF1D->SetTitle(SFTitle);
    StartBin = EndBin = -1;
    for (unsigned ib = 1; ib <= nbins; ++ib) { // Trimming bins with little stats
      if (DataHist->GetBinContent(ib) < 10 || ttbarHists[0]->GetBinContent(ib) < 10) {
        SF1D->SetBinContent(ib,0.0);
        SF1D->SetBinError(ib,0.0);
        continue;
      }
      if (StartBin == -1) StartBin = ib;
      EndBin = ib; // LastBin with enough stats
      if (SF1D->GetBinContent(ib) > 5.0 || SF1D->GetBinContent(ib) < 0.2) {
        cout << SF1D->GetName() << " at ibin = " << ib << ", bincenter = " << SF1D->GetBinCenter(ib) << ", has bin value = " << SF1D->GetBinContent(ib) << " +- " << SF1D->GetBinError(ib);
        if (SF1D->GetBinErrorUp(ib) != SF1D->GetBinError(ib) || SF1D->GetBinErrorLow(ib) != SF1D->GetBinError(ib)){
          cout << "(+" << SF1D->GetBinErrorUp(ib) << ",-" << SF1D->GetBinErrorLow(ib) << ")";
        }
        cout << endl;
        cout << Form("Data: %f(+ %f, - %f)", DataHist->GetBinContent(ib), DataHist->GetBinErrorUp(ib), DataHist->GetBinErrorLow(ib)) << endl;
        cout << Form("OtherMC: %f(+ %f, - %f)", OtherMCHists[0]->GetBinContent(ib), OtherMCHists[0]->GetBinErrorUp(ib), OtherMCHists[0]->GetBinErrorLow(ib)) << endl;
        cout << Form("ttbar: %f(+ %f, - %f)", ttbarHists[0]->GetBinContent(ib), ttbarHists[0]->GetBinErrorUp(ib), ttbarHists[0]->GetBinErrorLow(ib)) << endl;
      }
    }
    
    TString FuncName = "MCRFunc" + SourceRegion;
    // SF1DF = new TF1(FuncName,"[0]/x/x+[1]/x+[2]*x+[3]*x*x+[4]*x*x*x+[5]",SF1D->GetBinLowEdge(StartBin),SF1D->GetBinLowEdge(EndBin+1));
    SF1DF = new TF1(FuncName,"[0]/x/x+[1]/x+[2]+[3]*x",SF1D->GetBinLowEdge(StartBin),SF1D->GetBinLowEdge(EndBin+1));
    SF1D->Fit(SF1DF,"RM");
  }

  float GetSF1D(double v) {
    return SF1D->GetBinContent(SF1D->FindBin(v));
  }
  float GetSF1DF(double v, bool Verbose = true) {
    float sf = 1.0;
    if (v < xmax && v > xmin) sf = SF1DF->Eval(v);
    if (v <= xmin) sf = SF1DF->Eval(xmin);
    if (v >= xmax) sf = SF1DF->Eval(xmax);
    if (sf > 3.0 || sf < 0.3) {
      cout << SF1D->GetName() << " function at " << v << " has extreme value of " << sf << endl;
    }
    return sf;
  }

  int SourceRegionInt;
  string SourceRegion;
  string StringRange;
  string Observable;
  unsigned nbins;
  int StartBin, EndBin;
  float xmin, xmax;
  TH1F* DataHist = nullptr;
  vector<string> Variations;
  int VarSize;
  vector<TH1F*> OtherMCHists, ttbarHists;// [iVariation]
  TH1F* SF1D;
  TF1* SF1DF;
};

class MCReweightManager {
public:
  MCReweightManager(string obs) {
    Observable = obs;
    Verbose = true;
  };

  void Init(int nbtag = 1) {
    rws.push_back(new MCReweight(1150 + nbtag));
    rws.push_back(new MCReweight(1160 + nbtag));
    rws.push_back(new MCReweight(2150 + nbtag));
    rws.push_back(new MCReweight(2160 + nbtag));
  }

  void ReadFromFiles(string path, string prefix) {
    for (unsigned i = 0; i < rws.size(); ++i) {
      rws[i]->ReadFromFiles(path, prefix, Observable);
      rws[i]->CreateSF1DPlot();
    }
  }

  bool ReadFromFile(TString rwfn) {
    cout << "Reading Reweight file: " << rwfn << endl;
    TFile *f = new TFile(rwfn, "READ");
    if (!f || !(f->IsOpen())) return false;
    for (unsigned i = 0; i < rws.size(); ++i) {
      if (!(rws[i]->ReadFromFile(f, Observable))) {
        cout << "Cannot read from " << rws[i]->Observable << " in region " << rws[i]->StringRange << endl;
        return false;
      }
    }
    f->Close();
    return true;
  }

  bool ReadFromFile(string path, string prefix) {
    TString rwfn = StandardNames::HistFileName(path, prefix, "ReweightSF");
    if (ReadFromFile(rwfn)) return true;
    rwfn = StandardNames::HistFileName(path, prefix + "_RW", "ReweightSF");
    if (ReadFromFile(rwfn)) return true;
    rwfn = StandardNames::HistFileName(path, prefix + "_NRW", "ReweightSF");
    if (ReadFromFile(rwfn)) return true;
    cout << "Failed to read Reweight plots file" << endl;
    return false;
  }

  void SaveToFile(string path, string prefix) {
    TString rwfn = StandardNames::HistFileName(path, prefix, "ReweightSF");
    TFile *f = new TFile(rwfn,"RECREATE");
    f->cd();
    for (unsigned i = 0; i < rws.size(); ++i) {
      TH1F* h = (TH1F*)rws[i]->SF1D->Clone();
      h->SetDirectory(f);
    }
    f->Write();
    f->Save();
    f->Close();
    cout << "Reweight file has been saved to " << rwfn << endl;
    delete f;
  }

  float GetSF1D(double v, int rid) {
    for (unsigned i = 0; i < rws.size(); ++i) {
      if (rws[i]->SourceRegionInt / 10 == rid / 10 && rws[i]->SourceRegionInt / 1000 == rid / 1000) return rws[i]->GetSF1D(v); 
    }
    return 1.0;
  }

  float GetSF1DF(double v, int rid) {
    for (unsigned i = 0; i < rws.size(); ++i) {
      if (rws[i]->SourceRegionInt / 10 == rid / 10) return rws[i]->GetSF1DF(v, Verbose); 
    }
    return 1.0;
  }

  vector<MCReweight*> rws;
  string Observable;
  bool Verbose;

};


#endif