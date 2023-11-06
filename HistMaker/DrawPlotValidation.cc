#include "TFile.h"
#include "Utilities/HistManager.cc"
#include "Utilities/MCReweight.cc"
#include "TF1.h"
#include "TROOT.h"

void DrawPlotValidation(int isampleyear = 3, int iobs = -99, bool DoMCReweight = false, bool DrawMCReweight = false, bool SaveUncerts = false) {
  vector<pair<string,string> > obs;
  obs.push_back({"LeptonPt", "Lepton p_{T}"});  // 0
  obs.push_back({"LeptonEta", "Lepton #eta"} );  // 1
  obs.push_back({"LeptonPhi", "Lepton #phi"} );  // 2
  obs.push_back({"METPt", "#slash{E}_{T} p_{T}"} ); // 3
  obs.push_back({"METPhi", "#slash{E}_{T} #phi"} ); // 4
  obs.push_back({"dPhiMetLep","#Delta#phi(#slash{E}_{T},l)"}); // 5
  obs.push_back({"mT", "m_{T}"} );  // 6
  obs.push_back({"HT", "H_{T}"} );  // 7
  obs.push_back({"ST","ST"}); // 8
  obs.push_back({"WPrimeMassSimpleFL", "Simple m(W'_{H})"} ); // 9
  obs.push_back({"WPrimeMassSimpleLL", "Simple m(W'_{L})"} ); // 10
  // obs.push_back({"WPrimeMass", "m(W')"} );  // 11
  // obs.push_back({"WPrimeMassFL", "m(W'_{H})"} );  // 12
  // obs.push_back({"WPrimeMassLL", "m(W'_{L})"} );  // 13
  // obs.push_back({"Likelihood", "Likelihood"} );   // 14
  int JetQuantitiesIndex = obs.size();
  for (unsigned ij = 0; ij < 5; ++ij) { // 5 for JetPt,Eta,Phi, and 10 for dR(Jeta,Jetb), in total 15 in the loop
    obs.push_back({Form("Jet%iPt", ij), Form("Jet[%i] p_{T}", ij)});
    obs.push_back({Form("Jet%iEta", ij), Form("Jet[%i] #eta", ij)});
    obs.push_back({Form("Jet%iPhi", ij), Form("Jet[%i] #phi", ij)});
    for (unsigned ij2 = ij + 1; ij2 < 5; ++ij2) {
      obs.push_back({Form("dR(Jet%i,Jet%i)",ij, ij2), Form("dR(Jet%i,Jet%i)",ij, ij2)});
    }
  }
  if (iobs == -99) {
    cout << "Obs has size = " << obs.size() << " :" << endl;
    for (unsigned io = 0; io < obs.size(); ++io) {
      cout << io << ": " << obs[io].first << endl;
    }
    return;
  }

  if (iobs >= (int)obs.size()) {
    cout << "iobs = " << iobs << ", greater equal to obs size = " << obs.size() << ", terminating" << endl;
    return;
  }
  if (iobs == -1 && !DrawMCReweight) {
    cout << "Running over all observables but jets quantities" << endl;
    for (int iobss = 0; iobss < JetQuantitiesIndex; ++iobss) {
      DrawPlotValidation(isampleyear, iobss, DoMCReweight, false, SaveUncerts);
    }
    return;
  }
  if (iobs == -2 && !DrawMCReweight) {
    cout << "Running over all jets quantities" << endl;
    for (int iobss = JetQuantitiesIndex; iobss < (int) obs.size(); ++iobss) {
      DrawPlotValidation(isampleyear, iobss, DoMCReweight, false, SaveUncerts);
    }
    return;
  }

  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  rm.AcceptRegions({1,2},{1},{5,6},{1,2,3,4,5,6});
  rm.Variations = {"" // 0
  , "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESUp", "JESDown", "JERUp", "JERDown" // 1-8
  , "EleSFUp", "EleSFDown", "MuonTriggerUp", "MuonTriggerDown", "MuonIdUp", "MuonIdDown", "MuonIsoUp", "MuonIsoDown" // 9-16
  , "BjetTagCorrUp", "BjetTagCorrDown", "BjetTagUncorrUp", "BjetTagUncorrDown", "PUIDSFUp", "PUIDSFDown", "L1PreFiringSFUp", "L1PreFiringSFDown" // 17-24
  , "PUreweightSFUp", "PUreweightSFDown"
  , "PDFUp", "PDFDown"
  , "LHEScaleUp", "LHEScaleDown" // 25 - 30
  };
  // SampleTypes = {"FL500"};
  // vector<string> StringRanges = rm.StringRanges;
  
  
  string Observable = obs[iobs].first;
  string ObservablesXTitle = obs[iobs].second;

  // string HistFilePath = "outputs/";
  string HistFilePath = "/eos/user/s/siluo/WPrimeAnalysis/Validation/Hists/";
  string HistFilePrefix = SampleYear + "_Validation";
  string PlotNamePrefix = HistFilePrefix;

  if (DrawMCReweight) {
    setTDRStyle();
    gStyle->SetOptFit(0000);
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    string MCRWVar = "ST";
    MCReweightManager *mcrm = new MCReweightManager(MCRWVar);
    mcrm->Init();
    if (!mcrm->ReadFromFile(HistFilePath, HistFilePrefix)) return;
    TString SFPlotTitle = "NoTitle; NoTitle; NoTitle";
    for (unsigned i = 0; i < obs.size(); ++i) {
      if (mcrm->Observable == obs[i].first) SFPlotTitle = ";" + obs[i].second + ";Scale Factor";
    }
    TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
    vector<int> RegionColors = {2,3,4,5};
    if (RegionColors.size() < mcrm->rws.size()) cout << "Not enough color palette set for " << mcrm->rws.size() << " plots" << endl;
    c1->cd();
    for (unsigned i = 0; i < mcrm->rws.size(); ++i) {
      mcrm->rws[i]->SF1D->SetLineColor(RegionColors[i]);
      mcrm->rws[i]->SF1DF->SetLineColor(RegionColors[i]);
      TString label = "";
      if (mcrm->rws[i]->SourceRegionInt / 1000 == 1) label += "#mu ";
      else label += "e ";
      label += Form("%ij%ib", mcrm->rws[i]->SourceRegionInt / 10 % 10, mcrm->rws[i]->SourceRegionInt % 10);
      leg->AddEntry(mcrm->rws[i]->SF1D, label);
      if (i == 0) {
        mcrm->rws[i]->SF1D->SetMaximum(2.5);
        mcrm->rws[i]->SF1D->GetXaxis()->CenterTitle();
        mcrm->rws[i]->SF1D->Draw("E1");
      }
      else mcrm->rws[i]->SF1D->Draw("E1same");
    }
    leg->Draw();
    TString rwfn = "plots/" + SampleYear + "/" + PlotNamePrefix + "_ReweightSF.pdf";
    c1->SaveAs(rwfn);
    return;
  }

  HistManager* AllPlots = new HistManager();
  // AllPlots->SetSampleTypes(SampleTypes);
  // AllPlots->SetRegions(StringRanges);
  AllPlots->SetTitles(ObservablesXTitle);
  AllPlots->SetPrefix(PlotNamePrefix);
  AllPlots->SetObservable(Observable);
  AllPlots->SetDrawSensitivity(true);
  AllPlots->SetDrawPurity(true);
  AllPlots->ReadHistograms(HistFilePath, HistFilePrefix, DoMCReweight);

  for (unsigned ir = 0; ir < rm.StringRanges.size(); ++ir) {
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    AllPlots->CreateAuxiliaryPlots(ir);
    AllPlots->DrawPlot(ir, c1, isampleyear);
    TString PlotName = AllPlots->Plots[ir]->PlotName;
    TString PlotYear = SampleYear;
    TString pn  = "plots/" + SampleYear + "/" + PlotName;
    if (DoMCReweight) pn += "_RW.pdf";
    else pn += ".pdf";
    c1->SaveAs(pn);
    string sr = rm.StringRanges[ir];
    if (sr == "1153" || sr == "1163" || sr == "2153" || sr == "2163") if (SaveUncerts) AllPlots->SaveUncertContribution(ir, PlotName);
    delete c1;
  }


  

}