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

#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "DrawDataFormat.cc"
#include "CMSStyle.cc"
#include "RatioPlot.cc"

class HistManager : public Histograms {
public:
  HistManager() : Histograms() {
    GroupNames = dlib.GroupNames;
  };
  vector<string> GroupNames;

  void SetPrefix(string prefix) {
    PlotNamePrefix = prefix;
  }
  string PlotNamePrefix;

  void SetTitles(vector<string> xt, vector<string> yt) {
    XTitles = xt;
    YTitles = yt;
  }
  vector<string> XTitles, YTitles;

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
  vector< vector< vector< vector<TH1F*> > > > GroupHists; // [Group][Variation][Region][Observable]

  void PrepHists() {
    SortHists();
    Plots.clear();
    Plots.resize(Regions.size());
    for (unsigned ir = 0; ir < Regions.size(); ++ir) {
      bool IsSR = rm.Ranges[ir].IsSR;
      Plots[ir].resize(Observables.size());
      for (unsigned io = 0; io < Observables.size(); ++io) {
        TString PlotName = PlotNamePrefix + "_" + Observables[io] + "_" + rm.StringRanges[ir];
        Plots[ir][io] = new RatioPlot(PlotName,IsSR);
        Plots[ir][io]->SetXTitle(XTitles[io]);
        Plots[ir][io]->SetYTitle(YTitles[io]);
        for (unsigned ig = 0; ig < GroupNames.size(); ++ig) {
          string gn = dlib.GroupNames[ig];
          Plots[ir][io]->AddHist(gn, GroupHists[ig][0][ir][io], dlib.Groups[gn].Type);
        }
        Plots[ir][io]->SetLogy(true);
        Plots[ir][io]->Legend(LegendPos);
        dlib.AddLegend(Plots[ir][io]->leg,IsSR);
        Plots[ir][io]->PrepHists();
      }
    }
  }
  vector< vector<RatioPlot*> > Plots;

  double GetMaximum(int ir, int io) {
    return Plots[ir][io]->GetMaximum();
  }

  void SetMaximum(int ir, int io, double max) {
    Plots[ir][io]->MCStack->SetMaximum(max);
  }

  void DrawPlot(int ir, int io, TVirtualPad* p_, int year) {
    Plots[ir][io]->SetPad(p_);
    Plots[ir][io]->DrawPlot(year);
    p_->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.025);
    latex.SetTextAlign(23);
    latex.DrawLatex((LegendPos[0] + LegendPos[2])/2., LegendPos[1] - 0.025, rm.LatexRanges[ir]);
  }
  vector<double> LegendPos = {0.65,0.65,0.9,0.9};

  // void ErrorCalc() {
  //   // Resize the Error vectors
  //   ErrorUp.clear();
  //   ErrorUp.resize(Variations.size());
  //   ErrorLow.clear();
  //   ErrorLow.resize(Variations.size());
  //   for (unsigned iv = 0; iv < Variations.size(); ++iv)
  //   {
  //     ErrorUp[iv].resize(Regions.size());
  //     ErrorLow[iv].resize(Regions.size());
  //     for (unsigned ir = 0; ir < Regions.size(); ++ir)
  //     {
  //       ErrorUp[iv][ir].resize(Observables.size());
  //       ErrorLow[iv][ir].resize(Observables.size());
  //     }
  //   }

  //   // Sum the error of MC datasets of the same variation type, region, and observable together
  //   for (unsigned ig = 0; ig < GroupNames.size(); ++ig) {
  //     string gp = GroupNames[ig];
  //     if (dlib.Groups[gp].Type != 1) continue;
  //     for (unsigned iv = 0; iv < Variations.size(); ++iv) {
  //       for (unsigned ir = 0; ir < Regions.size(); ++ir) {
  //         for (unsigned io = 0; io < Observables.size(); ++io) {
  //           for (unsigned ib = 0; ib < nbins[io]; ++ib) {
  //             double errup, errlow;
  //           }
  //         }
  //       }
  //     }
  //   }
  // }
  // vector< vector< vector< vector<double> > > > ErrorUp, ErrorLow; // [Variation][Region][Observable][Bin]

  // void SumMCHists() {
  //   SumHists.clear();
  //   SumHists.resize(Variations.size());
  //   for (unsigned iv = 0; iv < Variations.size(); ++iv)
  //   {
  //     SumHists[iv].resize(Regions.size());
  //     for (unsigned ir = 0; ir < Regions.size(); ++ir)
  //     {
  //       SumHists[iv][ir].resize(Observables.size());
  //       for (unsigned io = 0; io < Observables.size(); ++io) {
  //         for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
  //           if (Hists[ist][iv][ir][io] == nullptr) continue;
  //           if (SumHists[iv][ir][io] == nullptr) {
  //             SumHists[iv][ir][io] = (TH1F*) Hists[ist][iv][ir][io]->Clone();
  //           }
  //           else SumHists[iv][ir][io]->Add(Hists[ist][iv][ir][io]);
  //         }
  //       }
  //     }
  //   }

  // }
  // vector< vector< vector<TH1F*> > > SumHists; // [Variation][Region][Observable]
};

/*
class HistManagerBack{
public:
  HistManagerBack(bool IsSR_ = false) {
    ResetMembers();
    IsSR = IsSR_;
    LegendPos = {0.65,0.65,0.9,0.9};
    Hists.clear();
  };

  void SetName(TString fn) {
    PlotName = fn;
  }
  
  void SetRegionLatex(TString rl) {
    RegionLatex = rl;
  }

  int GetType(string ds) {
    return dlib.GetType(ds);
  }

  void AddHist(string ds, TH1F* h_) {
    if (h_ == nullptr) return;
    if (h_->GetEntries() == 0) return;
    TH1F* h = (TH1F*)h_->Clone();
    Hists[ds] = h;
  }

  // Moved to the step when filling histograms
  // void NormToLumi(int isy_) { 
  //   for (auto it = Hists.begin(); it != Hists.end(); ++it) {
  //     auto sn = it->first;
  //     if (dlib.GetType(sn) == 0) continue;
  //     it->second->Scale(dlib.GetNormFactor(sn,isy_));
  //   }
  // }

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

  void PrepHists(TString tx, TString ty) {
    SortHists();
    rp = new RatioPlot(PlotName, IsSR);
    rp->SetXTitle(tx);
    rp->SetYTitle(ty);
    for (unsigned i = 0; i < dlib.GroupNames.size(); ++i) {
      string gn = dlib.GroupNames[i];
      rp->AddHist(gn,GroupHists[gn],dlib.Groups[gn].Type);
    }
    rp->SetLogy(true);
    rp->Legend(LegendPos);
    dlib.AddLegend(rp->leg, IsSR);
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
*/
#endif
