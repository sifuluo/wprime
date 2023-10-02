#include "TFile.h"
#include "Utilities/HistManager.cc"
#include "Utilities/MCReweight.cc"
#include "TF1.h"
#include "TROOT.h"

void DrawPlotValidation(int isampleyear = 3, int iobs = 0, bool DoMCReweight = false, bool DrawMCReweight = false, bool SaveUncerts = false) {
  vector<pair<string,string> > obs;
  obs.push_back({"LeptonPt", "Lepton p_{T}"});  // 0
  obs.push_back({"LeptonEta", "Lepton #eta"} );  // 1
  obs.push_back({"LeptonPhi", "Lepton #phi"} );  // 2
  obs.push_back({"Jet0Pt", "Jet[0] p_{T}"} );  // 3
  obs.push_back({"Jet0Eta", "Jet[0] #eta"} );  // 4
  obs.push_back({"Jet0Phi", "Jet[0] #phi"} );  // 5
  obs.push_back({"Jet1Pt", "Jet[1] p_{T}"} );  // 6
  obs.push_back({"Jet1Eta", "Jet[1] #eta"} );  // 7
  obs.push_back({"Jet1Phi", "Jet[1] #phi"} );  // 8
  obs.push_back({"Jet2Pt", "Jet[2] p_{T}"} );  // 9
  obs.push_back({"Jet2Eta", "Jet[2] #eta"} );  // 10
  obs.push_back({"Jet2Phi", "Jet[2] #phi"} );  // 11
  obs.push_back({"Jet3Pt", "Jet[3] p_{T}"} );  // 12
  obs.push_back({"Jet3Eta", "Jet[3] #eta"} );  // 13
  obs.push_back({"Jet3Phi", "Jet[3] #phi"} );  // 14
  obs.push_back({"Jet4Pt", "Jet[4] p_{T}"} );  // 15
  obs.push_back({"Jet4Eta", "Jet[4] #eta"} );  // 16
  obs.push_back({"Jet4Phi", "Jet[4] #phi"} );  // 17
  obs.push_back({"METPt", "#slash{E}_{T} p_{T}"} ); // 18
  obs.push_back({"METPhi", "#slash{E}_{T} #phi"} ); // 19
  obs.push_back({"dPhiMetLep","#Delta#phi(#slash{E}_{T},l)"}); // 20
  obs.push_back({"mT", "m_{T}"} );  // 21
  obs.push_back({"HT", "H_{T}"} );  // 22
  obs.push_back({"ST","ST"}); // 23
  obs.push_back({"WPrimeMassSimpleFL", "Simple m(W'_{H})"} ); // 24
  obs.push_back({"WPrimeMassSimpleLL", "Simple m(W'_{L})"} ); // 25
  // obs.push_back({"WPrimeMass", "m(W')"} );  // 26
  // obs.push_back({"WPrimeMassFL", "m(W'_{H})"} );  // 27
  // obs.push_back({"WPrimeMassLL", "m(W'_{L})"} );  // 28
  // obs.push_back({"Likelihood", "Likelihood"} );   // 29

  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  rm.TightOnlyInit();
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
    TString pn  = "plots/" + PlotName + ".pdf";
    if (DoMCReweight) pn = "plots/" + PlotName + "_RW.pdf";
    c1->SaveAs(pn);
    string sr = rm.StringRanges[ir];
    if (sr == "1153" || sr == "1163" || sr == "2153" || sr == "2163") if (SaveUncerts) AllPlots->SaveUncertContribution(ir, PlotName);
    delete c1;
  }


  if (DrawMCReweight) {
    gStyle->SetOptFit(0000);
    
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    MCReweightManager *mcrm = new MCReweightManager("Jet0Pt");
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
      if (i == 0) mcrm->rws[i]->SF1D->Draw("E1");
      else mcrm->rws[i]->SF1D->Draw("E1same");
    }
    leg->Draw();
    c1->SaveAs("plots/ReweightSF.pdf");
  }

}