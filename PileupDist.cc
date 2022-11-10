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
#include "Utilities/NanoAODReader.cc"
#include "Utilities/DataFormat.cc"
#include "Utilities/BTag.cc"
#include "Utilities/ScaleFactor.cc"
#include "Utilities/ProgressBar.cc"

class ThisAnalysis : public Analyzer {
public:
  ThisAnalysis(Configs *conf_) : Analyzer(conf_) {
    // Reimplementation of Analyzer to customize the branches to save
  };

  int nPU;
  double nTrueInt;
  int nPV, nPVGood;

  void BookBranches() {
    // t->Branch("PassedSelections",&PassedSelections);
    // t->Branch("EventScaleFactor",&EventScaleFactor);
    t->Branch("nPU", &nPU);
    t->Branch("nTrueInt", &nTrueInt);
    t->Branch("nPV", &nPV);
    t->Branch("nPVGood", &nPVGood);
  }

  void BranchContent() {
    if (IsMC) {
      nPU = r->Pileup_nPU;
      nTrueInt = r->Pileup_nTrueInt;
    }
    else {
      nPU = 0;
      nTrueInt = 0;
    }
    nPV = r->PV_npvs;
    nPVGood = r->PV_npvsGood;
  }
};

void PileupDist(int isampleyear = 1, int isampletype = 2, int itrigger = 0, int ifile = 0) {
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->Debug = false;
  conf->PUEvaluation = true;
  conf->DASInput = true;
  conf->PrintProgress = true;
  conf->InputFile = "ttbar_2016.root";
  ThisAnalysis *a = new ThisAnalysis(conf);
  a->SetOutput("PUEval");
  // a->SetEntryMax(10000);
  for (Long64_t iEvent = 0; iEvent < a->GetEntryMax(); ++iEvent) {
    bool pass = a->ReadEvent(iEvent);
    // if (pass) continue;
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
  a->SuccessFlag();
}
