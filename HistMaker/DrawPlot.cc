#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "TPad.h"

#include <vector>
#include <string>

#include "DrawDataFormat.cc"
#include "HistManager.cc"

void DrawPlot(int isampleyear = 3, int iPUWP = 0, int ibWP = 0) {
  vector<string> PUWPs{"_PUloose","_PUmedium","_PUtight"};
  vector<string> bWPs{"_bTagLoose","_bTagMedium","_bTagTight"};
  TH1Collection tc = TH1Collection();
  vector<string> Variables{"SimpleWprime","LeadingJetPt"};
  vector<string> VariablesTitle{"m(W')[GeV/c^2]","Leading Jet p_{T}"};
  tc.Init(Variables);
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  vector<string> StringRanges = rm.StringRanges;

  vector<vector<vector<vector<HistManager*> > > > AllPlots; // [iPUWP][ibWP][iv][ir]
  AllPlots.resize(PUWPs.size());
  for (int iPUWP = 0; iPUWP < PUWPs.size(); ++iPUWP) {
    AllPlots[iPUWP].resize(bWPs.size());
    for (int ibWP = 0; ibWP < bWPs.size(); ++ibWP) {
      TString filename = "outputs/" + SampleYear + PUWPs[iPUWP] + bWPs[ibWP] + ".root";
      TFile* f = new TFile(filename,"READ");
      TString NameTemplate = "=SampleType=_=Variable=_=RegionRange=";
      tc.ReadHistograms(NameTemplate, f);

      AllPlots[iPUWP][ibWP].resize(Variables.size());
      for (unsigned iv = 0; iv < Variables.size(); ++iv) {
        AllPlots[iPUWP][ibWP][iv].resize(StringRanges.size());
        for (unsigned ir = 0; ir < StringRanges.size(); ++ir) {
          AllPlots[iPUWP][ibWP][iv][ir] = new HistManager(rm.Ranges[ir].IsSR);
          HistManager* hm = AllPlots[iPUWP][ibWP][iv][ir];
          TString fn = SampleYear + PUWPs[iPUWP] + bWPs[ibWP] + "_" + Variables[iv] + "_" + StringRanges[ir];
          hm->SetName(fn);
          for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
            if (tc.SampleValid[ist] == false) continue;
            hm->AddHist(SampleTypes[ist],tc.histos[ist][iv][ir]);  
          }
          hm->NormToLumi(isampleyear);
          hm->RebinHists(-100);
          hm->SetRegionLatex(rm.LatexRanges[ir]);
          TString tx = VariablesTitle[iv];
          TString ty = "Number of Entries";
          hm->PrepHists(tx,ty);
        }
      }
    }
  }

  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  for (unsigned iv = 0; iv < Variables.size(); ++iv) {
    // if (iv != 0) continue; // Only save one variable
    for (unsigned ir = 0; ir < StringRanges.size(); ++ir) {
      // if (rm.Ranges[ir].b1 != 1152 || rm.Ranges[ir].b2 != 1155) continue; // Only save one region
      double ymax = 0;
      for (int iPUWP = 0; iPUWP < PUWPs.size(); ++iPUWP) {
        for (int ibWP = 0; ibWP < bWPs.size(); ++ibWP) {
          double m = AllPlots[iPUWP][ibWP][iv][ir]->GetMaximum();
          if (m > ymax) ymax = m;
        }
      }
      if (ymax == 0) continue;
      for (int iPUWP = 0; iPUWP < PUWPs.size(); ++iPUWP) {
        for (int ibWP = 0; ibWP < bWPs.size(); ++ibWP) {
          AllPlots[iPUWP][ibWP][iv][ir]->DrawPlot(c1,isampleyear);
          AllPlots[iPUWP][ibWP][iv][ir]->SetMaximum(ymax);
          TString fn = "plots/" + AllPlots[iPUWP][ibWP][iv][ir]->PlotName + ".pdf";
          c1->SaveAs(fn);
          c1->Clear();
        }
      }
    }
  }
}
