#include <string>
#include <iostream>
#include <fstream>

#include "TH1.h"
#include "TFile.h"
#include "TString.h"
#include "TChain.h"

#include "Utilities/Constants.cc"

void MakePUReweight() {
  TChain *c = new TChain("t");
  TString infilepath = "/eos/user/s/siluo/WPrimeAnalysis/PUEval/2016_ttbar_SE";
  TString infilename = "2016_ttbar_SE_-1.root";
  c->Add(infilepath + infilename);
  int nPU, nPV, nPVGood;
  double nTrueInt;

  c->SetBranchAddress("nPU", &nPU);
  c->SetBranchAddress("nTrueInt", &nTrueInt);
  c->SetBranchAddress("nPV", &nPV);
  c->SetBranchAddress("nPVGood", &nPVGood);

  TString datafilename = "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/UltraLegacy/PileupHistogram-goldenJSON-13tev-2016-postVFP-69200ub-99bins.root";
  TFile * datafile = new TFile(datafilename,"READ");
  TH1D* datahist = (TH1D*) datafile->Get("pileup");

  TFile* out = new TFile("MCReweight.root","RECREATE");
  out->cd();

  // Create MC PU histogram
  TH1D* mcpu = new TH1D("mcpu","mcpu",99,0.,99);
  c->Draw("nTrueInt >> mcpu");

  TH1D* weight = (TH1D*) datahist->Clone();
  weight->Divide(mcpu);
  weight->SetName("PUWeight");

  for (Long64_t i = 0; i < c->GetEntries(); ++i) {

  }
}
