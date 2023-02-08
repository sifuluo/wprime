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
    setTDRStyle();
    // pad->cd();
    // TString pname = Pad->GetName() + (TString)"Styled";
    // Pad = new TPad(pname,pname,0,0,1,1);
    Pad = pad;
    Pad->UseCurrentStyle();
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

  int GetType(string ds) {
    return dlib.GetType(ds);
  }

  void SetAutoScaleSignal(bool d = true) {
    AutoScaleSignal = d;
  }

  void AddHist(string ds, TH1F* h_) {
    TH1F* h = (TH1F*)h_->Clone();
    if (h->GetEntries() == 0) return;
    // h->SetLineColor(dlib.GetColor(ds));
    // if (GetType(ds) == 0) {
    //   h->SetLineStyle(1);
    //   h->SetLineColor(1);
    //   h->SetMarkerStyle(20); // 20:filled circle, 21:filled square
    //   DataHists[ds] = h;
    // }
    // if (GetType(ds) == 1) {
    //   h->SetLineStyle(1);
    //   h->SetLineColor(dlib.GetColor(ds));
    //   h->SetFillColor(dlib.GetColor(ds));
    //   MCHists[ds] = h;
    // }
    // if (GetType(ds) == 2) {
    //   h->SetLineStyle(2);
    //   h->SetLineColor(1);
    //   SignalHists[ds] = h;
    // }
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
        // cout << it->second->GetName() << " has " << it->second->GetNbinsX() << " bins, rebinning with " << rb_ <<endl;
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


    // //Group and order MC dataset groups by their dataset indices
    // for (auto it = MCHists.begin(); it != MCHists.end(); ++it) {
    //   string gp = dlib.GetGroup(it->first);
    //   if (GroupMCHists.find(gp) == GroupMCHists.end()) {
    //     GroupMCHists[gp] = (TH1F*)it->second->Clone();
    //     // First dataset of a group determines the index of the group
    //     // The dataset list should be ordered by group manually in the first place
    //     GroupMCOrderedByIndices[dlib.GetIndex(it->first)] = gp;
    //   }
    //   else GroupMCHists[gp]->Add(it->second);
    // }
    // // Order MC dataset groups by their integrals
    // for (auto it = GroupMCHists.begin(); it != GroupMCHists.end(); ++it) {
    //   double integral = it->second->Integral();
    //   GroupMCOrderedByIntegral[integral] = it->first;
    // }
    // // Order Signal dataset by their indices
    // for (auto it = SignalHists.begin(); it != SignalHists.end(); ++it) {
    //   SignalOrderedByIndices[dlib.GetIndex(it->first)] = it->first;
    // }
  }

  void DrawRatioPlot(TString tx, TString ty, TString fn, int year) {
    RatioPlot *rp = new RatioPlot(Pad);
    rp->SetXTitle(tx);
    rp->SetYTitle(ty);

    for (unsigned i = 0; i < dlib.GroupNames.size(); ++i) {
      string gn = dlib.GroupNames[i];
      rp->AddHist(gn,GroupHists[gn],dlib.Groups[gn].Type);
    }

    // if (DataHists.size() == 0) throw runtime_error("No Data histogram provided for RatioPlot");
    // rp->AddData(DataHists.begin()->second);
    // for (auto it = GroupMCOrderedByIndices.begin(); it != GroupMCOrderedByIndices.end(); ++it) {
    //   rp->AddMC(it->second, GroupMCHists[it->second]);
    // }
    // for (auto it = SignalOrderedByIndices.begin(); it != SignalOrderedByIndices.end(); ++it) {
    //   rp->AddSig(it->second, SignalHists[it->second]);
    // }
    rp->SetLogy();
    rp->Legend(LegendPos);
    dlib.AddLegend(rp->leg);
    rp->DrawPlot(fn, year);
  }

  void DrawSRPlot(TString tx, TString ty, TString fn, int year) {
    SRPlot *srp = new SRPlot(Pad);

    // for (auto it = GroupHists.begin(); it != GroupHists.end(); ++it) {
    //   srp->AddHist(it->first, it->second, dlib.Groups[it->first].Type);
    // }
    for (unsigned i = 0; i < dlib.GroupNames.size(); ++i) {
      string gn = dlib.GroupNames[i];
      srp->AddHist(gn,GroupHists[gn],dlib.Groups[gn].Type);
    }

    // for (auto it = GroupMCOrderedByIndices.begin(); it != GroupMCOrderedByIndices.end(); ++it) {
    //   srp->AddMC(it->second, GroupMCHists[it->second]);
    // }
    // for (auto it = SignalOrderedByIndices.begin(); it != SignalOrderedByIndices.end(); ++it) {
    //   srp->AddSig(it->second, SignalHists[it->second]);
    // }
    srp->SetLogy();
    srp->Legend(LegendPos);
    dlib.AddLegend(srp->leg);
    srp->SetXTitle(tx);
    srp->SetYTitle(ty);
    srp->DrawPlot(fn,year);
  }

  void DrawPlot(TString tx, TString ty, TString fn, int year) {
    SortHists();
    if (fn != "") fn = "plots/" + fn + ".pdf";
    if (DrawRatio) {
      DrawRatioPlot(tx,ty,fn,year);
    }
    else DrawSRPlot(tx,ty,fn,year);
  }

  void ResetMembers() {
    DrawData = DrawRatio = true;
    AutoScaleSignal = true;
    Hists.clear();
    DataHists.clear();
    MCHists.clear();
    SignalHists.clear();
    SignalOrderedByIndices.clear();
    GroupMCHists.clear();
    GroupMCOrderedByIndices.clear();
    GroupMCOrderedByIntegral.clear();
  }

  TVirtualPad* Pad;
  bool DrawData, DrawRatio, AutoScaleSignal;
  vector<double> LegendPos;
  map<string, TH1F* > Hists;
  map<string, TH1F* > DataHists; // Pair may also do, for there will be only 1 data. But map stays consistent with other containers
  map<string, TH1F* > MCHists;
  map<string, TH1F* > SignalHists;
  map<int,string> SignalOrderedByIndices;

  map<string, TH1F* > GroupMCHists;
  map<int,string> GroupMCOrderedByIndices; // For legend ordering
  map<double,string> GroupMCOrderedByIntegral; // For THStack ordering:
  map<string, TH1F*> GroupHists;
};

#endif
