#ifndef ANALYZER_CC
#define ANALYZER_CC

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TString.h>

#include <iostream>
#include <string>

#include "DataFormat.cc"
#include "NanoAODReader.cc"
#include "BTag.cc"
#include "Constants.cc"
#include "ProgressBar.cc"
#include "ScaleFactor.cc"
#include "DataSelection.cc"
// #include "PUReweight.cc"

using namespace std;

class Analyzer{
public:
  Analyzer(Configs *conf_) {
    // iSampleYear = isy_;
    // iSampleType = ist_;
    // iTrigger = itr_;
    // iFile = ifile_;
    // debug = dbg_;
    // SampleYear = Constants::SampleYears[isy_];
    // SampleType = Constants::SampleTypes[ist_];
    // Trigger = Constants::Triggers[itr_];
    conf = conf_;
    IsMC = (conf->iSampleType > 1);
    Init();
  };

  void Init() {
    cout <<"**1**" <<endl;
    r = new NanoAODReader(conf);
    cout <<"**2**" <<endl;
    bt = new BTag(conf);
    cout <<"**3**" <<endl;
    r->SetBTagger(bt);
    cout <<"**4**" <<endl;
    sf = new ScaleFactor(r);
    EntryMax = r->GetEntries();
    if (!IsMC) datasel = new DataSelection(conf);
    progress = new Progress(EntryMax, 1000);
    // pureweight = new PUReweight(iSampleYear);
  }

  Long64_t GetEntryMax() {return EntryMax;}

  Long64_t GetEntries() {return r->GetEntries();}

  void SetEntryMax(Long64_t mx) {
    EntryMax = mx;
    progress->SetEntryMax(mx);
  }

  void SetOutput(string folder = "Validation") {
    string path = "/eos/user/s/siluo/WPrimeAnalysis/" + folder + "/";
    string subpath = Form("%s_%s_%s/",conf->SampleYear.c_str(), conf->SampleType.c_str(), conf->Trigger.c_str());
    string outname = Form("%s_%s_%s_%i.root",conf->SampleYear.c_str(), conf->SampleType.c_str(), conf->Trigger.c_str(), conf->iFile);
    if (conf->Debug) {
      path = "";
      outname = "out.root";
    }
    TString ofname = path + subpath + outname;
    ofile = new TFile(ofname,"RECREATE");
    ofile->cd();
    t = new TTree("t","EventTree");
    BookBranches();
    evtCounter = new TH1F("EventCounter","Event Counter", 10, -0.5, 9.5);
  }

  int ReadEvent(Long64_t ievt) {
    iEvent = ievt;
    evtCounter->Fill(0);
    r->ReadEvent(ievt);
    progress->Print(ievt);
    BaseLineSelections();
    if (GetDataSelection()) {
      evtCounter->Fill(1);
      if (TriggerSelection()) {
        evtCounter->Fill(2);
        if (TopologySelection()) {
          evtCounter->Fill(3);
        }
      }
    }
    if (PassedSelections || conf->PUEvaluation) {
      evtCounter->Fill(5);
      GetEventScaleFactor();
      BranchContent();
      return 0;
    }
    else {
      evtCounter->Fill(4);
      return -1;
    }
  }

  bool TriggerSelection() {
    if (conf->iTrigger == 0) return r->isolated_electron_trigger;
    else if (conf->iTrigger == 1) return (r->isolated_muon_trigger || r->isolated_muon_track_trigger);
    else {
      cout << "Ineligeble iTrigger: " << conf->iTrigger << endl;
      return false;
    }
  }

  bool TopologySelection() {
    return (r->Jets.size() >=5 && r->Leptons.size() == 1 && r->N_BJets >= 2);
  }

  bool GetDataSelection() {
    if (IsMC) return true;
    else return datasel->GetDataSelection(r->run, r->luminosityBlock);
  }

  bool BaseLineSelections() {
    PassedSelections = true;
    PassedSelections &= TriggerSelection();
    PassedSelections &= TopologySelection();
    PassedSelections &= GetDataSelection();
    return PassedSelections;
  }

  double GetEventScaleFactor() {
    if (IsMC) EventScaleFactor = sf->CalcEventSF();
    else EventScaleFactor = 1.;
    return EventScaleFactor;
  }

  void FillTree() {
    t->Fill();
  }

  void SaveOutput() {
    ofile->Write();
    ofile->Save();
  }

  void CloseOutput() {
    ofile->Close();
  }

  void SuccessFlag() {
    if (iEvent < EntryMax - 1) return;
    string basepath = "/afs/cern.ch/work/s/siluo/wprime/SuccessFlags/";
    string sucf = Form("%i_%i_%i_%i.txt",conf->iSampleYear, conf->iSampleType, conf->iTrigger, conf->iFile);
    string flag = basepath + sucf;
    ofstream suc(flag);
    suc << "Completed\n";
  }

  virtual void BookBranches() {
    t->Branch("PassedSelections",&PassedSelections);
    t->Branch("EventScaleFactor",&EventScaleFactor);
    return;
  }

  virtual void BranchContent() {
    return;
  }

  // void BookBranches_PUEval() {
  //   t->Branch("PassedSelections",&PassedSelections);
  //   t->Branch("EventScaleFactor",&EventScaleFactor);
  //   t->Branch("nPU", &nPU);
  //   t->Branch("nTrueInt", &nTrueInt);
  //   t->Branch("nPV", &nPV);
  //   t->Branch("nPVGood", &nPVGood);
  // }
  //
  // void BranchContent_PUEval() {
  //   if (IsMC) {
  //     nPU = r->Pileup_nPU;
  //     nTrueInt = r->Pileup_nTrueInt;
  //   }
  //   else {
  //     nPU = 0;
  //     nTrueInt = 0;
  //   }
  //   nPV = r->PV_npvs;
  //   nPVGood = r->PV_npvsGood;
  // }
  //
  // void BookBranches_Validation() {
  //   t->Branch("PassedSelections",&PassedSelections);
  //   t->Branch("EventScaleFactor",&EventScaleFactor);
  //   t->Branch("LeptonPt",&LeptonPt);
  //   t->Branch("LeptonEta",&LeptonEta);
  //   JetPt = new vector<double>;
  //   t->Branch("JetPt",&JetPt);
  //   JetEta = new vector<double>;
  //   t->Branch("JetEta",&JetEta);
  //   t->Branch("nJets",&nJets);
  //   t->Branch("METPt",&METPt);
  //
  //   t->Branch("PUWeight", &PUWeight);
  //   t->Branch("nPU", &nPU);
  //   t->Branch("nTrueInt", &nTrueInt);
  //
  //   t->Branch("nPV", &nPV);
  //   t->Branch("nPVGood", &nPVGood);
  // }
  //
  // void BranchContent_Validation() {
  //   LeptonPt = r->Leptons[0].Pt();
  //   LeptonEta = r->Leptons[0].Eta();
  //   JetPt->clear();
  //   JetEta->clear();
  //   for (Jet j : r->Jets) {
  //     JetPt->push_back(j.Pt());
  //     JetEta->push_back(j.Eta());
  //   }
  //   nJets =  r->Jets.size();
  //   METPt = r->Met.Pt();
  //
  //   if (IsMC) {
  //     nPU = r->Pileup_nPU;
  //     nTrueInt = r->Pileup_nTrueInt;
  //     // PUWeight = pureweight->GetWeight(nPU);
  //   }
  //   else {
  //     nPU = 0;
  //     nTrueInt = 0;
  //     PUWeight = 1.;
  //   }
  //   nPV = r->PV_npvs;
  //   nPVGood = r->PV_npvsGood;
  //
  // }

  // double LeptonPt;
  // double LeptonEta;
  // vector<double> *JetPt;
  // vector<double> *JetEta;
  // int nJets;
  // double METPt;
  // double PUWeight;
  // int nPU;
  // double nTrueInt;
  // int nPV, nPVGood;


  Long64_t EntryMax;
  Long64_t iEvent;
  TFile *ofile;
  TTree *t;
  TH1F* evtCounter;
  NanoAODReader *r;
  BTag* bt;
  ScaleFactor *sf;
  DataSelection *datasel;
  Progress* progress;
  // PUReweight* pureweight;

  Configs *conf;
  // int iSampleYear, iSampleType, iTrigger, iFile;
  // string SampleYear, SampleType, Trigger;
  bool IsMC;
  int PassedSelections;
  float EventScaleFactor;

};

#endif
