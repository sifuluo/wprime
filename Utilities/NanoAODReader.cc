#ifndef NANOAODREADER_CC
#define NANOAODREADER_CC

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "TChain.h"
#include "TFile.h"

#include "DataFormat.cc"
#include "BranchReader.cc"
#include "BTag.cc"
#include "Constants.cc"

using namespace std;

class NanoAODReader {
public:
  NanoAODReader(Configs *conf_) {
    chain = new TChain("Events");

    conf = conf_;

    IsMC = (conf->iSampleType > 1);

    JetPtThreshold = 30.;
    JetIdThreshold = 6;
    LepPtThreshold = 40.;
    if (conf->iFile >= 0 || conf->InputFile == "All") { // batch mode
      vector<string> rootfiles = GetFileNames();
      for (string rf : rootfiles) {
        chain->Add(TString(rf));
        cout << "Successfully loaded root file: " << rf << endl;
      }
      cout << "In total, " << rootfiles.size() << " files are loaded" << endl;
    }
    else if (conf->InputFile == "") cout << "iFile set to negative but a valid test InputFile name is missing" << endl;
    else {
      chain->Add(conf->InputFile);
      cout << "Loaded test InputFile: " << conf->InputFile << endl;
    }
    cout << "Running with SampleYear = " << conf->SampleYear << ", SampleType = " << conf->SampleType << ", Trigger = " << conf->Trigger << endl;
    evts = new Events(chain, conf->SampleYear, IsMC);
    if (conf->InputFile != "All" && conf->FilesPerJob == 1) cout << "This iteration contains " << GetEntries() << " events" <<endl;
  };

  vector<string> GetFileNames() {
    vector<string> out;
    string basepath = "/afs/cern.ch/work/s/siluo/wprime/filenames/";
    if (conf->DASInput) basepath = basepath + "DASFileNames/DAS_";
    string filename = basepath + conf->SampleType + "_" + conf->SampleYear + ".txt";
    ifstream infile(filename);
    if (!infile) {
      cout << "Cannot read from file " << filename << endl;
      throw runtime_error("Cannot Read from file");
      return out;
    }
    else cout << "Reading from file " << filename << endl;

    int startfile = conf->iFile * conf->FilesPerJob;
    int endfile = (conf->iFile + 1) * conf->FilesPerJob - 1;
    string rootf;
    int counter = -1;
    while (getline(infile, rootf)) {
      ++counter;
      if (counter < startfile) continue;
      if (counter > endfile && conf->InputFile != "All") break;
      if (rootf.find("/store/") == 0) rootf = "root://cms-xrd-global.cern.ch/" + rootf;
      cout << "Loading root file " << rootf << endl;
      out.push_back(rootf);
    }
    return out;
  }

  Long64_t GetEntries() {
    return chain->GetEntries();
  }

  void SetBTagger(BTag* b_) {
    btagger = b_;
    Jet::btagger = b_;
  }

  void SetBtagWP(int wp_) {
    conf->Btag_WP = wp_;
    btagger->SetBtagWP(wp_);
  }

  bool JetSelection(Jet& j) {
    bool pass = true;
    pass &= (j.Pt() > JetPtThreshold);
    pass &= (j.jetId >= JetIdThreshold);
    return pass;
  }

  void ReadEvent(Long64_t i) {
    evts->GetEntry(i);
    run = evts->run;
    luminosityBlock = evts->luminosityBlock;
    if (!IsMC && (run < 0 || luminosityBlock < 0)) cout << "Run/LuminosityBlock number is negative" <<endl;
    if (conf->PUEvaluation) { // It will only run on MC
      ReadPileup();
      ReadVertices();
      return;
    }

    if (IsMC) {
      ReadGenParts();
      ReadGenJets();
      ReadGenMET();
      ReadPileup();
    }
    ReadJets();
    ReadLeptons();
    ReadMET();
    ReadTriggers();
    ReadVertices();
  }

  void ReadGenParts() {
    GenParts.clear();
    for (unsigned i = 0; i < evts->nGenPart; ++i) {
      GenPart tmp;
      tmp.SetPtEtaPhiM(evts->GenPart_pt[i],evts->GenPart_eta[i],evts->GenPart_phi[i],evts->GenPart_mass[i]);
      tmp.index = i;
      tmp.genPartIdxMother = evts->GenPart_genPartIdxMother[i];
      tmp.pdgId = evts->GenPart_pdgId[i];
      tmp.status = evts->GenPart_status[i];
      GenParts.push_back(tmp);
    }
  }

  void ReadGenJets() {
    GenJets.clear();
    for (unsigned i = 0; i < evts->nGenJet; ++i) {
      GenJet tmp;
      tmp.SetPtEtaPhiM(evts->GenJet_pt[i],evts->GenJet_eta[i],evts->GenJet_phi[i],evts->GenJet_mass[i]);
      tmp.index = i;
      tmp.partonFlavour = evts->GenJet_partonFlavour[i];
      tmp.hadronFlavour = evts->GenJet_hadronFlavour[i];
      GenJets.push_back(tmp);
    }
  }

  void ReadJets() {
    AllJets.clear();
    Jets.clear();
    nBJets.clear();
    nNBJets.clear();

    for (unsigned i = 0; i < evts->nJet; ++i) {
      Jet tmp;
      tmp.SetPtEtaPhiM(evts->Jet_pt[i],evts->Jet_eta[i],evts->Jet_phi[i],evts->Jet_mass[i]);
      tmp.index = i;
      tmp.jetId = evts->Jet_jetId[i];
      tmp.puId = evts->Jet_puId[i];
      if (IsMC) {
        tmp.genJetIdx = evts->Jet_genJetIdx[i];
        tmp.hadronFlavour = evts->Jet_hadronFlavour[i];
        tmp.partonFlavour = evts->Jet_partonFlavour[i];
      }
      tmp.btagDeepFlavB = evts->Jet_btagDeepFlavB[i];
      tmp.SetBtags();
      AllJets.push_back(tmp);
      if (!JetSelection(tmp)) continue;
      Jets.push_back(tmp);
    }
    for (unsigned i = 0; i < 3; ++i) {
      int bjs, nbjs;
      bjs = nbjs = 0;
      for (unsigned j = 0; j < Jets.size(); ++j) {
        if (Jets[j].btags[i]) bjs++;
        else nbjs++;
      }
      nBJets.push_back(bjs);
      nNBJets.push_back(nbjs);
    }
  }

  void ReadLeptons() {
    Leptons.clear();
    Electrons.clear();
    Muons.clear();
    for (unsigned i = 0; i < evts->nElectron; ++i) {
      Electron tmp;
      tmp.SetPtEtaPhiM(evts->Electron_pt[i],evts->Electron_eta[i],evts->Electron_phi[i],evts->Electron_mass[i]);
      tmp.index = i;
      tmp.charge = evts->Electron_charge[i];

      //CommonSelectionBlock
      float absEta = fabs(tmp.Eta());
      bool passCommon = (absEta < 2.4);
      passCommon &= (absEta < 1.44 || absEta > 1.57);
      passCommon &= (tmp.Pt() >= 10.);

      //TripleSelectionsBlock
      bool passVeto = (evts->Electron_cutBased[i] >= 2) && passCommon;
      passCommon &= (tmp.Pt() >= 40.); //change pT cut for non-veto electrons to be on trigger plateau
      bool passLoose = (evts->Electron_cutBased[i] >= 3) && passCommon;
      bool passPrimary = evts->Electron_cutBased_HEEP[i] && passCommon;

      tmp.IsPrimary = passPrimary;
      tmp.IsLoose = passLoose;
      tmp.IsVeto = passVeto;

      if(!passVeto && !passLoose && !passPrimary) continue;

      Electrons.push_back(tmp);
      Leptons.push_back(tmp);
    }
    for (unsigned i = 0; i < evts->nMuon; ++i) {
      Muon tmp;
      tmp.SetPtEtaPhiM(evts->Muon_pt[i],evts->Muon_eta[i],evts->Muon_phi[i],evts->Muon_mass[i]);
      tmp.index = i;
      tmp.charge = evts->Muon_charge[i];

      //CommonSelectionBlock
      float absEta = fabs(tmp.Eta());
      bool passCommon = (fabs(tmp.Eta()) < 2.4);
      passCommon &= (tmp.Pt() > 10.);
      passCommon &= (evts->Muon_pfRelIso04_all[i] < 0.25);

      //TripleSelectionBlock
      bool passVeto = (evts->Muon_looseId[i]) && passCommon;
      passCommon &= (tmp.Pt() > 35.); //change pT cut for non-veto muons to be on trigger plateau
      bool passLoose = (evts->Muon_tightId[i]) && passCommon;
      passCommon &= (evts->Muon_pfRelIso04_all[i] < 0.15); //change isolation cut for primary muons
      bool passPrimary = (evts->Muon_highPtId[i] == 2) && passCommon;

      tmp.IsPrimary = passPrimary;
      tmp.IsLoose = passLoose;
      tmp.IsVeto = passVeto;

      if(!passVeto && !passLoose && !passPrimary) continue;

      Muons.push_back(tmp);
      Leptons.push_back(tmp);
    }
  }

  void ReadMET() {
    Met = TLorentzVector();
    Met.SetPtEtaPhiM(evts->MET_pt,0,evts->MET_phi,0);
  }

  void ReadGenMET() {
    GenMet = TLorentzVector();
    GenMet.SetPtEtaPhiM(evts->GenMET_pt, 0, evts->GenMET_phi, 0);
  }

  void ReadTriggers() {
    isolated_electron_trigger = evts->isolated_electron_trigger;
    isolated_muon_trigger = evts->isolated_muon_trigger;
    isolated_muon_track_trigger = evts->isolated_muon_track_trigger;
  }

  void ReadPileup() {
    Pileup_nPU = evts->Pileup_nPU;
    Pileup_nTrueInt = evts->Pileup_nTrueInt;
  }

  void ReadVertices() {
    PV_npvs = evts->PV_npvs;
    PV_npvsGood = evts->PV_npvsGood;
  }

  bool ReadMETFilterStatus() {
    bool status = true;
    if (!IsMC || true) {
      status = status && evts->Flag_goodVertices;
      status = status && evts->Flag_globalSuperTightHalo2016Filter;
      status = status && evts->Flag_HBHENoiseFilter;
      status = status && evts->Flag_HBHENoiseIsoFilter;
      status = status && evts->Flag_EcalDeadCellTriggerPrimitiveFilter;
      status = status && evts->Flag_BadPFMuonFilter;
      status = status && evts->Flag_BadPFMuonDzFilter;
      status = status && evts->Flag_eeBadScFilter;
      status = status && evts->Flag_ecalBadCalibFilter;
    }
    METFilterStatus = status;
    return METFilterStatus;
  }

  Configs *conf;

  bool IsMC;
  TChain* chain;
  BTag* btagger;
  Events* evts;
  int run, luminosityBlock;
  vector<GenPart> GenParts;
  vector<GenJet> GenJets;
  float JetPtThreshold;
  int JetIdThreshold;
  vector<Jet> AllJets, Jets;
  vector<int> nBJets, nNBJets;
  float LepPtThreshold;
  vector<Lepton> Leptons;
  vector<Electron> Electrons;
  vector<Muon> Muons;
  MET Met;
  GenMET GenMet;

  bool isolated_electron_trigger, isolated_muon_trigger, isolated_muon_track_trigger;
  bool METFilterStatus;
  int Pileup_nPU;
  float Pileup_nTrueInt;
  int PV_npvs, PV_npvsGood;
  // nlohmann::json GoodSections;
  // bool LumiStatus;
};


#endif
