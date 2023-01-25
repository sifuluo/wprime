#ifndef ANALYZER_CC
#define ANALYZER_CC

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TString.h>

#include <iostream>
#include <string>

//#include "DataFormat.cc"
#include "NanoAODReader.cc"
// #include "BTag.cc"
#include "Constants.cc"
#include "ProgressBar.cc"
// #include "ScaleFactor.cc"
#include "DataSelection.cc"
#include "PUReweight.cc"
#include "UserSpecifics.cc"

using namespace std;

class Analyzer{
public:
  Analyzer(Configs *conf_) {
    conf = conf_;
    IsMC = (conf->IsMC);
    Init();
  };

  ~Analyzer() {
    cout << "Analyzer Destructed" << endl;
  }
  void Init() {
    r = new NanoAODReader(conf);
    if (conf->EntryMax > 0 && conf->EntryMax < r->GetEntries()) EntryMax = conf->EntryMax;
    else EntryMax = r->GetEntries();
    cout << "Processing " << EntryMax << " events" << endl;
    progress = new Progress(EntryMax, conf->ProgressInterval);
    if (IsMC) pureweight = new PUReweight(conf);
  }

  Long64_t GetEntryMax() {return EntryMax;}

  Long64_t GetEntries() {return r->GetEntries();}

  void SetEntryMax(Long64_t mx) {
    EntryMax = mx;
    progress->SetEntryMax(mx);
  }

  void SetOutput(string folder = "Validation") {
    string path = UserSpecifics::EOSBasePath + folder + "/"; //FIXME: Needs to be propagated from elsewhere
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
  }

  virtual void BookBranches() {
    return;
  }

  virtual void FillBranchContent() {
    return;
  }

  void ReadEvent(Long64_t ievt) {
    iEvent = ievt;
    r->ReadEvent(ievt);
  }

  double GetEventPUWeight(int ixsec = 1) {
    if (!IsMC) return 1.;
    else return pureweight->GetWeight(r->Pileup_nTrueInt, ixsec);
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

  Long64_t EntryMax;
  Long64_t iEvent;
  TFile *ofile;
  TTree *t;
  NanoAODReader *r;
  Progress* progress;
  PUReweight* pureweight;

  Configs *conf;
  bool IsMC;

};

#endif
