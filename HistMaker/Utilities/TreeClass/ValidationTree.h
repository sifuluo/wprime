//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jan 10 23:51:12 2024 by ROOT version 6.30/02
// from TTree t/EventTree
// found on file: root://eoshome-s.cern.ch//eos/user/s/siluo/WPrimeAnalysis/ValidationFitted/2018_ttbar/2018_ttbar_0.root
//////////////////////////////////////////////////////////

#ifndef ValidationTree_h
#define ValidationTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class ValidationTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           RegionIdentifier[9];
   Float_t         EventWeight[30];
   Float_t         HEMWeight;
   Float_t         LeptonPt;
   Float_t         LeptonPt_SU;
   Float_t         LeptonPt_SD;
   Float_t         LeptonPt_RU;
   Float_t         LeptonPt_RD;
   Float_t         LeptonEta;
   Float_t         LeptonPhi;
   vector<float>   *JetPt;
   vector<float>   *JetPt_SU;
   vector<float>   *JetPt_SD;
   vector<float>   *JetPt_RU;
   vector<float>   *JetPt_RD;
   vector<float>   *JetM;
   vector<float>   *JetM_SU;
   vector<float>   *JetM_SD;
   vector<float>   *JetM_RU;
   vector<float>   *JetM_RD;
   vector<float>   *JetEta;
   vector<float>   *JetPhi;
   vector<bool>    *JetbTag;
   vector<float>   *Jet_btagDeepFlavB;
   Float_t         METPt;
   Float_t         METPt_SU;
   Float_t         METPt_SD;
   Float_t         METPt_RU;
   Float_t         METPt_RD;
   Float_t         METPhi;
   Float_t         METPhi_SU;
   Float_t         METPhi_SD;
   Float_t         METPhi_RU;
   Float_t         METPhi_RD;
   Float_t         dPhiMetLep;
   Int_t           nPU;
   Float_t         nTrueInt;
   Int_t           nPV;
   Int_t           nPVGood;
   vector<float>   *mT;
   vector<float>   *WPrimeMassSimpleFL;
   vector<float>   *WPrimeMassSimpleLL;
   vector<float>   *TruePermTrueScales;
   Int_t           PermDiffCode;
   vector<int>     *Best_Perm;
   vector<int>     *Best_PtPerm;
   vector<int>     *Best_bTagPerm;
   vector<int>     *Best_WPType;
   vector<double>  *Best_PPtPerm;
   vector<double>  *Best_PbTag;
   vector<double>  *Best_Scales;
   vector<double>  *Best_PScales;
   vector<double>  *Best_Jet_btagDeepFlavB;
   vector<double>  *Best_TotalPScale;
   vector<double>  *Best_WPdR;
   vector<double>  *Best_PWPdR;
   vector<double>  *Best_HadW;
   vector<double>  *Best_HadT;
   vector<double>  *Best_LepT;
   vector<double>  *Best_PHadW;
   vector<double>  *Best_PHadT;
   vector<double>  *Best_PLep;
   vector<double>  *Best_PFitter;
   vector<double>  *Best_Likelihood;
   vector<double>  *Best_WPrimeMass;
   vector<int>     *True_Perm;
   vector<int>     *True_PtPerm;
   vector<int>     *True_bTagPerm;
   vector<int>     *True_WPType;
   vector<double>  *True_PPtPerm;
   vector<double>  *True_PbTag;
   vector<double>  *True_Scales;
   vector<double>  *True_PScales;
   vector<double>  *True_Jet_btagDeepFlavB;
   vector<double>  *True_TotalPScale;
   vector<double>  *True_WPdR;
   vector<double>  *True_PWPdR;
   vector<double>  *True_HadW;
   vector<double>  *True_HadT;
   vector<double>  *True_LepT;
   vector<double>  *True_PHadW;
   vector<double>  *True_PHadT;
   vector<double>  *True_PLep;
   vector<double>  *True_PFitter;
   vector<double>  *True_Likelihood;
   vector<double>  *True_WPrimeMass;
   vector<int>     *TT_Perm;
   vector<int>     *TT_PtPerm;
   vector<int>     *TT_bTagPerm;
   vector<int>     *TT_WPType;
   vector<double>  *TT_PPtPerm;
   vector<double>  *TT_PbTag;
   vector<double>  *TT_Scales;
   vector<double>  *TT_PScales;
   vector<double>  *TT_Jet_btagDeepFlavB;
   vector<double>  *TT_TotalPScale;
   vector<double>  *TT_WPdR;
   vector<double>  *TT_PWPdR;
   vector<double>  *TT_HadW;
   vector<double>  *TT_HadT;
   vector<double>  *TT_LepT;
   vector<double>  *TT_PHadW;
   vector<double>  *TT_PHadT;
   vector<double>  *TT_PLep;
   vector<double>  *TT_PFitter;
   vector<double>  *TT_Likelihood;
   vector<double>  *TT_WPrimeMass;

   // List of branches
   TBranch        *b_RegionIdentifier;   //!
   TBranch        *b_EventWeight;   //!
   TBranch        *b_HEMWeight;   //!
   TBranch        *b_LeptonPt;   //!
   TBranch        *b_LeptonPt_SU;   //!
   TBranch        *b_LeptonPt_SD;   //!
   TBranch        *b_LeptonPt_RU;   //!
   TBranch        *b_LeptonPt_RD;   //!
   TBranch        *b_LeptonEta;   //!
   TBranch        *b_LeptonPhi;   //!
   TBranch        *b_JetPt;   //!
   TBranch        *b_JetPt_SU;   //!
   TBranch        *b_JetPt_SD;   //!
   TBranch        *b_JetPt_RU;   //!
   TBranch        *b_JetPt_RD;   //!
   TBranch        *b_JetM;   //!
   TBranch        *b_JetM_SU;   //!
   TBranch        *b_JetM_SD;   //!
   TBranch        *b_JetM_RU;   //!
   TBranch        *b_JetM_RD;   //!
   TBranch        *b_JetEta;   //!
   TBranch        *b_JetPhi;   //!
   TBranch        *b_JetbTag;   //!
   TBranch        *b_Jet_btagDeepFlavB;   //!
   TBranch        *b_METPt;   //!
   TBranch        *b_METPt_SU;   //!
   TBranch        *b_METPt_SD;   //!
   TBranch        *b_METPt_RU;   //!
   TBranch        *b_METPt_RD;   //!
   TBranch        *b_METPhi;   //!
   TBranch        *b_METPhi_SU;   //!
   TBranch        *b_METPhi_SD;   //!
   TBranch        *b_METPhi_RU;   //!
   TBranch        *b_METPhi_RD;   //!
   TBranch        *b_dPhiMetLep;   //!
   TBranch        *b_nPU;   //!
   TBranch        *b_nTrueInt;   //!
   TBranch        *b_nPV;   //!
   TBranch        *b_nPVGood;   //!
   TBranch        *b_mT;   //!
   TBranch        *b_WPrimeMassSimpleFL;   //!
   TBranch        *b_WPrimeMassSimpleLL;   //!
   TBranch        *b_TruePermTrueScales;   //!
   TBranch        *b_PermDiffCode;   //!
   TBranch        *b_Best_Perm;   //!
   TBranch        *b_Best_PtPerm;   //!
   TBranch        *b_Best_bTagPerm;   //!
   TBranch        *b_Best_WPType;   //!
   TBranch        *b_Best_PPtPerm;   //!
   TBranch        *b_Best_PbTag;   //!
   TBranch        *b_Best_Scales;   //!
   TBranch        *b_Best_PScales;   //!
   TBranch        *b_Best_Jet_btagDeepFlavB;   //!
   TBranch        *b_Best_TotalPScale;   //!
   TBranch        *b_Best_WPdR;   //!
   TBranch        *b_Best_PWPdR;   //!
   TBranch        *b_Best_HadW;   //!
   TBranch        *b_Best_HadT;   //!
   TBranch        *b_Best_LepT;   //!
   TBranch        *b_Best_PHadW;   //!
   TBranch        *b_Best_PHadT;   //!
   TBranch        *b_Best_PLep;   //!
   TBranch        *b_Best_PFitter;   //!
   TBranch        *b_Best_Likelihood;   //!
   TBranch        *b_Best_WPrimeMass;   //!
   TBranch        *b_True_Perm;   //!
   TBranch        *b_True_PtPerm;   //!
   TBranch        *b_True_bTagPerm;   //!
   TBranch        *b_True_WPType;   //!
   TBranch        *b_True_PPtPerm;   //!
   TBranch        *b_True_PbTag;   //!
   TBranch        *b_True_Scales;   //!
   TBranch        *b_True_PScales;   //!
   TBranch        *b_True_Jet_btagDeepFlavB;   //!
   TBranch        *b_True_TotalPScale;   //!
   TBranch        *b_True_WPdR;   //!
   TBranch        *b_True_PWPdR;   //!
   TBranch        *b_True_HadW;   //!
   TBranch        *b_True_HadT;   //!
   TBranch        *b_True_LepT;   //!
   TBranch        *b_True_PHadW;   //!
   TBranch        *b_True_PHadT;   //!
   TBranch        *b_True_PLep;   //!
   TBranch        *b_True_PFitter;   //!
   TBranch        *b_True_Likelihood;   //!
   TBranch        *b_True_WPrimeMass;   //!
   TBranch        *b_TT_Perm;   //!
   TBranch        *b_TT_PtPerm;   //!
   TBranch        *b_TT_bTagPerm;   //!
   TBranch        *b_TT_WPType;   //!
   TBranch        *b_TT_PPtPerm;   //!
   TBranch        *b_TT_PbTag;   //!
   TBranch        *b_TT_Scales;   //!
   TBranch        *b_TT_PScales;   //!
   TBranch        *b_TT_Jet_btagDeepFlavB;   //!
   TBranch        *b_TT_TotalPScale;   //!
   TBranch        *b_TT_WPdR;   //!
   TBranch        *b_TT_PWPdR;   //!
   TBranch        *b_TT_HadW;   //!
   TBranch        *b_TT_HadT;   //!
   TBranch        *b_TT_LepT;   //!
   TBranch        *b_TT_PHadW;   //!
   TBranch        *b_TT_PHadT;   //!
   TBranch        *b_TT_PLep;   //!
   TBranch        *b_TT_PFitter;   //!
   TBranch        *b_TT_Likelihood;   //!
   TBranch        *b_TT_WPrimeMass;   //!

   ValidationTree(TTree *tree=0);
   virtual ~ValidationTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ValidationTree_cxx
ValidationTree::ValidationTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root://eoshome-s.cern.ch//eos/user/s/siluo/WPrimeAnalysis/ValidationFitted/2018_ttbar/2018_ttbar_0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root://eoshome-s.cern.ch//eos/user/s/siluo/WPrimeAnalysis/ValidationFitted/2018_ttbar/2018_ttbar_0.root");
      }
      f->GetObject("t",tree);

   }
   Init(tree);
}

ValidationTree::~ValidationTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ValidationTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ValidationTree::LoadTree(Long64_t entry)
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

void ValidationTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   JetPt = 0;
   JetPt_SU = 0;
   JetPt_SD = 0;
   JetPt_RU = 0;
   JetPt_RD = 0;
   JetM = 0;
   JetM_SU = 0;
   JetM_SD = 0;
   JetM_RU = 0;
   JetM_RD = 0;
   JetEta = 0;
   JetPhi = 0;
   JetbTag = 0;
   Jet_btagDeepFlavB = 0;
   mT = 0;
   WPrimeMassSimpleFL = 0;
   WPrimeMassSimpleLL = 0;
   TruePermTrueScales = 0;
   Best_Perm = 0;
   Best_PtPerm = 0;
   Best_bTagPerm = 0;
   Best_WPType = 0;
   Best_PPtPerm = 0;
   Best_PbTag = 0;
   Best_Scales = 0;
   Best_PScales = 0;
   Best_Jet_btagDeepFlavB = 0;
   Best_TotalPScale = 0;
   Best_WPdR = 0;
   Best_PWPdR = 0;
   Best_HadW = 0;
   Best_HadT = 0;
   Best_LepT = 0;
   Best_PHadW = 0;
   Best_PHadT = 0;
   Best_PLep = 0;
   Best_PFitter = 0;
   Best_Likelihood = 0;
   Best_WPrimeMass = 0;
   True_Perm = 0;
   True_PtPerm = 0;
   True_bTagPerm = 0;
   True_WPType = 0;
   True_PPtPerm = 0;
   True_PbTag = 0;
   True_Scales = 0;
   True_PScales = 0;
   True_Jet_btagDeepFlavB = 0;
   True_TotalPScale = 0;
   True_WPdR = 0;
   True_PWPdR = 0;
   True_HadW = 0;
   True_HadT = 0;
   True_LepT = 0;
   True_PHadW = 0;
   True_PHadT = 0;
   True_PLep = 0;
   True_PFitter = 0;
   True_Likelihood = 0;
   True_WPrimeMass = 0;
   TT_Perm = 0;
   TT_PtPerm = 0;
   TT_bTagPerm = 0;
   TT_WPType = 0;
   TT_PPtPerm = 0;
   TT_PbTag = 0;
   TT_Scales = 0;
   TT_PScales = 0;
   TT_Jet_btagDeepFlavB = 0;
   TT_TotalPScale = 0;
   TT_WPdR = 0;
   TT_PWPdR = 0;
   TT_HadW = 0;
   TT_HadT = 0;
   TT_LepT = 0;
   TT_PHadW = 0;
   TT_PHadT = 0;
   TT_PLep = 0;
   TT_PFitter = 0;
   TT_Likelihood = 0;
   TT_WPrimeMass = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RegionIdentifier", RegionIdentifier, &b_RegionIdentifier);
   fChain->SetBranchAddress("EventWeight", EventWeight, &b_EventWeight);
   fChain->SetBranchAddress("HEMWeight", &HEMWeight, &b_HEMWeight);
   fChain->SetBranchAddress("LeptonPt", &LeptonPt, &b_LeptonPt);
   fChain->SetBranchAddress("LeptonPt_SU", &LeptonPt_SU, &b_LeptonPt_SU);
   fChain->SetBranchAddress("LeptonPt_SD", &LeptonPt_SD, &b_LeptonPt_SD);
   fChain->SetBranchAddress("LeptonPt_RU", &LeptonPt_RU, &b_LeptonPt_RU);
   fChain->SetBranchAddress("LeptonPt_RD", &LeptonPt_RD, &b_LeptonPt_RD);
   fChain->SetBranchAddress("LeptonEta", &LeptonEta, &b_LeptonEta);
   fChain->SetBranchAddress("LeptonPhi", &LeptonPhi, &b_LeptonPhi);
   fChain->SetBranchAddress("JetPt", &JetPt, &b_JetPt);
   fChain->SetBranchAddress("JetPt_SU", &JetPt_SU, &b_JetPt_SU);
   fChain->SetBranchAddress("JetPt_SD", &JetPt_SD, &b_JetPt_SD);
   fChain->SetBranchAddress("JetPt_RU", &JetPt_RU, &b_JetPt_RU);
   fChain->SetBranchAddress("JetPt_RD", &JetPt_RD, &b_JetPt_RD);
   fChain->SetBranchAddress("JetM", &JetM, &b_JetM);
   fChain->SetBranchAddress("JetM_SU", &JetM_SU, &b_JetM_SU);
   fChain->SetBranchAddress("JetM_SD", &JetM_SD, &b_JetM_SD);
   fChain->SetBranchAddress("JetM_RU", &JetM_RU, &b_JetM_RU);
   fChain->SetBranchAddress("JetM_RD", &JetM_RD, &b_JetM_RD);
   fChain->SetBranchAddress("JetEta", &JetEta, &b_JetEta);
   fChain->SetBranchAddress("JetPhi", &JetPhi, &b_JetPhi);
   fChain->SetBranchAddress("JetbTag", &JetbTag, &b_JetbTag);
   fChain->SetBranchAddress("Jet_btagDeepFlavB", &Jet_btagDeepFlavB, &b_Jet_btagDeepFlavB);
   fChain->SetBranchAddress("METPt", &METPt, &b_METPt);
   fChain->SetBranchAddress("METPt_SU", &METPt_SU, &b_METPt_SU);
   fChain->SetBranchAddress("METPt_SD", &METPt_SD, &b_METPt_SD);
   fChain->SetBranchAddress("METPt_RU", &METPt_RU, &b_METPt_RU);
   fChain->SetBranchAddress("METPt_RD", &METPt_RD, &b_METPt_RD);
   fChain->SetBranchAddress("METPhi", &METPhi, &b_METPhi);
   fChain->SetBranchAddress("METPhi_SU", &METPhi_SU, &b_METPhi_SU);
   fChain->SetBranchAddress("METPhi_SD", &METPhi_SD, &b_METPhi_SD);
   fChain->SetBranchAddress("METPhi_RU", &METPhi_RU, &b_METPhi_RU);
   fChain->SetBranchAddress("METPhi_RD", &METPhi_RD, &b_METPhi_RD);
   fChain->SetBranchAddress("dPhiMetLep", &dPhiMetLep, &b_dPhiMetLep);
   fChain->SetBranchAddress("nPU", &nPU, &b_nPU);
   fChain->SetBranchAddress("nTrueInt", &nTrueInt, &b_nTrueInt);
   fChain->SetBranchAddress("nPV", &nPV, &b_nPV);
   fChain->SetBranchAddress("nPVGood", &nPVGood, &b_nPVGood);
   fChain->SetBranchAddress("mT", &mT, &b_mT);
   fChain->SetBranchAddress("WPrimeMassSimpleFL", &WPrimeMassSimpleFL, &b_WPrimeMassSimpleFL);
   fChain->SetBranchAddress("WPrimeMassSimpleLL", &WPrimeMassSimpleLL, &b_WPrimeMassSimpleLL);
   fChain->SetBranchAddress("TruePermTrueScales", &TruePermTrueScales, &b_TruePermTrueScales);
   fChain->SetBranchAddress("PermDiffCode", &PermDiffCode, &b_PermDiffCode);
   fChain->SetBranchAddress("Best_Perm", &Best_Perm, &b_Best_Perm);
   fChain->SetBranchAddress("Best_PtPerm", &Best_PtPerm, &b_Best_PtPerm);
   fChain->SetBranchAddress("Best_bTagPerm", &Best_bTagPerm, &b_Best_bTagPerm);
   fChain->SetBranchAddress("Best_WPType", &Best_WPType, &b_Best_WPType);
   fChain->SetBranchAddress("Best_PPtPerm", &Best_PPtPerm, &b_Best_PPtPerm);
   fChain->SetBranchAddress("Best_PbTag", &Best_PbTag, &b_Best_PbTag);
   fChain->SetBranchAddress("Best_Scales", &Best_Scales, &b_Best_Scales);
   fChain->SetBranchAddress("Best_PScales", &Best_PScales, &b_Best_PScales);
   fChain->SetBranchAddress("Best_Jet_btagDeepFlavB", &Best_Jet_btagDeepFlavB, &b_Best_Jet_btagDeepFlavB);
   fChain->SetBranchAddress("Best_TotalPScale", &Best_TotalPScale, &b_Best_TotalPScale);
   fChain->SetBranchAddress("Best_WPdR", &Best_WPdR, &b_Best_WPdR);
   fChain->SetBranchAddress("Best_PWPdR", &Best_PWPdR, &b_Best_PWPdR);
   fChain->SetBranchAddress("Best_HadW", &Best_HadW, &b_Best_HadW);
   fChain->SetBranchAddress("Best_HadT", &Best_HadT, &b_Best_HadT);
   fChain->SetBranchAddress("Best_LepT", &Best_LepT, &b_Best_LepT);
   fChain->SetBranchAddress("Best_PHadW", &Best_PHadW, &b_Best_PHadW);
   fChain->SetBranchAddress("Best_PHadT", &Best_PHadT, &b_Best_PHadT);
   fChain->SetBranchAddress("Best_PLep", &Best_PLep, &b_Best_PLep);
   fChain->SetBranchAddress("Best_PFitter", &Best_PFitter, &b_Best_PFitter);
   fChain->SetBranchAddress("Best_Likelihood", &Best_Likelihood, &b_Best_Likelihood);
   fChain->SetBranchAddress("Best_WPrimeMass", &Best_WPrimeMass, &b_Best_WPrimeMass);
   fChain->SetBranchAddress("True_Perm", &True_Perm, &b_True_Perm);
   fChain->SetBranchAddress("True_PtPerm", &True_PtPerm, &b_True_PtPerm);
   fChain->SetBranchAddress("True_bTagPerm", &True_bTagPerm, &b_True_bTagPerm);
   fChain->SetBranchAddress("True_WPType", &True_WPType, &b_True_WPType);
   fChain->SetBranchAddress("True_PPtPerm", &True_PPtPerm, &b_True_PPtPerm);
   fChain->SetBranchAddress("True_PbTag", &True_PbTag, &b_True_PbTag);
   fChain->SetBranchAddress("True_Scales", &True_Scales, &b_True_Scales);
   fChain->SetBranchAddress("True_PScales", &True_PScales, &b_True_PScales);
   fChain->SetBranchAddress("True_Jet_btagDeepFlavB", &True_Jet_btagDeepFlavB, &b_True_Jet_btagDeepFlavB);
   fChain->SetBranchAddress("True_TotalPScale", &True_TotalPScale, &b_True_TotalPScale);
   fChain->SetBranchAddress("True_WPdR", &True_WPdR, &b_True_WPdR);
   fChain->SetBranchAddress("True_PWPdR", &True_PWPdR, &b_True_PWPdR);
   fChain->SetBranchAddress("True_HadW", &True_HadW, &b_True_HadW);
   fChain->SetBranchAddress("True_HadT", &True_HadT, &b_True_HadT);
   fChain->SetBranchAddress("True_LepT", &True_LepT, &b_True_LepT);
   fChain->SetBranchAddress("True_PHadW", &True_PHadW, &b_True_PHadW);
   fChain->SetBranchAddress("True_PHadT", &True_PHadT, &b_True_PHadT);
   fChain->SetBranchAddress("True_PLep", &True_PLep, &b_True_PLep);
   fChain->SetBranchAddress("True_PFitter", &True_PFitter, &b_True_PFitter);
   fChain->SetBranchAddress("True_Likelihood", &True_Likelihood, &b_True_Likelihood);
   fChain->SetBranchAddress("True_WPrimeMass", &True_WPrimeMass, &b_True_WPrimeMass);
   fChain->SetBranchAddress("TT_Perm", &TT_Perm, &b_TT_Perm);
   fChain->SetBranchAddress("TT_PtPerm", &TT_PtPerm, &b_TT_PtPerm);
   fChain->SetBranchAddress("TT_bTagPerm", &TT_bTagPerm, &b_TT_bTagPerm);
   fChain->SetBranchAddress("TT_WPType", &TT_WPType, &b_TT_WPType);
   fChain->SetBranchAddress("TT_PPtPerm", &TT_PPtPerm, &b_TT_PPtPerm);
   fChain->SetBranchAddress("TT_PbTag", &TT_PbTag, &b_TT_PbTag);
   fChain->SetBranchAddress("TT_Scales", &TT_Scales, &b_TT_Scales);
   fChain->SetBranchAddress("TT_PScales", &TT_PScales, &b_TT_PScales);
   fChain->SetBranchAddress("TT_Jet_btagDeepFlavB", &TT_Jet_btagDeepFlavB, &b_TT_Jet_btagDeepFlavB);
   fChain->SetBranchAddress("TT_TotalPScale", &TT_TotalPScale, &b_TT_TotalPScale);
   fChain->SetBranchAddress("TT_WPdR", &TT_WPdR, &b_TT_WPdR);
   fChain->SetBranchAddress("TT_PWPdR", &TT_PWPdR, &b_TT_PWPdR);
   fChain->SetBranchAddress("TT_HadW", &TT_HadW, &b_TT_HadW);
   fChain->SetBranchAddress("TT_HadT", &TT_HadT, &b_TT_HadT);
   fChain->SetBranchAddress("TT_LepT", &TT_LepT, &b_TT_LepT);
   fChain->SetBranchAddress("TT_PHadW", &TT_PHadW, &b_TT_PHadW);
   fChain->SetBranchAddress("TT_PHadT", &TT_PHadT, &b_TT_PHadT);
   fChain->SetBranchAddress("TT_PLep", &TT_PLep, &b_TT_PLep);
   fChain->SetBranchAddress("TT_PFitter", &TT_PFitter, &b_TT_PFitter);
   fChain->SetBranchAddress("TT_Likelihood", &TT_Likelihood, &b_TT_Likelihood);
   fChain->SetBranchAddress("TT_WPrimeMass", &TT_WPrimeMass, &b_TT_WPrimeMass);
   Notify();
}

Bool_t ValidationTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ValidationTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ValidationTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ValidationTree_cxx
