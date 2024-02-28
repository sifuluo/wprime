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
#include "HistsForCombine.cc"

class HistManager{
public:
  HistManager() {
    GroupNames = dlib.GroupNames;
    SampleTypes = dlib.DatasetNames;
    Variations = rm.Variations;
    Regions = rm.StringRanges;
    rebin = 1;
  };

  // void SetObservable(string ob) {
  //   Observable = ob;
  // }

  // void SetTitles(string xt, string yt = "") {
  //   XTitle = xt;
  //   if (yt == "") yt = "Number of Entries";
  //   YTitle = yt;
  // }

  void SetObservable(PlotObservable& o) {
    if (o.YTitle == "") o.YTitle = "Number of Entries";
    if (o.LegPos == 0) LegendPos = {0.65,0.65,0.9,0.9};
    if (o.LegPos == 1) LegendPos = {0.2,0.65,0.45,0.9};
    if (o.LegPos == 2) LegendPos = {0.5,0.5,0.9,0.9};
    if (o.LegPos == 3) LegendPos = {0.55,0.55,0.9,0.9};
    if (o.LegPos == 4) LegendPos = {0.6,0.6,0.9,0.9};
    if (o.LegPos == -1) LegendPos = o.LegendPos;
    po = o;
  }

  void Rebin(int r) {
    rebin = r;
  }

  void SetSampleTypes(vector<string> sts) {
    SampleTypes = sts;
  }
  
  void SetPrefix(string prefix) {
    PlotNamePrefix = prefix;
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
  void ReadHistograms(string path, string prefix, int DoMCReweight = 0) {
    Plots.clear();
    Plots.resize(Regions.size());
    InFiles.clear();
    for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
      string thisprefix = prefix;
      if (SampleTypes[ist] == "ttbar") {
        if (DoMCReweight == 1) thisprefix += "_RW";
        else if (DoMCReweight == 2) thisprefix += "_RW2On2";
        else thisprefix += "_NRW";
      }
      TString fn = StandardNames:: HistFileName(path, thisprefix, po.Observable, SampleTypes[ist]);
      InFiles.push_back(new TFile(fn, "READ"));
      cout << "Reading from file " << fn << endl;
    }
    for (unsigned ir = 0; ir < Regions.size(); ++ir) {
      bool IsSR = rm.Ranges[ir].IsSR;
      TString PlotName = PlotNamePrefix + "_" + po.Observable + "_" + rm.StringRanges[ir];
      Plots[ir] = new RatioPlot(PlotName,IsSR, po.XTitle, po.YTitle);
      Plots[ir]->SetVariations(Variations);
      if (po.YEnlarge != 1.0) Plots[ir]->YEnlarge(po.YEnlarge);
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
          TString hn = StandardNames::HistName(SampleTypes[ist], po.Observable, Regions[ir], Variations[iv]);
          TH1F* h = (TH1F*) InFiles[ist]->Get(hn);
          if (h == nullptr) {
            // cout << hn << " is nullptr" <<endl;
            continue;
          }
          if (rebin > 1) h->Rebin(rebin);
          // if (po.Observable == "METPt") h->GetXaxis()->SetRangeUser(0,1000); // Very special case, should be changed in MakeHistValidation step instead
          if (PlotGroupHists[ig][iv] == nullptr) { // First hist for the group
            PlotGroupHists[ig][iv] = (TH1F*) h->Clone();
            int col = dlib.Groups[gp].Color;
            if (dlib.Groups[gp].Type == 1) PlotGroupHists[ig][iv]->SetFillColor(col);
            PlotGroupHists[ig][iv]->SetLineColor(col);
            TString ghn = StandardNames::HistName(GroupNames[ig], po.Observable, Regions[ir], Variations[iv]);
            PlotGroupHists[ig][iv]->SetName(ghn);
          }
          else {
            PlotGroupHists[ig][iv]->Add(h);
          }
          // delete h;
        }
      }
      
      HistsForCombine *hc;
      bool InCombineRegion = (Regions[ir] == "1153" || Regions[ir] == "1163" || Regions[ir] == "2153" || Regions[ir] == "2163");
      bool dohc = false;
      if (po.Observable == "WPrimeMass" && InCombineRegion) {
        hc = new HistsForCombine("SimpleShapes", Regions[ir]);
        dohc = true;
      }
      if (po.Observable == "WPrimeMassSimpleFL" && InCombineRegion) {
        hc = new HistsForCombine("SimpleWpMassFL", Regions[ir]);
        dohc = true;
      }
      
      for (unsigned ig = 0; ig < GroupNames.size(); ++ig) {
        string gn = GroupNames[ig];
        bool gpadded = false;
        for (unsigned iv = 0; iv < Variations.size(); ++iv) {
          if (PlotGroupHists[ig][iv] == nullptr) {
            if (PlotGroupHists[ig][0] == nullptr) continue;
            PlotGroupHists[ig][iv] = (TH1F*) PlotGroupHists[ig][0]->Clone();
            TString hn = PlotGroupHists[ig][0]->GetName();
            TString post = "_" + rm.Variations[iv];
            PlotGroupHists[ig][iv]->SetName(hn + post);
            // cout << "Replacing " << iv << " with " << PlotGroupHists[ig][0]->GetName() << endl;
          }
          gpadded = true;
          // cout << "Adding " << PlotGroupHists[ig][iv]->GetName() << endl;
          if (NormalizePlot) PlotGroupHists[ig][iv]->Scale(1./PlotGroupHists[ig][iv]->Integral());
          // if (rebin > 1) PlotGroupHists[ig][iv]->Rebin(rebin);
          Plots[ir]->AddHist(gn, PlotGroupHists[ig][iv], dlib.Groups[gn].Type,iv);
          if (dohc && gn != "Data") hc->AddHist(gn, PlotGroupHists[ig][iv],iv);
        }
        for (unsigned iv = 0; iv < Variations.size(); ++iv) {
          delete PlotGroupHists[ig][iv];
        }
        // if (gpadded) cout << "Added " << gn << " with type = " << dlib.Groups[gn].Type << endl;
      }
      Plots[ir]->SetLogy(DoLogy);
      Plots[ir]->Legend(LegendPos);
      dlib.AddLegend(Plots[ir]->leg,IsSR);
      if (dohc) {
        hc->MakeDummyData();
        hc->Done();
      }
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
    Plots[ir]->DrawPlot(year, po.xmin, po.xmax);
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

  void SaveInfos(int ir, TString PlotName) {
    Plots[ir]->SaveInfos(PlotName);
  }

  vector<double> LegendPos = {0.65,0.65,0.9,0.9};

  vector<string> GroupNames;
  string PlotNamePrefix;
  // string Observable;
  // string XTitle, YTitle;
  PlotObservable po;
  int rebin;
  vector<string> SampleTypes, Variations, Regions;
  vector<RatioPlot*> Plots; // [Region]
  vector<TFile*> InFiles; // [iSampleType]
  bool NormalizePlot = false;
  bool DoLogy = true;
  bool DrawSensitivity = false;
  bool DrawPurity = false;
};
#endif
