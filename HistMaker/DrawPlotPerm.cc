#include "TFile.h"
#include "Utilities/HistManager.cc"
#include "../Utilities/Permutations.cc"

void DrawPlotPerm(int isampleyear = 3) {
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  SampleTypes = {"FL300", "FL500","FL700", "FL1100", "LL300", "LL500", "LL700", "LL1100"};
  vector<string> Variations = {""};
  // vector<string> StringRanges = rm.StringRanges;
  
  Permutations *PM = new Permutations();
  
  rm.InclusiveInit();
  RatioPlot *p_pt = new RatioPlot("PtPerm", true, "Pt Permutation Index", "Normalized Distrubtion");
  p_pt->SetVariations(Variations);
  RatioPlot *p_btag = new RatioPlot("bTagPerm", true, "bTag Permutation Index", "Normalized Distrubtion");
  p_btag->SetVariations(Variations);
  RatioPlot *p_pfs = new RatioPlot("PermFaults", true, "Permutation Fault Type", "Normalized by Events");
  p_pfs->SetVariations(Variations);
  p_pfs->XBinLabels = {"Correct", "TopbSwap", "LJ misassigned to b", "W' b wrong"};

  TString HistFilePath = "/eos/user/s/siluo/WPrimeAnalysis/AuxHists/";
  TString HistFilePrefix = "Permutations";
  vector<double> LegendPos = {0.65,0.65,0.9,0.9};
  for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
    TString stname = "_" + SampleYear + "_" + SampleTypes[ist];
    TString filename = HistFilePath + HistFilePrefix + stname + ".root";
    TFile *f = new TFile(filename, "READ");
    cout << "Reading from file " << filename << endl;
    TH1F* hpt = (TH1F*) f->Get("PtPerm" + stname)->Clone();
    cout << "Reading plot " << "PtPerm" << stname << endl;
    TH1F* hbtag = (TH1F*) f->Get("bTagPerm" + stname)->Clone();
    cout << "Reading plot " << "bTagPerm" << stname << endl;
    hpt->SetDirectory(0);
    hbtag->SetDirectory(0);
    hpt->Scale(1./hpt->Integral(0,-1));
    hbtag->Scale(1./hbtag->Integral(0,-1));
    int col = dlib.Datasets[SampleTypes[ist]].Color;
    TH1F* hpfs = new TH1F("PermFaults" + stname, "Perm Fault",4,-0.5,3.5);
    int SimplePerm = 0;
    if (stname.Contains("FL")) SimplePerm = 34120;
    else if (stname.Contains("LL")) SimplePerm = 34210;
    for (unsigned ib = 1; ib <= hpt->GetNbinsX(); ++ib) {
      int faultcode = PM->ComparePerm(SimplePerm, PM->PtPerms[ib-1]);
      double bc = hpt->GetBinContent(ib);
      if (faultcode == 0) hpfs->Fill(0., bc);
      if (faultcode / 1 % 10) hpfs->Fill(1., bc);
      if (faultcode / 10 % 10) hpfs->Fill(2., bc);
      if (faultcode / 100 % 10) hpfs->Fill(3., bc);
    }
    hpt->SetLineColor(col);
    hbtag->SetLineColor(col);
    hpfs->SetLineColor(col);
    int style = -1;
    if (stname.Contains("LL")) {
      style = 3;
    }
    p_pt->AddHist(SampleTypes[ist], hpt,2, 0,style);
    p_btag->AddHist(SampleTypes[ist], hbtag,2, 0, style);
    p_pfs->AddHist(SampleTypes[ist], hpfs, 2, 0,style);
  }
  p_pt->TrueMaximumScale = 1.2;
  p_btag->TrueMaximumScale = 1.2;
  p_pfs->TrueMaximumScale = 1.2;
  p_pt->PrepHists();
  p_btag->PrepHists();
  p_pfs->PrepHists();
  p_pt->Legend(LegendPos);
  p_btag->Legend(LegendPos);
  p_pfs->Legend(LegendPos);
  p_pt->CreateLegendEntries();
  p_btag->CreateLegendEntries();
  p_pfs->CreateLegendEntries();
  p_pt->SetLogy(false);
  p_btag->SetLogy(false);
  p_pfs->SetLogy(false);
  
  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  p_pt->SetPad(c1);
  p_pt->DrawPlot(isampleyear);
  c1->SaveAs("plots/PtPerm.pdf");
  delete c1;
  TCanvas *c2 = new TCanvas("c2","c2",800,800);
  p_btag->SetPad(c2);
  p_btag->DrawPlot(isampleyear);
  c2->SaveAs("plots/bTagPerm.pdf");
  TCanvas *c3 = new TCanvas("c3","c3",800,800);
  p_pfs->SetPad(c3);
  p_pfs->DrawPlot(isampleyear);
  c3->SaveAs("plots/PermFaults.pdf");
}