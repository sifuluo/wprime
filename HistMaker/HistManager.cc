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
#include "SRPlot.cc"
#include "Tools.cc"

class HistManager{
public:
  HistManager(TVirtualPad* pad) {
    Pad = pad;
    ResetMembers();
    LegendPos = {0.65,0.65,0.9,0.9};
  };

  void SetDrawData(bool d = true) {
    DrawData = d;
    DrawRatio = d;
  }

  void SetDrawRatio(bool d = true) {
    DrawRatio = d;
    if (DrawRatio && !DrawData) cout << "Ratio cannot be drawn without drawing data" <<endl;
  }

  void SetRegionLatex(TString rl) {
    RegionLatex = rl;
  }

  int GetType(string ds) {
    return dlib.GetType(ds);
  }

  void AddHist(string ds, TH1F* h_) {
    TH1F* h = (TH1F*)h_->Clone();
    if (h->GetEntries() == 0) return;
    Hists[ds] = h;
  }

  void NormToLumi(int isy_) {
    for (auto it = Hists.begin(); it != Hists.end(); ++it) {
      auto sn = it->first;
      if (dlib.GetType(sn) == 0) continue;
      it->second->Scale(dlib.GetNormFactor(sn,isy_));
    }
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

  void DrawRatioPlot(TString tx, TString ty, TString fn, int year) {
    rp = new RatioPlot(fn, !DrawRatio);
    rp->SetPad(Pad);
    rp->SetXTitle(tx);
    rp->SetYTitle(ty);

    for (unsigned i = 0; i < dlib.GroupNames.size(); ++i) {
      string gn = dlib.GroupNames[i];
      rp->AddHist(gn,GroupHists[gn],dlib.Groups[gn].Type);
    }
    rp->SetLogy();
    rp->Legend(LegendPos, RegionLatex);
    dlib.AddLegend(rp->leg);
    rp->DrawPlot(year);
    rp->SavePlot(fn);
  }

  // void DrawSRPlot(TString tx, TString ty, TString fn, int year) {
  //   srp = new SRPlot();
  //   srp->SetPad(Pad);
  //   srp->SetXTitle(tx);
  //   srp->SetYTitle(ty);
  //   for (unsigned i = 0; i < dlib.GroupNames.size(); ++i) {
  //     string gn = dlib.GroupNames[i];
  //     srp->AddHist(gn,GroupHists[gn],dlib.Groups[gn].Type);
  //   }
  //   srp->SetLogy();
  //   srp->Legend(LegendPos,RegionLatex);
  //   dlib.AddLegend(srp->leg);
  //   srp->DrawPlot(fn,year);
  //   srp->SavePlot(fn);
  // }

  void DrawPlot(TString tx, TString ty, TString fn, int year) {
    SortHists();
    if (fn != "") fn = "plots/" + fn + ".pdf";
    DrawRatioPlot(tx,ty,fn,year);
    // if (DrawRatio) {
    //   DrawRatioPlot(tx,ty,fn,year);
    // }
    // else DrawSRPlot(tx,ty,fn,year);
  }

  void ResetMembers() {
    DrawData = DrawRatio = true;
    Hists.clear();
  }

  TVirtualPad* Pad;
  SRPlot *srp;
  RatioPlot *rp;
  bool DrawData, DrawRatio;
  vector<double> LegendPos;
  TString RegionLatex;
  map<string, TH1F* > Hists;
  map<string, TH1F*> GroupHists;
};

#endif
