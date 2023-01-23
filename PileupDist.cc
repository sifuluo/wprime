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
  vector<double> *PUWeight;
  TH1F* nPVGoodBeforePUReweight;
  vector<TH1F*> nPVGoodAfterPUReweight;

  void BookBranches() {
    // t->Branch("PassedSelections",&PassedSelections);
    t->Branch("EventScaleFactor",&EventScaleFactor);
    // // t->Branch("nPU", &nPU);
    // t->Branch("nTrueInt", &nTrueInt);
    // // t->Branch("nPV", &nPV);
    t->Branch("nPVGood", &nPVGood);
    PUWeight = new vector<double>;
    t->Branch("PUweight", &PUWeight);
    nPVGoodBeforePUReweight = new TH1F("nPVGoodBeforePUReweight","nPVGoodBeforePUReweight", 99,0,99);
    nPVGoodAfterPUReweight.clear();
    nPVGoodAfterPUReweight.push_back(new TH1F("nPVGoodAfterPUReweight0","nPVGoodAfterPUReweight0", 99,0,99));
    nPVGoodAfterPUReweight.push_back(new TH1F("nPVGoodAfterPUReweight1","nPVGoodAfterPUReweight1", 99,0,99));
    nPVGoodAfterPUReweight.push_back(new TH1F("nPVGoodAfterPUReweight2","nPVGoodAfterPUReweight2", 99,0,99));
    nPVGoodAfterPUReweight.push_back(new TH1F("nPVGoodAfterPUReweight3","nPVGoodAfterPUReweight3", 99,0,99));
  }

  void FillEventCounter() {
    return;
  }

  void FillBranchContent() {
    nTrueInt = 0;
    *PUWeight = vector<double>(4,1);
    // nPV = r->PV_npvs;
    nPVGood = r->PV_npvsGood;
    nPVGoodBeforePUReweight->Fill(nPVGood, EventScaleFactor);
    if (IsMC) {
      // nPU = r->Pileup_nPU;
      nTrueInt = r->Pileup_nTrueInt;
      for (unsigned i = 0; i < 4; ++i) {
        PUWeight->at(i) = GetEventPUWeight(i);
        nPVGoodAfterPUReweight.at(i)->Fill(nPVGood, EventScaleFactor * PUWeight->at(i));
      }
    }
  }
};

void PileupDist(int isampleyear = 3, int isampletype = 2, int itrigger = 1, int ifile = 1) {
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->Debug = false;
  // conf->PUEvaluation = true;
  // conf->DASInput = true;
  conf->PrintProgress = true;
  // conf->FilesPerJob = 100;
  // conf->EntryMax = 10000;
  conf->SetSwitch("LocalOutput",true);
  // conf->FilesPerJob = 20;
  // conf->InputFile = "All";
  ThisAnalysis *a = new ThisAnalysis(conf);
  a->SetOutput("PUEval");
  for (Long64_t iEvent = 6000; iEvent < a->GetEntryMax(); ++iEvent) {
    bool failed = a->ReadEvent(iEvent);
    if (failed) continue;
    cout << "Event passed" <<endl;
    a->FillBranchContent();
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
}
