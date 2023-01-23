//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jan 23 00:54:16 2023 by ROOT version 6.24/08
// from TTree Events/Events
// found on file: /eos/user/p/pflanaga/andrewsdata/skimmed_samples/ttbar/2018/014BDC65-BE41-D14F-B57A-ABD0054C28FE.root
//////////////////////////////////////////////////////////

#ifndef Events_h
#define Events_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class Events {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UInt_t          run;
   UInt_t          luminosityBlock;
   UInt_t          nElectron;
   Float_t         Electron_dEscaleDown[10];   //[nElectron]
   Float_t         Electron_dEscaleUp[10];   //[nElectron]
   Float_t         Electron_dEsigmaDown[10];   //[nElectron]
   Float_t         Electron_dEsigmaUp[10];   //[nElectron]
   Float_t         Electron_eta[10];   //[nElectron]
   Float_t         Electron_mass[10];   //[nElectron]
   Float_t         Electron_phi[10];   //[nElectron]
   Float_t         Electron_pt[10];   //[nElectron]
   Int_t           Electron_charge[10];   //[nElectron]
   Int_t           Electron_cutBased[10];   //[nElectron]
   Bool_t          Electron_cutBased_HEEP[10];   //[nElectron]
   UInt_t          nGenJet;
   Float_t         GenJet_eta[25];   //[nGenJet]
   Float_t         GenJet_mass[25];   //[nGenJet]
   Float_t         GenJet_phi[25];   //[nGenJet]
   Float_t         GenJet_pt[25];   //[nGenJet]
   UInt_t          nGenPart;
   Float_t         GenPart_eta[151];   //[nGenPart]
   Float_t         GenPart_mass[151];   //[nGenPart]
   Float_t         GenPart_phi[151];   //[nGenPart]
   Float_t         GenPart_pt[151];   //[nGenPart]
   Int_t           GenPart_genPartIdxMother[151];   //[nGenPart]
   Int_t           GenPart_pdgId[151];   //[nGenPart]
   Int_t           GenPart_status[151];   //[nGenPart]
   UInt_t          nJet;
   Float_t         Jet_btagDeepFlavB[26];   //[nJet]
   Float_t         Jet_eta[26];   //[nJet]
   Float_t         Jet_mass[26];   //[nJet]
   Float_t         Jet_phi[26];   //[nJet]
   Float_t         Jet_pt[26];   //[nJet]
   Float_t         Jet_bRegCorr[26];   //[nJet]
   Float_t         Jet_bRegRes[26];   //[nJet]
   Int_t           Jet_jetId[26];   //[nJet]
   Int_t           Jet_puId[26];   //[nJet]
   Float_t         L1PreFiringWeight_Dn;
   Float_t         L1PreFiringWeight_ECAL_Dn;
   Float_t         L1PreFiringWeight_ECAL_Nom;
   Float_t         L1PreFiringWeight_ECAL_Up;
   Float_t         L1PreFiringWeight_Muon_Nom;
   Float_t         L1PreFiringWeight_Muon_StatDn;
   Float_t         L1PreFiringWeight_Muon_StatUp;
   Float_t         L1PreFiringWeight_Muon_SystDn;
   Float_t         L1PreFiringWeight_Muon_SystUp;
   Float_t         L1PreFiringWeight_Nom;
   Float_t         L1PreFiringWeight_Up;
   Float_t         GenMET_phi;
   Float_t         GenMET_pt;
   Float_t         MET_phi;
   Float_t         MET_pt;
   UInt_t          nMuon;
   Float_t         Muon_eta[20];   //[nMuon]
   Float_t         Muon_mass[20];   //[nMuon]
   Float_t         Muon_pfRelIso04_all[20];   //[nMuon]
   Float_t         Muon_phi[20];   //[nMuon]
   Float_t         Muon_pt[20];   //[nMuon]
   Int_t           Muon_charge[20];   //[nMuon]
   Bool_t          Muon_looseId[20];   //[nMuon]
   Bool_t          Muon_tightId[20];   //[nMuon]
   Float_t         Pileup_nTrueInt;
   Float_t         Pileup_pudensity;
   Float_t         Pileup_gpudensity;
   Int_t           Pileup_nPU;
   Int_t           Pileup_sumEOOT;
   Int_t           Pileup_sumLOOT;
   UInt_t          nTrigObj;
   Float_t         TrigObj_pt[49];   //[nTrigObj]
   Float_t         TrigObj_eta[49];   //[nTrigObj]
   Float_t         TrigObj_phi[49];   //[nTrigObj]
   Int_t           TrigObj_id[49];   //[nTrigObj]
   Int_t           PV_npvs;
   Int_t           PV_npvsGood;
   Int_t           GenJet_partonFlavour[25];   //[nGenJet]
   UChar_t         GenJet_hadronFlavour[25];   //[nGenJet]
   Int_t           Jet_genJetIdx[26];   //[nJet]
   Int_t           Jet_hadronFlavour[26];   //[nJet]
   Int_t           Jet_partonFlavour[26];   //[nJet]
   Bool_t          Flag_HBHENoiseFilter;
   Bool_t          Flag_HBHENoiseIsoFilter;
   Bool_t          Flag_globalSuperTightHalo2016Filter;
   Bool_t          Flag_EcalDeadCellTriggerPrimitiveFilter;
   Bool_t          Flag_ecalBadCalibFilter;
   Bool_t          Flag_goodVertices;
   Bool_t          Flag_eeBadScFilter;
   Bool_t          Flag_BadPFMuonFilter;
   Bool_t          Flag_BadPFMuonDzFilter;
   Bool_t          HLT_Ele32_WPTight_Gsf;
   Bool_t          HLT_IsoMu24;
   Float_t         Jet_pt_raw[26];   //[nJet]
   Float_t         Jet_pt_nom[26];   //[nJet]
   Float_t         Jet_mass_raw[26];   //[nJet]
   Float_t         Jet_mass_nom[26];   //[nJet]
   Float_t         Jet_corr_JEC[26];   //[nJet]
   Float_t         Jet_corr_JER[26];   //[nJet]
   Float_t         MET_T1_pt;
   Float_t         MET_T1_phi;
   Float_t         MET_T1Smear_pt;
   Float_t         MET_T1Smear_phi;
   Float_t         Jet_pt_jerUp[26];   //[nJet]
   Float_t         Jet_mass_jerUp[26];   //[nJet]
   Float_t         MET_T1_pt_jerUp;
   Float_t         MET_T1_phi_jerUp;
   Float_t         MET_T1Smear_pt_jerUp;
   Float_t         MET_T1Smear_phi_jerUp;
   Float_t         Jet_pt_jesTotalUp[26];   //[nJet]
   Float_t         Jet_mass_jesTotalUp[26];   //[nJet]
   Float_t         MET_T1_pt_jesTotalUp;
   Float_t         MET_T1_phi_jesTotalUp;
   Float_t         MET_T1Smear_pt_jesTotalUp;
   Float_t         MET_T1Smear_phi_jesTotalUp;
   Float_t         Jet_pt_jesHEMIssueUp[26];   //[nJet]
   Float_t         Jet_mass_jesHEMIssueUp[26];   //[nJet]
   Float_t         MET_T1_pt_jesHEMIssueUp;
   Float_t         MET_T1_phi_jesHEMIssueUp;
   Float_t         MET_T1Smear_pt_jesHEMIssueUp;
   Float_t         MET_T1Smear_phi_jesHEMIssueUp;
   Float_t         MET_T1_pt_unclustEnUp;
   Float_t         MET_T1_phi_unclustEnUp;
   Float_t         MET_T1Smear_pt_unclustEnUp;
   Float_t         MET_T1Smear_phi_unclustEnUp;
   Float_t         Jet_pt_jerDown[26];   //[nJet]
   Float_t         Jet_mass_jerDown[26];   //[nJet]
   Float_t         MET_T1_pt_jerDown;
   Float_t         MET_T1_phi_jerDown;
   Float_t         MET_T1Smear_pt_jerDown;
   Float_t         MET_T1Smear_phi_jerDown;
   Float_t         Jet_pt_jesTotalDown[26];   //[nJet]
   Float_t         Jet_mass_jesTotalDown[26];   //[nJet]
   Float_t         MET_T1_pt_jesTotalDown;
   Float_t         MET_T1_phi_jesTotalDown;
   Float_t         MET_T1Smear_pt_jesTotalDown;
   Float_t         MET_T1Smear_phi_jesTotalDown;
   Float_t         Jet_pt_jesHEMIssueDown[26];   //[nJet]
   Float_t         Jet_mass_jesHEMIssueDown[26];   //[nJet]
   Float_t         MET_T1_pt_jesHEMIssueDown;
   Float_t         MET_T1_phi_jesHEMIssueDown;
   Float_t         MET_T1Smear_pt_jesHEMIssueDown;
   Float_t         MET_T1Smear_phi_jesHEMIssueDown;
   Float_t         MET_T1_pt_unclustEnDown;
   Float_t         MET_T1_phi_unclustEnDown;
   Float_t         MET_T1Smear_pt_unclustEnDown;
   Float_t         MET_T1Smear_phi_unclustEnDown;
   Float_t         Electron_scaleFactor[64];
   Float_t         Electron_scaleFactorUp[64];
   Float_t         Electron_scaleFactorDown[64];
   Float_t         Muon_scaleFactor[64];
   Float_t         Muon_scaleFactorSyst[64];
   Float_t         Muon_scaleFactorStat[64];
   Float_t         Jet_bTagScaleFactorTight[64];
   Float_t         Jet_bTagScaleFactorTightUp[64];
   Float_t         Jet_bTagScaleFactorTightDown[64];
   Float_t         Jet_bTagScaleFactorMedium[64];
   Float_t         Jet_bTagScaleFactorMediumUp[64];
   Float_t         Jet_bTagScaleFactorMediumDown[64];
   Float_t         Jet_bTagScaleFactorLoose[64];
   Float_t         Jet_bTagScaleFactorLooseUp[64];
   Float_t         Jet_bTagScaleFactorLooseDown[64];
   Float_t         Jet_puIdScaleFactorTight[64];
   Float_t         Jet_puIdScaleFactorTightUp[64];
   Float_t         Jet_puIdScaleFactorTightDown[64];
   Float_t         Jet_puIdScaleFactorMedium[64];
   Float_t         Jet_puIdScaleFactorMediumUp[64];
   Float_t         Jet_puIdScaleFactorMediumDown[64];
   Float_t         Jet_puIdScaleFactorLoose[64];
   Float_t         Jet_puIdScaleFactorLooseUp[64];
   Float_t         Jet_puIdScaleFactorLooseDown[64];
   Float_t         Pileup_scaleFactor;
   Float_t         Pileup_scaleFactorUp;
   Float_t         Pileup_scaleFactorDown;

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_luminosityBlock;   //!
   TBranch        *b_nElectron;   //!
   TBranch        *b_Electron_dEscaleDown;   //!
   TBranch        *b_Electron_dEscaleUp;   //!
   TBranch        *b_Electron_dEsigmaDown;   //!
   TBranch        *b_Electron_dEsigmaUp;   //!
   TBranch        *b_Electron_eta;   //!
   TBranch        *b_Electron_mass;   //!
   TBranch        *b_Electron_phi;   //!
   TBranch        *b_Electron_pt;   //!
   TBranch        *b_Electron_charge;   //!
   TBranch        *b_Electron_cutBased;   //!
   TBranch        *b_Electron_cutBased_HEEP;   //!
   TBranch        *b_nGenJet;   //!
   TBranch        *b_GenJet_eta;   //!
   TBranch        *b_GenJet_mass;   //!
   TBranch        *b_GenJet_phi;   //!
   TBranch        *b_GenJet_pt;   //!
   TBranch        *b_nGenPart;   //!
   TBranch        *b_GenPart_eta;   //!
   TBranch        *b_GenPart_mass;   //!
   TBranch        *b_GenPart_phi;   //!
   TBranch        *b_GenPart_pt;   //!
   TBranch        *b_GenPart_genPartIdxMother;   //!
   TBranch        *b_GenPart_pdgId;   //!
   TBranch        *b_GenPart_status;   //!
   TBranch        *b_nJet;   //!
   TBranch        *b_Jet_btagDeepFlavB;   //!
   TBranch        *b_Jet_eta;   //!
   TBranch        *b_Jet_mass;   //!
   TBranch        *b_Jet_phi;   //!
   TBranch        *b_Jet_pt;   //!
   TBranch        *b_Jet_bRegCorr;   //!
   TBranch        *b_Jet_bRegRes;   //!
   TBranch        *b_Jet_jetId;   //!
   TBranch        *b_Jet_puId;   //!
   TBranch        *b_L1PreFiringWeight_Dn;   //!
   TBranch        *b_L1PreFiringWeight_ECAL_Dn;   //!
   TBranch        *b_L1PreFiringWeight_ECAL_Nom;   //!
   TBranch        *b_L1PreFiringWeight_ECAL_Up;   //!
   TBranch        *b_L1PreFiringWeight_Muon_Nom;   //!
   TBranch        *b_L1PreFiringWeight_Muon_StatDn;   //!
   TBranch        *b_L1PreFiringWeight_Muon_StatUp;   //!
   TBranch        *b_L1PreFiringWeight_Muon_SystDn;   //!
   TBranch        *b_L1PreFiringWeight_Muon_SystUp;   //!
   TBranch        *b_L1PreFiringWeight_Nom;   //!
   TBranch        *b_L1PreFiringWeight_Up;   //!
   TBranch        *b_GenMET_phi;   //!
   TBranch        *b_GenMET_pt;   //!
   TBranch        *b_MET_phi;   //!
   TBranch        *b_MET_pt;   //!
   TBranch        *b_nMuon;   //!
   TBranch        *b_Muon_eta;   //!
   TBranch        *b_Muon_mass;   //!
   TBranch        *b_Muon_pfRelIso04_all;   //!
   TBranch        *b_Muon_phi;   //!
   TBranch        *b_Muon_pt;   //!
   TBranch        *b_Muon_charge;   //!
   TBranch        *b_Muon_looseId;   //!
   TBranch        *b_Muon_tightId;   //!
   TBranch        *b_Pileup_nTrueInt;   //!
   TBranch        *b_Pileup_pudensity;   //!
   TBranch        *b_Pileup_gpudensity;   //!
   TBranch        *b_Pileup_nPU;   //!
   TBranch        *b_Pileup_sumEOOT;   //!
   TBranch        *b_Pileup_sumLOOT;   //!
   TBranch        *b_nTrigObj;   //!
   TBranch        *b_TrigObj_pt;   //!
   TBranch        *b_TrigObj_eta;   //!
   TBranch        *b_TrigObj_phi;   //!
   TBranch        *b_TrigObj_id;   //!
   TBranch        *b_PV_npvs;   //!
   TBranch        *b_PV_npvsGood;   //!
   TBranch        *b_GenJet_partonFlavour;   //!
   TBranch        *b_GenJet_hadronFlavour;   //!
   TBranch        *b_Jet_genJetIdx;   //!
   TBranch        *b_Jet_hadronFlavour;   //!
   TBranch        *b_Jet_partonFlavour;   //!
   TBranch        *b_Flag_HBHENoiseFilter;   //!
   TBranch        *b_Flag_HBHENoiseIsoFilter;   //!
   TBranch        *b_Flag_globalSuperTightHalo2016Filter;   //!
   TBranch        *b_Flag_EcalDeadCellTriggerPrimitiveFilter;   //!
   TBranch        *b_Flag_ecalBadCalibFilter;   //!
   TBranch        *b_Flag_goodVertices;   //!
   TBranch        *b_Flag_eeBadScFilter;   //!
   TBranch        *b_Flag_BadPFMuonFilter;   //!
   TBranch        *b_Flag_BadPFMuonDzFilter;   //!
   TBranch        *b_HLT_Ele32_WPTight_Gsf;   //!
   TBranch        *b_HLT_IsoMu24;   //!
   TBranch        *b_Jet_pt_raw;   //!
   TBranch        *b_Jet_pt_nom;   //!
   TBranch        *b_Jet_mass_raw;   //!
   TBranch        *b_Jet_mass_nom;   //!
   TBranch        *b_Jet_corr_JEC;   //!
   TBranch        *b_Jet_corr_JER;   //!
   TBranch        *b_MET_T1_pt;   //!
   TBranch        *b_MET_T1_phi;   //!
   TBranch        *b_MET_T1Smear_pt;   //!
   TBranch        *b_MET_T1Smear_phi;   //!
   TBranch        *b_Jet_pt_jerUp;   //!
   TBranch        *b_Jet_mass_jerUp;   //!
   TBranch        *b_MET_T1_pt_jerUp;   //!
   TBranch        *b_MET_T1_phi_jerUp;   //!
   TBranch        *b_MET_T1Smear_pt_jerUp;   //!
   TBranch        *b_MET_T1Smear_phi_jerUp;   //!
   TBranch        *b_Jet_pt_jesTotalUp;   //!
   TBranch        *b_Jet_mass_jesTotalUp;   //!
   TBranch        *b_MET_T1_pt_jesTotalUp;   //!
   TBranch        *b_MET_T1_phi_jesTotalUp;   //!
   TBranch        *b_MET_T1Smear_pt_jesTotalUp;   //!
   TBranch        *b_MET_T1Smear_phi_jesTotalUp;   //!
   TBranch        *b_Jet_pt_jesHEMIssueUp;   //!
   TBranch        *b_Jet_mass_jesHEMIssueUp;   //!
   TBranch        *b_MET_T1_pt_jesHEMIssueUp;   //!
   TBranch        *b_MET_T1_phi_jesHEMIssueUp;   //!
   TBranch        *b_MET_T1Smear_pt_jesHEMIssueUp;   //!
   TBranch        *b_MET_T1Smear_phi_jesHEMIssueUp;   //!
   TBranch        *b_MET_T1_pt_unclustEnUp;   //!
   TBranch        *b_MET_T1_phi_unclustEnUp;   //!
   TBranch        *b_MET_T1Smear_pt_unclustEnUp;   //!
   TBranch        *b_MET_T1Smear_phi_unclustEnUp;   //!
   TBranch        *b_Jet_pt_jerDown;   //!
   TBranch        *b_Jet_mass_jerDown;   //!
   TBranch        *b_MET_T1_pt_jerDown;   //!
   TBranch        *b_MET_T1_phi_jerDown;   //!
   TBranch        *b_MET_T1Smear_pt_jerDown;   //!
   TBranch        *b_MET_T1Smear_phi_jerDown;   //!
   TBranch        *b_Jet_pt_jesTotalDown;   //!
   TBranch        *b_Jet_mass_jesTotalDown;   //!
   TBranch        *b_MET_T1_pt_jesTotalDown;   //!
   TBranch        *b_MET_T1_phi_jesTotalDown;   //!
   TBranch        *b_MET_T1Smear_pt_jesTotalDown;   //!
   TBranch        *b_MET_T1Smear_phi_jesTotalDown;   //!
   TBranch        *b_Jet_pt_jesHEMIssueDown;   //!
   TBranch        *b_Jet_mass_jesHEMIssueDown;   //!
   TBranch        *b_MET_T1_pt_jesHEMIssueDown;   //!
   TBranch        *b_MET_T1_phi_jesHEMIssueDown;   //!
   TBranch        *b_MET_T1Smear_pt_jesHEMIssueDown;   //!
   TBranch        *b_MET_T1Smear_phi_jesHEMIssueDown;   //!
   TBranch        *b_MET_T1_pt_unclustEnDown;   //!
   TBranch        *b_MET_T1_phi_unclustEnDown;   //!
   TBranch        *b_MET_T1Smear_pt_unclustEnDown;   //!
   TBranch        *b_MET_T1Smear_phi_unclustEnDown;   //!
   TBranch        *b_Electron_scaleFactor;   //!
   TBranch        *b_Electron_scaleFactorUp;   //!
   TBranch        *b_Electron_scaleFactorDown;   //!
   TBranch        *b_Muon_scaleFactor;   //!
   TBranch        *b_Muon_scaleFactorSyst;   //!
   TBranch        *b_Muon_scaleFactorStat;   //!
   TBranch        *b_Jet_bTagScaleFactorTight;   //!
   TBranch        *b_Jet_bTagScaleFactorTightUp;   //!
   TBranch        *b_Jet_bTagScaleFactorTightDown;   //!
   TBranch        *b_Jet_bTagScaleFactorMedium;   //!
   TBranch        *b_Jet_bTagScaleFactorMediumUp;   //!
   TBranch        *b_Jet_bTagScaleFactorMediumDown;   //!
   TBranch        *b_Jet_bTagScaleFactorLoose;   //!
   TBranch        *b_Jet_bTagScaleFactorLooseUp;   //!
   TBranch        *b_Jet_bTagScaleFactorLooseDown;   //!
   TBranch        *b_Jet_puIdScaleFactorTight;   //!
   TBranch        *b_Jet_puIdScaleFactorTightUp;   //!
   TBranch        *b_Jet_puIdScaleFactorTightDown;   //!
   TBranch        *b_Jet_puIdScaleFactorMedium;   //!
   TBranch        *b_Jet_puIdScaleFactorMediumUp;   //!
   TBranch        *b_Jet_puIdScaleFactorMediumDown;   //!
   TBranch        *b_Jet_puIdScaleFactorLoose;   //!
   TBranch        *b_Jet_puIdScaleFactorLooseUp;   //!
   TBranch        *b_Jet_puIdScaleFactorLooseDown;   //!
   TBranch        *b_Pileup_scaleFactor;   //!
   TBranch        *b_Pileup_scaleFactorUp;   //!
   TBranch        *b_Pileup_scaleFactorDown;   //!

   Events(TTree *tree=0);
   virtual ~Events();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Events_cxx
Events::Events(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/eos/user/p/pflanaga/andrewsdata/skimmed_samples/ttbar/2018/014BDC65-BE41-D14F-B57A-ABD0054C28FE.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/eos/user/p/pflanaga/andrewsdata/skimmed_samples/ttbar/2018/014BDC65-BE41-D14F-B57A-ABD0054C28FE.root");
      }
      f->GetObject("Events",tree);

   }
   Init(tree);
}

Events::~Events()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Events::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Events::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Events::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("luminosityBlock", &luminosityBlock, &b_luminosityBlock);
   fChain->SetBranchAddress("nElectron", &nElectron, &b_nElectron);
   fChain->SetBranchAddress("Electron_dEscaleDown", Electron_dEscaleDown, &b_Electron_dEscaleDown);
   fChain->SetBranchAddress("Electron_dEscaleUp", Electron_dEscaleUp, &b_Electron_dEscaleUp);
   fChain->SetBranchAddress("Electron_dEsigmaDown", Electron_dEsigmaDown, &b_Electron_dEsigmaDown);
   fChain->SetBranchAddress("Electron_dEsigmaUp", Electron_dEsigmaUp, &b_Electron_dEsigmaUp);
   fChain->SetBranchAddress("Electron_eta", Electron_eta, &b_Electron_eta);
   fChain->SetBranchAddress("Electron_mass", Electron_mass, &b_Electron_mass);
   fChain->SetBranchAddress("Electron_phi", Electron_phi, &b_Electron_phi);
   fChain->SetBranchAddress("Electron_pt", Electron_pt, &b_Electron_pt);
   fChain->SetBranchAddress("Electron_charge", Electron_charge, &b_Electron_charge);
   fChain->SetBranchAddress("Electron_cutBased", Electron_cutBased, &b_Electron_cutBased);
   fChain->SetBranchAddress("Electron_cutBased_HEEP", Electron_cutBased_HEEP, &b_Electron_cutBased_HEEP);
   fChain->SetBranchAddress("nGenJet", &nGenJet, &b_nGenJet);
   fChain->SetBranchAddress("GenJet_eta", GenJet_eta, &b_GenJet_eta);
   fChain->SetBranchAddress("GenJet_mass", GenJet_mass, &b_GenJet_mass);
   fChain->SetBranchAddress("GenJet_phi", GenJet_phi, &b_GenJet_phi);
   fChain->SetBranchAddress("GenJet_pt", GenJet_pt, &b_GenJet_pt);
   fChain->SetBranchAddress("nGenPart", &nGenPart, &b_nGenPart);
   fChain->SetBranchAddress("GenPart_eta", GenPart_eta, &b_GenPart_eta);
   fChain->SetBranchAddress("GenPart_mass", GenPart_mass, &b_GenPart_mass);
   fChain->SetBranchAddress("GenPart_phi", GenPart_phi, &b_GenPart_phi);
   fChain->SetBranchAddress("GenPart_pt", GenPart_pt, &b_GenPart_pt);
   fChain->SetBranchAddress("GenPart_genPartIdxMother", GenPart_genPartIdxMother, &b_GenPart_genPartIdxMother);
   fChain->SetBranchAddress("GenPart_pdgId", GenPart_pdgId, &b_GenPart_pdgId);
   fChain->SetBranchAddress("GenPart_status", GenPart_status, &b_GenPart_status);
   fChain->SetBranchAddress("nJet", &nJet, &b_nJet);
   fChain->SetBranchAddress("Jet_btagDeepFlavB", Jet_btagDeepFlavB, &b_Jet_btagDeepFlavB);
   fChain->SetBranchAddress("Jet_eta", Jet_eta, &b_Jet_eta);
   fChain->SetBranchAddress("Jet_mass", Jet_mass, &b_Jet_mass);
   fChain->SetBranchAddress("Jet_phi", Jet_phi, &b_Jet_phi);
   fChain->SetBranchAddress("Jet_pt", Jet_pt, &b_Jet_pt);
   fChain->SetBranchAddress("Jet_bRegCorr", Jet_bRegCorr, &b_Jet_bRegCorr);
   fChain->SetBranchAddress("Jet_bRegRes", Jet_bRegRes, &b_Jet_bRegRes);
   fChain->SetBranchAddress("Jet_jetId", Jet_jetId, &b_Jet_jetId);
   fChain->SetBranchAddress("Jet_puId", Jet_puId, &b_Jet_puId);
   fChain->SetBranchAddress("L1PreFiringWeight_Dn", &L1PreFiringWeight_Dn, &b_L1PreFiringWeight_Dn);
   fChain->SetBranchAddress("L1PreFiringWeight_ECAL_Dn", &L1PreFiringWeight_ECAL_Dn, &b_L1PreFiringWeight_ECAL_Dn);
   fChain->SetBranchAddress("L1PreFiringWeight_ECAL_Nom", &L1PreFiringWeight_ECAL_Nom, &b_L1PreFiringWeight_ECAL_Nom);
   fChain->SetBranchAddress("L1PreFiringWeight_ECAL_Up", &L1PreFiringWeight_ECAL_Up, &b_L1PreFiringWeight_ECAL_Up);
   fChain->SetBranchAddress("L1PreFiringWeight_Muon_Nom", &L1PreFiringWeight_Muon_Nom, &b_L1PreFiringWeight_Muon_Nom);
   fChain->SetBranchAddress("L1PreFiringWeight_Muon_StatDn", &L1PreFiringWeight_Muon_StatDn, &b_L1PreFiringWeight_Muon_StatDn);
   fChain->SetBranchAddress("L1PreFiringWeight_Muon_StatUp", &L1PreFiringWeight_Muon_StatUp, &b_L1PreFiringWeight_Muon_StatUp);
   fChain->SetBranchAddress("L1PreFiringWeight_Muon_SystDn", &L1PreFiringWeight_Muon_SystDn, &b_L1PreFiringWeight_Muon_SystDn);
   fChain->SetBranchAddress("L1PreFiringWeight_Muon_SystUp", &L1PreFiringWeight_Muon_SystUp, &b_L1PreFiringWeight_Muon_SystUp);
   fChain->SetBranchAddress("L1PreFiringWeight_Nom", &L1PreFiringWeight_Nom, &b_L1PreFiringWeight_Nom);
   fChain->SetBranchAddress("L1PreFiringWeight_Up", &L1PreFiringWeight_Up, &b_L1PreFiringWeight_Up);
   fChain->SetBranchAddress("GenMET_phi", &GenMET_phi, &b_GenMET_phi);
   fChain->SetBranchAddress("GenMET_pt", &GenMET_pt, &b_GenMET_pt);
   fChain->SetBranchAddress("MET_phi", &MET_phi, &b_MET_phi);
   fChain->SetBranchAddress("MET_pt", &MET_pt, &b_MET_pt);
   fChain->SetBranchAddress("nMuon", &nMuon, &b_nMuon);
   fChain->SetBranchAddress("Muon_eta", Muon_eta, &b_Muon_eta);
   fChain->SetBranchAddress("Muon_mass", Muon_mass, &b_Muon_mass);
   fChain->SetBranchAddress("Muon_pfRelIso04_all", Muon_pfRelIso04_all, &b_Muon_pfRelIso04_all);
   fChain->SetBranchAddress("Muon_phi", Muon_phi, &b_Muon_phi);
   fChain->SetBranchAddress("Muon_pt", Muon_pt, &b_Muon_pt);
   fChain->SetBranchAddress("Muon_charge", Muon_charge, &b_Muon_charge);
   fChain->SetBranchAddress("Muon_looseId", Muon_looseId, &b_Muon_looseId);
   fChain->SetBranchAddress("Muon_tightId", Muon_tightId, &b_Muon_tightId);
   fChain->SetBranchAddress("Pileup_nTrueInt", &Pileup_nTrueInt, &b_Pileup_nTrueInt);
   fChain->SetBranchAddress("Pileup_pudensity", &Pileup_pudensity, &b_Pileup_pudensity);
   fChain->SetBranchAddress("Pileup_gpudensity", &Pileup_gpudensity, &b_Pileup_gpudensity);
   fChain->SetBranchAddress("Pileup_nPU", &Pileup_nPU, &b_Pileup_nPU);
   fChain->SetBranchAddress("Pileup_sumEOOT", &Pileup_sumEOOT, &b_Pileup_sumEOOT);
   fChain->SetBranchAddress("Pileup_sumLOOT", &Pileup_sumLOOT, &b_Pileup_sumLOOT);
   fChain->SetBranchAddress("nTrigObj", &nTrigObj, &b_nTrigObj);
   fChain->SetBranchAddress("TrigObj_pt", TrigObj_pt, &b_TrigObj_pt);
   fChain->SetBranchAddress("TrigObj_eta", TrigObj_eta, &b_TrigObj_eta);
   fChain->SetBranchAddress("TrigObj_phi", TrigObj_phi, &b_TrigObj_phi);
   fChain->SetBranchAddress("TrigObj_id", TrigObj_id, &b_TrigObj_id);
   fChain->SetBranchAddress("PV_npvs", &PV_npvs, &b_PV_npvs);
   fChain->SetBranchAddress("PV_npvsGood", &PV_npvsGood, &b_PV_npvsGood);
   fChain->SetBranchAddress("GenJet_partonFlavour", GenJet_partonFlavour, &b_GenJet_partonFlavour);
   fChain->SetBranchAddress("GenJet_hadronFlavour", GenJet_hadronFlavour, &b_GenJet_hadronFlavour);
   fChain->SetBranchAddress("Jet_genJetIdx", Jet_genJetIdx, &b_Jet_genJetIdx);
   fChain->SetBranchAddress("Jet_hadronFlavour", Jet_hadronFlavour, &b_Jet_hadronFlavour);
   fChain->SetBranchAddress("Jet_partonFlavour", Jet_partonFlavour, &b_Jet_partonFlavour);
   fChain->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter, &b_Flag_HBHENoiseFilter);
   fChain->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter, &b_Flag_HBHENoiseIsoFilter);
   fChain->SetBranchAddress("Flag_globalSuperTightHalo2016Filter", &Flag_globalSuperTightHalo2016Filter, &b_Flag_globalSuperTightHalo2016Filter);
   fChain->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter, &b_Flag_EcalDeadCellTriggerPrimitiveFilter);
   fChain->SetBranchAddress("Flag_ecalBadCalibFilter", &Flag_ecalBadCalibFilter, &b_Flag_ecalBadCalibFilter);
   fChain->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices, &b_Flag_goodVertices);
   fChain->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter, &b_Flag_eeBadScFilter);
   fChain->SetBranchAddress("Flag_BadPFMuonFilter", &Flag_BadPFMuonFilter, &b_Flag_BadPFMuonFilter);
   fChain->SetBranchAddress("Flag_BadPFMuonDzFilter", &Flag_BadPFMuonDzFilter, &b_Flag_BadPFMuonDzFilter);
   fChain->SetBranchAddress("HLT_Ele32_WPTight_Gsf", &HLT_Ele32_WPTight_Gsf, &b_HLT_Ele32_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_IsoMu24", &HLT_IsoMu24, &b_HLT_IsoMu24);
   fChain->SetBranchAddress("Jet_pt_raw", Jet_pt_raw, &b_Jet_pt_raw);
   fChain->SetBranchAddress("Jet_pt_nom", Jet_pt_nom, &b_Jet_pt_nom);
   fChain->SetBranchAddress("Jet_mass_raw", Jet_mass_raw, &b_Jet_mass_raw);
   fChain->SetBranchAddress("Jet_mass_nom", Jet_mass_nom, &b_Jet_mass_nom);
   fChain->SetBranchAddress("Jet_corr_JEC", Jet_corr_JEC, &b_Jet_corr_JEC);
   fChain->SetBranchAddress("Jet_corr_JER", Jet_corr_JER, &b_Jet_corr_JER);
   fChain->SetBranchAddress("MET_T1_pt", &MET_T1_pt, &b_MET_T1_pt);
   fChain->SetBranchAddress("MET_T1_phi", &MET_T1_phi, &b_MET_T1_phi);
   fChain->SetBranchAddress("MET_T1Smear_pt", &MET_T1Smear_pt, &b_MET_T1Smear_pt);
   fChain->SetBranchAddress("MET_T1Smear_phi", &MET_T1Smear_phi, &b_MET_T1Smear_phi);
   fChain->SetBranchAddress("Jet_pt_jerUp", Jet_pt_jerUp, &b_Jet_pt_jerUp);
   fChain->SetBranchAddress("Jet_mass_jerUp", Jet_mass_jerUp, &b_Jet_mass_jerUp);
   fChain->SetBranchAddress("MET_T1_pt_jerUp", &MET_T1_pt_jerUp, &b_MET_T1_pt_jerUp);
   fChain->SetBranchAddress("MET_T1_phi_jerUp", &MET_T1_phi_jerUp, &b_MET_T1_phi_jerUp);
   fChain->SetBranchAddress("MET_T1Smear_pt_jerUp", &MET_T1Smear_pt_jerUp, &b_MET_T1Smear_pt_jerUp);
   fChain->SetBranchAddress("MET_T1Smear_phi_jerUp", &MET_T1Smear_phi_jerUp, &b_MET_T1Smear_phi_jerUp);
   fChain->SetBranchAddress("Jet_pt_jesTotalUp", Jet_pt_jesTotalUp, &b_Jet_pt_jesTotalUp);
   fChain->SetBranchAddress("Jet_mass_jesTotalUp", Jet_mass_jesTotalUp, &b_Jet_mass_jesTotalUp);
   fChain->SetBranchAddress("MET_T1_pt_jesTotalUp", &MET_T1_pt_jesTotalUp, &b_MET_T1_pt_jesTotalUp);
   fChain->SetBranchAddress("MET_T1_phi_jesTotalUp", &MET_T1_phi_jesTotalUp, &b_MET_T1_phi_jesTotalUp);
   fChain->SetBranchAddress("MET_T1Smear_pt_jesTotalUp", &MET_T1Smear_pt_jesTotalUp, &b_MET_T1Smear_pt_jesTotalUp);
   fChain->SetBranchAddress("MET_T1Smear_phi_jesTotalUp", &MET_T1Smear_phi_jesTotalUp, &b_MET_T1Smear_phi_jesTotalUp);
   fChain->SetBranchAddress("Jet_pt_jesHEMIssueUp", Jet_pt_jesHEMIssueUp, &b_Jet_pt_jesHEMIssueUp);
   fChain->SetBranchAddress("Jet_mass_jesHEMIssueUp", Jet_mass_jesHEMIssueUp, &b_Jet_mass_jesHEMIssueUp);
   fChain->SetBranchAddress("MET_T1_pt_jesHEMIssueUp", &MET_T1_pt_jesHEMIssueUp, &b_MET_T1_pt_jesHEMIssueUp);
   fChain->SetBranchAddress("MET_T1_phi_jesHEMIssueUp", &MET_T1_phi_jesHEMIssueUp, &b_MET_T1_phi_jesHEMIssueUp);
   fChain->SetBranchAddress("MET_T1Smear_pt_jesHEMIssueUp", &MET_T1Smear_pt_jesHEMIssueUp, &b_MET_T1Smear_pt_jesHEMIssueUp);
   fChain->SetBranchAddress("MET_T1Smear_phi_jesHEMIssueUp", &MET_T1Smear_phi_jesHEMIssueUp, &b_MET_T1Smear_phi_jesHEMIssueUp);
   fChain->SetBranchAddress("MET_T1_pt_unclustEnUp", &MET_T1_pt_unclustEnUp, &b_MET_T1_pt_unclustEnUp);
   fChain->SetBranchAddress("MET_T1_phi_unclustEnUp", &MET_T1_phi_unclustEnUp, &b_MET_T1_phi_unclustEnUp);
   fChain->SetBranchAddress("MET_T1Smear_pt_unclustEnUp", &MET_T1Smear_pt_unclustEnUp, &b_MET_T1Smear_pt_unclustEnUp);
   fChain->SetBranchAddress("MET_T1Smear_phi_unclustEnUp", &MET_T1Smear_phi_unclustEnUp, &b_MET_T1Smear_phi_unclustEnUp);
   fChain->SetBranchAddress("Jet_pt_jerDown", Jet_pt_jerDown, &b_Jet_pt_jerDown);
   fChain->SetBranchAddress("Jet_mass_jerDown", Jet_mass_jerDown, &b_Jet_mass_jerDown);
   fChain->SetBranchAddress("MET_T1_pt_jerDown", &MET_T1_pt_jerDown, &b_MET_T1_pt_jerDown);
   fChain->SetBranchAddress("MET_T1_phi_jerDown", &MET_T1_phi_jerDown, &b_MET_T1_phi_jerDown);
   fChain->SetBranchAddress("MET_T1Smear_pt_jerDown", &MET_T1Smear_pt_jerDown, &b_MET_T1Smear_pt_jerDown);
   fChain->SetBranchAddress("MET_T1Smear_phi_jerDown", &MET_T1Smear_phi_jerDown, &b_MET_T1Smear_phi_jerDown);
   fChain->SetBranchAddress("Jet_pt_jesTotalDown", Jet_pt_jesTotalDown, &b_Jet_pt_jesTotalDown);
   fChain->SetBranchAddress("Jet_mass_jesTotalDown", Jet_mass_jesTotalDown, &b_Jet_mass_jesTotalDown);
   fChain->SetBranchAddress("MET_T1_pt_jesTotalDown", &MET_T1_pt_jesTotalDown, &b_MET_T1_pt_jesTotalDown);
   fChain->SetBranchAddress("MET_T1_phi_jesTotalDown", &MET_T1_phi_jesTotalDown, &b_MET_T1_phi_jesTotalDown);
   fChain->SetBranchAddress("MET_T1Smear_pt_jesTotalDown", &MET_T1Smear_pt_jesTotalDown, &b_MET_T1Smear_pt_jesTotalDown);
   fChain->SetBranchAddress("MET_T1Smear_phi_jesTotalDown", &MET_T1Smear_phi_jesTotalDown, &b_MET_T1Smear_phi_jesTotalDown);
   fChain->SetBranchAddress("Jet_pt_jesHEMIssueDown", Jet_pt_jesHEMIssueDown, &b_Jet_pt_jesHEMIssueDown);
   fChain->SetBranchAddress("Jet_mass_jesHEMIssueDown", Jet_mass_jesHEMIssueDown, &b_Jet_mass_jesHEMIssueDown);
   fChain->SetBranchAddress("MET_T1_pt_jesHEMIssueDown", &MET_T1_pt_jesHEMIssueDown, &b_MET_T1_pt_jesHEMIssueDown);
   fChain->SetBranchAddress("MET_T1_phi_jesHEMIssueDown", &MET_T1_phi_jesHEMIssueDown, &b_MET_T1_phi_jesHEMIssueDown);
   fChain->SetBranchAddress("MET_T1Smear_pt_jesHEMIssueDown", &MET_T1Smear_pt_jesHEMIssueDown, &b_MET_T1Smear_pt_jesHEMIssueDown);
   fChain->SetBranchAddress("MET_T1Smear_phi_jesHEMIssueDown", &MET_T1Smear_phi_jesHEMIssueDown, &b_MET_T1Smear_phi_jesHEMIssueDown);
   fChain->SetBranchAddress("MET_T1_pt_unclustEnDown", &MET_T1_pt_unclustEnDown, &b_MET_T1_pt_unclustEnDown);
   fChain->SetBranchAddress("MET_T1_phi_unclustEnDown", &MET_T1_phi_unclustEnDown, &b_MET_T1_phi_unclustEnDown);
   fChain->SetBranchAddress("MET_T1Smear_pt_unclustEnDown", &MET_T1Smear_pt_unclustEnDown, &b_MET_T1Smear_pt_unclustEnDown);
   fChain->SetBranchAddress("MET_T1Smear_phi_unclustEnDown", &MET_T1Smear_phi_unclustEnDown, &b_MET_T1Smear_phi_unclustEnDown);
   fChain->SetBranchAddress("Electron_scaleFactor", Electron_scaleFactor, &b_Electron_scaleFactor);
   fChain->SetBranchAddress("Electron_scaleFactorUp", Electron_scaleFactorUp, &b_Electron_scaleFactorUp);
   fChain->SetBranchAddress("Electron_scaleFactorDown", Electron_scaleFactorDown, &b_Electron_scaleFactorDown);
   fChain->SetBranchAddress("Muon_scaleFactor", Muon_scaleFactor, &b_Muon_scaleFactor);
   fChain->SetBranchAddress("Muon_scaleFactorSyst", Muon_scaleFactorSyst, &b_Muon_scaleFactorSyst);
   fChain->SetBranchAddress("Muon_scaleFactorStat", Muon_scaleFactorStat, &b_Muon_scaleFactorStat);
   fChain->SetBranchAddress("Jet_bTagScaleFactorTight", Jet_bTagScaleFactorTight, &b_Jet_bTagScaleFactorTight);
   fChain->SetBranchAddress("Jet_bTagScaleFactorTightUp", Jet_bTagScaleFactorTightUp, &b_Jet_bTagScaleFactorTightUp);
   fChain->SetBranchAddress("Jet_bTagScaleFactorTightDown", Jet_bTagScaleFactorTightDown, &b_Jet_bTagScaleFactorTightDown);
   fChain->SetBranchAddress("Jet_bTagScaleFactorMedium", Jet_bTagScaleFactorMedium, &b_Jet_bTagScaleFactorMedium);
   fChain->SetBranchAddress("Jet_bTagScaleFactorMediumUp", Jet_bTagScaleFactorMediumUp, &b_Jet_bTagScaleFactorMediumUp);
   fChain->SetBranchAddress("Jet_bTagScaleFactorMediumDown", Jet_bTagScaleFactorMediumDown, &b_Jet_bTagScaleFactorMediumDown);
   fChain->SetBranchAddress("Jet_bTagScaleFactorLoose", Jet_bTagScaleFactorLoose, &b_Jet_bTagScaleFactorLoose);
   fChain->SetBranchAddress("Jet_bTagScaleFactorLooseUp", Jet_bTagScaleFactorLooseUp, &b_Jet_bTagScaleFactorLooseUp);
   fChain->SetBranchAddress("Jet_bTagScaleFactorLooseDown", Jet_bTagScaleFactorLooseDown, &b_Jet_bTagScaleFactorLooseDown);
   fChain->SetBranchAddress("Jet_puIdScaleFactorTight", Jet_puIdScaleFactorTight, &b_Jet_puIdScaleFactorTight);
   fChain->SetBranchAddress("Jet_puIdScaleFactorTightUp", Jet_puIdScaleFactorTightUp, &b_Jet_puIdScaleFactorTightUp);
   fChain->SetBranchAddress("Jet_puIdScaleFactorTightDown", Jet_puIdScaleFactorTightDown, &b_Jet_puIdScaleFactorTightDown);
   fChain->SetBranchAddress("Jet_puIdScaleFactorMedium", Jet_puIdScaleFactorMedium, &b_Jet_puIdScaleFactorMedium);
   fChain->SetBranchAddress("Jet_puIdScaleFactorMediumUp", Jet_puIdScaleFactorMediumUp, &b_Jet_puIdScaleFactorMediumUp);
   fChain->SetBranchAddress("Jet_puIdScaleFactorMediumDown", Jet_puIdScaleFactorMediumDown, &b_Jet_puIdScaleFactorMediumDown);
   fChain->SetBranchAddress("Jet_puIdScaleFactorLoose", Jet_puIdScaleFactorLoose, &b_Jet_puIdScaleFactorLoose);
   fChain->SetBranchAddress("Jet_puIdScaleFactorLooseUp", Jet_puIdScaleFactorLooseUp, &b_Jet_puIdScaleFactorLooseUp);
   fChain->SetBranchAddress("Jet_puIdScaleFactorLooseDown", Jet_puIdScaleFactorLooseDown, &b_Jet_puIdScaleFactorLooseDown);
   fChain->SetBranchAddress("Pileup_scaleFactor", &Pileup_scaleFactor, &b_Pileup_scaleFactor);
   fChain->SetBranchAddress("Pileup_scaleFactorUp", &Pileup_scaleFactorUp, &b_Pileup_scaleFactorUp);
   fChain->SetBranchAddress("Pileup_scaleFactorDown", &Pileup_scaleFactorDown, &b_Pileup_scaleFactorDown);
   Notify();
}

Bool_t Events::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Events::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Events::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Events_cxx
