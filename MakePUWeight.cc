#include "TFile.h"
#include "TString.h"

#include "Utilities/Configs.cc"

TString DataPileupFile(int isy_, int ixsec_ = 1) {
  // "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/UltraLegacy/PileupHistogram-goldenJSON-13tev-2016-preVFP-69200ub-99bins.root",
  // "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/UltraLegacy/PileupHistogram-goldenJSON-13tev-2016-postVFP-69200ub-99bins.root",
  // "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/PileUp/UltraLegacy/PileupHistogram-goldenJSON-13tev-2017-69200ub-99bins.root",
  // "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions18/13TeV/PileUp/UltraLegacy/PileupHistogram-goldenJSON-13tev-2018-69200ub-99bins.root"
  vector<string> years_short, years_long, xsecs;
  years_short = vector<string>{"16","16","17","18"};
  years_long  = vector<string>{"2016-preVFP-","2016-postVFP-","2017-","2018-"};
  xsecs = vector<string>{"66000","69200","72400","80000"};

  string path1 = "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/";
  string path2 = "Collisions" + years_short[isy_] + "/13TeV/PileUp/UltraLegacy/";
  string path3 = "PileupHistogram-goldenJSON-13tev-" + years_long[isy_] + xsecs[ixsec_] + "ub-99bins.root";
  string path = path1 + path2 + path3;
  TString out = path;
  return out;
}

void MakePUWeight(int isampleyear, int isampletype) {
  Configs *conf = new Configs(isampleyear, isampletype,0,0);
  TString mcfilename = Form("PUReweight/nTrueInt/%s_%s.root",conf->SampleYear.c_str(), conf->SampleType.c_str());
  TFile * mcfile = new TFile(mcfilename,"READ");

  TString weightfilename = Form("PUReweight/%s_%s.root",conf->SampleYear.c_str(), conf->SampleType.c_str());
  TFile *weightfile = new TFile(weightfilename,"RECREATE");

  weightfile->cd();
  TH1D* mcpu = (TH1D*) mcfile->Get("mcpu")->Clone();
  TH1D* mcpunorm = (TH1D*)mcpu->Clone();
  mcpunorm->SetName("mcpunorm");
  mcpunorm->Scale(1./mcpunorm->Integral());

  for (unsigned ixsec = 0; ixsec < 4; ++ixsec) {
    TString datafilename = DataPileupFile(isampleyear, ixsec);
    TFile * datafile = new TFile(datafilename,"READ");
    weightfile->cd();
    TString datapuname = Form("datapu%i",ixsec);
    TString weightname = Form("weight%i",ixsec);
    TH1D* datapu = (TH1D*) datafile->Get("pileup")->Clone();
    datapu->SetName(datapuname);
    TH1D* weight = (TH1D*) datapu->Clone();
    weight->Scale(1./weight->Integral());
    weight->SetName(weightname);
    weight->Divide(mcpunorm);
  }
  weightfile->Write();
  weightfile->Save();
}

void MakePUWeight() {
  for (unsigned i = 2; i <= 16; ++i) {
    MakePUWeight(3,i);
  }
}
