#include "TFile.h"
#include "Utilities/HistManager.cc"
#include "TF1.h"
#include "TROOT.h"

void DrawPlotValidation(int isampleyear = 3, int iobs = 0, bool DoMCReweight = false) {
  vector<string> obs{"LeptonPt","LeptonEta","LeadingJetPt","LeadingJetEta","METPt","METPhi","mT","HT","WPrimeMassSimpleFL","WPrimeMassSimpleLL"};
  vector<string> obstitle{"Lepton p_{T}","Lepton #eta","Leading Jet p_{T}","Leading Jet #eta","MET p_{T}","MET #phi","m_{T}","H_{T}","Simple m(W'_{FL})"," Simple m(W'_{LL})"};
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  rm.TightOnlyInit(1);
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
  
  
  string Observable = obs[iobs];
  string ObservablesXTitle = obstitle[iobs];

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


  if (obs[iobs] == "WPrimeMassSimpleFL" && DoMCReweight) {
    gStyle->SetOptFit(0000);
    TString fsfname = StandardNames::HistFileName(HistFilePath, HistFilePrefix, "ReweightSF");
    TFile *fsf = new TFile(fsfname,"READ");
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    TH1F* mcr1161sf1d = (TH1F*) fsf->Get("ttbarReweightSFFrom1161");
    TH1F* mcr1151sf1d = (TH1F*) fsf->Get("ttbarReweightSFFrom1151");
    // TF1* mcr1161f = new TF1("mcr1161f","[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    // TF1* mcr1151f = new TF1("mcr1151f","[0]/x+[1]*x+[2]*x*x+[3]",100,2000);
    // mcr1161f->SetParameters(1.,2.,0.1,0.1);
    // mcr1151f->SetParameters(1.,2.,0.1,0.1);
    mcr1161sf1d->SetLineColor(2);
    mcr1151sf1d->SetLineColor(3);
    // mcr1161sf1d->Fit(mcr1161f,"RM","");
    // mcr1151sf1d->Fit(mcr1151f,"RM","");
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