#include "TFile.h"
#include "Utilities/RatioPlot.cc"
#include "Utilities/Regions.cc"

void DrawPlotCombine(int it = 0, int ir = 0) {
  vector<string> Regions = {"1153", "1163", "2153", "2163"};
  string Region = Regions[ir];
  string subfolder = "BinW20/";

  string InputPath = "CombinedFiles/" + subfolder;
  string InputFile = "SimpleShapes_Wprime";

  string OutputPath = "Brazil/" + subfolder;

  if (it == 1) {
    // InputPath = "../SifuCombine/" + subfolder;
    InputPath = "CombinedFiles/Denis/" + subfolder;
  }
  if (it == 2) {
    InputPath = "/afs/cern.ch/user/s/siluo/b2g/wprime/SifuCombine/2018/v4/";
  }
  TString InputFileName = InputPath + InputFile + Region + ".root";
  cout << "Reading from file: " << InputFileName << endl;
  TFile *f = new TFile(InputFileName,"READ");
  vector<string> Groups = {"ttbar", "wjets", "single_top", "diboson", "M300", "M400", "M500", "M600", "M700", "M800", "M900", "M1000", "M1100"};
  rm.MakeCombineVariations();
  if (it == 0) rm.AddVariationSource("RwStat2018" + Region);
  vector<double> LegendPos = {0.65,0.65,0.9,0.9};

  RatioPlot *rp = new RatioPlot("wprime", true, "m(W')", "Number of Entries / 20 GeV");
  rp->SetVariations(rm.Variations);
  rp->Legend(LegendPos);
  
  vector<TH1F*> Hists;
  for (unsigned ig = 0; ig < Groups.size(); ++ig) {
    int Type = 1; // MC
    if (ig > 2) Type = 2;
    for (unsigned iv = 0; iv < rm.Variations.size(); ++iv) {
      TString hn = Groups[ig] + "_Wprime" + Region + "_" + rm.Variations[iv];
      TH1F* h = (TH1F*) f->Get(hn);
      if (h == nullptr) continue;
      Hists.push_back(h);
      h->GetXaxis()->SetRangeUser(20,2000);
      // cout << "HistName = " << h->GetName() << " , Integral = " << h->Integral() << endl;
      rp->AddHist(Groups[ig], h, Type, iv);
    }
  }
  dlib.AddLegend(rp->leg,1);
  rp->PrepHists();
  rp->CreateRatioPlots();
  rp->CreateErrorGraphs();

  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  rp->SetPad(c1);
  rp->DrawPlot(3);

  TString PlotName = OutputPath + "MyPlots" + Region;
  if (it == 1) PlotName = OutputPath + "DenisPlots" + Region;
  c1->SaveAs(PlotName + ".pdf");

  rp->SaveInfos(PlotName);


}