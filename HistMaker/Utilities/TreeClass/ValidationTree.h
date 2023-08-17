//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Aug 14 18:09:49 2023 by ROOT version 6.24/08
// from TTree t/EventTree
// found on file: /eos/user/s/siluo/WPrimeAnalysis/Validation/2018_ttbar/2018_ttbar_287.root
//////////////////////////////////////////////////////////

#ifndef ValidationTree_h
#define ValidationTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
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
   vector<float>   *JetEta;
   vector<float>   *JetPhi;
   Float_t         METPt;
   Float_t         METPt_SU;
   Float_t         METPt_SD;
   Float_t         METPt_RU;
   Float_t         METPt_RD;
   Float_t         METPhi;
   Float_t         dPhiMetLep;
   vector<float>   *mT;
   vector<float>   *WPrimeMassSimpleFL;
   vector<float>   *WPrimeMassSimpleLL;
   vector<float>   *WPrimeMass;
   vector<float>   *Likelihood;
   vector<int>     *WPType;
   Int_t           nPU;
   Float_t         nTrueInt;
   Int_t           nPV;
   Int_t           nPVGood;

   // List of branches
   TBranch        *b_RegionIdentifier;   //!
   TBranch        *b_EventWeight;   //!
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
   TBranch        *b_JetEta;   //!
   TBranch        *b_JetPhi;   //!
   TBranch        *b_METPt;   //!
   TBranch        *b_METPt_SU;   //!
   TBranch        *b_METPt_SD;   //!
   TBranch        *b_METPt_RU;   //!
   TBranch        *b_METPt_RD;   //!
   TBranch        *b_METPhi;   //!
   TBranch        *b_dPhiMetLep;   //!
   TBranch        *b_mT;   //!
   TBranch        *b_WPrimeMassSimpleFL;   //!
   TBranch        *b_WPrimeMassSimpleLL;   //!
   TBranch        *b_WPrimeMass;   //!
   TBranch        *b_Likelihood;   //!
   TBranch        *b_WPType;   //!
   TBranch        *b_nPU;   //!
   TBranch        *b_nTrueInt;   //!
   TBranch        *b_nPV;   //!
   TBranch        *b_nPVGood;   //!

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
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/eos/user/s/siluo/WPrimeAnalysis/Validation/2018_ttbar/2018_ttbar_287.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/eos/user/s/siluo/WPrimeAnalysis/Validation/2018_ttbar/2018_ttbar_287.root");
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
   JetEta = 0;
   JetPhi = 0;
   mT = 0;
   WPrimeMassSimpleFL = 0;
   WPrimeMassSimpleLL = 0;
   WPrimeMass = 0;
   Likelihood = 0;
   WPType = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RegionIdentifier", RegionIdentifier, &b_RegionIdentifier);
   fChain->SetBranchAddress("EventWeight", EventWeight, &b_EventWeight);
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
   fChain->SetBranchAddress("JetEta", &JetEta, &b_JetEta);
   fChain->SetBranchAddress("JetPhi", &JetPhi, &b_JetPhi);
   fChain->SetBranchAddress("METPt", &METPt, &b_METPt);
   fChain->SetBranchAddress("METPt_SU", &METPt_SU, &b_METPt_SU);
   fChain->SetBranchAddress("METPt_SD", &METPt_SD, &b_METPt_SD);
   fChain->SetBranchAddress("METPt_RU", &METPt_RU, &b_METPt_RU);
   fChain->SetBranchAddress("METPt_RD", &METPt_RD, &b_METPt_RD);
   fChain->SetBranchAddress("METPhi", &METPhi, &b_METPhi);
   fChain->SetBranchAddress("dPhiMetLep", &dPhiMetLep, &b_dPhiMetLep);
   fChain->SetBranchAddress("mT", &mT, &b_mT);
   fChain->SetBranchAddress("WPrimeMassSimpleFL", &WPrimeMassSimpleFL, &b_WPrimeMassSimpleFL);
   fChain->SetBranchAddress("WPrimeMassSimpleLL", &WPrimeMassSimpleLL, &b_WPrimeMassSimpleLL);
   fChain->SetBranchAddress("WPrimeMass", &WPrimeMass, &b_WPrimeMass);
   fChain->SetBranchAddress("Likelihood", &Likelihood, &b_Likelihood);
   fChain->SetBranchAddress("WPType", &WPType, &b_WPType);
   fChain->SetBranchAddress("nPU", &nPU, &b_nPU);
   fChain->SetBranchAddress("nTrueInt", &nTrueInt, &b_nTrueInt);
   fChain->SetBranchAddress("nPV", &nPV, &b_nPV);
   fChain->SetBranchAddress("nPVGood", &nPVGood, &b_nPVGood);
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
