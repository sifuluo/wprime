#ifndef BRANCHREADER_CC
#define BRANCHREADER_CC

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class Events {
public:
  TTree          *chain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain
  UInt_t          run;
  UInt_t          luminosityBlock;
  ULong64_t       event;

  string          SampleYear;
  bool            IsMC;

  // GenPart
  UInt_t          nGenPart;
  Float_t         GenPart_pt[149];   //[nGenPart]
  Float_t         GenPart_eta[149];   //[nGenPart]
  Float_t         GenPart_phi[149];   //[nGenPart]
  Float_t         GenPart_mass[149];   //[nGenPart]
  Int_t           GenPart_pdgId[149];   //[nGenPart]
  Int_t           GenPart_genPartIdxMother[149];   //[nGenPart]
  Int_t           GenPart_status[149];   //[nGenPart]
  // Int_t           GenPart_statusFlags[149];   //[nGenPart]

  // GenJet
  UInt_t          nGenJet;
  Float_t         GenJet_eta[24];   //[nGenJet]
  Float_t         GenJet_mass[24];   //[nGenJet]
  Float_t         GenJet_phi[24];   //[nGenJet]
  Float_t         GenJet_pt[24];   //[nGenJet]
  Int_t           GenJet_partonFlavour[24];   //[nGenJet]
  UChar_t         GenJet_hadronFlavour[24];   //[nGenJet]

  // Jet
  UInt_t          nJet;
  Float_t         Jet_pt[25];   //[nJet]
  Float_t         Jet_eta[25];   //[nJet]
  Float_t         Jet_phi[25];   //[nJet]
  Float_t         Jet_mass[25];   //[nJet]
  Int_t           Jet_jetId[25];   //[nJet]
  Int_t           Jet_puId[25];   //[nJet]
  Int_t           Jet_genJetIdx[25];   //[nJet]
  // Float_t         Jet_btagDeepB[25];   //[nJet]
  Float_t         Jet_btagDeepFlavB[25];   //[nJet]
  Int_t           Jet_hadronFlavour[25];   //[nJet]
  Int_t           Jet_partonFlavour[25];   //[nJet]

  // Electron
  UInt_t          nElectron;
  Float_t         Electron_pt[9];   //[nElectron]
  Float_t         Electron_eta[9];   //[nElectron]
  Float_t         Electron_phi[9];   //[nElectron]
  Float_t         Electron_mass[9];   //[nElectron]
  Int_t           Electron_charge[9];   //[nElectron]
  // Int_t           Electron_genPartIdx[9];   //[nElectron]

  //Muon
  UInt_t          nMuon;
  Float_t         Muon_pt[18];   //[nMuon]
  Float_t         Muon_eta[18];   //[nMuon]
  Float_t         Muon_phi[18];   //[nMuon]
  Float_t         Muon_mass[18];   //[nMuon]
  Int_t           Muon_charge[18];   //[nMuon]
  // Int_t           Muon_genPartIdx[18];   //[nMuon]
  Int_t           Muon_tightId[18];   //[nMuon]
  Float_t         Muon_pfRelIso04_all[18];   //[nMuon]

  //MET
  Float_t         MET_phi;
  Float_t         MET_pt;

  //GenMET
  Float_t         GenMET_phi;
  Float_t         GenMET_pt;

  //Trigger
  bool isolated_electron_trigger;
  bool isolated_muon_trigger;
  bool isolated_muon_track_trigger;

  //Pileup
  Int_t Pileup_nPU;
  Float_t Pileup_nTrueInt;

  // Primary Vertices
  Int_t PV_npvs;
  Int_t PV_npvsGood;


  Events(TChain *chain_, string sy_, bool mc_){
     chain = chain_;
     SampleYear = sy_;
     IsMC = mc_;
     Init();
  };

  ~Events() {
     if (!chain) return;
     delete chain->GetCurrentFile();
  };

  Int_t GetEntry(Long64_t entry) {
  // Read contents of entry.
     if (!chain) return 0;
     return chain->GetEntry(entry);
  }

  void Init() {
    chain->SetBranchAddress("nJet", &nJet);
    chain->SetBranchAddress("Jet_pt", &Jet_pt);
    chain->SetBranchAddress("Jet_eta", &Jet_eta);
    chain->SetBranchAddress("Jet_phi", &Jet_phi);
    chain->SetBranchAddress("Jet_mass", &Jet_mass);
    chain->SetBranchAddress("Jet_jetId", &Jet_jetId);
    chain->SetBranchAddress("Jet_puId", &Jet_puId);
    chain->SetBranchAddress("Jet_btagDeepFlavB", &Jet_btagDeepFlavB);

    chain->SetBranchAddress("nElectron", &nElectron);
    chain->SetBranchAddress("Electron_pt", &Electron_pt);
    chain->SetBranchAddress("Electron_eta", &Electron_eta);
    chain->SetBranchAddress("Electron_phi", &Electron_phi);
    chain->SetBranchAddress("Electron_mass", &Electron_mass);
    chain->SetBranchAddress("Electron_charge", &Electron_charge);

    chain->SetBranchAddress("nMuon", &nMuon);
    chain->SetBranchAddress("Muon_pt", &Muon_pt);
    chain->SetBranchAddress("Muon_eta", &Muon_eta);
    chain->SetBranchAddress("Muon_phi", &Muon_phi);
    chain->SetBranchAddress("Muon_mass", &Muon_mass);
    chain->SetBranchAddress("Muon_charge", &Muon_charge);
    chain->SetBranchAddress("Muon_tightId", &Muon_tightId);
    chain->SetBranchAddress("Muon_pfRelIso04_all", &Muon_pfRelIso04_all);

    chain->SetBranchAddress("MET_phi", &MET_phi);
    chain->SetBranchAddress("MET_pt", &MET_pt);

    chain->SetBranchAddress("run", &run);
    chain->SetBranchAddress("luminosityBlock", &luminosityBlock);

    if (IsMC){
      chain->SetBranchAddress("Jet_hadronFlavour", &Jet_hadronFlavour);
      chain->SetBranchAddress("Jet_partonFlavour", &Jet_partonFlavour);
      chain->SetBranchAddress("Jet_genJetIdx", &Jet_genJetIdx);

      chain->SetBranchAddress("nGenJet", &nGenJet);
      chain->SetBranchAddress("GenJet_pt", &GenJet_pt);
      chain->SetBranchAddress("GenJet_eta", &GenJet_eta);
      chain->SetBranchAddress("GenJet_phi", &GenJet_phi);
      chain->SetBranchAddress("GenJet_mass", &GenJet_mass);
      chain->SetBranchAddress("GenJet_hadronFlavour", &GenJet_hadronFlavour);
      chain->SetBranchAddress("GenJet_partonFlavour", &GenJet_partonFlavour);

      chain->SetBranchAddress("GenMET_phi", &GenMET_phi);
      chain->SetBranchAddress("GenMET_pt", &GenMET_pt);

      chain->SetBranchAddress("nGenPart", &nGenPart);
      chain->SetBranchAddress("GenPart_pt", &GenPart_pt);
      chain->SetBranchAddress("GenPart_eta", &GenPart_eta);
      chain->SetBranchAddress("GenPart_phi", &GenPart_phi);
      chain->SetBranchAddress("GenPart_mass", &GenPart_mass);
      chain->SetBranchAddress("GenPart_pdgId", &GenPart_pdgId);
      chain->SetBranchAddress("GenPart_genPartIdxMother", &GenPart_genPartIdxMother);
      chain->SetBranchAddress("GenPart_status", &GenPart_status);

      chain->SetBranchAddress("Pileup_nPU", &Pileup_nPU);
      chain->SetBranchAddress("Pileup_nTrueInt", &Pileup_nTrueInt);
    }

    if (SampleYear == "2016" || SampleYear == "2016apv"){
      chain->SetBranchAddress("HLT_Ele27_WPTight_Gsf", &isolated_electron_trigger);
      chain->SetBranchAddress("HLT_IsoMu24",&isolated_muon_trigger);
      chain->SetBranchAddress("HLT_IsoTkMu24",&isolated_muon_track_trigger);
    }
    else if (SampleYear == "2017"){
      chain->SetBranchAddress("HLT_Ele35_WPTight_Gsf", &isolated_electron_trigger);
      chain->SetBranchAddress("HLT_IsoMu27",&isolated_muon_trigger);
      isolated_muon_track_trigger = false;
    }
    else if (SampleYear == "2018"){
      chain->SetBranchAddress("HLT_Ele32_WPTight_Gsf", &isolated_electron_trigger);
      chain->SetBranchAddress("HLT_IsoMu24",&isolated_muon_trigger);
      isolated_muon_track_trigger = false;
    }

    chain->SetBranchAddress("PV_npvs", &PV_npvs);
    chain->SetBranchAddress("PV_npvsGood", &PV_npvsGood);
  }

};

#endif
