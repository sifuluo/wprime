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
    bTagWP = conf->bTagWP;
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
    const double HistPtBins[12] = {20,30,50,70,100,140,200,300,600,1000,3000,10000}; // 12 digits, 11 bins
    h_eff[0] = new TH2F("BtagPass_L","BtagPass_L",8,-2.5,5.5, 11,HistPtBins);
    h_eff[1] = new TH2F("BtagPass_M","BtagPass_M",8,-2.5,5.5, 11,HistPtBins);
    h_eff[2] = new TH2F("BtagPass_T","BtagPass_T",8,-2.5,5.5, 11,HistPtBins);
    TotalEvts = new TH2F("TotalEvts","TotalEvts",8,-2.5,5.5, 11,HistPtBins);
    // x axis:
    // -2: all lights(0,1,2,3,4);
    // -1: greater than 5; 
    //  0: some datasets group 1,2,3 into 0, which is the case in my signal sample
    //1-5: Corresponding pdgid absolute value.
  }

  void FillJet(Jet& j) {
    int had = abs(j.hadronFlavour);
    if (had > 5) had = -1; // Other particles
    if (had >= 0) TotalEvts->Fill(had,j.Pt());
    if (had >= 0 && had < 5) TotalEvts->Fill(-2,j.Pt());
    for (unsigned i = 0; i < 3; ++i) {
      if (j.bTagPasses[i]) {
        h_eff[i]->Fill(had,j.Pt());
        if (had >= 0 && had < 5) h_eff[i]->Fill(-2,j.Pt());
      }
    }
  }

  void PostProcess() {
    if (!conf->bTagEffCreation) return;
    for (unsigned i = 0; i < 3; ++i) {
      h_eff[i]->Divide(TotalEvts);
    }
    // delete TotalEvts;
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
    TotalEvts = (TH2F*) f_eff->Get("TotalEvts");
    cout << "Done reading bTagEff file. Testing tight bTagging eff. at pT = 100 is ";
    cout << h_eff[2]->GetBinContent(h_eff[2]->FindBin(5,100)) <<endl;
  }

  vector<float> GetEff(Jet& j) { // This function is intented to be used by NanoAODReader 
    vector<float> out = {0.9,0.7,0.5};
    if (conf->bTagEffCreation) {
      return out;
    }
    for (unsigned iwp = 0; iwp < 3; ++iwp) {
      out[iwp] = h_eff[iwp]->GetBinContent(h_eff[iwp]->FindBin(abs(j.hadronFlavour), j.Pt()));
      // out[iwp] = h_eff[iwp]->GetBinContent(h_eff[iwp]->FindBin(5, j.Pt()));
    }
    j.bTagEffs = out;
    return out;
  }

  double GetEffWP(Jet& j) {
    return h_eff[conf->bTagWP]->GetBinContent(h_eff[conf->bTagWP]->FindBin(5,j.Pt()));
  }

  double GetLightLikelihood(double pt, bool tag) {
    if (!tag) return 1.;
    double MR = h_eff[conf->bTagWP]->GetBinContent(h_eff[conf->bTagWP]->FindBin(-2,pt));
    return MR / (1. - MR);
  }

  double GetbLikelihood(double pt, bool tag) {
    if (tag) return 1.;
    double eff = h_eff[conf->bTagWP]->GetBinContent(h_eff[conf->bTagWP]->FindBin(5,pt));
    return (1. - eff) / eff;
  }

  double GetLikelihood(vector<TLorentzVector>& lvs, vector<bool>& tags, vector<unsigned> perm = {0,1,2,3}) {
    double p0 = GetLightLikelihood(lvs[0].Pt(), tags[0]);
    double p1 = GetLightLikelihood(lvs[1].Pt(), tags[1]);
    double p2 = GetbLikelihood(lvs[2].Pt(),tags[2]);
    double p3 = GetbLikelihood(lvs[3].Pt(),tags[3]);
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
  int bTagWP;
  TString FileName;
  TFile *f_eff;
  vector<TH2F*> h_eff;
  vector<double> PLightMisTag;
  vector<double> PbJetMisTag;
  TH2F* TotalEvts;
  vector<double> MistagEff = {0.1,0.01,0.001};
};


#endif