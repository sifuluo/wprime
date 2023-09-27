#ifndef HISTMANAGER_CC
#define HISTMANAGER_CC

#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLatex.h"
#include "TLine.h"
#include "TFile.h"
#include "TGraph.h"

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <cmath>

// include block for poisson calculation
// #include "Math/QuantFuncMathCore.h"
// #include "TMath.h"
// #include "TGraphAsymmErrors.h"
// end of the block

#include "DrawDataFormat.cc"
#include "CMSStyle.cc"
#include "RatioPlot.cc"

class HistManager{
public:
  HistManager() {
    GroupNames = dlib.GroupNames;
    SampleTypes = dlib.DatasetNames;
    Variations = rm.Variations;
    Regions = rm.StringRanges;
  };

  void SetObservable(string ob) {
    Observable = ob;
  }

  void SetSampleTypes(vector<string> sts) {
    SampleTypes = sts;
    // for (unsigned i = 0; i < SampleTypes.size(); ++i) {

    // }
  }
  
  void SetPrefix(string prefix) {
    PlotNamePrefix = prefix;
  }

  void SetTitles(string xt, string yt = "Number of Entries") {
    XTitle = xt;
    YTitle = yt;
  }

  void SetRegions(vector<string> rs) {
    Regions = rs;
  }

  void SetDrawSensitivity(bool b) {
    DrawSensitivity = b;
  }
  void SetDrawPurity(bool b) {
    DrawPurity = b;
  }
  
  // Reading Histograms
  void ReadHistograms(TFile *f) {
    Plots.clear();
    Plots.resize(Regions.size());
    for (unsigned ir = 0; ir < Regions.size(); ++ir) {
      bool IsSR = rm.Ranges[ir].IsSR;
      TString PlotName = PlotNamePrefix + "_" + Observable + "_" + rm.StringRanges[ir];
      Plots[ir] = new RatioPlot(PlotName,IsSR, XTitle, YTitle);
      Plots[ir]->SetVariations(Variations);
      // Continue work from here
      vector<vector<TH1F*> > PlotGroupHists;
      PlotGroupHists.resize(GroupNames.size());
      for (unsigned ig = 0; ig < GroupNames.size(); ++ig) {
        PlotGroupHists[ig].resize(Variations.size());
      }
      for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
        if ((SampleTypes[ist] == "SingleElectron" && rm.Ranges[ir].B1[3] != 2) || (SampleTypes[ist] == "SingleMuon" && rm.Ranges[ir].B1[3] != 1)) continue;
        string gp = dlib.GetGroup(SampleTypes[ist]);
        int ig = dlib.GetGroupIndexFromGroupName(gp);
        if (ig < 0) continue;
        for (unsigned iv = 0; iv < Variations.size(); ++iv){
          TString hn = StandardNames::HistName(SampleTypes[ist], Observable, Regions[ir], Variations[iv]);
          TH1F* h = (TH1F*) f->Get(hn);
          if (h == nullptr) {
            // cout << hn << " is nullptr" <<endl;
            continue;
          }
          if (PlotGroupHists[ig][iv] == nullptr) { // First hist for the group
            PlotGroupHists[ig][iv] = (TH1F*) h->Clone();
            int col = dlib.Groups[gp].Color;
            if (dlib.Groups[gp].Type == 1) PlotGroupHists[ig][iv]->SetFillColor(col);
            PlotGroupHists[ig][iv]->SetLineColor(col);
            TString ghn = StandardNames::HistName(GroupNames[ig], Observable, Regions[ir], Variations[iv]);
            PlotGroupHists[ig][iv]->SetName(ghn);
          }
          else {
            PlotGroupHists[ig][iv]->Add(h);
          }
          // delete h;
        }
      }

      for (unsigned ig = 0; ig < GroupNames.size(); ++ig) {
        string gn = GroupNames[ig];
        bool gpadded = false;
        for (unsigned iv = 0; iv < Variations.size(); ++iv) {
          if (PlotGroupHists[ig][iv] == nullptr) continue;
          gpadded = true;
          if (NormalizePlot) PlotGroupHists[ig][iv]->Scale(1./PlotGroupHists[ig][iv]->Integral());
          Plots[ir]->AddHist(gn, PlotGroupHists[ig][iv], dlib.Groups[gn].Type,iv);
          delete PlotGroupHists[ig][iv];
        }
        // if (gpadded) cout << "Added " << gn << " with type = " << dlib.Groups[gn].Type << endl;
      }
      Plots[ir]->SetLogy(DoLogy);
      Plots[ir]->Legend(LegendPos);
      dlib.AddLegend(Plots[ir]->leg,IsSR);
    }
  }

  void CreateAuxiliaryPlots(int ir) {
    Plots[ir]->PrepHists();
    Plots[ir]->CreateRatioPlots();
    Plots[ir]->CreateErrorGraphs();
  }

  double GetMaximum(int ir) {
    return Plots[ir]->GetMaximum();
  }

  void SetMaximum(int ir, double max) {
    Plots[ir]->SetMaximum(max);
  }

  void DrawPlot(int ir, TVirtualPad* p_, int year) {
    Plots[ir]->SetPad(p_);
    Plots[ir]->DrawPlot(year);
    Plots[ir]->UPad->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.035);
    latex.SetTextAlign(23);
    float xpos = (LegendPos[0] + LegendPos[2])/2.;
    float ypos = LegendPos[1] - 0.025;
    latex.DrawLatex(xpos, ypos, rm.LatexRanges[ir]);
    ypos -= 0.05;
    if (DrawSensitivity) {
      latex.DrawLatex(xpos, ypos, Plots[ir]->GetSensitivityLatex());
      ypos -= 0.05;
    }
    if (DrawPurity) {
      latex.DrawLatex(xpos, ypos, Plots[ir]->GetMCPurityLatex());
      ypos -= 0.05;
    }
  }

  void SaveUncertContribution(int ir, TString PlotName) {
    Plots[ir]->SaveUncertContribution(PlotName);
  }

  vector<double> LegendPos = {0.65,0.65,0.9,0.9};

  vector<string> GroupNames;
  string PlotNamePrefix;
  string Observable;
  vector<string> SampleTypes, Variations, Regions;
  string XTitle, YTitle;// [Observable]
  vector<RatioPlot*> Plots; // [Region]
  bool NormalizePlot = false;
  bool DoLogy = true;
  bool DrawSensitivity = false;
  bool DrawPurity = false;
};
#endif
