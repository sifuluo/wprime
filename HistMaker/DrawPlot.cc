#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "TPad.h"

#include <vector>
#include <string>

#include "DrawDataFormat.cc"
#include "HistManager.cc"

void DrawPlot(int isampleyear = 3, int PUWP = 0, int bWP = 0) {
  vector<string> PUWPs{"_PUloose","_PUmedium","_PUtight"};
  vector<string> bTWPs{"_bTagLoose","_bTagMedium","_bTagTight"};
  TH1Collection tc = TH1Collection();
  vector<string> Variables{"SimpleWprime","LeadingJetPt"};
  vector<string> VariablesTitle{"m(W')[GeV/c^2]","Leading Jet p_{T}"};
  tc.Init(Variables);
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  TString filename = "outputs/" + SampleYear + PUWPs[PUWP] + bTWPs[bWP] + ".root";
  TFile* f = new TFile(filename,"READ");
  TString NameTemplate = "=SampleType=_=Variable=_=RegionRange=";
  tc.ReadHistograms(NameTemplate, f);

  vector<string> Ranges = rm.StringRanges;

  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  // HistManager* hm = new HistManager(c1);

  for (unsigned iv = 0; iv < Variables.size(); ++iv) {
    for (unsigned ir = 0; ir < Ranges.size(); ++ir) {
      HistManager* hm = new HistManager(c1);
      hm->ResetMembers();
      if (rm.Ranges[ir].IsSR) hm->SetDrawData(false);
      else hm->SetDrawData(true);
      for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
        if (tc.SampleValid[ist] == false) continue;
        // cout << SampleTypes[ist] <<endl;
        hm->AddHist(SampleTypes[ist],tc.histos[ist][iv][ir]);  
      }
      hm->NormToLumi(isampleyear);
      hm->RebinHists(-100);
      TString tx = VariablesTitle[iv] + "["+ Ranges[ir] + "]";
      TString ty = "Number of Entries";
      TString fn = SampleYear + PUWPs[PUWP] + bTWPs[bWP] + "_" + Variables[iv] + "_" + Ranges[ir];
      // fn = "";
      hm->DrawPlot(tx, ty, fn, isampleyear);
      c1->Clear();
    }
    TString fnv = "plots/" + SampleYear + PUWPs[PUWP] + bTWPs[bWP] + "_" + Variables[iv] + ".pdf";
    // c1->SaveAs(fnv);
    c1->Clear();
  }

}