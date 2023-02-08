#ifndef HISTMANAGER_CC
#define HISTMANAGER_CC

#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLatex.h"
#include "TLine.h"
#include "TBox.h"

#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "../Utilities/Dataset.cc"
#include "CMSStyle.cc"
#include "RatioPlot.cc"

class HistManager{
public:
  HistManager(bool IsSR_ = false) {
    ResetMembers();
    IsSR = IsSR_;
    LegendPos = {0.65,0.65,0.9,0.9};
    Hists.clear();
  };

  void SetRegionLatex(TString rl) {
    RegionLatex = rl;
  }

  int GetType(string ds) {
    return dlib.GetType(ds);
  }

  void AddHist(string ds, TH1F* h_) {
    if (h_->GetEntries() == 0) return;
    TH1F* h = (TH1F*)h_->Clone();
    Hists[ds] = h;
  }

  void NormToLumi(int isy_) {
    for (auto it = Hists.begin(); it != Hists.end(); ++it) {
      auto sn = it->first;
      if (dlib.GetType(sn) == 0) continue;
      it->second->Scale(dlib.GetNormFactor(sn,isy_));
    }
  }

  // Manual factor to scale too small signal up, such that it can be visible in comparison
  double SignalScaleCalc(double sigmax, double targetmax) {
    double sc = 1.;
    while (sigmax * sc * 10. < targetmax) {
      sc *= 10.;
    }
    return sc;
  }

  float RebinCalc(double nbins, int target = 100) {
    float rb_ = 1;
    vector<float> inc{2.,2.5,2.}; // 2, 5, 10, 20, 50, 100 etc rebinning
    int ind = 0;
    while (nbins  / rb_ > target) { // rebin untill less than target bins in histogram;
      rb_ = inc[ind] * rb_;
      if (ind == 2) ind = 0;
      else ind++;
    }
    return rb_;
  }

  void RebinHists(int rb) {
    for (auto it = Hists.begin(); it != Hists.end(); ++it) {
      int rb_ = rb;
      // For rb < 0, it will try to rebin with factor 2,5,10,20,50,100, etc until the bin number is just less than rb * -1
      // For rb > 0, it will simply rebin every histograms.
      if (rb < 0) { 
        rb_ = RebinCalc(it->second->GetNbinsX(),-1 * rb);
      }
      it->second->Rebin(rb_);
    }
  }

  void SortHists() {
    for (unsigned ig = 0; ig < dlib.GroupNames.size(); ++ig) {
      string gp = dlib.GroupNames[ig];
      vector<string> dss = dlib.Groups[gp].DatasetNames;
      for (unsigned id = 0; id < dss.size(); ++id) {
        if (Hists[dss[id]] == nullptr) continue;
        if (GroupHists[gp] == nullptr) {
          GroupHists[gp] = (TH1F*) Hists[dss[id]]->Clone();
          int col = dlib.Groups[gp].Color;
          if (dlib.Groups[gp].Type == 1)GroupHists[gp]->SetFillColor(col);
          GroupHists[gp]->SetLineColor(col);
        }
        else GroupHists[gp]->Add(Hists[dss[id]]);
      }
    }
  }

  void PrepHists(TString tx, TString ty, TString fn) {
    SortHists();
    rp = new RatioPlot(fn, IsSR);
    PlotName = fn;
    rp->SetXTitle(tx);
    rp->SetYTitle(ty);
    for (unsigned i = 0; i < dlib.GroupNames.size(); ++i) {
      string gn = dlib.GroupNames[i];
      rp->AddHist(gn,GroupHists[gn],dlib.Groups[gn].Type);
    }
    rp->SetLogy(true);
    rp->Legend(LegendPos);
    dlib.AddLegend(rp->leg);
    rp->PrepHists();
  }

  double GetMaximum() {
    return rp->GetMaximum();
  }

  void SetMaximum(double ymax) {
    rp->MCStack->SetMaximum(ymax);
  }

  void DrawPlot(TVirtualPad* p_, int year) {
    rp->SetPad(p_);
    rp->DrawPlot(year);
    p_->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.025);
    latex.SetTextAlign(23);
    latex.DrawLatex((LegendPos[0] + LegendPos[2])/2., LegendPos[1] - 0.025, RegionLatex);
  }

  void ResetMembers() {
    Hists.clear();
  }

  // TVirtualPad* Pad;
  RatioPlot *rp;
  bool IsSR;
  TString PlotName;
  vector<double> LegendPos;
  TString RegionLatex;
  map<string, TH1F* > Hists;
  map<string, TH1F*> GroupHists;
};

#endif
