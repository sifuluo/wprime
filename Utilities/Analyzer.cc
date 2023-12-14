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
    if (conf->EntriesMax > 0 && conf->EntriesMax < r->GetEntriesMax()) EntriesMax = conf->EntriesMax;
    else EntriesMax = r->GetEntriesMax();
    cout << "Processing " << EntriesMax << " events" << endl;
    progress = new Progress(EntriesMax, conf->ProgressInterval);
  }

  Long64_t GetEntriesMax() {return EntriesMax;}

  Long64_t GetEntries() {return r->GetEntries();}

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
    bool torun = false;
    cout << "Output Status: ";
    if (conf->ErrorRerunCode == 2) {
      cout << "FirstRun" << endl;
      torun = true;
    }
    if (conf->ErrorRerunCode == 1) {
      cout << "RetryFailedJob" << endl;
      torun = true;
    }
    if (!torun && HasZombieOutput(ofname)) {
      cout << "Zombie" << endl;
      torun = true;
    }
    if (!torun && conf->InRerunList) {
      cout << "InRerunList" << endl;
      torun = true;
    }
    if (!torun) {
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
    return r->HasRegionsOfInterest;
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
    r->RunEndSummary();
    ofile->Write();
    ofile->Save();
  }

  void CloseOutput() {
    ofile->Close();
    if (conf->PrintProgress) progress->JobEnd();
  }

  Long64_t EntriesMax;
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

class HypothesisBranches {
public:
  HypothesisBranches(TString p, int nv = 9, TTree* t = nullptr) {
    Prefix = p;
    nVariations = nv;
    if (t != nullptr) CreateBranches(t);
  }
  void CreateBranches(TTree *t) {
    Perm = new vector<int>(nVariations);
    PtPerm = new vector<int>(nVariations);
    bTagPerm = new vector<int>(nVariations);
    WPType = new vector<int>(nVariations);

    PPtPerm = new vector<double>(nVariations);
    PbTag = new vector<double>(nVariations);
    Scales = new vector<double>(nVariations * 4);
    PScales = new vector<double>(nVariations * 4);
    Jet_btagDeepFlavB = new vector<double>(nVariations * 4);
    TotalPScale = new vector<double>(nVariations);
    WPdR = new vector<double>(nVariations);
    PWPdR = new vector<double>(nVariations);
    HadW = new vector<double>(nVariations);
    HadT = new vector<double>(nVariations);
    LepT = new vector<double>(nVariations);
    PHadW = new vector<double>(nVariations);
    PHadT = new vector<double>(nVariations);
    PLep = new vector<double>(nVariations);
    PFitter = new vector<double>(nVariations);
    Likelihood = new vector<double>(nVariations);
    WPrimeMass = new vector<double>(nVariations);

    TString pp = Prefix + "_";
    t->Branch(pp + "Perm", &Perm);
    t->Branch(pp + "PtPerm", &PtPerm);
    t->Branch(pp + "bTagPerm", &bTagPerm);
    t->Branch(pp + "WPType", &WPType);

    t->Branch(pp + "PPtPerm", &PPtPerm);
    t->Branch(pp + "PbTag", &PbTag);
    t->Branch(pp + "Scales", &Scales);
    t->Branch(pp + "PScales", &PScales);
    t->Branch(pp + "Jet_btagDeepFlavB", &Jet_btagDeepFlavB);
    t->Branch(pp + "TotalPScale", &TotalPScale);
    t->Branch(pp + "WPdR", &WPdR);
    t->Branch(pp + "PWPdR", &PWPdR);
    t->Branch(pp + "HadW", &HadW);
    t->Branch(pp + "HadT", &HadT);
    t->Branch(pp + "LepT", &LepT);
    t->Branch(pp + "PHadW", &PHadW);
    t->Branch(pp + "PHadT", &PHadT);
    t->Branch(pp + "PLep", &PLep);
    t->Branch(pp + "PFitter", &PFitter);
    t->Branch(pp + "Likelihood", &Likelihood);
    t->Branch(pp + "WPrimeMass", &WPrimeMass);
  }
  
  void FillHypothesis(Hypothesis& h, int ir) {
    Perm->at(ir) = h.Perm;
    PtPerm->at(ir) = h.PtPerm;
    bTagPerm->at(ir) = h.bTagPerm;
    WPType->at(ir) = h.WPType;

    PPtPerm->at(ir) = h.PPtPerm;
    PbTag->at(ir) = h.PbTag;
    for (unsigned i = 0; i < 4; ++i) {
      Scales->at(ir * 4 + i) = h.Scales[i];
      PScales->at(ir * 4 + i) = h.PScales[i];
      Jet_btagDeepFlavB->at(ir * 4 + i) = h.Jet_btagDeepFlavB[i];
    }
    TotalPScale->at(ir) = h.GetTotalPScale();
    WPdR->at(ir) = h.WPdR();
    PWPdR->at(ir) = h.PWPdR;
    HadW->at(ir) = h.HadW().M();
    HadT->at(ir) = h.HadT().M();
    LepT->at(ir) = h.LepT().M();
    PHadW->at(ir) = h.PHadW;
    PHadT->at(ir) = h.PHadT;
    PLep->at(ir) = h.PLep;
    PFitter->at(ir) = h.GetPFitter();
    Likelihood->at(ir) = h.GetTotalP();
    WPrimeMass->at(ir) = h.WP().M();
  }

  void Reset() {
    for (vector<int>* vec : {Perm, PtPerm, bTagPerm, WPType}) {
      *vec = vector<int>(vec->size(), -1);
    }
    for (vector<double>* vec : {PPtPerm, PbTag, Scales, PScales, Jet_btagDeepFlavB, TotalPScale, WPdR, PWPdR, HadW, HadT, LepT, PHadW, PHadT, PLep, PFitter, Likelihood, WPrimeMass}) {
      *vec = vector<double>(vec->size(),0.);
    }
  }

  vector<int> *Perm, *PtPerm, *bTagPerm, *WPType; // [iVariation]
  vector<double> *PPtPerm, *PbTag; // [iVariation]
  vector<double> *Scales, *PScales, *Jet_btagDeepFlavB; // [iVariation[4]], Each variation has 4 scales
  vector<double> *TotalPScale; // [iVariation]
  vector<double> *WPdR; // [iVariation]
  vector<double> *PWPdR; // [iVariation]
  vector<double> *HadW, *HadT, *LepT, *PHadW, *PHadT, *PLep; //[iVariation]
  vector<double> *PFitter; // [iVariation]
  vector<double> *Likelihood; // [iVariation]

  vector<double> *WPrimeMass; // [iVariation]

  TString Prefix;
  int nVariations;
};

#endif
