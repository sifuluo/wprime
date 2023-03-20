#include "TFile.h"
#include "Utilities/HistManager.cc"

void WprimeMassDraw(int isampleyear = 3) {
  vector<string> obs{"SimpleWprimeFL","SimpleWprimeLL","WprimeFL","WprimeLL","WprimeFLPicked","WprimeLLPicked","Likelihood"};
  vector<string> ObservablesXTitle{"Simple m(W'_{FL})[GeV]","Simple m(W'_{LL})[GeV]"
  , "m(W'_{FL})[GeV]", "m(W'_{LL})[GeV]", "Chosen m(W'_{FL})[GeV]", "Chosen m(W'_{FL})[GeV]"
  ,"Likelihood"
  };
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = {"FL500"};
  vector<string> StringRanges = rm.StringRanges;

  TString filename = "outputs/" + SampleYear + "_WPMasses.root";
  TFile* f = new TFile(filename, "READ");
  HistManager* AllPlots = new HistManager();
  AllPlots->SetSampleTypes(SampleTypes);
  AllPlots->ReadHistograms(obs, f);
  
  for (unsigned ir = 0; ir < StringRanges.size(); ++ir) {
    // if (rm.Ranges[ir].b1 != 1153) continue;
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    TString PlotName = "2018_WprimeMass_" + StringRanges[ir];
    RatioPlot *rp = new RatioPlot("2018", true, "m(W') [GeV]","Number of Entries");
    rp->SetLogy(false);
    rp->SetVariationTypes(AllPlots->Variations);
    rp->Legend(AllPlots->LegendPos);
    for (unsigned iv = 0; iv < AllPlots->Variations.size(); ++iv) {
      for (unsigned io = 0; io < 6; ++io) {
        // if (io > 0) continue;
        AllPlots->Hists[0][iv][ir][io]->SetLineColor(io + 1);
        AllPlots->Hists[0][iv][ir][io]->Rebin(5);
        rp->AddHist(obs[io], AllPlots->Hists[0][iv][ir][io], 2, iv);
      }
    }
    rp->PrepHists();
    rp->CreateLegendEntries();
    rp->CreateErrorGraphs();
    rp->SetPad(c1);
    if (rp->DrawPlot(isampleyear)) {
      rp->DrawLatex(rm.LatexRanges[ir],0);
      int lbin = AllPlots->Hists[0][0][ir][4]->FindBin(1900) - 1;
      double rat = AllPlots->Hists[0][0][ir][4]->Integral(3,lbin) / (AllPlots->Hists[0][0][ir][5]->Integral(3,lbin) + AllPlots->Hists[0][0][ir][4]->Integral(3,lbin));
      rp->DrawLatex(Form("FL Flag rate = %.3f",rat),1);
      TString pn = "plots/" + PlotName + ".pdf";
      c1->SaveAs(pn);
      // pn = "plots/" + PlotName + ".C";
      // c1->SaveAs(pn);
    }
    delete c1;
  }
  

}