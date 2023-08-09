#ifndef JETSCALE_CC
#define JETSCALE_CC
// Creates and read jet resolution histograms for fitter to use to evaluate the likelihood of jet scaled to a certain energy

#include <vector>
#include <string>
#include <algorithm>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TString.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include "Configs.cc"
#include "DataFormat.cc"

using namespace std;

class JetScale{
public:
  JetScale(Configs* conf_) {
    conf = conf_;
    GetFileName();
    if (conf->AuxHistCreation && conf->IsMC) CreateScaleHists();
    else {
      ReadScaleHists();
      SetUpMassFunctions();
    }
  };

  const vector<double> etabins{0., 1.3, 2.5, 3.0, 5.2}; // size 5, 4 bins, ieta top at 3;

  const vector<vector<double> > ptbins{
    {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,130., 150.,180.,220., 260., 300.,350.,400.,500.,1000.,10000.}, // 23 bins, 24 numbers
    {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150., 180.,220.,260., 300.,10000.}, // 18 bins
    {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150., 180.,220.,260.,10000.}, // 17 bins
    {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150.,10000.} //14 bins
  };

  void GetFileName() {
    string sampletype = conf->SampleType;
    if ((!conf->AuxHistCreation && conf->UseMergedAuxHist) || !conf->IsMC) sampletype = "Merged";
    string filename = "Scale_" + conf->SampleYear + "_" + sampletype + ".root";
    FileName = conf->AuxHistBasePath + filename;
  }

  pair<int,int> FindBin(double et,double pt) {
    int ieta = -1;
    int ipt = -1;
    for (unsigned ie = 0; ie < etabins.size() - 1; ++ie) {
      if (fabs(et) >= etabins[ie] && fabs(et) < etabins[ie+1]) ieta = ie;
    }
    if (ieta == -1) {
      if (!conf->AuxHistCreation)cout << "Encountered eta = " << et << ", Using last eta bin 3.0 - 5.2 " << endl;
      ieta = etabins.size() - 2; // Using last bin.
    }
    // if (ieta == -1) ieta = etabins.size() - 2; // if not found in bins, use the last bin.

    for (unsigned ip = 0; ip < ptbins[ieta].size() - 1; ++ip) {
      if (pt >= ptbins[ieta][ip] && pt < ptbins[ieta][ip+1]) ipt = ip;
    }
    // if (ipt == -1) ipt = ptbins[ieta].size() - 2; // if not found in bins, use the last bin.
    if (pt < 30 && !conf->AuxHistCreation) {
      cout << "Encountered pt = " << pt << ", Should have been filtered out already" <<endl;
    }
    else if (pt > ptbins[ieta].back()) {
      if (!conf->AuxHistCreation) cout << "Encountered pt = " << pt << ", Using last pt bin of pt up to 10000" <<endl;
      ipt = ptbins[ieta].size() - 2;
    }
    else if (ipt == -1 && !conf->AuxHistCreation) cout << "Unknown pt break : pt = " << pt << endl;
    return pair<int,int>(ieta, ipt);
  }

  void CreateScaleHists() {
    ScaleFile = new TFile(FileName,"RECREATE");
    vector<vector<TH1F*> > jes;
    jes.clear();
    for (unsigned ieta = 0; ieta < etabins.size() - 1; ++ieta) {
      vector<TH1F*> jeseta;
      jeseta.clear();
      for (unsigned ipt = 0; ipt < ptbins[ieta].size() - 1; ++ipt) {
        TString sn = Form("eta%d_pt%d", ieta, ipt);
        TString st = Form("eta%.1fto%.1f_pt%dto%d;Pt_{Gen}/Pt_{Reco}",etabins[ieta],etabins[ieta+1], int(ptbins[ieta][ipt]), int(ptbins[ieta][ipt+1]));
        jeseta.push_back(new TH1F(sn,st,600,0,6));
      }
      jes.push_back(jeseta);
    }
    ScaleHists = jes;
  }

  void FillJet(double eta, double pt, double gpt, double ew = 1) {
    pair<int,int> ibins = FindBin(eta, pt);
    if (ibins.first == -1 || ibins.second == -1) {
      return;
    }
    double ratio = gpt / pt;
    ScaleHists[ibins.first][ibins.second]->Fill(ratio, ew);
  }

  void FillJet(Jet& j, GenJet& g, double ew = 1) {
    FillJet(j.Eta(), j.Pt(), g.Pt(), ew);
  }

  void PostProcess() {
    if (!conf->AuxHistCreation) return;
    ScaleFile->Write();
    ScaleFile->Save();
  }

  void ReadScaleHists() {// Read the jet response file. Then read and fit the jet response histos.
    ScaleFile = new TFile(FileName, "READ");
    // vector< vector<TH1F*> > jes;
    vector< vector<TF1*> > fjes;
    // jes.clear();
    fjes.clear();
    for (unsigned ieta = 0; ieta < etabins.size() - 1; ++ieta) {
      vector<TH1F*> jeseta;
      vector<TF1*> fjeseta;
      // jeseta.clear();
      fjeseta.clear();
      for (unsigned ipt = 0; ipt < ptbins[ieta].size() - 1; ++ipt) {
        TString sn = Form("eta%d_pt%d", ieta, ipt);
        TH1F* h1 = (TH1F*) ScaleFile->Get(sn);
        // jeseta.push_back(h1);
        double max = h1->GetMaximum();
        double mean = h1->GetMean();
        double stddev = h1->GetStdDev();
        double fitlow = mean - 2. * stddev;
        if (fitlow < 0) fitlow = 0.;
        double fitup = mean + 2. * stddev;
        if (fitup > 2) fitup = 2.;
        TString fsn = Form("f_eta%d_pt%d", ieta, ipt);
        TF1* f1 = new TF1(fsn,"gaus",0., 2.);
        f1->SetParameters(max / 4.0, mean, stddev);
        h1->Fit(f1, "RMQ0", "", 0., 2.);
        // h1->Fit(f1, "RM0", "", 0., 2.);
        fjeseta.push_back(f1);
      }
      // jes.push_back(jeseta);
      fjes.push_back(fjeseta);
    }
    // ScaleHists = jes;
    ScaleFuncs = fjes;
    ScaleFile->Close();
    cout << "Finished reading jet scale histograms and fitting." << endl;
    cout << "Function of etalow = " << etabins[1] << ", ptlow = " << ptbins[1][2] << ", mean = " << ScaleFuncs[1][2]->GetParameter(1) <<endl;
  }

  pair<double, double> ScaleLimits(double eta, double pt) { // Get the variation limit as +- 2 sigma from the mean
    pair<int,int> ibins = FindBin(eta, pt);
    double mean = ScaleFuncs[ibins.first][ibins.second]->GetParameter(1);
    double sigma = ScaleFuncs[ibins.first][ibins.second]->GetParameter(2);
    return pair<double,double>(mean - 2*sigma, mean + 2*sigma);
  }

  double JetScaleLikelihood(double eta, double pt, double scale) { // Get the likelihood of variating  a jet in a eta, pt region.
    pair<int,int> ibins = FindBin(eta,pt);
    if (ibins.first == -1 || ibins.second == -1) return 0;
    double n = ScaleFuncs[ibins.first][ibins.second]->Eval(scale);
    double norm = ScaleFuncs[ibins.first][ibins.second]->GetMaximum();
    double p = n / norm;
    return p;
  }

  // Evaluating the likelihood of mass of a W / t
  double EvalW(TLorentzVector w) {
    return WMassDis->Eval(w.M());
  }
  double EvalTop(TLorentzVector t) {
    return TopMassDis->Eval(t.M());
  }

  double SolveNeutrinos(TLorentzVector LVLep, TLorentzVector ScaledMET, vector<TLorentzVector>& LVNeu_, bool debug_ = false) {
    // Solve for the neutrino with the lepton and met provided
    bool debug = debug_;
    LVNeu_.clear();
    const double LepWMass = 80.4;
    double xn = ScaledMET.X();
    double yn = ScaledMET.Y();
    double xe = LVLep.X();
    double ye = LVLep.Y();
    double ze = LVLep.Z();
    double te = LVLep.T();
    double tar = LepWMass;
    double zn1, tn1, zn2, tn2;

    double tar2(tar*tar),tar4(tar2*tar2), xexn(xe*xn), yeyn(ye*yn), xe2(xe*xe), ye2(ye*ye), te2(te*te), xnye(xn*ye), xeyn(xe*yn);
    if (te2 - xe2 - ye2 - ze*ze != 0 && debug) cout << "Lepton mass is not zero!! : " << te2 - xe2 - ye2 - ze*ze <<endl;
    double v1 = 4.*pow((xnye - xeyn),2);
    double v2 = 4.*tar2*(xexn + yeyn);
    double radical = (tar4 - v1 + v2)*te2;

    if(debug) {
      cout << Form("Lepton X= %f, Y= %f, Z= %f, T= %f, MET X= %f Y= %f", xe, ye, ze, te, xn, yn)<<endl;
      cout << Form("tar2= %f, tar4= %f, xexn= %f, yeyn= %f, xe2= %f, ye2= %f, te2= %f, xnye= %f, xeyn= %f",tar2,tar4,xexn,yeyn,xe2,ye2,te2,xnye,xeyn)<<endl;
      cout << Form("radical = (tar4 - v1 + v2)*te2, here tar4 = %f, v1 = %f, v2 = %f, te2 = %f", tar4, v1, v2, te2)<<endl;
    }

    if (radical < 0) { // No neutrino and be achieved, return negative value
      LVNeu_.push_back(TLorentzVector());
      LVNeu_.push_back(TLorentzVector());
      return radical;
    }

    double coe = 1. / (2.*(xe2 + ye2));
    double a = (tar2 + 2.*xexn + 2.*yeyn) * ze;
    double b = sqrt(radical);
    zn1 = coe*(a + b);
    zn2 = coe*(a - b);
    tn1 = sqrt(zn1*zn1 + xn*xn + yn*yn);
    tn2 = sqrt(zn2*zn2 + xn*xn + yn*yn);
    TLorentzVector LVNeu1, LVNeu2;
    LVNeu1.SetXYZT(xn,yn,zn1,tn1);
    LVNeu2.SetXYZT(xn,yn,zn2,tn2);
    LVNeu_.push_back(LVNeu1);
    LVNeu_.push_back(LVNeu2);
    return 1; // Neutrino solutions are achieved, return 1
  }

  void SetUpMassFunctions() {
    TopMassDis = new TF1("TBW","[0]*TMath::BreitWigner(x,[1],[2])",0.0,400.0);
    WMassDis = new TF1("WBW","[0]*TMath::BreitWigner(x,[1],[2])",0.0,200.0);
    TopMassDis->SetParameters(100.,171.186,26.76);
    WMassDis->SetParameters(100.,80.385,2.738);
    TopMassDis->SetParameter(0,100./TopMassDis->Eval(171.186)); // normalized it to peak at y = 1;
    WMassDis->SetParameter(0,100./WMassDis->Eval(80.385)); // normalized it to peak at y = 1;
  }

  void Clear() {
    ScaleHists.clear();
  }

  Configs* conf;
  TString FileName;
  TFile* ScaleFile;
  vector< vector<TH1F*> > ScaleHists;
  vector< vector<TF1*> > ScaleFuncs;
  TF1* TopMassDis = new TF1("TBW","[0]*TMath::BreitWigner(x,[1],[2])",0.0,300.0);
  TF1* WMassDis = new TF1("WBW","[0]*TMath::BreitWigner(x,[1],[2])",0.0,200.0);

};

#endif