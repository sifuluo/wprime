#include "TFile.h"
#include "Utilities/HistManager.cc"
#include "Utilities/MCReweight.cc"
#include "TF1.h"
#include "TROOT.h"
#include "TMath.h"

void DrawPlotValidation(int isampleyear = 3, int iobs = -99, int DoMCReweight = 0, int ForceLegPos = -1, bool SaveInfos = false) {
  vector<PlotObservable> obs;
  vector<TString> extensions;
  // extensions.push_back(".root");
  extensions.push_back(".pdf");
  // extensions.push_back(".C");
  int rebin = 1;
  // PlotObservable(Observable Name, Xtitle, YTitle, YEnlarge, xmin, xmax, LegendPosType, LegendPos)
  obs.push_back({"LeptonPt", "Lepton p_{T} [GeV]", "Number of Entries / 10 GeV", 1., -1, -1, 2, {}});  // 0
  obs.push_back({"LeptonEta", "Lepton #eta", "Number of Entries / 0.1", 3., -1, -1, 0, {}} );  // 1
  obs.push_back({"LeptonPhi", "Lepton #phi", "Number of Entries / 0.1", 3., -1, -1, 0, {}} );  // 2
  obs.push_back({"METPt", "p_{T}^{miss} [GeV]", "Number of Entries / 20 GeV", 1., 0, 500, 2, {}} ); // 3
  obs.push_back({"METPhi", "#phi_{p_{T}^{miss}}", "Number of Entries / 0.1", 0.7, -1, -1, 2, {}} ); // 4
  obs.push_back({"dPhiMetLep","#Delta#phi(#slash{E}_{T},l)", "Number of Entries / 0.1", 3., -1, -1, 0, {}}); // 5
  obs.push_back({"mT", "m_{T} [GeV]", "Number of Entries / 20 GeV", 1., 0., 400, 0, {}} );  // 6
  obs.push_back({"HT", "H_{T} [GeV]", "Number of Entries / 10 GeV", 1., -1, -1, 0, {}} );  // 7
  obs.push_back({"ST","S_{T} [GeV]", "Number of Entries / 10 GeV", 1., -1, -1, 0, {}}); // 8
  obs.push_back({"WPrimeMassSimpleFL", "Simple m(W'_{H}) [GeV]", "Number of Entries / 20 GeV", 1., -1, -1, 0, {}} ); // 9
  obs.push_back({"WPrimeMassSimpleLL", "Simple m(W'_{L}) [GeV]", "Number of Entries / 20 GeV", 1., -1, -1, 0, {}} ); // 10
  obs.push_back({"WPrimeMass", "m(W') [GeV]", "Number of Entries / 20 GeV", 1., -1, -1, 2, {}} );  // 11
  obs.push_back({"WPrimeMassFL", "m(W'_{H}) [GeV]", "Number of Entries / 20 GeV", 1., -1, -1, 2, {}} );  // 12
  obs.push_back({"WPrimeMassLL", "m(W'_{L}) [GeV]", "Number of Entries / 20 GeV", 1., -1, -1, 2, {}} );  // 13
  obs.push_back({"Likelihood", "log(Likelihood)", "Number of Entries / 0.1", 1., -1, -1, 1, {}} );   // 14
  obs.push_back({"LikelihoodCorrect", "Correct Perm log(Likelihood)", "Number of Entries / 0.1", 1., -1, -1, 1, {}}); // 15
  obs.push_back({"LikelihoodEffCorrect", "Effectively Correct Perm log(Likelihood)", "Number of Entries / 0.1", 1., -1, -1, 1, {}}); // 16
  obs.push_back({"LikelihoodInCorrect", "Incorrect Perm log(Likelihood)", "Number of Entries / 0.1", 1., -1, -1, 1, {}}); // 17
  int JetQuantitiesIndex = obs.size();
  for (unsigned ij = 0; ij < 5; ++ij) { // 5 for JetPt,Eta,Phi, and 10 for dR(Jeta,Jetb), in total 15 in the loop
    obs.push_back({Form("Jet%iPt", ij), Form("Jet[%i] p_{T} [GeV]", ij), "Number of Entries / 10 GeV", 1., -1, -1, 2, {}}); // 18, 21, 24, 27, 30 
    obs.push_back({Form("Jet%iEta", ij), Form("Jet[%i] #eta", ij), "Number of Entries / 0.1", 8., -1, -1, 0, {}}); // 19, 22, 25, 28, 31
    obs.push_back({Form("Jet%iPhi", ij), Form("Jet[%i] #phi", ij), "Number of Entries / 0.1", 3., -1, -1, 0, {}}); // 20, 23, 26, 29, 32
  }
  for (unsigned ij = 0; ij < 5; ++ij) {
    for (unsigned ij2 = ij + 1; ij2 < 5; ++ij2) {
      obs.push_back({Form("dR(Jet%i,Jet%i)",ij, ij2), Form("dR(Jet%i,Jet%i)",ij, ij2), "Number of Entries / 0.1", 3., -1, -1, 0, {}});
    }
  }
  if (iobs == -99) {
    cout << "Obs has size = " << obs.size() << " :" << endl;
    for (unsigned io = 0; io < obs.size(); ++io) {
      cout << io << ": " << obs[io].Observable << endl;
    }
    return;
  }

  if (iobs >= (int)obs.size()) {
    cout << "iobs = " << iobs << ", greater equal to obs size = " << obs.size() << ", terminating" << endl;
    return;
  }
  if (iobs == -1) {
    cout << "Running over all observables but jets quantities" << endl;
    for (int iobss = 0; iobss < JetQuantitiesIndex; ++iobss) {
      DrawPlotValidation(isampleyear, iobss, DoMCReweight, ForceLegPos, SaveInfos);
    }
    return;
  }
  if (iobs == -2) {
    cout << "Running over all jets quantities" << endl;
    for (int iobss = JetQuantitiesIndex; iobss < (int) obs.size(); ++iobss) {
      DrawPlotValidation(isampleyear, iobss, DoMCReweight, ForceLegPos, SaveInfos);
    }
    return;
  }

  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  rm.AcceptRegions({1,2},{1},{5,6},{1,2,3,4});
  if (DoMCReweight) rm.AddVariationSource("RwStat");
  if (ForceLegPos >= 0) obs[iobs].LegPos = ForceLegPos;
  
  // string Observable = obs[iobs].Observable;
  // string ObservablesXTitle = obs[iobs].Title;

  // string HistFilePath = "outputs/";
  string HistFilePath = "/eos/user/s/siluo/WPrimeAnalysis/ValidationFitted/Hists/";
  string HistFilePrefix = SampleYear + "_Validation";
  string PlotNamePrefix = HistFilePrefix;

  HistManager* AllPlots = new HistManager();
  // AllPlots->SetSampleTypes(SampleTypes);
  // AllPlots->SetRegions(StringRanges);
  // AllPlots->SetTitles(ObservablesXTitle);
  AllPlots->SetPrefix(PlotNamePrefix);
  // Special rebinning for W' mass
  if (!(iobs >=9 && iobs <= 13)) rebin = 1; // rebin only applies to WprimeMass related plots
  if (rebin  > 1) {
    AllPlots->Rebin(rebin);
    obs[iobs].YTitle = Form("Number of Entries / %d GeV", 20 * rebin);
  }
  // End of special rebinning
  AllPlots->SetObservable(obs[iobs]);
  // AllPlots->SetDrawSensitivity(true);
  // if (iobs == 0) AllPlots->SetDrawPurity(true);
  AllPlots->ReadHistograms(HistFilePath, HistFilePrefix, DoMCReweight);

  for (unsigned ir = 0; ir < rm.StringRanges.size(); ++ir) {
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    AllPlots->CreateAuxiliaryPlots(ir);
    AllPlots->DrawPlot(ir, c1, isampleyear);
    TString PlotName = AllPlots->Plots[ir]->PlotName;
    if (rebin > 1) PlotName = AllPlots->Plots[ir]->PlotName + Form("Rebin%d",rebin);
    TString PlotYear = SampleYear;
    TString pn  = "plots/" + SampleYear + "/" + PlotName;
    for (unsigned iext = 0; iext < extensions.size(); ++iext) {
      if (DoMCReweight == 1) pn += "_RW" + extensions[iext];
      else if (DoMCReweight == 2) pn += "_RW2On2" + extensions[iext];
      else pn += extensions[iext];
      c1->SaveAs(pn);
    }
    string sr = rm.StringRanges[ir];
    // if (sr == "1153" || sr == "1163" || sr == "2153" || sr == "2163") 
    if (SaveInfos) {
      TString uncname = PlotName;
      if (DoMCReweight == 1) uncname += "RW";
      else if (DoMCReweight == 2) uncname += "RW2On2";
      AllPlots->SaveInfos(ir, uncname);
    }
    delete c1;
  }

  
  

}