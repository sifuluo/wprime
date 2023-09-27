#include "TFile.h"
#include "Utilities/HistManager.cc"
#include "TF1.h"
#include "TROOT.h"

void DrawPlotValidation(int isampleyear = 3, int iobs = 0, bool DoMCReweight = false) {
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
  obs.push_back({"WPrimeMassSimpleFL", "Simple m(W'_{H})"} ); // 23
  obs.push_back({"WPrimeMassSimpleLL", "Simple m(W'_{L})"} ); // 24
  // obs.push_back({"WPrimeMass", "m(W')"} );  // 25
  // obs.push_back({"WPrimeMassFL", "m(W'_{H})"} );  // 26
  // obs.push_back({"WPrimeMassLL", "m(W'_{L})"} );  // 27
  // obs.push_back({"Likelihood", "Likelihood"} );   // 28

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

  string HistFilePath = "outputs/";
  string HistFilePrefix = SampleYear + "_Validation";
  if (DoMCReweight) HistFilePrefix += "_RW";
  string PlotNamePrefix = HistFilePrefix;
  TString filename = StandardNames::HistFileName(HistFilePath, HistFilePrefix, Observable);
  cout << "Reading From File " << filename << endl;
  TFile* f = new TFile(filename, "READ");
  HistManager* AllPlots = new HistManager();
  // AllPlots->SetSampleTypes(SampleTypes);
  // AllPlots->SetRegions(StringRanges);
  AllPlots->SetTitles(ObservablesXTitle);
  AllPlots->SetPrefix(PlotNamePrefix);
  AllPlots->SetObservable(Observable);
  AllPlots->SetDrawSensitivity(true);
  AllPlots->SetDrawPurity(true);
  AllPlots->ReadHistograms(f);

  for (unsigned ir = 0; ir < rm.StringRanges.size(); ++ir) {
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    AllPlots->CreateAuxiliaryPlots(ir);
    AllPlots->DrawPlot(ir, c1, isampleyear);
    TString PlotName = AllPlots->Plots[ir]->PlotName;
    TString pn  = "plots/" + PlotName + ".pdf";
    c1->SaveAs(pn);
    string sr = rm.StringRanges[ir];
    if (sr == "1153" || sr == "1163" || sr == "2153" || sr == "2163") if (DoMCReweight && iobs == 11) AllPlots->SaveUncertContribution(ir, PlotName);
    delete c1;
  }


  if (iobs == 11 && DoMCReweight) {
    gStyle->SetOptFit(0000);
    TString fsfname = StandardNames::HistFileName(HistFilePath, HistFilePrefix, "ReweightSF");
    TFile *fsf = new TFile(fsfname,"READ");
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    TH1F* mcr1161sf1d = (TH1F*) fsf->Get("ttbarReweightSFFrom1161");
    TH1F* mcr1151sf1d = (TH1F*) fsf->Get("ttbarReweightSFFrom1151");
    TH1F* mcr2161sf1d = (TH1F*) fsf->Get("ttbarReweightSFFrom2161");
    TH1F* mcr2151sf1d = (TH1F*) fsf->Get("ttbarReweightSFFrom2151");
    TF1* mcr1161f = new TF1("mcr1161f","[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    TF1* mcr1151f = new TF1("mcr1151f","[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    TF1* mcr2161f = new TF1("mcr2161f","[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    TF1* mcr2151f = new TF1("mcr2151f","[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    // mcr1161f->SetParameters(1.,2.,0.1,0.1);
    // mcr1151f->SetParameters(1.,2.,0.1,0.1); // Doesn't affect the final fit parameters at all in this practice
    mcr1161sf1d->SetLineColor(2);
    mcr1151sf1d->SetLineColor(3);
    mcr2161sf1d->SetLineColor(4);
    mcr2151sf1d->SetLineColor(5);
    mcr1161sf1d->Fit(mcr1161f,"RM","");
    mcr1151sf1d->Fit(mcr1151f,"RM","");
    mcr2161sf1d->Fit(mcr2161f,"RM","");
    mcr2151sf1d->Fit(mcr2151f,"RM","");
    TString SFPlotTitle = ";" + obs[iobs].second + ";Scale Factor";
    mcr1161sf1d->SetTitle(SFPlotTitle);
    mcr1151sf1d->SetTitle(SFPlotTitle);
    mcr2161sf1d->SetTitle(SFPlotTitle);
    mcr2151sf1d->SetTitle(SFPlotTitle);
    TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
    leg->AddEntry(mcr1161sf1d, "#mu 6j1b","l");
    leg->AddEntry(mcr1151sf1d, "#mu 5j1b","l");
    leg->AddEntry(mcr2161sf1d, "e 6j1b","l");
    leg->AddEntry(mcr2151sf1d, "e 5j1b","l");

    c1->cd();
    mcr1161sf1d->Draw("E1");
    mcr1151sf1d->Draw("E1same");
    mcr2161sf1d->Draw("E1same");
    mcr2151sf1d->Draw("E1same");
    leg->Draw();
    c1->SaveAs("plots/ReweightSF.pdf");
  }

}