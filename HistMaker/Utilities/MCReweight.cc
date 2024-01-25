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

#include "TFitResult.h"
#include "TMatrixD.h"
#include "TMath.h"

class MCReweight {
public:
  MCReweight(int r_, vector<int> rwr = {}) {
    SourceRegionInt = r_;
    SourceRegion = Form("%d",r_);
    StringRange = rm.StringRanges[rm.GetRangeIndex(r_)];
    if (rwr.size() == 0) RwRegions = {0,1,2,3,4,5,6}; // if not set, it will cover all the regions
    else RwRegions = rwr;
    OrgApplicationRegions();
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

  void SetVariations(vector<string>& vars) {
    Variations = vars;
    VarSize = Variations.size();
    OtherMCHists.resize(VarSize);
    ttbarHists.resize(VarSize);
    SF1D.resize(VarSize);
    SF1DF.resize(VarSize);
  }

  void SetXaxis(vector<double>& xs) {
    nbins = xs.size() - 1;
    if (nbins > 199) cout << "Please check the xaxis array size to > " << xs.size();
    // cout << "New Binning is : {";
    for (unsigned i = 0; i < xs.size(); ++i) {
      xaxis[i] = xs[i];
      // cout << " " << xs[i] << ",";
    }
    // cout << "}" << endl;
  }

  TH1F* RebinHist(TH1F* h) {
    cout <<"Rebinning " << h->GetName() << endl;
    TH1F* hnew = new TH1F(h->GetName(), h->GetTitle(), nbins, xaxis);
    hnew->SetDirectory(0);
    double bc(0.), be2(0.);
    for (int i = 1; i <= h->GetNbinsX(); ++i) {
      int nb = hnew->FindBin(h->GetBinLowEdge(i)); // Find bin by the old bin upper edge
      if (hnew->GetBinLowEdge(nb + 1) < h->GetBinLowEdge(i + 1)) { // if the old upper edge exceed the new upper edge, the two binning are not aligned
        cout << "In rebinning: Old bin " << i << " [" << h->GetBinLowEdge(i) << "," << h->GetBinLowEdge(i + 1) << "] is not fully covered by new bin " << nb << "[" << hnew->GetBinLowEdge(nb) << "," << hnew->GetBinLowEdge(nb + 1) << "]" << endl;
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
    cout << "Done Rebinning " << h->GetName() << endl;
    return hnew;
  }

  void RebinAndReplace(TH1F* h) {
    TH1F* hnew= RebinHist(h);
    *h = *hnew;
    delete hnew;
    // TH1F* tmp = h; // save the address of the previous plot for delete later.
    // *h = *(RebinHist(h));
    // delete tmp;
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

  void AddHist(int type, unsigned iv, TH1F* h_, string note = "") {
    if (h_ == nullptr) return;
    TH1F* h1;
    if (type == 0) h1 = DataHist;
    else if (type == 1) h1 = ttbarHists[iv];
    else h1 = OtherMCHists[iv];
    if (h1 == nullptr) {
      if (type == 0) h1 = DataHist = (TH1F*) h_->Clone();
      else if (type == 1) h1 = ttbarHists[iv] = (TH1F*) h_->Clone();
      else if (type == 2) h1 = OtherMCHists[iv] = (TH1F*) h_->Clone();
      h1->SetDirectory(0);
      if (note != "") cout << "Init " << note << " in region " << SourceRegion << " with hist: " << h1->GetName() << endl;
    }
    else {
      h1->Add(h_);
      if (note != "") cout << "Adding " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
    }
  }

  // void AddData(TH1F* h_, string note = "") {
  //   if (h_ == nullptr) return;
  //   if (DataHist == nullptr) {
  //     DataHist = (TH1F*) h_->Clone();
  //     DataHist->SetDirectory(0);
  //     if (note != "") cout << "Init " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
  //   }
  //   else {
  //     DataHist->Add(h_);
  //     if (note != "") cout << "Adding " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
  //   }
  // }
  // void Addttbar(TH1F* h_, string note = "") {
  //   if (h_ == nullptr) return;
  //   if (DataHist == nullptr) {
  //     DataHist = (TH1F*) h_->Clone();
  //     DataHist->SetDirectory(0);
  //     if (note != "") cout << "Init " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
  //   }
  //   else {
  //     DataHist->Add(h_);
  //     if (note != "") cout << "Adding " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
  //   }
  // }
  // void AddOtherMC(TH1F* h_, string note = "") {
  //   if (h_ == nullptr) return;
  //   if (DataHist == nullptr) {
  //     DataHist = (TH1F*) h_->Clone();
  //     DataHist->SetDirectory(0);
  //     if (note != "") cout << "Init " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
  //   }
  //   else {
  //     DataHist->Add(h_);
  //     if (note != "") cout << "Adding " << note << " in region " << SourceRegion << " with hist: " << h_->GetName() << endl;
  //   }
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
        TString histname = StandardNames::HistName(ds, obs, StringRange, Variations[iv]);
        TH1F* h_ = (TH1F*) f->Get(histname);
        if (h_ == nullptr) continue;
        if ((iv == 0) && ((ds == "SingleMuon" && SourceRegionInt / 1000 == 1) || (ds == "SingleElectron" && SourceRegionInt / 1000 == 2))) {
          AddHist(0, 0 , h_,"Data");
          cout << "DataHistName " << DataHist->GetName() << endl;
        }
        else if (ds == "ttbar") AddHist(1,iv, h_,"ttbar");
        else if (dlib.Datasets[ds].Type == 1) AddHist(2,iv, h_,"OtherMC");
      }
      f->Close();
      delete f;
    }
    RebinHists();
  }

  void CreateSF1DPlot() {
    for (unsigned iv = 0; iv < VarSize; ++iv) {
      SF1D[iv] = (TH1F*) DataHist->Clone();
      SF1D[iv]->SetDirectory(0);
      SF1D[iv]->Add(OtherMCHists[iv], -1.);
      SF1D[iv]->Divide(ttbarHists[iv]);
      SF1D[iv]->SetName(("ttbarReweightSF_" + Observable + "_" + SourceRegion + "_" + Variations[iv]).c_str());
      TString SFTitle = Observable + " in " + StringRange +  "; (Data - OtherMC) / ttbar for " + Observable + "(" + Variations[iv] + ");";
      SF1D[iv]->SetTitle(SFTitle);
      StartBin = EndBin = -1;
      for (int ib = 1; ib <= DataHist->GetNbinsX(); ++ib) { // Trimming bins with little stats
        if (DataHist->GetBinContent(ib) < 10 || ttbarHists[iv]->GetBinContent(ib) < 10) {
          SF1D[iv]->SetBinContent(ib,0.0);
          SF1D[iv]->SetBinError(ib,0.0);
          continue;
        }
        if (StartBin == -1) StartBin = ib;
        EndBin = ib; // LastBin with enough stats
        if (SF1D[iv]->GetBinContent(ib) > 5.0 || SF1D[iv]->GetBinContent(ib) < 0.2) {
          cout << SF1D[iv]->GetName() << " at ibin = " << ib << ", bincenter = " << SF1D[iv]->GetBinCenter(ib) << ", has bin value = " << SF1D[iv]->GetBinContent(ib) << " +- " << SF1D[iv]->GetBinError(ib);
          if (SF1D[iv]->GetBinErrorUp(ib) != SF1D[iv]->GetBinError(ib) || SF1D[iv]->GetBinErrorLow(ib) != SF1D[iv]->GetBinError(ib)){
            cout << "(+" << SF1D[iv]->GetBinErrorUp(ib) << ",-" << SF1D[iv]->GetBinErrorLow(ib) << ")";
          }
          cout << endl;
          cout << Form("Data: %f(+ %f, - %f)", DataHist->GetBinContent(ib), DataHist->GetBinErrorUp(ib), DataHist->GetBinErrorLow(ib)) << endl;
          cout << Form("OtherMC: %f(+ %f, - %f)", OtherMCHists[iv]->GetBinContent(ib), OtherMCHists[iv]->GetBinErrorUp(ib), OtherMCHists[iv]->GetBinErrorLow(ib)) << endl;
          cout << Form("ttbar: %f(+ %f, - %f)", ttbarHists[iv]->GetBinContent(ib), ttbarHists[iv]->GetBinErrorUp(ib), ttbarHists[iv]->GetBinErrorLow(ib)) << endl;
        }
      }
      
      TString FuncName = "MCRFunc" + SourceRegion + Variations[iv];
      SF1DF[iv] = new TF1(FuncName,"[0]/x/x+[1]/x+[2]+[3]*x",SF1D[iv]->GetBinLowEdge(StartBin),SF1D[iv]->GetBinLowEdge(EndBin+1));
      SF1D[iv]->Fit(SF1DF[iv],"RMQ0");
    }
  }

  bool ReadFromFile(TFile *f, string obs) { // Read the reweight file for later application
    Observable = obs;
    xmin.resize(VarSize);
    xmax.resize(VarSize);
    for (unsigned iv = 0; iv < VarSize; ++iv) {
      TString hn = "ttbarReweightSF_" + Observable + "_" + SourceRegion + "_" + Variations[iv];
      TString fn = "MCRFunc" + SourceRegion + Variations[iv];
      TH1F* htmp = (TH1F*)f->Get(hn);
      // cout << "Trying to access SF plot: " << hn << endl;
      if (htmp == nullptr) return false;
      SF1D[iv] = (TH1F*)f->Get(hn)->Clone();
      SF1D[iv]->SetDirectory(0);
      nbins = SF1D[iv]->GetNbinsX();
      StartBin = EndBin = -1;
      for (int ib = 1; ib <= SF1D[iv]->GetNbinsX(); ++ib) {
        if (SF1D[iv]->GetBinContent(ib) == 0) continue;
        if (StartBin == -1) StartBin = ib;
        EndBin = ib;
      }
      SF1DF[iv] = SF1D[iv]->GetFunction(fn);
      if (iv == 0) {
        // SF1DF[iv] = new TF1(fn,"[0]/x/x+[1]/x+[2]+[3]*x",SF1D[iv]->GetBinLowEdge(StartBin),SF1D[iv]->GetBinLowEdge(EndBin+1));
        FitR = SF1D[iv]->Fit(SF1DF[iv],"RMQS0");
        TMatrixDSym m = FitR->GetCovarianceMatrix();
        unsigned npar = SF1DF[iv]->GetNpar();
        Cov.resize(npar);
        for (unsigned ip = 0; ip < npar; ++ip) {
          Cov[ip].resize(npar);
          for (unsigned ip2 = 0; ip2 < npar; ++ip2) {
            Cov[ip][ip2] = m(ip,ip2);
          }
        }
        cout << "m(0,1) = " << m(0,1) << ", Cov[0,1] = " << Cov[0][1]<< endl;
      }
      xmin[iv] = SF1D[iv]->GetBinLowEdge(StartBin);
      xmax[iv] = SF1D[iv]->GetBinLowEdge(EndBin + 1);
    }
    cout << "SF Value at 685.592GeV from" << SourceRegion << " is " << SF1D[0]->GetBinContent(SF1D[0]->FindBin(685.592)) << endl;
    cout << "SF Evaled value of the central is " << SF1DF[0]->Eval(685.592) << endl;
    return true;
  }

  void SaveToFile(TFile *f) {
    for (unsigned iv = 0; iv < SF1D.size(); ++iv) {
      TH1F* h = (TH1F*) SF1D[iv]->Clone();
      h->SetDirectory(f);
    }
  }

  bool IsIncludedRegion(int rid) {
    for (unsigned i = 0; i < RwRegions.size(); ++i) {
      if (RwRegions[i] == rid) return true;
    }
    return false;
  }

  float GetSF1D(double v, int iv) {
    return SF1D[iv]->GetBinContent(SF1D[iv]->FindBin(v));
  }
  float GetSF1DF(double v, int iv, bool Verbose = true) {
    double vv = v;
    if (v < xmin[iv]) vv = xmin[iv];
    if (v > xmax[iv]) vv = xmax[iv];
    float sf = SF1DF[iv]->Eval(vv);
    if (sf > 3.0 || sf < 0.3) {
      cout << SF1D[iv]->GetName() << " function at " << v << " has extreme value of " << sf << endl;
    }
    return sf;
  }
  float GetSF1DFError(double v) {
    vector<double> deriv = {1./v/v, 1./v, 1., v};
    float envelope = 0.;
    for (unsigned x = 0; x < deriv.size(); ++x) {
      for (unsigned y = 0; y < deriv.size(); ++y) {
        envelope += deriv[x] * deriv[y] * Cov[x][y];
      }
    }
    return envelope;
  }

  int SourceRegionInt;
  vector<int> RwRegions;
  string SourceRegion;
  string StringRange;
  string Observable;
  unsigned nbins;
  double xaxis[200];
  int StartBin, EndBin;
  vector<float> xmin, xmax;
  TH1F* DataHist = nullptr;
  vector<string> Variations;
  unsigned VarSize;
  vector<TH1F*> OtherMCHists, ttbarHists;// [iVariation]
  vector<TH1F*> SF1D;
  vector<TF1*> SF1DF;
  TFitResultPtr FitR;
  vector<vector<double> > Cov;
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

  void SetXaxis(vector<double>& xs) {
    for (unsigned i = 0; i < rws.size(); ++i) {
      rws[i]->SetXaxis(xs);
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
      rws[i]->SaveToFile(f);
    }
    f->Write();
    f->Save();
    f->Close();
    cout << "Reweight file has been saved to " << rwfn << endl;
    delete f;
  }

  float GetSF1D(double v, int rid, int iv) {
    for (unsigned i = 0; i < rws.size(); ++i) {
      if (rws[i]->IsIncludedRegion(rid)) return rws[i]->GetSF1D(v,iv);
    }
    return 1.0;
  }

  float GetSF1DF(double v, int rid, int iv) {
    for (unsigned i = 0; i < rws.size(); ++i) {
      if (rws[i]->IsIncludedRegion(rid)) return rws[i]->GetSF1DF(v,iv);
    }
    return 1.0;
  }

  float GetSF1DFError(double v, int rid) {
    for (unsigned i = 0; i < rws.size(); ++i) {
      if (rws[i]->IsIncludedRegion(rid)) return rws[i]->GetSF1DFError(v);
    }
    return 0;
  }

  vector<MCReweight*> rws;
  string Observable;
  bool Verbose;

};


#endif