#include "TFile.h"
#include "Utilities/HistManager.cc"

void DrawPlotValidation(int isampleyear = 3, int iobs = 0) {
  vector<string> obs{"LeptonPt","LeptonEta","LeadingJetPt","LeadingJetEta","METPt","METPhi","mT","WPrimeMassSimpleFL","WPrimeMassSimpleLL"};
  vector<string> obstitle{"Lepton p_{T}","Lepton #eta","Leading Jet p_{T}","Leading Jet #eta","MET p_{T}","MET #phi","m_{T}","Simple m(W'_{FL})"," Simple m(W'_{LL})"};
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  rm.SplitInit();
  rm.Variations = {"central" // 0
  , "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown" // 1-8
  , "EleSFup", "EleSFdown", "MuonTriggerWup", "MuonTriggerWdown", "MuonIdWup", "MuonIdWdown", "MuonIsoWup", "MuonIsoWdown" // 9-16
  , "BjetTagCorrup", "BjetTagCorrdown", "BjetTagUncorrup", "BjetTagUncorrdown", "PUIDSFup", "PUIDSFdown", "L1PreFiringSFup", "L1PreFiringSFdown" // 17-24
  , "PUreweightSFup", "PUreweightSFdown"
  , "PDFWup", "PDFWdown"
  // , "LHEScaleWup", "LHEScaleWdown" // 25 - 30
  };
  // SampleTypes = {"FL500"};
  // vector<string> StringRanges = rm.StringRanges;
  
  
  string Observable = obs[iobs];
  string ObservablesXTitle = obstitle[iobs];

  string HistFilePath = "outputs/";
  string HistFilePrefix = SampleYear + "_Validation";
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
    AllPlots->DrawPlot(ir, c1, isampleyear, false, true);
    TString PlotName = AllPlots->Plots[ir]->PlotName;
    TString pn  = "plots/" + PlotName + ".pdf";
    c1->SaveAs(pn);
    delete c1;
  }

}