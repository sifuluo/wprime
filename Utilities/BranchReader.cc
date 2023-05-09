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
  Bool_t          IsMC;

  static const int nGenPartMax = 250;
  static const int nGenJetMax = 35;
  static const int nJetMax = 35;
  static const int nElectronMax = 20;
  static const int nMuonMax = 23;
  static const int nTrigObjMax = 61;
  static const int nSFMax = 64;
  static const int nLHEPdfWeightMax = 200;
  static const int nLHEScaleWeightMax = 9;

  // GenPart
  UInt_t          nGenPart;
  Float_t         GenPart_pt[nGenPartMax];   //[nGenPart]
  Float_t         GenPart_eta[nGenPartMax];   //[nGenPart]
  Float_t         GenPart_phi[nGenPartMax];   //[nGenPart]
  Float_t         GenPart_mass[nGenPartMax];   //[nGenPart]
  Int_t           GenPart_pdgId[nGenPartMax];   //[nGenPart]
  Int_t           GenPart_genPartIdxMother[nGenPartMax];   //[nGenPart]
  Int_t           GenPart_status[nGenPartMax];   //[nGenPart]
  // Int_t           GenPart_statusFlags[nGenPartMax];   //[nGenPart]

  // GenJet
  UInt_t          nGenJet;
  Float_t         GenJet_eta[nGenJetMax];   //[nGenJet]
  Float_t         GenJet_mass[nGenJetMax];   //[nGenJet]
  Float_t         GenJet_phi[nGenJetMax];   //[nGenJet]
  Float_t         GenJet_pt[nGenJetMax];   //[nGenJet]
  Int_t           GenJet_partonFlavour[nGenJetMax];   //[nGenJet]
  UChar_t         GenJet_hadronFlavour[nGenJetMax];   //[nGenJet]

  // Jet
  UInt_t          nJet;
  Float_t         Jet_pt[nJetMax];   //[nJet]
  Float_t         Jet_eta[nJetMax];   //[nJet]
  Float_t         Jet_phi[nJetMax];   //[nJet]
  Float_t         Jet_mass[nJetMax];   //[nJet]
  Int_t           Jet_jetId[nJetMax];   //[nJet]
  Int_t           Jet_puId[nJetMax];   //[nJet]
  Int_t           Jet_genJetIdx[nJetMax];   //[nJet]
  // Float_t         Jet_btagDeepB[25];   //[nJet]
  Float_t         Jet_btagDeepFlavB[nJetMax];   //[nJet]
  Int_t           Jet_hadronFlavour[nJetMax];   //[nJet]
  Int_t           Jet_partonFlavour[nJetMax];   //[nJet]
  Float_t	  Jet_pt_nom[nJetMax];   //[nJet]
  Float_t         Jet_pt_jesTotalUp[nJetMax];   //[nJet]
  Float_t         Jet_pt_jesTotalDown[nJetMax];   //[nJet]
  Float_t         Jet_pt_jerUp[nJetMax];   //[nJet]
  Float_t         Jet_pt_jerDown[nJetMax];   //[nJet]
  Float_t         Jet_mass_nom[nJetMax];   //[nJet]
  Float_t         Jet_mass_jesTotalUp[nJetMax];   //[nJet]
  Float_t         Jet_mass_jesTotalDown[nJetMax];   //[nJet]
  Float_t         Jet_mass_jerUp[nJetMax];   //[nJet]
  Float_t         Jet_mass_jerDown[nJetMax];   //[nJet]
  Float_t	  Jet_bRegCorr[nJetMax];   //[nJet]
  Float_t	  Jet_bRegRes[nJetMax];  //[nJet]

  // Electron
  UInt_t          nElectron;
  Float_t         Electron_pt[nElectronMax];   //[nElectron]
  Float_t         Electron_eta[nElectronMax];   //[nElectron]
  Float_t         Electron_phi[nElectronMax];   //[nElectron]
  Float_t         Electron_mass[nElectronMax];   //[nElectron]
  Int_t           Electron_charge[nElectronMax];   //[nElectron]
  Int_t           Electron_cutBased[nElectronMax];   //[nElectron]
  Bool_t          Electron_cutBased_HEEP[nElectronMax];   //[nElectron]
  Float_t	  Electron_dEsigmaDown[nElectronMax];    //[nElectron]
  Float_t	  Electron_dEsigmaUp[nElectronMax];    //[nElectron]
  // Int_t           Electron_genPartIdx[9];   //[nElectron]

  //Muon
  UInt_t          nMuon;
  Float_t         Muon_pt[nMuonMax];   //[nMuon]
  Float_t         Muon_eta[nMuonMax];   //[nMuon]
  Float_t         Muon_phi[nMuonMax];   //[nMuon]
  Float_t         Muon_mass[nMuonMax];   //[nMuon]
  Int_t           Muon_charge[nMuonMax];   //[nMuon]
  // Int_t           Muon_genPartIdx[nMuonMax];   //[nMuon]
  Int_t           Muon_tightId[nMuonMax];   //[nMuon]
  Bool_t          Muon_looseId[nMuonMax];   //[nMuon]
  UChar_t	  Muon_highPtId[nMuonMax];  //[nMuon]
  Float_t         Muon_pfRelIso04_all[nMuonMax];   //[nMuon]

  //MET
  Float_t         MET_phi;
  Float_t         MET_pt;
  Float_t	  MET_T1_pt;
  Float_t	  MET_T1_phi;
  Float_t	  MET_T1Smear_pt;
  Float_t	  MET_T1Smear_phi;
  Float_t	  MET_T1Smear_pt_jesTotalUp;
  Float_t         MET_T1Smear_phi_jesTotalUp;
  Float_t         MET_T1Smear_pt_jesTotalDown;
  Float_t         MET_T1Smear_phi_jesTotalDown;
  Float_t	  MET_T1Smear_pt_jerUp;
  Float_t         MET_T1Smear_phi_jerUp;
  Float_t         MET_T1Smear_pt_jerDown;
  Float_t         MET_T1Smear_phi_jerDown;

  //GenMET
  Float_t         GenMET_phi;
  Float_t         GenMET_pt;

  //TrigObj
  UInt_t          nTrigObj;
  Float_t         TrigObj_pt[nTrigObjMax];   //[nTrigObj]
  Float_t         TrigObj_eta[nTrigObjMax];   //[nTrigObj]
  Float_t         TrigObj_phi[nTrigObjMax];   //[nTrigObj]
  Int_t           TrigObj_id[nTrigObjMax];   //[nTrigObj]

  //SFs
  Float_t	  Electron_scaleFactor[nSFMax]; //nElectron
  Float_t	  Electron_scaleFactorUp[nSFMax]; //[nElectron]
  Float_t	  Electron_scaleFactorDown[nSFMax]; //[nElectron]
  Float_t   Muon_triggerScaleFactor[nSFMax]; // [nMuon]
  Float_t   Muon_triggerScaleFactorSystUp[nSFMax]; // [nMuon]
  Float_t   Muon_triggerScaleFactorSystDown[nSFMax]; // [nMuon]
  Float_t   Muon_idScaleFactor[nSFMax]; // [nMuon]
  Float_t   Muon_idScaleFactorSystUp[nSFMax]; // [nMuon]
  Float_t   Muon_idScaleFactorSystDown[nSFMax]; // [nMuon]
  Float_t   Muon_isoScaleFactor[nSFMax]; // [nMuon]
  Float_t   Muon_isoScaleFactorSystUp[nSFMax]; // [nMuon]
  Float_t   Muon_isoScaleFactorSystDown[nSFMax]; // [nMuon]
  Float_t	  Jet_bTagScaleFactorTight[nSFMax]; //[nJet]
  Float_t	  Jet_bTagScaleFactorTightUp[nSFMax]; //[nJet]
  Float_t	  Jet_bTagScaleFactorTightDown[nSFMax]; //[nJet]
  Float_t         Jet_bTagScaleFactorMedium[nSFMax]; //[nJet]
  Float_t         Jet_bTagScaleFactorMediumUp[nSFMax]; //[nJet]
  Float_t         Jet_bTagScaleFactorMediumDown[nSFMax]; //[nJet]
  Float_t         Jet_bTagScaleFactorLoose[nSFMax]; //[nJet]
  Float_t         Jet_bTagScaleFactorLooseUp[nSFMax]; //[nJet]
  Float_t         Jet_bTagScaleFactorLooseDown[nSFMax]; //[nJet]
  Float_t	  Jet_puIdScaleFactorTight[nSFMax]; //[nJet]
  Float_t	  Jet_puIdScaleFactorTightUp[nSFMax]; //nJet]
  Float_t	  Jet_puIdScaleFactorTightDown[nSFMax]; //[nJet]
  Float_t         Jet_puIdScaleFactorMedium[nSFMax]; //[nJet]
  Float_t         Jet_puIdScaleFactorMediumUp[nSFMax]; //nJet]
  Float_t         Jet_puIdScaleFactorMediumDown[nSFMax]; //[nJet]
  Float_t         Jet_puIdScaleFactorLoose[nSFMax]; //[nJet]
  Float_t         Jet_puIdScaleFactorLooseUp[nSFMax]; //nJet]
  Float_t         Jet_puIdScaleFactorLooseDown[nSFMax]; //[nJet]

  Int_t      nLHEPdfWeight;
  Float_t    LHEPdfWeight[200];
  Int_t      nLHEScaleWeight;
  Float_t    LHEScaleWeight[9];

  //Trigger
  Bool_t isolated_electron_trigger;
  Bool_t isolated_muon_trigger;
  Bool_t isolated_muon_track_trigger;

  TBranch *b_isolated_electron_trigger, *b_isolated_muon_trigger, *b_isolated_muon_track_trigger;

  //Pileup
  Float_t         Pileup_nTrueInt;
  Float_t         Pileup_pudensity;
  Float_t         Pileup_gpudensity;
  Int_t           Pileup_nPU;
  Int_t           Pileup_sumEOOT;
  Int_t           Pileup_sumLOOT;

  // Primary Vertices
  Int_t PV_npvs;
  Int_t PV_npvsGood;

  TBranch *b_Pileup_nTrueInt, *b_Pileup_pudensity, *b_Pileup_gpudensity, *b_Pileup_nPU, *b_Pileup_sumEOOT, *b_Pileup_sumLOOT, *b_PV_npvs, *b_PV_npvsGood;

  // Flags
  Bool_t Flag_goodVertices;
  Bool_t Flag_globalSuperTightHalo2016Filter;
  Bool_t Flag_HBHENoiseFilter;
  Bool_t Flag_HBHENoiseIsoFilter;
  Bool_t Flag_EcalDeadCellTriggerPrimitiveFilter;
  Bool_t Flag_BadPFMuonFilter;
  Bool_t Flag_BadPFMuonDzFilter;
  Bool_t Flag_eeBadScFilter;
  Bool_t Flag_ecalBadCalibFilter;

  TBranch *b_Flag_goodVertices, *b_Flag_globalSuperTightHalo2016Filter, *b_Flag_HBHENoiseFilter, *b_Flag_HBHENoiseIsoFilter, *b_Flag_EcalDeadCellTriggerPrimitiveFilter, *b_Flag_BadPFMuonFilter, *b_Flag_BadPFMuonDzFilter, *b_Flag_eeBadScFilter, *b_Flag_ecalBadCalibFilter;

  // L1PreFiring weights for 2016 and 2017
  Float_t L1PreFiringWeight_Nom;
  Float_t L1PreFiringWeight_Up;
  Float_t L1PreFiringWeight_Down;

  Float_t Pileup_scaleFactor, Pileup_scaleFactorUp, Pileup_scaleFactorDown;


  Events(TChain *chain_, string sy_ = "2018", Bool_t mc_ = true) : chain(0){
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
     Int_t out = chain->GetEntry(entry);
    //  std::cout <<"flag1 " <<  Flag_goodVertices <<std::endl;
     return out;
  }

  void Init() {
    if(!chain) return;
    fCurrent = -1;
    chain->SetMakeClass(1);

    chain->SetBranchAddress("nJet", &nJet);
    chain->SetBranchAddress("Jet_pt", Jet_pt);
    chain->SetBranchAddress("Jet_eta", Jet_eta);
    chain->SetBranchAddress("Jet_phi", Jet_phi);
    chain->SetBranchAddress("Jet_mass", Jet_mass);
    chain->SetBranchAddress("Jet_jetId", Jet_jetId);
    chain->SetBranchAddress("Jet_puId", Jet_puId);
    chain->SetBranchAddress("Jet_btagDeepFlavB", Jet_btagDeepFlavB);
    chain->SetBranchAddress("Jet_bRegCorr", Jet_bRegCorr);
    chain->SetBranchAddress("Jet_bRegRes", Jet_bRegRes);
    chain->SetBranchAddress("Jet_pt_nom", Jet_pt_nom);
    chain->SetBranchAddress("Jet_mass_nom", Jet_mass_nom);

    chain->SetBranchAddress("nElectron", &nElectron);
    chain->SetBranchAddress("Electron_pt", Electron_pt);
    chain->SetBranchAddress("Electron_eta", Electron_eta);
    chain->SetBranchAddress("Electron_phi", Electron_phi);
    chain->SetBranchAddress("Electron_mass", Electron_mass);
    chain->SetBranchAddress("Electron_charge", Electron_charge);
    chain->SetBranchAddress("Electron_cutBased", Electron_cutBased);
    chain->SetBranchAddress("Electron_cutBased_HEEP", Electron_cutBased_HEEP);

    chain->SetBranchAddress("nMuon", &nMuon);
    chain->SetBranchAddress("Muon_pt", Muon_pt);
    chain->SetBranchAddress("Muon_eta", Muon_eta);
    chain->SetBranchAddress("Muon_phi", Muon_phi);
    chain->SetBranchAddress("Muon_mass", Muon_mass);
    chain->SetBranchAddress("Muon_charge", Muon_charge);
    chain->SetBranchAddress("Muon_looseId", Muon_looseId);
    chain->SetBranchAddress("Muon_tightId", Muon_tightId);
    // chain->SetBranchAddress("Muon_highPtId", Muon_highPtId);
    chain->SetBranchAddress("Muon_pfRelIso04_all", Muon_pfRelIso04_all);

    chain->SetBranchAddress("MET_phi", &MET_phi);
    chain->SetBranchAddress("MET_pt", &MET_pt);
    chain->SetBranchAddress("MET_T1_pt", &MET_T1_pt);
    chain->SetBranchAddress("MET_T1_phi", &MET_T1_phi);

    chain->SetBranchAddress("nTrigObj", &nTrigObj);
    chain->SetBranchAddress("TrigObj_pt", TrigObj_pt);
    chain->SetBranchAddress("TrigObj_eta", TrigObj_eta);
    chain->SetBranchAddress("TrigObj_phi", TrigObj_phi);
    chain->SetBranchAddress("TrigObj_id", TrigObj_id);

    chain->SetBranchAddress("run", &run);
    chain->SetBranchAddress("luminosityBlock", &luminosityBlock);

    if (IsMC){
      chain->SetBranchAddress("Jet_pt_jesTotalUp", Jet_pt_jesTotalUp);
      chain->SetBranchAddress("Jet_pt_jesTotalDown", Jet_pt_jesTotalDown);
      chain->SetBranchAddress("Jet_pt_jerUp", Jet_pt_jerUp);
      chain->SetBranchAddress("Jet_pt_jerDown", Jet_pt_jerDown);
      chain->SetBranchAddress("Jet_mass_jesTotalUp", Jet_mass_jesTotalUp);
      chain->SetBranchAddress("Jet_mass_jesTotalDown", Jet_mass_jesTotalDown);
      chain->SetBranchAddress("Jet_mass_jerUp", Jet_mass_jerUp);
      chain->SetBranchAddress("Jet_mass_jerDown", Jet_mass_jerDown);

      chain->SetBranchAddress("Jet_hadronFlavour", Jet_hadronFlavour);
      chain->SetBranchAddress("Jet_partonFlavour", Jet_partonFlavour);
      chain->SetBranchAddress("Jet_genJetIdx", Jet_genJetIdx);

      chain->SetBranchAddress("nGenJet", &nGenJet);
      chain->SetBranchAddress("GenJet_pt", GenJet_pt);
      chain->SetBranchAddress("GenJet_eta", GenJet_eta);
      chain->SetBranchAddress("GenJet_phi", GenJet_phi);
      chain->SetBranchAddress("GenJet_mass", GenJet_mass);
      chain->SetBranchAddress("GenJet_hadronFlavour", GenJet_hadronFlavour);
      chain->SetBranchAddress("GenJet_partonFlavour", GenJet_partonFlavour);

      chain->SetBranchAddress("MET_T1Smear_pt", &MET_T1Smear_pt);
      chain->SetBranchAddress("MET_T1Smear_phi", &MET_T1Smear_phi);
      chain->SetBranchAddress("MET_T1Smear_pt_jesTotalUp", &MET_T1Smear_pt_jesTotalUp);
      chain->SetBranchAddress("MET_T1Smear_phi_jesTotalUp", &MET_T1Smear_phi_jesTotalUp);
      chain->SetBranchAddress("MET_T1Smear_pt_jesTotalDown", &MET_T1Smear_pt_jesTotalDown);
      chain->SetBranchAddress("MET_T1Smear_phi_jesTotalDown", &MET_T1Smear_phi_jesTotalDown);
      chain->SetBranchAddress("MET_T1Smear_pt_jerUp", &MET_T1Smear_pt_jerUp);
      chain->SetBranchAddress("MET_T1Smear_phi_jerUp", &MET_T1Smear_phi_jerUp);
      chain->SetBranchAddress("MET_T1Smear_pt_jerDown", &MET_T1Smear_pt_jerDown);
      chain->SetBranchAddress("MET_T1Smear_phi_jerDown", &MET_T1Smear_phi_jerDown);

      chain->SetBranchAddress("GenMET_phi", &GenMET_phi);
      chain->SetBranchAddress("GenMET_pt", &GenMET_pt);

      chain->SetBranchAddress("nGenPart", &nGenPart);
      chain->SetBranchAddress("GenPart_pt", GenPart_pt);
      chain->SetBranchAddress("GenPart_eta", GenPart_eta);
      chain->SetBranchAddress("GenPart_phi", GenPart_phi);
      chain->SetBranchAddress("GenPart_mass", GenPart_mass);
      chain->SetBranchAddress("GenPart_pdgId", GenPart_pdgId);
      chain->SetBranchAddress("GenPart_genPartIdxMother", GenPart_genPartIdxMother);
      chain->SetBranchAddress("GenPart_status", GenPart_status);

      chain->SetBranchAddress("Pileup_gpudensity", &Pileup_gpudensity, &b_Pileup_gpudensity);
      chain->SetBranchAddress("Pileup_nPU", &Pileup_nPU, &b_Pileup_nPU);
      chain->SetBranchAddress("Pileup_nTrueInt", &Pileup_nTrueInt, &b_Pileup_nTrueInt);
      chain->SetBranchAddress("Pileup_pudensity", &Pileup_pudensity, &b_Pileup_pudensity);
      chain->SetBranchAddress("Pileup_sumEOOT", &Pileup_sumEOOT, &b_Pileup_sumEOOT);
      chain->SetBranchAddress("Pileup_sumLOOT", &Pileup_sumLOOT, &b_Pileup_sumLOOT);

      if (nSFMax < nJetMax) cout << "nSFMax < nJetMax" <<endl;
      if (nSFMax < nElectronMax) cout << "nSFMax < nElectronMax" <<endl;
      if (nSFMax < nMuonMax) cout << "nSFMax < nMuonMax" <<endl;

      chain->SetBranchAddress("Electron_scaleFactor", Electron_scaleFactor);
      chain->SetBranchAddress("Electron_scaleFactorUp", Electron_scaleFactorUp);
      chain->SetBranchAddress("Electron_scaleFactorDown", Electron_scaleFactorDown);
      chain->SetBranchAddress("Muon_triggerScaleFactor", Muon_triggerScaleFactor);
      chain->SetBranchAddress("Muon_triggerScaleFactorSystUp", Muon_triggerScaleFactorSystUp);
      chain->SetBranchAddress("Muon_triggerScaleFactorSystDown", Muon_triggerScaleFactorSystDown);
      chain->SetBranchAddress("Muon_idScaleFactor", Muon_idScaleFactor);
      chain->SetBranchAddress("Muon_idScaleFactorSystUp", Muon_idScaleFactorSystUp);
      chain->SetBranchAddress("Muon_idScaleFactorSystDown", Muon_idScaleFactorSystDown);
      chain->SetBranchAddress("Muon_isoScaleFactor", Muon_isoScaleFactor);
      chain->SetBranchAddress("Muon_isoScaleFactorSystUp", Muon_isoScaleFactorSystUp);
      chain->SetBranchAddress("Muon_isoScaleFactorSystDown", Muon_isoScaleFactorSystDown);

      chain->SetBranchAddress("Jet_bTagScaleFactorTight", Jet_bTagScaleFactorTight);
      chain->SetBranchAddress("Jet_bTagScaleFactorTightUp", Jet_bTagScaleFactorTightUp);
      chain->SetBranchAddress("Jet_bTagScaleFactorTightDown", Jet_bTagScaleFactorTightDown);
      chain->SetBranchAddress("Jet_bTagScaleFactorMedium", Jet_bTagScaleFactorMedium);
      chain->SetBranchAddress("Jet_bTagScaleFactorMediumUp", Jet_bTagScaleFactorMediumUp);
      chain->SetBranchAddress("Jet_bTagScaleFactorMediumDown", Jet_bTagScaleFactorMediumDown);
      chain->SetBranchAddress("Jet_bTagScaleFactorLoose", Jet_bTagScaleFactorLoose);
      chain->SetBranchAddress("Jet_bTagScaleFactorLooseUp", Jet_bTagScaleFactorLooseUp);
      chain->SetBranchAddress("Jet_bTagScaleFactorLooseDown", Jet_bTagScaleFactorLooseDown);
      chain->SetBranchAddress("Jet_puIdScaleFactorTight", Jet_puIdScaleFactorTight);
      chain->SetBranchAddress("Jet_puIdScaleFactorTightUp", Jet_puIdScaleFactorTightUp);
      chain->SetBranchAddress("Jet_puIdScaleFactorTightDown", Jet_puIdScaleFactorTightDown);
      chain->SetBranchAddress("Jet_puIdScaleFactorMedium", Jet_puIdScaleFactorMedium);
      chain->SetBranchAddress("Jet_puIdScaleFactorMediumUp", Jet_puIdScaleFactorMediumUp);
      chain->SetBranchAddress("Jet_puIdScaleFactorMediumDown", Jet_puIdScaleFactorMediumDown);
      chain->SetBranchAddress("Jet_puIdScaleFactorLoose", Jet_puIdScaleFactorLoose);
      chain->SetBranchAddress("Jet_puIdScaleFactorLooseUp", Jet_puIdScaleFactorLooseUp);
      chain->SetBranchAddress("Jet_puIdScaleFactorLooseDown", Jet_puIdScaleFactorLooseDown);

      chain->SetBranchAddress("nLHEPdfWeight", &nLHEPdfWeight);
      chain->SetBranchAddress("LHEPdfWeight", LHEPdfWeight);
      chain->SetBranchAddress("nLHEScaleWeight", &nLHEScaleWeight);
      chain->SetBranchAddress("LHEScaleWeight", LHEScaleWeight);

      chain->SetBranchAddress("Pileup_scaleFactor", &Pileup_scaleFactor);
      chain->SetBranchAddress("Pileup_scaleFactorUp", &Pileup_scaleFactorUp);
      chain->SetBranchAddress("Pileup_scaleFactorDown", &Pileup_scaleFactorDown);
    }
    if (SampleYear == "2016" || SampleYear == "2016apv"){
      chain->SetBranchAddress("HLT_Ele27_WPTight_Gsf", &isolated_electron_trigger, &b_isolated_electron_trigger);
      chain->SetBranchAddress("HLT_IsoMu24",&isolated_muon_trigger, &b_isolated_muon_trigger);
      chain->SetBranchAddress("HLT_IsoTkMu24",&isolated_muon_track_trigger, &b_isolated_muon_track_trigger);
      Flag_eeBadScFilter = true;
      Flag_ecalBadCalibFilter = true;
    }
    else if (SampleYear == "2017"){
      // chain->SetBranchAddress("HLT_Ele35_WPTight_Gsf", &isolated_electron_trigger, &b_isolated_electron_trigger);
      chain->SetBranchAddress("HLT_Ele32_WPTight_Gsf_L1DoubleEG", &isolated_electron_trigger, &b_isolated_electron_trigger);
      chain->SetBranchAddress("HLT_IsoMu27",&isolated_muon_trigger, &b_isolated_muon_trigger);
      isolated_muon_track_trigger = false;
      chain->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter, &b_Flag_eeBadScFilter);
      chain->SetBranchAddress("Flag_ecalBadCalibFilter", &Flag_ecalBadCalibFilter, &b_Flag_ecalBadCalibFilter);
    }
    else if (SampleYear == "2018"){
      chain->SetBranchAddress("HLT_Ele32_WPTight_Gsf", &isolated_electron_trigger, &b_isolated_electron_trigger);
      chain->SetBranchAddress("HLT_IsoMu24",&isolated_muon_trigger, &b_isolated_muon_trigger);
      isolated_muon_track_trigger = false;
      chain->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter, &b_Flag_eeBadScFilter);
      chain->SetBranchAddress("Flag_ecalBadCalibFilter", &Flag_ecalBadCalibFilter, &b_Flag_ecalBadCalibFilter);
    }

    if(SampleYear == "2016" || SampleYear == "2016apv" || SampleYear == "2017"){
      chain->SetBranchAddress("L1PreFiringWeight_Nom", &L1PreFiringWeight_Nom);
      chain->SetBranchAddress("L1PreFiringWeight_Up", &L1PreFiringWeight_Up);
      chain->SetBranchAddress("L1PreFiringWeight_Dn", &L1PreFiringWeight_Down);
    }

    chain->SetBranchAddress("PV_npvs", &PV_npvs, &b_PV_npvs);
    chain->SetBranchAddress("PV_npvsGood", &PV_npvsGood, &b_PV_npvsGood);

    // chain->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices);
    chain->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices, &b_Flag_goodVertices);
    chain->SetBranchAddress("Flag_globalSuperTightHalo2016Filter", &Flag_globalSuperTightHalo2016Filter, &b_Flag_globalSuperTightHalo2016Filter);
    chain->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter, &b_Flag_HBHENoiseFilter);
    chain->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter, &b_Flag_HBHENoiseIsoFilter);
    chain->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter, &b_Flag_EcalDeadCellTriggerPrimitiveFilter);
    chain->SetBranchAddress("Flag_BadPFMuonFilter", &Flag_BadPFMuonFilter, &b_Flag_BadPFMuonFilter);
    if (SampleYear == "2017" || SampleYear == "2018") {
      chain->SetBranchAddress("Flag_BadPFMuonDzFilter", &Flag_BadPFMuonDzFilter, &b_Flag_BadPFMuonDzFilter);
    }
    else {
      Flag_BadPFMuonDzFilter = true;
    }
    Notify();
  }

  
  Long64_t LoadTree(Long64_t entry)
  {
// Set the environment to read one entry
   if (!chain) return -5;
   Long64_t centry = chain->LoadTree(entry);
   if (centry < 0) return centry;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

Bool_t Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Show(Long64_t entry)
{
  if(!chain) return;
  chain->Show(entry);
}

};

#endif
