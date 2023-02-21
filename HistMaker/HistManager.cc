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

class HistManager : public Histograms {
public:
  HistManager() : Histograms() {
    GroupNames = dlib.GroupNames;
  };
  
  void SetPrefix(string prefix) {
    PlotNamePrefix = prefix;
  }

  void SetTitles(vector<string> xt, vector<string> yt) {
    XTitles = xt;
    YTitles = yt;
  }
  
  // Reading Histograms
  void ReadHistograms(vector<string> obss, TFile *f) {
    Observables = obss;
    Hists.clear();
    Hists.resize(SampleTypes.size());
    nbins.clear();
    xlow.clear();
    xup.clear();
    nbins.resize(Observables.size());
    xlow.resize(Observables.size());
    xup.resize(Observables.size());
    for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
      Hists[ist].resize(Variations.size());
      for (unsigned iv = 0; iv < Variations.size(); ++iv) {
        Hists[ist][iv].resize(Regions.size());
        for (unsigned ir = 0; ir < Regions.size(); ++ir) {
          Hists[ist][iv][ir].resize(Observables.size());
          for (unsigned io = 0; io < Observables.size(); ++io) {
            TString histname = GetHistName(ist, iv, ir, io);
            Hists[ist][iv][ir][io] = (TH1F*) f->Get(histname);
            if (Hists[ist][iv][ir][io] != nullptr && nbins[io] == 0) {
              nbins[io] = Hists[ist][iv][ir][io]->GetNbinsX();
              xlow[io] = Hists[ist][iv][ir][io]->GetXaxis()->GetXmin();
              xup[io] = Hists[ist][iv][ir][io]->GetXaxis()->GetXmax();
            }
          }
        }
      }
    }
  }

  void SortHists() {
    GroupHists.clear();
    GroupHists.resize(GroupNames.size());
    for (unsigned ig = 0; ig < GroupNames.size(); ++ig)
    {
      GroupHists[ig].resize(Variations.size());
      for (unsigned iv = 0; iv < Variations.size(); ++iv)
      {
        GroupHists[ig][iv].resize(Regions.size());
        for (unsigned ir = 0; ir < Regions.size(); ++ir)
        {
          GroupHists[ig][iv][ir].resize(Observables.size());
        }
      }
    }

    for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
      string gp = dlib.GetGroup(SampleTypes[ist]);
      int ig = dlib.GetGroupIndexFromGroupName(gp);
      if (ig < 0) continue;
      for (unsigned iv = 0; iv < Variations.size(); ++iv) {
        for (unsigned ir = 0; ir < Regions.size(); ++ir) {
          for (unsigned io = 0; io < Observables.size(); ++io) {
            if (Hists[ist][iv][ir][io] == nullptr) continue;
            if (GroupHists[ig][iv][ir][io] == nullptr) { // First hist for the group
              GroupHists[ig][iv][ir][io] = (TH1F*) Hists[ist][iv][ir][io]->Clone();
              int col = dlib.Groups[gp].Color;
              if (dlib.Groups[gp].Type == 1) GroupHists[ig][iv][ir][io]->SetFillColor(col);
              GroupHists[ig][iv][ir][io]->SetLineColor(col);
            }
            else
              GroupHists[ig][iv][ir][io]->Add(Hists[ist][iv][ir][io]);
          }
        }
      }
    }
  }

  void PrepHists() {
    SortHists();
    // PrepErrorGraphs();
    Plots.clear();
    Plots.resize(Regions.size());
    for (unsigned ir = 0; ir < Regions.size(); ++ir) {
      bool IsSR = rm.Ranges[ir].IsSR;
      Plots[ir].resize(Observables.size());
      for (unsigned io = 0; io < Observables.size(); ++io) {
        TString PlotName = PlotNamePrefix + "_" + Observables[io] + "_" + rm.StringRanges[ir];
        Plots[ir][io] = new RatioPlot(PlotName,IsSR, XTitles[io], YTitles[io]);
        Plots[ir][io]->SetVariationTypes(Variations);
        for (unsigned ig = 0; ig < GroupNames.size(); ++ig) {
          for (unsigned iv = 0; iv < Variations.size(); ++iv) {
            string gn = GroupNames[ig];
            Plots[ir][io]->AddHist(gn, GroupHists[ig][iv][ir][io], dlib.Groups[gn].Type,iv);
          }
        }
        Plots[ir][io]->SetLogy(true);
        Plots[ir][io]->Legend(LegendPos);
        dlib.AddLegend(Plots[ir][io]->leg,IsSR);
      }
    }
  }

  void CreateAuxiliaryPlots(int ir, int io) {
    Plots[ir][io]->CreateRatioPlots();
    Plots[ir][io]->CreateErrorGraphs();
  }

  double GetMaximum(int ir, int io) {
    return Plots[ir][io]->GetMaximum();
  }

  void SetMaximum(int ir, int io, double max) {
    Plots[ir][io]->SetMaximum(max);
  }

  void DrawPlot(int ir, int io, TVirtualPad* p_, int year) {
    Plots[ir][io]->SetPad(p_);
    Plots[ir][io]->DrawPlot(year);
    Plots[ir][io]->UPad->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.035);
    latex.SetTextAlign(23);
    latex.DrawLatex((LegendPos[0] + LegendPos[2])/2., LegendPos[1] - 0.025, rm.LatexRanges[ir]);
    TString sens = Form("Sig/#sqrt{Sig + BG} = %f", Plots[ir][io]->GetSensitivity());
    latex.DrawLatex((LegendPos[0] + LegendPos[2])/2., LegendPos[1] - 0.065, sens);
  }
  vector<double> LegendPos = {0.65,0.65,0.9,0.9};

  // Inherited members
  // vector< vector< vector< vector<TH1F*> > > > Hists; // Hists[isampletype][ivariation][irange][iobservable]
  // vector<string> SampleTypes, Variations, Regions, Observables;
  // vector<int> nbins; // [nObservables]
  // vector<double> xlow, xup; // [nObservables]
  // TString NameFormat;

  vector<string> GroupNames;
  string PlotNamePrefix;
  vector<string> XTitles, YTitles;
  vector< vector< vector< vector<TH1F*> > > > GroupHists; // [Group][Variation][Region][Observable]
  vector< vector<RatioPlot*> > Plots; // [Region][Observable]
};
#endif
