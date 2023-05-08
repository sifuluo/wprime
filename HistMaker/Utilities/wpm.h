//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Mar 13 14:01:11 2023 by ROOT version 6.24/08
// from TTree t/EventTree
// found on file: 2018_FL500_1.root
//////////////////////////////////////////////////////////

#ifndef wpm_h
#define wpm_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class wpm {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           RegionIdentifier[9];
   Float_t         EventWeight[18];
   Float_t         SimpleWprimeFL;
   Float_t         SimpleWprimeLL;
   Float_t         WprimeFL;
   Float_t         WprimeLL;
   Bool_t          WprimeType;
   Float_t         Likelihood;

   // List of branches
   TBranch        *b_RegionIdentifier;   //!
   TBranch        *b_EventWeight;   //!
   TBranch        *b_SimpleWprimeFL;   //!
   TBranch        *b_SimpleWprimeLL;   //!
   TBranch        *b_WprimeFL;   //!
   TBranch        *b_WprimeLL;   //!
   TBranch        *b_WprimeType;   //!
   TBranch        *b_Likelihood;   //!

   wpm(TTree *tree=0);
   virtual ~wpm();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef wpm_cxx
wpm::wpm(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("2018_FL500_1.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("2018_FL500_1.root");
      }
      f->GetObject("t",tree);

   }
   Init(tree);
}

wpm::~wpm()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t wpm::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t wpm::LoadTree(Long64_t entry)
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

void wpm::Init(TTree *tree)
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

   fChain->SetBranchAddress("RegionIdentifier", RegionIdentifier, &b_RegionIdentifier);
   fChain->SetBranchAddress("EventWeight", EventWeight, &b_EventWeight);
   fChain->SetBranchAddress("SimpleWprimeFL", &SimpleWprimeFL, &b_SimpleWprimeFL);
   fChain->SetBranchAddress("SimpleWprimeLL", &SimpleWprimeLL, &b_SimpleWprimeLL);
   fChain->SetBranchAddress("WprimeFL", &WprimeFL, &b_WprimeFL);
   fChain->SetBranchAddress("WprimeLL", &WprimeLL, &b_WprimeLL);
   fChain->SetBranchAddress("WprimeType", &WprimeType, &b_WprimeType);
   fChain->SetBranchAddress("Likelihood", &Likelihood, &b_Likelihood);
   Notify();
}

Bool_t wpm::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void wpm::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t wpm::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef wpm_cxx
