#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "TPad.h"

#include <vector>
#include <string>

#include "DrawDataFormat.cc"
#include "HistManager.cc"

void DrawPlot(int isampleyear = 3) {
  vector<string> PUWPs{"_PUloose","_PUmedium","_PUtight"};
  vector<string> bWPs{"_bTagLoose","_bTagMedium","_bTagTight"};
  vector<string> Observables{"SimpleWprime","LeadingJetPt"};
  vector<string> ObservablesXTitle{"m(W')[GeV/c^2]","Leading Jet p_{T}"};
  vector<string> ObservablesYTitle(Observables.size(),"Number of Entries");
  string SampleYear = dlib.SampleYears[isampleyear];
  vector<string> SampleTypes = dlib.DatasetNames;
  vector<string> StringRanges = rm.StringRanges;
  cout << "-1" <<endl;
  // vector<vector<vector<vector<HistManager*> > > > AllPlots; // [iPUWP][ibWP][io][ir]
  vector< vector <HistManager*> > AllPlots; // [iPUWP][ibWP]
  AllPlots.resize(PUWPs.size());
  for (int iPUWP = 0; iPUWP < PUWPs.size(); ++iPUWP) {
    // if (iPUWP != 2) continue;
    AllPlots[iPUWP].resize(bWPs.size());
    for (int ibWP = 0; ibWP < bWPs.size(); ++ibWP) {
      // if (ibWP != 2) continue;
      TString filename = "outputs/" + SampleYear + PUWPs[iPUWP] + bWPs[ibWP] + ".root";
      TFile* f = new TFile(filename,"READ");
      cout << "iPUWP = " << iPUWP << ", ibWP = " << ibWP << endl;
      AllPlots[iPUWP][ibWP] = new HistManager();
      AllPlots[iPUWP][ibWP]->ReadHistograms(Observables, f);
      string prefix = SampleYear + PUWPs[iPUWP] + bWPs[ibWP];
      AllPlots[iPUWP][ibWP]->SetPrefix(prefix);
      AllPlots[iPUWP][ibWP]->SetTitles(ObservablesXTitle, ObservablesYTitle);
      AllPlots[iPUWP][ibWP]->PrepHists();
    }
  }
  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  for (unsigned io = 0; io < Observables.size(); ++io) {
    // if (io != 0) continue; // Only save one variable
    for (unsigned ir = 0; ir < StringRanges.size(); ++ir) {
      // if (rm.Ranges[ir].b1 != 1251) continue; // Only save one region
      double ymax = 0;
      for (int iPUWP = 0; iPUWP < PUWPs.size(); ++iPUWP) {
        // if (iPUWP != 2) continue;
        for (int ibWP = 0; ibWP < bWPs.size(); ++ibWP) {
          // if (ibWP != 2) continue;
          double m = AllPlots[iPUWP][ibWP]->GetMaximum(ir, io);
          if (m > ymax) ymax = m;
        }
      }
      if (ymax == 0) continue;
      for (int iPUWP = 0; iPUWP < PUWPs.size(); ++iPUWP) {
        // if (iPUWP != 2) continue;
        for (int ibWP = 0; ibWP < bWPs.size(); ++ibWP) {
          // if (ibWP != 2) continue;
          AllPlots[iPUWP][ibWP]->DrawPlot(ir,io,c1,isampleyear);
          AllPlots[iPUWP][ibWP]->SetMaximum(ir, io, ymax);
          TString fn = "plots/" + AllPlots[iPUWP][ibWP]->Plots[ir][io]->PlotName + ".pdf";
          // cout << "Outside ";
          // cout << "iBin = 50: " << AllPlots[iPUWP][ibWP]->SumMCHists[0][ir][io]->GetBinContent(50) << endl;
          // cout << "In RP: " << AllPlots[iPUWP][ibWP]->Plots[ir][io]->MCSummed->GetBinContent(50) << endl;
          // cout << "MCErr up: X = " << AllPlots[iPUWP][ibWP]->ErrorGraphs[ir][io]->GetX()[2*49] << ", "<< AllPlots[iPUWP][ibWP]->ErrorGraphs[ir][io]->GetX()[2*49 + 1] <<endl;
          // cout << "MCErr up: Y = " << AllPlots[iPUWP][ibWP]->ErrorGraphs[ir][io]->GetY()[2*49] << ", "<< AllPlots[iPUWP][ibWP]->ErrorGraphs[ir][io]->GetY()[2*49 + 1] <<endl;
          c1->SaveAs(fn);
          c1->Clear();
        }
      }
    }
  }
}
