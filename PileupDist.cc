#include <TString.h>
// #include <TVector2.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
// #include <TFitResult.h>
// #include <TClonesArray.h>
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TProfile.h>
#include <TEfficiency.h>


// std
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>

#include "Utilities/Analyzer.cc"

class ThisAnalysis : public Analyzer {
public:
  ThisAnalysis(Configs *conf_) : Analyzer(conf_) {
  };

  std::array<int,9> RegionIdentifier;
  std::array<float,18> EventWeight;
  std::array<float,7> PUWeight;

  double nTrueInt;
  int nPVGood;

  void BookBranches() {
    t->Branch("RegionIdentifier", &RegionIdentifier);
    t->Branch("EventWeight", &EventWeight);
    t->Branch("PUWeight", &PUWeight);
    
    t->Branch("nTrueInt", &nTrueInt);
    t->Branch("nPVGood", &nPVGood);
  }

  void FillBranchContent() {
    nTrueInt = 0;
    nPVGood = r->PV_npvsGood;

    for (unsigned i = 0; i < r-> RegionAssociations.RegionCount; ++i) {
      RegionIdentifier[i] = r->RegionAssociations.Regions[i];
    }

    for (unsigned i = 0; i < r->EventWeights.size(); ++i) {
      EventWeight[i] = r->EventWeights[i].first;
    }

    if (IsMC) {
      nTrueInt = r->Pileup_nTrueInt;
      PUWeight[0] = r->evts->Pileup_scaleFactor;
      PUWeight[1] = r->evts->Pileup_scaleFactorUp;
      PUWeight[2] = r->evts->Pileup_scaleFactorDown;
      for (unsigned i = 0; i < 4; ++i) {
        PUWeight[i + 3] = GetEventPUWeight(i);
      }
    }
  }
};

void PileupDist(int isampleyear = 3, int isampletype = 2, int ifile = 0, int nfile = 1) {
  int itrigger = 1;
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->Debug = false;
  // conf->PUEvaluation = true;
  // conf->DASInput = true;
  conf->PrintProgress = true;
  conf->FilesPerJob = nfile;
  // conf->EntryMax = 10000;
  conf->SetSwitch("LocalOutput",true);
  // conf->InputFile = "All";
  ThisAnalysis *a = new ThisAnalysis(conf);
  a->SetOutput("PUEval");
  for (Long64_t iEvent = 0; iEvent < a->GetEntryMax(); ++iEvent) {
    bool KeepEvent = a->ReadEvent(iEvent);
    if (!KeepEvent) continue;
    a->FillBranchContent();
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
}
