#include "TFile.h"
#include "Utilities/HistManager.cc"

void DrawPlotPerm(int isampleyear = 3, int iobs = 0) {
  vector<string> obs{"PtPerm","bTagPerm"};
  vector<string> obstitle{"Pt Permutation Indices", "bTag Permutation "};
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  SampleTypes = {"FL300", "FL500", "FL1100"};
  // vector<string> StringRanges = rm.StringRanges;
  
  
  string Observable = obs[iobs];
  string ObservablesXTitle = obstitle[iobs];
  rm.InclusiveInit();

  string HistFilePath = "outputs/";
  string HistFilePrefix = SampleYear + "_Perm";
  string PlotNamePrefix = HistFilePrefix;
  TString filename = StandardNames::HistFileName(HistFilePath, HistFilePrefix, Observable);
  TFile* f = new TFile(filename, "READ");
  HistManager* AllPlots = new HistManager();
  AllPlots->SetSampleTypes(SampleTypes);
  // AllPlots->SetRegions(StringRanges);
  AllPlots->SetTitles(ObservablesXTitle);
  AllPlots->SetPrefix(PlotNamePrefix);
  AllPlots->SetObservable(Observable);
  AllPlots->NormalizePlot = true;
  AllPlots->DoLogy = false;
  AllPlots->ReadHistograms(f);

  for (unsigned ir = 0; ir < rm.StringRanges.size(); ++ir) {
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    AllPlots->CreateAuxiliaryPlots(ir);
    AllPlots->DrawPlot(ir, c1, isampleyear);
    TString PlotName = AllPlots->Plots[ir]->PlotName;
    TString pn  = "plots/" + PlotName + ".pdf";
    c1->SaveAs(pn);
    delete c1;
  }
}