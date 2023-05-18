#include "TFile.h"
#include "Utilities/HistManager.cc"

void DrawPlotValidation(int isampleyear = 3, int iobs = 0, int nobs = 0) {
  vector<string> obs{"LeptonPt","LeptonEta","LeadingJetPt","LeadingJetEta","METPt","METPhi","mT","WPrimeMassSimpleFL","WPrimeMassSimpleLL"};
  vector<string> ObservablesXTitle{"Lepton p_{T}","Lepton #eta","Leading Jet p_{T}","Leading Jet #eta","MET p_{T}","MET #phi","m_{T}","Simple m(W'_{FL})"," Simple m(W'_{LL})"};
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  // SampleTypes = {"FL500"};
  // vector<string> StringRanges = rm.StringRanges;
  
  
  if (iobs != 0 || nobs != 0) {
    vector<string> tmpobs;
    vector<string> tmpxtt;
    unsigned istart = iobs;
    if (nobs == 0) nobs = 1;
    unsigned iend = iobs + nobs;
    if (nobs == -1) iend = obs.size();
    cout << "Processing Observables: ";
    for (unsigned i = istart; i < iend; ++i) {
      if (i >= obs.size()) break;
      tmpobs.push_back(obs[i]);
      tmpxtt.push_back(ObservablesXTitle[i]);
      cout << obs[i] << ", ";
    }
    cout << endl;
    obs = tmpobs;
    ObservablesXTitle = tmpxtt;
  }

  TString filename = "outputs/" + SampleYear + "_Validation.root";
  TFile* f = new TFile(filename, "READ");
  HistManager* AllPlots = new HistManager();
  AllPlots->SetSampleTypes(SampleTypes);
  // AllPlots->SetRegions(StringRanges);
  AllPlots->SetTitles(ObservablesXTitle);
  AllPlots->SetPrefix("2018");
  AllPlots->ReadHistograms(obs, f);
  // AllPlots->SortHists();
  // AllPlots->PrepHists();

  for (unsigned ir = 0; ir < rm.StringRanges.size(); ++ir) {
    for (unsigned io = 0; io < obs.size(); ++io) {
      TCanvas* c1 = new TCanvas("c1","c1",800,800);
      AllPlots->CreateAuxiliaryPlots(ir,io);
      AllPlots->DrawPlot(ir,io,c1,isampleyear);
      TString PlotName = AllPlots->Plots[ir][io]->PlotName;
      TString pn  = "plots/" + PlotName + ".pdf";
      c1->SaveAs(pn);
      delete c1;
    }
  }
  
  
  // for (unsigned ir = 0; ir < StringRanges.size(); ++ir) {
  //   // if (rm.Ranges[ir].b1 != 1153) continue;
  //   TCanvas* c1 = new TCanvas("c1","c1",800,800);
  //   TString PlotName = "2018_WprimeMass_" + StringRanges[ir];
  //   RatioPlot *rp = new RatioPlot("2018", true, "m(W') [GeV]","Number of Entries");
  //   rp->SetLogy(false);
  //   rp->SetVariationTypes(AllPlots->Variations);
  //   rp->Legend(AllPlots->LegendPos);
  //   for (unsigned iv = 0; iv < AllPlots->Variations.size(); ++iv) {
  //     for (unsigned io = 0; io < 6; ++io) {
  //       // if (io > 0) continue;
  //       AllPlots->Hists[0][iv][ir][io]->SetLineColor(io + 1);
  //       AllPlots->Hists[0][iv][ir][io]->Rebin(5);
  //       rp->AddHist(obs[io], AllPlots->Hists[0][iv][ir][io], 2, iv);
  //     }
  //   }
  //   rp->PrepHists();
  //   rp->CreateLegendEntries();
  //   rp->CreateErrorGraphs();
  //   rp->SetPad(c1);
  //   if (rp->DrawPlot(isampleyear)) {
  //     rp->DrawLatex(rm.LatexRanges[ir],0);
  //     int lbin = AllPlots->Hists[0][0][ir][4]->FindBin(1900) - 1;
  //     double rat = AllPlots->Hists[0][0][ir][4]->Integral(3,lbin) / (AllPlots->Hists[0][0][ir][5]->Integral(3,lbin) + AllPlots->Hists[0][0][ir][4]->Integral(3,lbin));
  //     rp->DrawLatex(Form("FL Flag rate = %.3f",rat),1);
  //     TString pn = "plots/" + PlotName + ".pdf";
  //     c1->SaveAs(pn);
  //     // pn = "plots/" + PlotName + ".C";
  //     // c1->SaveAs(pn);
  //   }
  //   delete c1;
  // }
  

}