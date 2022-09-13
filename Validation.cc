// ROOT
// #include <TROOT.h>
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

class ValidationAna : public Analyzer {
public:
  ValidationAna(Configs *conf_)
  : Analyzer(conf_) {
    // Reimplementation of Analyzer to customize the branches to save
  };

  double LeptonPt;
  double LeptonEta;
  vector<double> *JetPt;
  vector<double> *JetEta;
  int nJets;
  double METPt;
  int nPU;
  double nTrueInt;
  int nPV, nPVGood;

  void BookBranches() {
    t->Branch("PassedSelections",&PassedSelections);
    t->Branch("EventScaleFactor",&EventScaleFactor);
    t->Branch("LeptonPt",&LeptonPt);
    t->Branch("LeptonEta",&LeptonEta);
    JetPt = new vector<double>;
    t->Branch("JetPt",&JetPt);
    JetEta = new vector<double>;
    t->Branch("JetEta",&JetEta);
    t->Branch("nJets",&nJets);
    t->Branch("METPt",&METPt);

    t->Branch("nPU", &nPU);
    t->Branch("nTrueInt", &nTrueInt);

    t->Branch("nPV", &nPV);
    t->Branch("nPVGood", &nPVGood);
  }

  void BranchContent() {
    LeptonPt = r->Leptons[0].Pt();
    LeptonEta = r->Leptons[0].Eta();
    JetPt->clear();
    JetEta->clear();
    for (Jet j : r->Jets) {
      JetPt->push_back(j.Pt());
      JetEta->push_back(j.Eta());
    }
    nJets =  r->Jets.size();
    METPt = r->Met.Pt();

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

void Validation(int isampleyear = 1, int isampletype = 0, int itrigger = 0, int ifile = 0) {
  // Indices details see Utilities/Constants.cc
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->Debug = false;
  conf->PUEvaluation = false;
  conf->DASInput = false;
  // cout << endl <<"0" <<endl;
  Analyzer *a = new Analyzer(conf);
  // cout <<endl <<"1" <<endl;
  a->SetOutput("Validation");
  // cout << endl << "2" <<endl;
  a->SetEntryMax(10000);
  for (Long64_t iEvent = 0; iEvent < a->GetEntryMax(); ++iEvent) {
    bool pass = a->ReadEvent(iEvent);
    if (pass) continue;
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
  a->SuccessFlag();
}
