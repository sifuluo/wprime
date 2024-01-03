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
  MCReweight(int r_, vector<int> rwr = {}) {
    SourceRegionInt = r_;
    SourceRegion = Form("%d",r_);
    StringRange = rm.StringRanges[rm.GetRangeIndex(r_)];
    if (rwr.size() == 0) RwRegions = {0,1,2,3,4,5,6}; // if not set, it will cover all the regions
    else RwRegions = rwr;
    OrgApplicationRegions()
    OtherMCHists.clear();
    ttbarHists.clear();
    SetVariations(rm.Variations);
    nbins = 0;
  };

  void OrgApplicationRegions() {
    int br = SourceRegionInt / 10;
    br = br * 10;
    for (unsigned i = 0; i < RwRegions.size(); ++i) {
      if (RwRegions[i] < 10) RwRegions[i] = RwRegions[i] + br;
    }
  }

  void SetVariations(vector<string> vars) {
    Variations = vars;
    VarSize = Variations.size();
    OtherMCHists.resize(VarSize);
    ttbarHists.resize(VarSize);
    SF1D->resize(VarSize);
    SF1DF->resize(VarSize);
  }

  void SetXaxis(vector<double> xs) {
    nbins = xs.size() - 1;
    for (unsigned i = 0; i < xs.size(); ++i) {
      xaxis[i] = xs[i];
    }
  }

  TH1F* RebinHist(TH1F* h) {
    TH1F* hnew = new TH1F(h->GetName(), h->GetTitle(), nbins, xaxis);
    hnew->SetDirectory(0);
    double bc(0.), be2(0.);
    for (unsigned i = 1; i <= h->GetNbinsX(); ++i) {
      int nb = hnew->FindBin(h->GetBinLowEdge(i)); // Find bin by the old bin upper edge
      if (hnew->GetBinLowEdge(nb + 1) < h->GetBinLowEdge(i + 1)) { // if the old upper edge exceed the new upper edge, the two binning are not aligned
        cout << "In rebinning: Old bin " << i << " [" << h->GetBinLowEdge[i] << "," << h->GetBinLowEdge[i + 1] << "] is not fully covered by new bin " << nb << "[" << hnew->GetBinLowEdge[nb] << "," << hnew->GetBinLowEdge[nb + 1] << "]" << endl;
      }
      bc += h->GetBinContent(i);
      be2 += h->GetBinError(i) * h->GetBinError(i);
      // if (hnew->GetBinLowEdge(nb + 1) == h->GetBinLowEdge(i + 1)) { // Last old bin to fill the new bin [nb]; GetBinLowEdge is safe to use since it can be called for bin number exceeding the bin range.
      if (hnew->FindBin(h->GetBinLowEdge(i + 1)) != nb) { // Fill bin when the old bin upper edges is no longer included in this bin (a bin does not include its upper edge)
        double be = sqrt(be2);
        hnew->SetBinContent(nb,bc);
        hnew->SetBinError(nb,be);
      }
    }
    return hnew;
  }

  void RebinAndReplace(TH1F* h) {
    TH1F* tmp = h; // save the address of the previous plot for delete later.
    h = RebinHist(h);
    delete tmp;
  }

  void RebinHists() {
    RebinAndReplace(DataHist);
    for (unsigned i = 0; i < ttbarHists.size(); ++i) {
      RebinAndReplace(ttbarHists[i]);
    }
    for (unsigned i = 0; i < OtherMCHists.size(); ++i) {
      RebinAndReplace(OtherMCHists[i]);
    }
  }

  void AddHist(TH1F* h1, TH1F* h2, string note = "") {
    if (h2 == nullptr) return;
    if (h1 == nullptr) {
      h1 = (TH1F*) h2->Clone();
      h1->SetDirectory(0);
      if (note != "") cout << "Init " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
    }
    else {
      h1->Add(h2);
      if (note != "") cout << "Adding " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
    }
  }

  // void AddData(TH1F* h_) {
  //   if (h_ == nullptr) return;
  //   if (DataHist == nullptr) {
  //     DataHist = (TH1F*) h_->Clone();
  //     DataHist->SetDirectory(0);
  //     cout << "init Data in region" << SourceRegion << " with hist: " << h_->GetName() << endl;
  //   }
  //   else {
  //     DataHist->Add(h_);
  //     cout << "Adding Data in region" << SourceRegion << " with hist: " << h_->GetName() << endl;
  //   }
  // }

  // void AddMC(TH1F* h_, int iv = 0) {
  //   if (h_ == nullptr) return;
  //   if (OtherMCHists[iv] == nullptr) {
  //     OtherMCHists[iv] = (TH1F*) h_->Clone();
  //     OtherMCHists[iv]->SetDirectory(0);
  //   }
  //   else OtherMCHists[iv]->Add(h_);
  // }

  // void Addttbar(TH1F* h_, int iv = 0) {
  //   if (h_ == nullptr) return;
  //   ttbarHists[iv] = (TH1F*) h_->Clone();
  //   ttbarHists[iv]->SetDirectory(0);
  // }

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
      for (unsigned iv = 0; iv < VarSize; ++iv) {
        TString histname = StandardNames::HistName(ds, obs, StringRange, rm.Variations[iv]);
        TH1F* h_ = (TH1F*) f->Get(histname);
        if ((iv == 0) && ((ds == "SingleMuon" && SourceRegionInt / 1000 == 1) || (ds == "SingleElectron" && SourceRegionInt / 1000 == 2))) AddHist(DataHist, h_);
        else if (ds == "ttbar") AddHist(ttbarHists[iv], h_);
        else if (dlib.Datasets[ds].Type == 1) AddHist(OtherMCHists[iv], h_);
      }
      f->Close();
      delete f;
    }
    RebinHists();
  }

  void CreateSF1DPlot() {

    SF1D = (TH1F*) DataHist->Clone();
    SF1D->SetDirectory(0);
    SF1D->Add(OtherMCHists[0], -1.);
    SF1D->Divide(ttbarHists[0]);
    SF1D->SetName(("ttbarReweightSF_" + Observable + "_" + SourceRegion).c_str());
    TString SFTitle = Observable + " in " + StringRange +  "; (Data - OtherMC) / ttbar for " + Observable + ";";
    SF1D->SetTitle(SFTitle);
    StartBin = EndBin = -1;
    for (unsigned ib = 1; ib <= DataHist->GetNbinsX(); ++ib) { // Trimming bins with little stats
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
    for (unsigned ib = 1; ib <= SF1D->GetNbinsX(); ++ib) {
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
  vector<int> RwRegions;
  string SourceRegion;
  string StringRange;
  string Observable;
  unsigned nbins;
  double xaxis[100];
  int StartBin, EndBin;
  float xmin, xmax;
  TH1F* DataHist = nullptr;
  vector<string> Variations;
  int VarSize;
  vector<TH1F*> OtherMCHists, ttbarHists;// [iVariation]
  vector<TH1F*> SF1D;
  vector<TF1*> SF1DF;
};

class MCReweightManager {
public:
  MCReweightManager(string obs) {
    Observable = obs;
    Verbose = true;
  };

  void AddbTagRegion(int nbtag = 1, vector<int> rwr = {}) {
    rws.push_back(new MCReweight(1150 + nbtag, rwr));
    rws.push_back(new MCReweight(1160 + nbtag, rwr));
    rws.push_back(new MCReweight(2150 + nbtag, rwr));
    rws.push_back(new MCReweight(2160 + nbtag, rwr));
  }

  void AddApplicationRegions(int baser, vector<int> appregions = {}) {
    
    vector<int> thiscoverage = vector<int>(appregions.size());
    for (unsigned i = 0; i < appregions.size(); ++i) {
      thiscoverage[i] = baser + appregions[i];
    }
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
  
  unsigned FindReweight(int rid) {

  }

  float GetSF1D(double v, int rid) {
    int irws = -1;
    for (unsigned i = 0; i < rws.size(); ++i) {
      if (rws[i]->SourceRegionInt / 10 != rid / 10 ) continue; 
      if 
    }
    if (irws > -1) return rws[i]->GetSF1D(v);
    return 1.0;
  }

  float GetSF1DF(double v, int rid) {
    int irws = -1;
    for (unsigned i = 0; i < rws.size(); ++i) {
      if (rws[i]->SourceRegionInt / 10 != rid / 10) continue; 

    }
    if (irws > -1) return rws[i]->GetSF1DF(v, Verbose);
    return 1.0;
  }

  vector<MCReweight*> rws;
  int MaxRWbTags = 0;
  string Observable;
  bool Verbose;

};


#endif