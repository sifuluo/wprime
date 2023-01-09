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
    // Reimplementation of Analyzer to customize the branches to save
  };

  // int nPU;
  double nTrueInt;
  // int nPV;
  int nPVGood;
  double PUWeight;
  TH1F* nPVGoodBeforePUReweight, *nPVGoodAfterPUReweight;

  void BookBranches() {
    // t->Branch("PassedSelections",&PassedSelections);
    // t->Branch("EventScaleFactor",&EventScaleFactor);
    // // t->Branch("nPU", &nPU);
    // t->Branch("nTrueInt", &nTrueInt);
    // // t->Branch("nPV", &nPV);
    // t->Branch("nPVGood", &nPVGood);
    // t->Branch("PUweight", &PUWeight);
    nPVGoodBeforePUReweight = new TH1F("nPVGoodBeforePUReweight","nPVGoodBeforePUReweight", 99,0,99);
    nPVGoodAfterPUReweight = new TH1F("nPVGoodAfterPUReweight","nPVGoodAfterPUReweight", 99,0,99);
  }

  void FillBranchContent() {
    if (IsMC) {
      // nPU = r->Pileup_nPU;
      nTrueInt = r->Pileup_nTrueInt;
      PUWeight = GetEventPUWeight();
    }
    else {
      // nPU = 0;
      nTrueInt = 0;
      PUWeight = 1;
    }
    // nPV = r->PV_npvs;
    nPVGood = r->PV_npvsGood;
    nPVGoodBeforePUReweight->Fill(nPVGood, EventScaleFactor);
    nPVGoodAfterPUReweight->Fill(nPVGood, EventScaleFactor * PUWeight);
  }
};

void PileupDist(int isampleyear = 3, int isampletype = 3, int itrigger = 1, int ifile = 0) {
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->Debug = false;
  // conf->PUEvaluation = true;
  // conf->DASInput = true;
  conf->PrintProgress = true;
  // conf->FilesPerJob = 100;
  conf->SetSwitch("LocalOutput",true);
  // conf->InputFile = "All";
  ThisAnalysis *a = new ThisAnalysis(conf);
  a->SetOutput("PUEval");
  // a->SetEntryMax(10000);
  for (Long64_t iEvent = 0; iEvent < a->GetEntryMax(); ++iEvent) {
    bool failed = a->ReadEvent(iEvent);
    if (failed) continue;
    a->FillBranchContent();
  }
  a->SaveOutput();
  a->CloseOutput();
}
