#ifndef ANALYZER_CC
#define ANALYZER_CC
// Central code that reads from NanoAODReader and conduct further (time consuming) processing such as fitting to hypothesis etc.
// Derived class of this class is suggested to define analysis functions within the class body in respective analysis.
// So in this code, there is only fundamental functions that would be needed by all processed

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TString.h>

#include <iostream>
#include <string>

#include "NanoAODReader.cc"
#include "Fitter.cc"

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
    r->SetbTag(bTE);
    if (conf->RunFitter) {
      JS = new JetScale(conf);
      PM = new Permutations(conf);
      Ftr = new Fitter(conf);
      Ftr->SetJetScale(JS);
      Ftr->SetPermutation(PM);
    }
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

  bool HasZombieOutput(TString ofname) {
    TFile *ftest = new TFile(ofname,"READ");
    bool IsZombie = false;
    if (ftest->IsZombie()) IsZombie = true;
    ftest->Close();
    delete ftest;
    return IsZombie;
  }

  bool SetOutput(string folder = "Validation") {
    string path = UserSpecifics::EOSBasePath + folder + "/";
    string subpath = Form("%s_%s/",conf->SampleYear.c_str(), conf->SampleType.c_str());
    string outname = Form("%s_%s_%i.root",conf->SampleYear.c_str(), conf->SampleType.c_str(), conf->iFile);
    if (conf->LocalOutput) {
      path = "";
      subpath = "outputs/";
    }
    TString ofname = path + subpath + outname;
    cout << "Output will be saved to " << ofname << endl;
    cout << "Output Status: Zombie: " << (HasZombieOutput(ofname)? "YES" : "No") << ", FirstRun: " << ((conf->FirstRun) ?  "YES" : "No") << ", NeedRerun: " << ((conf->NeedRerun) ?  "YES" : "No") << endl;
    if (!(HasZombieOutput(ofname)) && !(conf->FirstRun) && !(conf->NeedRerun)) {
      cout << "File skipped. Ending job" << endl;
      return false;
    }
    ofile = new TFile(ofname,"RECREATE");
    ofile->cd();
    t = new TTree("t","EventTree");
    BookBranches();
    return true;
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

  int ReadEvent(Long64_t ievt) {
    if (conf->PrintProgress) progress->Print(ievt);
    iEvent = ievt;
    return r->ReadEvent(ievt);
  }

  bool WithinROI() {
    return r->RegionAssociations.HasValidRegions(conf->AcceptedRegions);
  }

  void FillTree() {
    t->Fill();
  }

  void SetEventFitter(int iregion = 0) {
    if (!conf->RunFitter) return;
    Ftr->SetJets(r->Jets, iregion);
    Ftr->SetLep(r->TheLepton, iregion);
    Ftr->SetMET(r->Met, iregion);
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
  JetScale *JS;
  Permutations *PM;
  Fitter *Ftr;
  Progress* progress;
  

  std::array<int, 10> ExampleArray;

  Configs *conf;
  bool IsMC;

};

#endif
