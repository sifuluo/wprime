#ifndef PUID_CC
#define PUID_CC

#include <vector>
#include <string>
#include <iostream>

#include "DataFormat.cc"
#include "TFile.h"
#include "TH2.h"

//working points for PU jet ID implemented as in https://twiki.cern.ch/twiki/bin/view/CMS/PileupJetIDUL
std::vector<bool> PUID(float pT_, float absEta_, float PUIDdisc_, string year_){
 std::vector<bool> output = {true, true, true};
  if(pT_ >= 50.) output = {true, true, true};
  else if(year_ == "2016apv" || year_ == "2016"){
    if(absEta_ < 2.5){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.94) output = {true, true, true};
        else if(PUIDdisc_ > 0.86) output = {true, true, false};
        else if(PUIDdisc_ > -0.71) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.97) output = {true, true, true};
        else if(PUIDdisc_ > 0.93) output = {true, true, false};
        else if(PUIDdisc_ > -0.42) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 2.75){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.24) output = {true, true, true};
        else if(PUIDdisc_ > -0.10) output = {true, true, false};
        else if(PUIDdisc_ > -0.36) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.48) output = {true, true, true};
        else if(PUIDdisc_ > 0.19) output = {true, true, false};
        else if(PUIDdisc_ > -0.09) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 3.0){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.05) output = {true, true, true};
        else if(PUIDdisc_ > -0.15) output = {true, true, false};
        else if(PUIDdisc_ > -0.29) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.26) output = {true, true, true};
        else if(PUIDdisc_ > 0.04) output = {true, true, false};
        else if(PUIDdisc_ > -0.14) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 5.0){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.10) output = {true, true, true};
        else if(PUIDdisc_ > -0.08) output = {true, true, false};
        else if(PUIDdisc_ > -0.23) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.29) output = {true, true, true};
        else if(PUIDdisc_ > 0.12) output = {true, true, false};
        else if(PUIDdisc_ > -0.02) output = {true, false, false};
        else output = {false, false, false};
      }
    }
  }
  else if(year_ == "2017" || year_ == "2018"){
    if(absEta_ < 2.5){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.96) output = {true, true, true};
        else if(PUIDdisc_ > 0.90) output = {true, true, false};
        else if(PUIDdisc_ > -0.63) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.98) output = {true, true, true};
        else if(PUIDdisc_ > 0.96) output = {true, true, false};
        else if(PUIDdisc_ > -0.19) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 2.75){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.82) output = {true, true, true};
        else if(PUIDdisc_ > 0.36) output = {true, true, false};
        else if(PUIDdisc_ > -0.18) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.92) output = {true, true, true};
        else if(PUIDdisc_ > 0.61) output = {true, true, false};
        else if(PUIDdisc_ > 0.22) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 3.0){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.20) output = {true, true, true};
        else if(PUIDdisc_ > -0.16) output = {true, true, false};
        else if(PUIDdisc_ > -0.43) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.47) output = {true, true, true};
        else if(PUIDdisc_ > 0.14) output = {true, true, false};
        else if(PUIDdisc_ > -0.13) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 5.0){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.09) output = {true, true, true};
        else if(PUIDdisc_ > -0.09) output = {true, true, false};
        else if(PUIDdisc_ > -0.24) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.29) output = {true, true, true};
        else if(PUIDdisc_ > 0.12) output = {true, true, false};
        else if(PUIDdisc_ > -0.03) output = {true, false, false};
        else output = {false, false, false};
      }
    }
  }
  return output;
};

class PUIDSFReader {
  public:
  PUIDSFReader(Configs* conf_) {
    conf = conf_;
  };

  void ReadPUIDSF() {
    TString puidfilename = "AUXFiles/PUID_106XTraining_ULRun2_EffSFandUncties_v1.root";
    puidfile = new TFile(puidfilename);
    string puidsfname = "h2_eff_sfUL";
    if (conf->iSampleYear == 0) puidsfname = puidsfname + "2016APV";
    else puidsfname = puidsfname + conf->SampleYear;
    vector<string> wps = {"_L","_M","_T"};
    for (unsigned i = 0; i < 3; ++i) {
      TString tmpname = puidsfname + wps[i];
      TString tmpnameerr = tmpname + "_Systuncty";
      puidsfs.push_back((TH2F*)puidfile->Get(tmpname));
      puidsferrs.push_back((TH2F*)puidfile->Get(tmpnameerr));
    }
  }

  vector<float> GetPUIDSFSingleWP(double eta, double pt, int wp = -1) {
    if (wp < 0) wp = conf->PUIDWP;
    vector<float> out = {1.,1.,1.};
    float nominal = puidsfs[wp]->GetBinContent(puidsfs[wp]->FindBin(pt,eta));
    float err = puidsferrs[wp]->GetBinContent(puidsferrs[wp]->FindBin(pt,eta));
    out[0] = nominal;
    out[1] = nominal + err;
    out[2] = nominal - err;
    return out;
  }

  vector<vector<float> > GetScaleFactors(double eta, double pt, vector<bool> PUIDpasses = {}) {
    vector<vector<float> > out = {{1.,1.,1.},{1.,1.,1.},{1.,1.,1.}};
    if (PUIDpasses.size() != 3) PUIDpasses = {true, true, true};
    for (unsigned iwp = 0; iwp < 3; ++iwp) {
      if (!PUIDpasses[iwp]) continue;
      float nominal = puidsfs[iwp]->GetBinContent(puidsfs[iwp]->FindBin(pt,eta));
      float err = puidsferrs[iwp]->GetBinContent(puidsferrs[iwp]->FindBin(pt,eta));
      out[0][iwp] = nominal;
      out[1][iwp] = nominal + err;
      out[2][iwp] = nominal - err;
    }
    return out;
  }

  vector<vector<float> > GetScaleFactors(Jet& j) {
    return GetScaleFactors(j.Eta(), j.Pt(), j.PUIDpasses);
  }

  void CompareScaleFactors(Jet& j, vector<vector<float> > PUIDSFs) {
    vector<vector<float> > calc = GetScaleFactors(j);
    vector<bool> rep = {false, false, false};
    rep[0] = j.PUIDpasses[0] && !(PUIDSFs[0][0] == calc[0][0] && PUIDSFs[1][0] == calc[1][0] && PUIDSFs[2][0] == calc[2][0]);
    rep[1] = j.PUIDpasses[1] && !(PUIDSFs[0][1] == calc[0][1] && PUIDSFs[1][1] == calc[1][1] && PUIDSFs[2][1] == calc[2][1]);
    rep[2] = j.PUIDpasses[2] && !(PUIDSFs[0][2] == calc[0][2] && PUIDSFs[1][2] == calc[1][2] && PUIDSFs[2][2] == calc[2][2]);
    if (rep[0] || rep[1] || rep[2]) cout << "PUIDSFs diff. Jet pT = " << j.Pt() << ", eta = " << j.Eta() << endl;
    for (unsigned iwp = 0; iwp < 3; ++iwp) {
      if (rep[iwp]) cout << Form("WP %i: %f(%f), Up %f(%f), Down %f(%f)",iwp, PUIDSFs[0][iwp], calc[0][iwp],PUIDSFs[1][iwp], calc[1][iwp],PUIDSFs[2][iwp], calc[2][iwp]) <<endl;
    }
  }

  TFile* puidfile;
  vector<TH2F*> puidsfs;
  vector<TH2F*> puidsferrs;
  Configs* conf;
};

#endif