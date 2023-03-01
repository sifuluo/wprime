#ifndef JETSCALE_CC
#define JETSCALE_CC

#include <vector>
#include <string>
#include <algorithm>

#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TString.h"

#include "Configs.cc"
#include "DataFormat.cc"

using namespace std;

class JetScale{
public:
  JetScale(Configs* conf_) {
    conf = conf_;
    if (conf->JetScaleCreation) CreateScaleHists();
    else ReadScaleHists();
  };

  const vector<double> etabins{0., 1.3, 2.5, 3.0, 5.2}; // size 5, 4 bins, ieta top at 3;

  const vector<vector<double> > ptbins{
    {30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,130., 150.,180.,220., 260., 300.,350.,400.,500.,1000.,6000.}, // 22 bins, 23 numbers
    {30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150., 180.,220.,260., 300.,6000.}, // 17 bins
    {30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150., 180.,220.,260.,6000.}, // 16 bins
    {30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150.,6000.} //13 bins
  };

  pair<int,int> FindBin(double et,double pt) {
    int ieta = -1;
    int ipt = -1;
    for (unsigned ie = 0; ie < etabins.size() - 1; ++ie) {
      if (fabs(et) > etabins[ie] && fabs(et) < etabins[ie+1]) ieta = ie;
    }
    // if (ieta == -1) ieta = etabins.size() - 2; // if not found in bins, use the last bin.

    for (unsigned ip = 0; ip < ptbins[ieta].size() - 1; ++ip) {
      if (pt > ptbins[ieta][ip] && pt < ptbins[ieta][ip+1]) ipt = ip;
    }
    // if (ipt == -1) ipt = ptbins[ieta].size() - 2; // if not found in bins, use the last bin.
    return {ieta, ipt};
  }

  void CreateScaleHists() {
    string filename = "Scale_" + conf->SampleYear + "_" + conf->SampleType + ".root";
    TString filepath = conf->JetScaleBasepath + filename;
    ScaleFile = new TFile(filepath,"RECREATE");
    ScaleHists.resize(etabins.size() - 1);
    for (unsigned ieta = 0; ieta < etabins.size() - 1; ++ieta) {
      ScaleHists[ieta].resize(ptbins[ieta].size() - 1);
      for (unsigned ipt = 0; ipt < ptbins[ieta].size() - 1; ++ipt) {
        TString sn = Form("eta%d_pt%d", ieta, ipt);
        TString st = Form("eta%.1fto%.1f_pt%dto%d;Pt_{Gen}/Pt_{Reco}",etabins[ieta],etabins[ieta+1], int(ptbins[ieta][ipt]), int(ptbins[ieta][ipt+1]));
        ScaleHists[ieta][ipt] = new TH1F(sn,st,600,0,6);
      }
    }
  }

  void FillJet(double eta, double pt, double gpt) {
    pair<int,int> ibins = FindBin(eta, pt);
    if (ibins.first == -1 || ibins.second == -1) {
      return;
    }
    double ratio = gpt / pt;
    ScaleHists[ibins.first][ibins.second]->Fill(ratio);
  }

  void FillJet(Jet& j, GenJet& g) {
    FillJet(j.Eta(), j.Pt(), g.Pt());
  }

  void PostProcess() {
    if (!conf->JetScaleCreation) return;
    ScaleFile->Write();
    ScaleFile->Save();
  }

  void ReadScaleHists() {
    string filename = "Scale_" + conf->SampleYear + "_" + conf->SampleType + ".root";
    TString filepath = conf->JetScaleBasepath + filename;
    ScaleFile = new TFile(filepath, "READ");
    ScaleHists.resize(etabins.size() - 1);
    ScaleFuncs.resize(etabins.size() - 1);
    for (unsigned ieta = 0; ieta < etabins.size() - 1; ++ieta) {
      ScaleHists[ieta].resize(ptbins.size() - 1);
      ScaleFuncs[ieta].resize(ptbins.size() - 1);
      for (unsigned ipt = 0; ipt < ptbins[ieta].size() - 1; ++ipt) {
        TString sn = Form("eta%d_pt%d", ieta, ipt);
        ScaleHists[ieta][ipt] = (TH1F*) ScaleFile->Get(sn);
        double max = ScaleHists[ieta][ipt]->GetMaximum();
        double mean = ScaleHists[ieta][ipt]->GetMean();
        double stddev = ScaleHists[ieta][ipt]->GetStdDev();
        double fitlow = mean - 3. * stddev;
        if (fitlow < 0) fitlow = 0.;
        double fitup = mean + 3. * stddev;
        if (fitup > 2) fitup = 2.;
        TString fsn = Form("f_eta%d_pt%d", ieta, ipt);
        ScaleFuncs[ieta][ipt] = new TF1(fsn,"gaus",fitlow, fitup);
        ScaleFuncs[ieta][ipt]->SetParameters(max / 4.0, mean, stddev);
        ScaleHists[ieta][ipt]->Fit(fsn, "RMQ0", "", fitlow, fitup);
      }
    }
  }

  pair<double, double> ScaleLimits(double eta, double pt) {
    pair<int,int> ibins = FindBin(eta, pt);
    double mean = ScaleFuncs[ibins.first][ibins.second]->GetParameter(1);
    double sigma = ScaleFuncs[ibins.first][ibins.second]->GetParameter(2);
    // return pair<double,double>(mean - 2*sigma, mean + 2*sigma);
    return {mean - 2*sigma, mean + 2*sigma};
  }

  

  double JetScaleLikelihood(double eta, double pt, double scale) {
    pair<int,int> ibins = FindBin(eta,pt);
    if (ibins.first == -1 || ibins.second == -1) return 0;
    double n = ScaleFuncs[ibins.first][ibins.second]->Eval(scale);
    double norm = ScaleFuncs[ibins.first][ibins.second]->GetMaximum();
    double p = n / norm;
    return p;
  }

  double JetScaleLikelihood(vector<TLorentzVector>& js, TLorentzVector lep, TLorentzVector MET, const double* scales) {
    vector<TLorentzVector> sjs;
    double PScale = 1.0;
    sjs.resize(4);
    for (unsigned i = 0; i < 4; ++i) {
      PScale *= JetScaleLikelihood(js[i].Eta(),js[i].Pt(),scales[i]);
      sjs[i] = js[i] * scales[i];
      MET = MET + js[i] - sjs[i];
    }

    vector<TLorentzVector> Neus;
    double PNeutrino = SolveNeutrinos(lep,MET,Neus);
    if (PNeutrino < 0) return (-1.0 * PNeutrino + 1.);

    double PLep = TopMassDis->Eval((lep + Neus[0] + sjs[3]).M());
    double PLep2 = TopMassDis->Eval((lep + Neus[1] + sjs[3]).M());
    if (PLep < PLep2) PLep = PLep2;
    double PHadW = WMassDis->Eval((sjs[0] + sjs[1]).M());
    double PHadT = TopMassDis->Eval((sjs[0] + sjs[1] + sjs[2]).M());
    
    double p = PScale * PHadW * PHadT * PLep * -1.0 + 1;
    
    return p;
  }

  double SolveNeutrinos(TLorentzVector LVLep, TLorentzVector ScaledMET, vector<TLorentzVector>& LVNeu_, bool debug_ = false) {
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

    if (radical < 0) {
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
    return 1;
  }

  void SetUpMassFunctions() {
    TopMassDis = new TF1("TBW","[0]*TMath::BreitWigner(x,[1],[2])",0.0,300.0);
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
  TFile* ScaleFile;
  vector< vector<TH1F*> > ScaleHists;
  vector< vector<TF1*> > ScaleFuncs;
  TF1 *TopMassDis, *WMassDis;


};

#endif