#ifndef BTAG_CC
#define BTAG_CC

#include <vector>
#include <string>

#include "TH2.h"

#include "Configs.cc"
#include "DataFormat.cc"

std::vector<bool> bTag(float bTagDisc_, string year_){
  std::vector<float> WPs; //loose, medium, tight
  std::vector<bool> out = {false, false, false};
  //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16preVFP
  if(year_ == "2016apv") WPs = {0.0508,0.2598,0.6502};
  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL16postVFP
  else if(year_ == "2016") WPs = {0.0480,0.2489,0.6377};
  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL17
  else if(year_ == "2017") WPs = {0.0532,0.3040,0.7476};
  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL18
  else if(year_ == "2018") WPs = {0.0490,0.2783,0.7100};
  else return out;

  if(bTagDisc_ > WPs[2]) out = {true, true, true};
  else if(bTagDisc_ > WPs[1]) out = {true, true, false};
  else if(bTagDisc_ > WPs[0]) out = {true, false, false};

  return out;
}

class bTagEff {
public:
  bTagEff(Configs* conf_) {
    conf = conf_;
    if (conf->bTagEffCreation) CreateEffFile();
    else ReadEffFile();
  }

  void CreateEffFile() {
    string filename = "bTagEff_" + conf->SampleYear + "_" + conf->SampleType + ".root";
    TString filepath = conf->bTagEffBasepath + filename;
    f_eff = new TFile(filepath,"RECREATE");
    h_eff.resize(3);
    double pt[12] = {20,30,50,70,100,140,200,300,600,1000,3000,10000};
    h_eff[0] = new TH2F("BtagPass_L","BtagPass_L",7,-1.5,5.5, 11,pt);
    h_eff[1] = new TH2F("BtagPass_M","BtagPass_M",7,-1.5,5.5, 11,pt);
    h_eff[2] = new TH2F("BtagPass_T","BtagPass_T",7,-1.5,5.5, 11,pt);
    TotalEvts = new TH2F("TotalEvts","TotalEvts",7,-1.5,5.5, 11,pt);
  }

  void FillJet(Jet& j) {
    int had = abs(j.hadronFlavour);
    if (had > 5) had = -1;
    TotalEvts->Fill(had,j.Pt());
    for (unsigned i = 0; i < 3; ++i) {
      if (j.bTagPasses[i]) {
        h_eff[i]->Fill(had,j.Pt());
      }
    }
  }

  void PostProcess() {
    if (!conf->bTagEffCreation) return;
    for (unsigned i = 0; i < 3; ++i) {
      h_eff[i]->Divide(TotalEvts);
    }
    delete TotalEvts;
    f_eff->Write();
    f_eff->Save();
  }

  void ReadEffFile() {
    if (conf->bTagEffCreation) return;
    string filename = "savefile_" + conf->SampleYear + "_" + conf->SampleType + "_skim.root";
    TString filepath = conf->bTagEffBasepath + filename;
    f_eff = new TFile(filepath,"READ");
    h_eff.resize(3);
    h_eff[0] = (TH2F*) f_eff->Get("BtagPass_L");
    h_eff[1] = (TH2F*) f_eff->Get("BtagPass_M");
    h_eff[2] = (TH2F*) f_eff->Get("BtagPass_T");
  }

  vector<float> GetEff(Jet& j) {
    vector<float> out = {0.9,0.7,0.5};
    if (conf->bTagEffCreation) {
      return out;
    }
    for (unsigned iwp = 0; iwp < 3; ++iwp) {
      out[iwp] = h_eff[iwp]->GetBinContent(h_eff[iwp]->FindBin(j.hadronFlavour, j.Pt()));
    }
    j.bTagEffs = out;
    return out;
  }

  double GetLikelihood(vector<Jet>& js) {
    double p = 1.;
    double MR = MistagEff[conf->bTagWP];
    if (js[0].bTagPasses[conf->bTagWP]) p *= (MR / (1. - MR));
    if (js[1].bTagPasses[conf->bTagWP]) p *= (MR / (1. - MR));
    for (unsigned i = 2; i < js.size(); ++i) {
      if (!(js[i].bTagPasses[conf->bTagWP])) p *= ((1. - js[i].bTagEffs[conf->bTagWP]) / js[i].bTagEffs[conf->bTagWP]);
    }
    return p;
  }

  double GetWPBLikelihood(Jet& j) {
    if (j.bTagPasses[conf->bTagWP]) return 1.0;
    else return ((1. - j.bTagEffs[conf->bTagWP]) / j.bTagEffs[conf->bTagWP]);
  }

  void Clear() {
    for (unsigned i = 0; i < 3; ++i) delete h_eff[i];
  }
  
  Configs *conf;
  TFile *f_eff;
  vector<TH2F*> h_eff;
  TH2F* TotalEvts;
  vector<double> MistagEff = {0.1,0.01,0.001};
};


#endif