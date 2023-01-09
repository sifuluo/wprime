#ifndef PUREWEIGHT_CC
#define PUREWEIGHT_CC

#include <vector>
#include <string>
#include <iostream>

#include <TFile.h>
#include <TH1.h>
#include <TString.h>

#include "Configs.cc"

// https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData#Recommended_cross_section
class PUReweight{
public:
  PUReweight(Configs *conf_) {
    conf = conf_;
    Init();
  };

  // Note in DATA, pileup is the total interactions, while in MC, pileup is the additional interactions
  // So nTrueInt + 1 = pileup
  void Init() {
    if (!conf->IsMC) return;
    string basepath = "/afs/cern.ch/work/s/siluo/wprime/PUReweight/";
    TString filename = basepath + conf->SampleYear + "_" + conf->SampleType + ".root";
    cout << "PUReweighting file: " << filename << endl;
    weights.clear();
    TFile* f = new TFile(filename);
    // Read once and close file to avoid congestion accessing the same file.
    for (unsigned ixsec = 0; ixsec < 4; ++ixsec) {
      TString weightname = Form("weight%i",ixsec);
      TH1D* weighthist = (TH1D*) f->Get(weightname);
      vector<double> weightstmp = vector<double>(98,1);
      for (unsigned i = 0; i < weightstmp.size(); ++i) {
        // bin x corresponds to pileup = x - 1, which again comparable to nTrueInt = x - 2 ;
        // For example if there is 1 additional interaction, nTrueInt = 1, pileup = 2 and bin = 3
        // weight histo bins range is [1,99], and the pileup range is [0,99) or [0,98], and nTrueInt range is [0,97]
        // Finally weight of nTrueInt of i is bin content of i + 2, with i range [0,97]
        weightstmp[i] = weighthist->GetBinContent(i + 2);
      }
      weights.push_back(weightstmp);
    }
    f->Close();
  }

  double GetWeight(int nTrueInt, int ixsec = 1) {
    if (!conf->IsMC) return 1.;
    if (nTrueInt > 97 || nTrueInt < 0) {
      cout << "PUReweight: nTrueInt = " << nTrueInt << ", out of range [0,97], using 1.0 as weight" << endl;
      return 1.;
    }
    return weights[ixsec][nTrueInt];
  }


  Configs *conf;
  vector< vector<double> > weights;
  TH1D* weight;
};


#endif
