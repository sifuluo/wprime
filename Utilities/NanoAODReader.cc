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
// #include "FileLists.cc"
#include "BTag.cc"
#include "Constants.cc"

using namespace std;

class NanoAODReader {
public:
  NanoAODReader(Configs *conf_) {
    chain = new TChain("Events");
    // iSampleYear = isy_;
    // iSampleType = ist_;
    // SampleYear = Constants::SampleYears[isy_];
    // SampleType = Constants::SampleTypes[ist_];
    // iTrigger = itr_;
    conf = conf_;

    IsMC = (conf->iSampleType > 1);

    JetPtThreshold = 30.;
    JetIdThreshold = 6;
    LepPtThreshold = 30.;
    if (true) { // batch mode
      vector<string> rootfiles = GetFileNames(conf->iSampleYear, conf->iSampleType, conf->iFile);
      for (string rf : rootfiles) {
        chain->Add(TString(rf));
        cout << "Successfully loaded root file: " << rf << endl;
      }
    }
    else chain->Add("ttbar2018sample.root"); // single file mode
    cout << "Running with SampleYear = " << conf->SampleYear << ", SampleType = " << conf->SampleType << ", Trigger = " << conf->Trigger << endl;
    evts = new Events(chain, conf->SampleYear, IsMC);
    cout << "This iteration contains " << GetEntries() << " events" <<endl;
  };

  vector<string> GetFileNames(int isy_, int ist_, int igroup = 0, int groupsize = 1) {
    vector<string> out;
    string basepath = "/afs/cern.ch/work/s/siluo/wprime/filenames/";
    string filename = basepath + Constants::SampleTypes[ist_] + "_" + Constants::SampleYears[isy_] + ".txt";
    ifstream infile(filename);
    if (!infile) {
      cout << "Cannot read from file " << filename << endl;
      throw runtime_error("Cannot Read from file");
      return out;
    }
    else cout << "Reading from file " << filename << endl;

    int startfile = igroup * groupsize;
    int endfile = (igroup + 1) * groupsize - 1;
    string rootf;
    int counter = -1;
    while (getline(infile, rootf)) {
      ++counter;
      if (counter < startfile) continue;
      if (counter > endfile) break;
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
  }

  bool JetSelection(Jet& j) {
    bool pass = true;
    pass &= (j.Pt() > JetPtThreshold);
    pass &= (j.jetId >= JetIdThreshold);
    return pass;
  }

  bool ElectronSelection(Electron ele) {
    float et = fabs(ele.Eta());
    bool pass = true;
    pass &= (ele.Pt() > LepPtThreshold);
    pass &= (et < 2.4);
    pass &= (et < 1.44 || et > 1.57);
    return pass;
  }

  bool MuonSelection(Muon mu) {
    bool pass = true;
    pass &= (mu.Pt() > LepPtThreshold);
    pass &= (fabs(mu.Eta()) < 2.4);
    pass &= (mu.tightId > 0);
    pass &= (mu.relIso < 0.25);
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
    N_BJets = 0;
    N_NBJets = 0;
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
      btagger->TagB(tmp);
      AllJets.push_back(tmp);
      if (!JetSelection(tmp)) continue;
      Jets.push_back(tmp);
      if (tmp.btag) N_BJets++;
      else N_NBJets++;
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
      if (!ElectronSelection(tmp)) continue;
      Electrons.push_back(tmp);
      Leptons.push_back(tmp);
    }
    for (unsigned i = 0; i < evts->nMuon; ++i) {
      Muon tmp;
      tmp.SetPtEtaPhiM(evts->Muon_pt[i],evts->Muon_eta[i],evts->Muon_phi[i],evts->Muon_mass[i]);
      tmp.index = i;
      tmp.charge = evts->Muon_charge[i];
      tmp.tightId = evts->Muon_tightId[i];
      tmp.relIso = evts->Muon_pfRelIso04_all[i];
      // tmp.type = 1;
      if (!MuonSelection(tmp)) continue;
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

  // int iSampleYear, iSampleType, iTrigger, iFile;
  // string SampleYear, SampleType;
  // bool PUEvaluation;
  // bool DASInput;
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
  int N_BJets, N_NBJets;
  float LepPtThreshold;
  vector<Lepton> Leptons;
  vector<Electron> Electrons;
  vector<Muon> Muons;
  MET Met;
  GenMET GenMet;

  bool isolated_electron_trigger, isolated_muon_trigger, isolated_muon_track_trigger;
  int Pileup_nPU;
  float Pileup_nTrueInt;
  int PV_npvs, PV_npvsGood;
  // nlohmann::json GoodSections;
  // bool LumiStatus;
};


#endif
