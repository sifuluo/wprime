#include <string>
#include <iostream>
#include <fstream>

#include "TH1.h"
#include "TFile.h"
#include "TString.h"
#include "TChain.h"

// #include "Utilities/Analyzer.cc"
#include "Utilities/NanoAODReader.cc"

void GetnTrueInt(int isampleyear = 3, int isampletype = 2, int itrigger = 0, int ifile = 0) {
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->PUEvaluation = true;
  if (ifile < 0) conf->InputFile = "All";
  else conf->FilesPerJob = 20;
  // if (isampletype != 16) conf->DASInput = true;
  NanoAODReader *r = new NanoAODReader(conf);
  // TString outputfilename = "PUReweight/" + conf->SampleType + "_" + conf->SampleYear + "_" +  + ".root";
  if (ifile == 0) ifile = -2;
  TString outputfilename = Form("PUReweight/nTrueInt/%s_%s_%i.root",conf->SampleYear.c_str(), conf->SampleType.c_str(), ifile);
  if (conf->InputFile == "All") outputfilename = Form("PUReweight/nTrueInt/%s_%s.root",conf->SampleYear.c_str(), conf->SampleType.c_str());
  TFile* out = new TFile(outputfilename,"RECREATE");
  out->cd();
  TH1D* mcpu = new TH1D("mcpu","mcpu",99,0,99);
  r->chain->Draw("Pileup_nTrueInt + 1 >> mcpu ");

  out->Write();
  out->Save();
}
