#ifndef ANALYZER_CC
#define ANALYZER_CC

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TString.h>

#include <iostream>
#include <string>

#include "NanoAODReader.cc"
// #include "Fitter.cc"

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
    bTE = new bTagEff(conf);
    // JS = new JetScale(conf);
    r->SetbTag(bTE);
    // Ftr = new Fitter(conf);
    // Ftr->SetJetScale(JS);
    // Ftr->SetbTag(bTE);
    if (conf->EntryMax > 0 && conf->EntryMax < r->GetEntries()) EntryMax = conf->EntryMax;
    else EntryMax = r->GetEntries();
    cout << "Processing " << EntryMax << " events" << endl;
    progress = new Progress(EntryMax, conf->ProgressInterval);
  }

  Long64_t GetEntryMax() {return EntryMax;}

  Long64_t GetEntries() {return r->GetEntries();}

  void SetEntryMax(Long64_t mx) {
    EntryMax = mx;
    progress->SetEntryMax(mx);
  }

  void SetOutput(string folder = "Validation") {
    string path = UserSpecifics::EOSBasePath + folder + "/";
    string subpath = Form("%s_%s/",conf->SampleYear.c_str(), conf->SampleType.c_str());
    string outname = Form("%s_%s_%i.root",conf->SampleYear.c_str(), conf->SampleType.c_str(), conf->iFile);
    if (conf->LocalOutput) {
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
    t->Branch("ExampleArray",&ExampleArray);
    return;
  }

  virtual void FillBranchContent() {
    for (unsigned i = 0; i < 10; ++i) {
      ExampleArray[i] = i;
    }
    return;
  }

  bool ReadEvent(Long64_t ievt) {
    if (conf->PrintProgress) progress->Print(ievt);
    iEvent = ievt;
    r->ReadEvent(ievt);
    return r->KeepEvent;
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
    if (conf->PrintProgress) progress->JobEnd();
  }

  Long64_t EntryMax;
  Long64_t iEvent;
  TFile *ofile;
  TTree *t;
  NanoAODReader *r;
  bTagEff *bTE;
  // JetScale *JS;
  // Fitter *Ftr;
  Progress* progress;

  std::array<int, 10> ExampleArray;

  Configs *conf;
  bool IsMC;

};

#endif
