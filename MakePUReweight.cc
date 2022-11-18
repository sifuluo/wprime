#include <string>
#include <iostream>
#include <fstream>

#include "TH1.h"
#include "TFile.h"
#include "TString.h"
#include "TChain.h"

#include "Utilities/Analyzer.cc"

void MakePUReweight(int isampleyear = 3, int isampletype = 16, int itrigger = 0, int ifile = 0) {
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->PUEvaluation = true;
  conf->FilesPerJob = 50;
  TString datafilename = Constants::DataPileupHist[isampleyear];
  TFile * datafile = new TFile(datafilename,"READ");
  NanoAODReader *r = new NanoAODReader(conf);
  TString outputfilename = "PUReweight/" + conf->SampleType + "_" + conf->SampleYear + ".root";
  TFile* out = new TFile(outputfilename,"RECREATE");
  out->cd();
  TH1D* mcpu = new TH1D("mcpu","mcpu",99,0,99);
  r->chain->Draw("Pileup_nTrueInt + 1 >> mcpu ");
  TH1D* mcpunorm = (TH1D*)mcpu->Clone();
  mcpunorm->SetName("mcpunorm");
  mcpunorm->Scale(1./mcpunorm->Integral());
  TH1D* datahist = (TH1D*) datafile->Get("pileup")->Clone();
  datahist->SetName("datapu");
  TH1D* weight = (TH1D*) datahist->Clone();
  weight->Scale(1./weight->Integral());
  weight->SetName("weight");
  weight->Divide(mcpunorm);

  out->Write();
  out->Save();
}
