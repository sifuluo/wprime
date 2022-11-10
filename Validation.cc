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

class ThisAnalysis : public Analyzer {
public:
  ThisAnalysis(Configs *conf_)
  : Analyzer(conf_) {
    // Reimplementation of Analyzer to customize the branches to save
  };

  double LeptonPt;
  double LeptonEta;
  vector<double> *JetPt, *JetEta;
  int nJets;
  vector<int> *nBJets, *nNBJets; // {loose, medium, tight}
  double METPt;
  int nPU;
  double nTrueInt;
  int nPV, nPVGood;

  double CloseJetPt, CloseJetdR;
  int Triggered;
  int LepType;

  vector<double>* WPrimeMassSimple;

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
    nBJets = new vector<int>;
    nNBJets = new vector<int>;
    t->Branch("nBJets",nBJets);
    t->Branch("nNBJets",nNBJets);
    t->Branch("METPt",&METPt);

    t->Branch("nPU", &nPU);
    t->Branch("nTrueInt", &nTrueInt);

    t->Branch("nPV", &nPV);
    t->Branch("nPVGood", &nPVGood);

    t->Branch("CloseJetPt", &CloseJetPt);
    t->Branch("CloseJetdR", &CloseJetdR);
    t->Branch("LepType", &LepType);
    t->Branch("Triggered", &Triggered);
    WPrimeMassSimple = new vector<double>;
    t->Branch("WPrimeMassSimple", &WPrimeMassSimple);
  }

  void PreSelContent() {
    CloseJetPt = 0;
    CloseJetdR = 0;
    Triggered = 0;
    LepType = -1;
    Triggered = TriggerSelection();
    if (r->Electrons.size() == 1 && r->Muons.size() == 0) LepType = 0;
    else LepType = 1;
    CloseJetdR = 100;
    for (Jet j : r->Jets) {
      double dr = j.DeltaR(r->Leptons[0]);
      if (dr < CloseJetdR) {
        CloseJetPt = j.Pt();
        CloseJetdR = dr;
      }
    }
  }

  void FillBranchContent() {
    LeptonPt = 0;
    LeptonEta = 0;
    LeptonPt = r->Leptons[0].Pt();
    LeptonEta = r->Leptons[0].Eta();

    JetPt->clear();
    JetEta->clear();
    for (Jet j : r->Jets) {
      JetPt->push_back(j.Pt());
      JetEta->push_back(j.Eta());
    }

    nJets = 0;
    nJets =  r->Jets.size();

    nBJets->clear();
    nNBJets->clear();
    for (unsigned i = 0; i < 3; ++i) {
      nBJets->push_back(r->BJets[i].size());
      nNBJets->push_back(r->NBJets[i].size());
    }

    METPt = 0;
    METPt = r->Met.Pt();

    nPU = 0;
    nTrueInt = 0;
    if (IsMC) {
      nPU = r->Pileup_nPU;
      nTrueInt = r->Pileup_nTrueInt;
    }
    else {
      nPU = 0;
      nTrueInt = 0;
    }

    nPV = 0;
    nPVGood = 0;
    nPV = r->PV_npvs;
    nPVGood = r->PV_npvsGood;

    WPrimeLVSimple.clear();
    WPrimeMassSimple->clear();

    RecoWPrimeSimpleAll();
  }

  void FillEventCounter() {
    if (!GetDataSelection()) return;
    evtCounter->Fill("Lumi Sec",1);
    if (!TriggerSelection()) return;
    evtCounter->Fill("Trigger",1);
    if (Leptons().size() != 1) return;
    evtCounter->Fill("1Lep",1);
    if (Jets().size() < 5) return;
    evtCounter->Fill("5J",1);
    return;
  }

  bool ObjectsRequirement() {
    bool ob = (Leptons().size() == 1);
    ob = ob && (Jets().size() > 4);
    ob = ob && (NBJets_Loose().size() > 1);
    return ob;
  }

  bool Exception() {
    bool out = true;
    out &= !TriggerSelection(); // Trigger not triggered;
    out &= (r->Electrons.size() == 1 && r->Muons.size() == 0 && conf->iTrigger == 0) || (r->Electrons.size() == 0 && r->Muons.size() == 1 && conf->iTrigger == 1);
    return out;
  }


  vector<TLorentzVector> WPrimeLVSimple;

  void RecoWPrimeSimple(int btagwp) {
    unsigned lj1(0), lj2(0), had_b(0);
    TLorentzVector had_w, had_t;
    had_w.SetXYZM(0,0,0,0);
    had_t.SetXYZM(0,0,0,0);
    for (unsigned i = 0 ; i < r->Jets.size(); ++i) {
      if (r->Jets[i].btags[btagwp]) continue;
      for (unsigned j = i + 1; j < r->Jets.size(); ++j) {
        if (r->Jets[j].btags[btagwp]) continue;
        TLorentzVector tmpw = r->Jets[i] + r->Jets[j];
        if (fabs(tmpw.M() - 80.4) < fabs(had_w.M() - 80.4)) {
          lj1 = i;
          lj2 = j;
          had_w = tmpw;
        }
      }
    }
    // HadW determined;

    for (unsigned i = 0; i < r->Jets.size(); ++i) {
      if (i == lj1 || i == lj2) continue;
      TLorentzVector tmpt = had_w + r->Jets[i];
      if (fabs(tmpt.M() - 172.76) < fabs(had_t.M() - 172.76)) {
        had_b = i;
        had_t = tmpt;
      }
    }
    // HadT determined;

    for (unsigned i = 0; i < r->Jets.size(); ++i) {
      if (i == lj1 || i == lj2 || i == had_b ) continue;
      WPrimeLVSimple[btagwp] = had_t + r->Jets[i];
      WPrimeMassSimple->at(btagwp) = WPrimeLVSimple[btagwp].M();
      return;
    }
  }

  void RecoWPrimeSimpleAll() {
    //Check jet pt ordering
    for (unsigned i = 0; i < r->Jets.size(); ++i) {
      if (i > 0) {
        if (r->Jets[i].Pt() > r->Jets[i-1].Pt() + 0.1) {
          cout << "Jets are not properly ordered by Pt:" << endl;
          cout <<" Jet index = " << i-1 << " : " << r->Jets[i-1].PrintPtEtaPhiM() << endl;
          cout <<" Jet index = " << i << " : " << r->Jets[i].PrintPtEtaPhiM() << endl;
        }
      }
    }
    WPrimeLVSimple.resize(3);
    WPrimeMassSimple->resize(3);
    for (unsigned i = 0; i < 3; ++i) RecoWPrimeSimple(i);
  }
};

void Validation(int isampleyear = 3, int isampletype = 16, int itrigger = 0, int ifile = -1, bool test_ = false) {
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->Debug = false;
  conf->PUEvaluation = false;
  conf->DASInput = false;
  if (test_) { // TestRun
    conf->SetSwitch("LocalOutput",true);
    conf->PrintProgress = true;
    conf->ProgressInterval = 1;
  }
  if (ifile < 0) {
    if (conf->iSampleYear == 1 && conf->iSampleType == 0) {
      conf->InputFile = "/eos/cms/store/data/Run2016F/SingleElectron/NANOAOD/UL2016_MiniAODv2_NanoAODv9-v1/130000/31BADB98-BCD8-4B40-9825-7709B08299BD.root";
    }
    else if (conf->iSampleYear == 3 && conf->iSampleType == 16) {
      conf->InputFile = "FL500.root";
    }
    else if (conf->iSampleYear == 3 && conf->iSampleType == 1) {
      conf->InputFile = "SingleMuon_2018.root";
    }
    else if (conf->iSampleYear == 3 && conf->iSampleType == 2) {
      conf->InputFile = "ttbar_2018.root";
    }
    else {
      cout << "Test File not specified for SampleYear = " << conf->SampleYear << ", SampleType = " << conf->SampleType <<endl;
    }
  }
  ThisAnalysis *a = new ThisAnalysis(conf);
  a->SetOutput("Validation");
  if (ifile < 0) a->SetEntryMax(10000);
  for (Long64_t iEvent = 0; iEvent < a->GetEntryMax(); ++iEvent) {
    bool failed = a->ReadEvent(iEvent);
    // bool except = a->Exception();
    // if (failed && !except) continue;
    if (failed) continue;
    // a->PreSelContent();
    a->FillBranchContent();
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
  // a->SuccessFlag();
}
