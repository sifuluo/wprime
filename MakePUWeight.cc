#include "TFile.h"
#include "TString.h"

#include "Utilities/Configs.cc"
#include "Utilities/Constants.cc"

void MakePUWeight(int isampleyear, int isampletype) {
  Configs *conf = new Configs(isampleyear, isampletype,0,0);
  TString datafilename = Constants::DataPileupHist[isampleyear];
  TFile * datafile = new TFile(datafilename,"READ");

  TString mcfilename = Form("PUReweight/nTrueInt/%s_%s.root",conf->SampleYear.c_str(), conf->SampleType.c_str());
  TFile * mcfile = new TFile(mcfilename,"READ");

  TString weightfilename = Form("PUReweight/%s_%s.root",conf->SampleYear.c_str(), conf->SampleType.c_str());
  TFile *weightfile = new TFile(weightfilename,"RECREATE");

  weightfile->cd();
  TH1D* mcpu = (TH1D*) mcfile->Get("mcpu")->Clone();
  TH1D* mcpunorm = (TH1D*)mcpu->Clone();
  mcpunorm->SetName("mcpunorm");
  mcpunorm->Scale(1./mcpunorm->Integral());

  TH1D* datapu = (TH1D*) datafile->Get("pileup")->Clone();
  datapu->SetName("datapu");
  TH1D* weight = (TH1D*) datapu->Clone();
  weight->Scale(1./weight->Integral());
  weight->SetName("weight");
  weight->Divide(mcpunorm);

  weightfile->Write();
  weightfile->Save();
}

void MakePUWeight() {
  for (unsigned i = 2; i <= 16; ++i) {
    MakePUWeight(3,i);
  }
}
