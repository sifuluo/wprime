#ifndef BTAG_CC
#define BTAG_CC

#include <vector>
#include <string>

#include "TH2.h"

#include "Configs.cc"
#include "DataFormat.cc"

std::vector<bool> bTag(double bTagDisc_, string year_){
  std::vector<double> WPs; //loose, medium, tight
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
    GetFileName();
    if (conf->bTagEffCreation) CreateEffFile();
    else ReadEffFile();
  };

  void GetFileName() {
    string filename = "bTagEff_" + conf->SampleYear + "_" + conf->SampleType + ".root";
    FileName = conf->bTagEffBasepath + filename;
  }

  void CreateEffFile() {
    f_eff = new TFile(FileName,"RECREATE");
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
    f_eff = new TFile(FileName,"READ");
    cout << "Reading from bTagEff file " << FileName << endl;
    h_eff.resize(3);
    h_eff[0] = (TH2F*) f_eff->Get("BtagPass_L");
    h_eff[1] = (TH2F*) f_eff->Get("BtagPass_M");
    h_eff[2] = (TH2F*) f_eff->Get("BtagPass_T");
    cout << "Done reading bTagEff file. Tight bTagging eff. at pT = 100 is ";
    cout << h_eff[2]->GetBinContent(h_eff[2]->FindBin(5,100)) <<endl;
  }

  vector<float> GetEff(Jet& j) {
    vector<float> out = {0.9,0.7,0.5};
    if (conf->bTagEffCreation) {
      return out;
    }
    for (unsigned iwp = 0; iwp < 3; ++iwp) {
      // out[iwp] = h_eff[iwp]->GetBinContent(h_eff[iwp]->FindBin(abs(j.hadronFlavour), j.Pt()));
      out[iwp] = h_eff[iwp]->GetBinContent(h_eff[iwp]->FindBin(5, j.Pt()));
    }
    j.bTagEffs = out;
    return out;
  }

  double GetEffWP(Jet& j) {
    return h_eff[conf->bTagWP]->GetBinContent(h_eff[conf->bTagWP]->FindBin(5,j.Pt()));
  }

  double GetLikelihood(vector<Jet>& js, vector<unsigned> perm = {0,1,2,3}) {
    double p0(1.0), p1(1.0), p2(1.0), p3(1.0);
    double MR = MistagEff[conf->bTagWP];
    if (js[perm[0]].bTagPasses[conf->bTagWP]) p0 = (MR / (1. - MR));
    if (js[perm[1]].bTagPasses[conf->bTagWP]) p1 = (MR / (1. - MR));
    if (!(js[perm[2]].bTagPasses[conf->bTagWP])) {
      double eff = js[perm[2]].bTagEffs[conf->bTagWP];
      p2 = ((1. - eff) / eff);
      // if (p2 > 1.) cout << Form("p2 = %f, eff = %f, bincon = %f, flav = %i, pt = %f", p2, eff, GetEffWP(js[perm[2]]), js[perm[2]].hadronFlavour, js[perm[2]].Pt()) << endl;
    }
    if (!(js[perm[3]].bTagPasses[conf->bTagWP])) {
      double eff = js[perm[3]].bTagEffs[conf->bTagWP];
      p3 = ((1. - eff) / eff);
    }
    double p = p0 * p1 * p2 * p3;
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
  TString FileName;
  TFile *f_eff;
  vector<TH2F*> h_eff;
  TH2F* TotalEvts;
  vector<double> MistagEff = {0.1,0.01,0.001};
};


#endif