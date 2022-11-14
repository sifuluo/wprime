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
    r = new NanoAODReader(conf);
    EntryMax = r->GetEntries();
    progress = new Progress(EntryMax, conf->ProgressInterval);
    if (conf->PUEvaluation) return;
    bt = new BTag(conf);
    r->SetBTagger(bt);
    sf = new ScaleFactor(r);
    if (!IsMC) datasel = new DataSelection(conf);
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
    if (conf->GetSwitch("LocalOutput")) {
      path = "";
      subpath = "outputs/";
    }
    TString ofname = path + subpath + outname;
    ofile = new TFile(ofname,"RECREATE");
    cout << "Output will be saved to " << ofname << endl;
    ofile->cd();
    t = new TTree("t","EventTree");
    BookBranches();
    evtCounter = new TH1F("EventCounter","Event Counter", 10, 0.5, 10.5);
    const char *evtCounterLabel[20] = {"All", "Lumi Sec", "Trigger", "1 Lep", "5J", "2BJ"};
  }

  int ReadEvent(Long64_t ievt) {
    iEvent = ievt;
    evtCounter->Fill("All",1);
    r->ReadEvent(ievt);
    if (conf->PrintProgress) progress->Print(ievt);
    if (conf->PUEvaluation) {
      evtCounter->Fill("Passed",1);
      FillBranchContent();
      return 0;
    }
    BaseLineSelections();
    FillEventCounter();
    if (PassedSelections) {
      evtCounter->Fill("Passed Basic",1);
      GetEventScaleFactor();
      return 0;
    }
    else {
      evtCounter->Fill("Failed",1);
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

  bool GetDataSelection() {
    if (IsMC) return true;
    else return datasel->GetDataSelection(r->run, r->luminosityBlock);
  }

  virtual bool ObjectsRequirement() {
    bool ob = (Leptons().size() == 1);
    ob = ob && (Jets().size() > 4);
    return ob;
  }

  bool BaseLineSelections() {
    PassedSelections = true;
    PassedSelections &= TriggerSelection();
    PassedSelections &= GetDataSelection();
    PassedSelections &= r->ReadMETFilterStatus();
    PassedSelections &= ObjectsRequirement();
    return PassedSelections;
  }

  double GetEventScaleFactor() {
    if (IsMC) EventScaleFactor = sf->CalcEventSF();
    else EventScaleFactor = -1.;
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

  // void SuccessFlag() {
  //   if (iEvent < EntryMax - 1) return;
  //   string basepath = "/afs/cern.ch/work/s/siluo/wprime/SuccessFlags/";
  //   string sucf = Form("%i_%i_%i_%i.txt",conf->iSampleYear, conf->iSampleType, conf->iTrigger, conf->iFile);
  //   string flag = basepath + sucf;
  //   ofstream suc(flag);
  //   suc << "Completed\n";
  //   progress->JobEnd();
  // }

  // Accessors
  vector<GenPart>& GenParts() {return r->GenParts;}
  vector<GenJet>& GenJets() {return r->GenJets;}
  vector<Jet>& AllJets() {return r->AllJets;}
  vector<Jet>& Jets() {return r->Jets;}
  vector<Lepton>& Leptons() {return r->Leptons;}
  vector<Electron>& Electrons() {return r->Electrons;}
  vector<Muon>& Muons() {return r->Muons;}
  GenMET& GenMet() {return r->GenMet;}
  MET& Met() {return r->Met;}

  vector< vector<int> >& BJets() {return r->BJets;}
  vector< vector<int> >& NBJets() {return r->NBJets;}
  vector<int>& BJets_Loose(){return r->BJets[0];}
  vector<int>& BJets_Medium(){return r->BJets[1];}
  vector<int>& BJets_Tight(){return r->BJets[2];}
  vector<int>& NBJets_Loose(){return r->NBJets[0];}
  vector<int>& NBJets_Medium(){return r->NBJets[1];}
  vector<int>& NBJets_Tight(){return r->NBJets[2];}


  virtual void BookBranches() {
    t->Branch("PassedSelections",&PassedSelections);
    t->Branch("EventScaleFactor",&EventScaleFactor);
    return;
  }

  virtual void FillBranchContent() {
    return;
  }

  virtual void FillEventCounter() {
    if (!GetDataSelection()) return;
    evtCounter->Fill("Lumi Sec",1);
    if (!TriggerSelection()) return;
    evtCounter->Fill("Trigger",1);
    if (Leptons().size() != 1) return;
    evtCounter->Fill("1 Lep",1);
    return;
  }

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
  bool IsMC;
  int PassedSelections;
  float EventScaleFactor;

};

#endif