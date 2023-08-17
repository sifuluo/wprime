#include "TFile.h"
#include "Utilities/HistManager.cc"
#include "TF1.h"
#include "TROOT.h"

void DrawPlotValidation(int isampleyear = 3, int iobs = 0, bool DoMCReweight = false) {
  vector<pair<string,string> > obs;
  obs.push_back({"LeptonPt", "Lepton p_{T}"});
  obs.push_back({"LeptonEta", "Lepton #eta"} );
  obs.push_back({"LeptonPhi", "Lepton #phi"} );
  obs.push_back({"LeadingJetPt", "Leading Jet p_{T}"} );
  obs.push_back({"LeadingJetEta", "Leading Jet #eta"} );
  obs.push_back({"LeadingJetPhi", "Leading Jet #phi"} );
  obs.push_back({"METPt", "#slash{E}_{T} p_{T}"} );
  obs.push_back({"METPhi", "#slash{E}_{T} #phi"} );
  obs.push_back({"dPhiMetLep","#Delta#phi(#slash{E}_{T},l)"});
  obs.push_back({"mT", "m_{T}"} );
  obs.push_back({"HT", "H_{T}"} );
  obs.push_back({"WPrimeMassSimpleFL", "Simple m(W'_{H})"} );
  obs.push_back({"WPrimeMassSimpleLL", "Simple m(W'_{L})"} );
  obs.push_back({"WPrimeMass", "m(W')"} );
  obs.push_back({"WPrimeMassFL", "m(W'_{H})"} );
  obs.push_back({"WPrimeMassLL", "m(W'_{L})"} );
  obs.push_back({"Likelihood", "Likelihood"} );

  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  rm.TightOnlyInit();
  rm.Variations = {"central" // 0
  , "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown" // 1-8
  , "EleSFup", "EleSFdown", "MuonTriggerWup", "MuonTriggerWdown", "MuonIdWup", "MuonIdWdown", "MuonIsoWup", "MuonIsoWdown" // 9-16
  , "BjetTagCorrup", "BjetTagCorrdown", "BjetTagUncorrup", "BjetTagUncorrdown", "PUIDSFup", "PUIDSFdown", "L1PreFiringSFup", "L1PreFiringSFdown" // 17-24
  , "PUreweightSFup", "PUreweightSFdown"
  , "PDFWup", "PDFWdown"
  , "LHEScaleWup", "LHEScaleWdown" // 25 - 30
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
  TFile* f = new TFile(filename, "READ");
  HistManager* AllPlots = new HistManager();
  // AllPlots->SetSampleTypes(SampleTypes);
  // AllPlots->SetRegions(StringRanges);
  AllPlots->SetTitles(ObservablesXTitle);
  AllPlots->SetPrefix(PlotNamePrefix);
  AllPlots->SetObservable(Observable);
  AllPlots->ReadHistograms(f);

  for (unsigned ir = 0; ir < rm.StringRanges.size(); ++ir) {
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    AllPlots->CreateAuxiliaryPlots(ir);
    AllPlots->DrawPlot(ir, c1, isampleyear, true, true);
    TString PlotName = AllPlots->Plots[ir]->PlotName;
    TString pn  = "plots/" + PlotName + ".pdf";
    c1->SaveAs(pn);
    delete c1;
  }


  if (Observable == "WPrimeMassSimpleFL" && DoMCReweight) {
    gStyle->SetOptFit(0000);
    TString fsfname = StandardNames::HistFileName(HistFilePath, HistFilePrefix, "ReweightSF");
    TFile *fsf = new TFile(fsfname,"READ");
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    TH1F* mcr1161sf1d = (TH1F*) fsf->Get("ttbarReweightSFFrom1161");
    TH1F* mcr1151sf1d = (TH1F*) fsf->Get("ttbarReweightSFFrom1151");
    TF1* mcr1161f = new TF1("mcr1161f","[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    TF1* mcr1151f = new TF1("mcr1151f","[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    // mcr1161f->SetParameters(1.,2.,0.1,0.1);
    // mcr1151f->SetParameters(1.,2.,0.1,0.1); // Doesn't affect the final fit parameters at all in this practice
    mcr1161sf1d->SetLineColor(2);
    mcr1151sf1d->SetLineColor(3);
    mcr1161sf1d->Fit(mcr1161f,"RM","");
    mcr1151sf1d->Fit(mcr1151f,"RM","");
    TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
    leg->AddEntry(mcr1161sf1d, "1161","l");
    leg->AddEntry(mcr1151sf1d, "1151","l");

    c1->cd();
    mcr1161sf1d->Draw("E1");
    mcr1151sf1d->Draw("E1same");
    leg->Draw();
    c1->SaveAs("plots/ReweightSF.pdf");
  }

}