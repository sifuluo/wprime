#ifndef BTAG_CC
#define BTAG_CC

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "TH2.h"
#include "TF1.h"

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
    if (!conf->IsMC) return;
    GetFileName();
    if (conf->AuxHistCreation) CreateEffFile();
    else ReadEffFile();
  };
  

  void GetFileName() {
    string sampletype = conf->SampleType;
    if ((!conf->AuxHistCreation && conf->UseMergedAuxHist) || !conf->IsMC) sampletype = "Merged";
    string filename = "bTagEff_" + conf->SampleYear + "_" + conf->SampleType + ".root";
    FileName = conf->AuxHistBasePath + filename;
  }

  void CreateEffFile() {
    f_eff = new TFile(FileName,"RECREATE");
    h_eff.resize(6);
    const double HistPtBins[12] = {20,30,50,70,100,140,200,300,600,1000,3000,10000}; // 12 digits, 11 bins
    h_eff[0] = new TH2F("BtagEff_L","BtagEff_L",8,-2.5,5.5, 11,HistPtBins);
    h_eff[1] = new TH2F("BtagEff_M","BtagEff_M",8,-2.5,5.5, 11,HistPtBins);
    h_eff[2] = new TH2F("BtagEff_T","BtagEff_T",8,-2.5,5.5, 11,HistPtBins);
    h_eff[3] = new TH2F("BtagPass_L","BtagPass_L",8,-2.5,5.5, 11,HistPtBins);
    h_eff[4] = new TH2F("BtagPass_M","BtagPass_M",8,-2.5,5.5, 11,HistPtBins);
    h_eff[5] = new TH2F("BtagPass_T","BtagPass_T",8,-2.5,5.5, 11,HistPtBins);
    TotalEvts = new TH2F("TotalEvts","TotalEvts",8,-2.5,5.5, 11,HistPtBins);
    // x axis:
    // -2: all lights(0,1,2,3,4);
    // -1: greater than 5; 
    //  0: some datasets group 1,2,3 into 0, which is the case in my signal sample
    //1-5: Corresponding pdgid absolute value.
  }

  void FillJet(Jet& j) {
    if (!conf->IsMC) return;
    int had = abs(j.hadronFlavour);
    if (had > 5) had = -1; // Other particles
    if (had >= 0) TotalEvts->Fill(had,j.Pt());
    if (had >= 0 && had < 5) TotalEvts->Fill(-2,j.Pt());
    for (unsigned i = 0; i < 3; ++i) {
      if (j.bTagPasses[i]) {
        h_eff[i + 3]->Fill(had,j.Pt());
        if (had >= 0 && had < 5) h_eff[i + 3]->Fill(-2,j.Pt());
      }
    }
  }

  void PostProcess() {
    if (!conf->AuxHistCreation || !conf->IsMC) return;
    for (unsigned i = 0; i < 3; ++i) {
      h_eff[i] = (TH2F*) h_eff[i + 3]->Clone();
      h_eff[i]->Divide(TotalEvts);
    }
    // delete TotalEvts;
    f_eff->Write();
    f_eff->Save();
  }

  void ReadEffFile() {
    if (conf->AuxHistCreation || !conf->IsMC) return;
    f_eff = new TFile(FileName,"READ");
    cout << "Reading from bTagEff file " << FileName << endl;
    if (f_eff->IsZombie()) {
      cout << "Faild to read bTagEff file." <<endl;
      return;
    }
    h_eff.resize(6);
    h_eff[0] = (TH2F*) f_eff->Get("BtagEff_L");
    h_eff[1] = (TH2F*) f_eff->Get("BtagEff_M");
    h_eff[2] = (TH2F*) f_eff->Get("BtagEff_T");
    TotalEvts = (TH2F*) f_eff->Get("TotalEvts");
    h_eff[0]->SetDirectory(0);
    h_eff[1]->SetDirectory(0);
    h_eff[2]->SetDirectory(0);
    TotalEvts->SetDirectory(0);
    f_eff->Close();
    cout << "Done reading bTagEff file. Testing tight bTagging eff. at pT = 100 is ";
    cout << h_eff[2]->GetBinContent(h_eff[2]->FindBin(5,100)) <<endl;
  }

  vector<float> GetEff(Jet& j) { // This function is intented to be used by NanoAODReader 
    vector<float> out = {0.9,0.7,0.5};
    if (conf->AuxHistCreation || !conf->IsMC) {
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

struct bTagSFEntry {
// Since solely used by the following class, this struct is not meant to be placed in DataFormat.cc
// OperatingPoint,measurementType,sysType,jetFlavor,etaMin,etaMax,ptMin,ptMax,discrMin,discrMax,formula
  bTagSFEntry(vector<string> ent) {
    HasFunc = false;
    jetFlavor = stoi(ent[3]);
    etaMin = stof(ent[4]);
    etaMax = stof(ent[5]);
    ptMin = stof(ent[6]);
    ptMax = stof(ent[7]);
    discrMin = stof(ent[8]);
    discrMax = stof(ent[9]);
    formula = ent[10];
  };

  bool InRange(int flav, float eta, float pt, float dis = 1.0) {
    // lower edge included and upper edge excluded just like bins in root histograms.
    if (abs(flav) != jetFlavor) return false;
    if (fabs(eta) >= etaMax || fabs(eta) < etaMin) return false;
    if (pt >= ptMax || pt < ptMin) return false;
    // if (dis >= discrMax || dis < discrMin) return false;
    return true;
  }

  float GetScaleFactor(float pt) {
    if (!HasFunc) {
      func = new TF1("Func_bTagSF", formula.c_str(), ptMin, ptMax);
      HasFunc = true;
    }
    return func->Eval(pt);
  }


  bool HasFunc = false;;

  int jetFlavor;
  float etaMin;
  float etaMax;
  float ptMin;
  float ptMax;
  float discrMin;
  float discrMax;
  string formula;
  TF1* func;

};

class bTagSFReader {
public:
// OperatingPoint, measurementType, sysType, jetFlavor, etaMin, etaMax, ptMin, ptMax, discrMin, discrMax ,formula
  bTagSFReader(Configs* conf_) {
    conf = conf_;
    SFs = {{{},{},{}},{{},{},{}},{{},{},{}}};
  };

  void ReadCSVFile() {
    string basepath = "AUXFiles/BTV/";
    string filename = basepath + conf->SampleYear + "_wp_deepJet.csv";
    ifstream infile(filename);
    if (!infile) {
      cout << "Cannot read from file " << filename << endl;
      throw runtime_error("Cannot Read from file");
      return;
    }
    else cout << "Reading from bTag CSV file " << filename << endl;
    string line;
    getline(infile, line); // skipping the first line
    while (getline(infile, line)) {
      stringstream ss(line);
      vector<string> vs;
      while (ss.good()) {
        string subs;
        getline(ss, subs, ',');
        vs.push_back(subs);
      }
      int iv = -1;
      // incl are for light jets, and comb deals with c and b jets.
      if ((vs[1] == "incl" || vs[1] == "comb") && vs[2] == "central") iv = 0;
      else if ((vs[1] == "incl" || vs[1] == "comb") && vs[2] == "up") iv = 1;
      else if ((vs[1] == "incl" || vs[1] == "comb") && vs[2] == "down") iv = 2;
      else continue;
      int iwp = -1;
      if (vs[0] == "L") iwp = 0;
      else if (vs[0] == "M") iwp = 1;
      else if (vs[0] == "T") iwp = 2;
      else continue;
      // SFs[vs[1]][vs[2]][iwp] = bTagSFEntry(vs);
      SFs[iv][iwp].push_back(bTagSFEntry(vs));
    }
  }

  vector<vector<float> > GetScaleFactors(Jet& j) {
    vector<vector<float> > out = {{1,1,1},{1,1,1},{1,1,1}};
    for (unsigned iv = 0; iv < 3; ++iv) {
      for (unsigned iwp = 0; iwp < 3; ++iwp) {
        for (unsigned isf = 0; isf < SFs[iv][iwp].size(); ++isf) {
          if (SFs[iv][iwp][isf].InRange(j.hadronFlavour, j.Eta(), j.Pt())){
            out[iv][iwp] = SFs[iv][iwp][isf].GetScaleFactor(j.Pt());
            break;
          }
        }
      }
    }
    return out;
  }

  bool eq(float sf1, float sf2) {
    if (sf1 == sf2) return true;
    if (sf1 * sf2 == 0 && sf1 + sf2 == 1) return true;
    return false;
  }

  void CompareScaleFactors(Jet& j, vector<vector<float> > bTSFs) {
    vector<vector<float> > calc = GetScaleFactors(j);
    vector<bool> rep = {false, false, false};
    rep[0] = !(eq(bTSFs[0][0], calc[0][0]) && eq(bTSFs[1][0], calc[1][0]) && eq(bTSFs[2][0], calc[2][0]));
    rep[0] = !(eq(bTSFs[0][1], calc[0][1]) && eq(bTSFs[1][1], calc[1][1]) && eq(bTSFs[2][1], calc[2][1]));
    rep[0] = !(eq(bTSFs[0][2], calc[0][2]) && eq(bTSFs[1][2], calc[1][2]) && eq(bTSFs[2][2], calc[2][2]));
    // rep[0] = !(bTSFs[0][0] == calc[0][0] && bTSFs[1][0] == calc[1][0] && bTSFs[2][0] == calc[2][0]);
    // rep[1] = !(bTSFs[0][1] == calc[0][1] && bTSFs[1][1] == calc[1][1] && bTSFs[2][1] == calc[2][1]);
    // rep[2] = !(bTSFs[0][2] == calc[0][2] && bTSFs[1][2] == calc[1][2] && bTSFs[2][2] == calc[2][2]);
    if (rep[0] || rep[1] || rep[2]) cout << "bTSFs diff. Jet pT = " << j.Pt() << ", eta = " << j.Eta() << endl;
    for (unsigned iwp = 0; iwp < 3; ++iwp) {
      if (rep[iwp]) cout << Form("WP %i: %f(%f), Up %f(%f), Down %f(%f)",iwp, bTSFs[0][iwp], calc[0][iwp],bTSFs[1][iwp], calc[1][iwp],bTSFs[2][iwp], calc[2][iwp]) <<endl;
    }
  }

  Configs *conf;
  // map<string, map<string, map<int, bTagSFEntry> > > SFs; // [measurementType][sysType][iWP]
  vector< vector<vector<bTagSFEntry> > > SFs; // [nominal/ up/ down] [loose/ medium/ tight][index]


};

#endif