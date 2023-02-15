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
    PrepErrorGraphs();
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
        Plots[ir][io]->SetErrorGraph(ErrorGraphs[ir][io]);
      }
    }
  }

  void PrepErrorGraphs() {
    SumUpMCHists();
    ErrorCalc();
    CombineErrors();
    CreateErrorGraphs();
  }

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

  void SumUpMCHists() {
    SumMCHists.clear();
    SumMCHists.resize(Variations.size());
    for (unsigned iv = 0; iv < Variations.size(); ++iv)
    {
      SumMCHists[iv].resize(Regions.size());
      for (unsigned ir = 0; ir < Regions.size(); ++ir) {
        SumMCHists[iv][ir].resize(Observables.size());
        for (unsigned io = 0; io < Observables.size(); ++io) {
          for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
            if (dlib.GetType(SampleTypes[ist]) != 1) continue;
            if (Hists[ist][iv][ir][io] == nullptr) continue;
            if (SumMCHists[iv][ir][io] == nullptr) {
              SumMCHists[iv][ir][io] = (TH1F*) Hists[ist][iv][ir][io]->Clone();
            }
            else SumMCHists[iv][ir][io]->Add(Hists[ist][iv][ir][io]);
          }
        }
      }
    }
  }

  void ErrorCalc() {
    // Resize the Error vectors
    ErrorUp.clear();
    ErrorUp.resize(Variations.size());
    ErrorLow.clear();
    ErrorLow.resize(Variations.size());
    for (unsigned iv = 0; iv < Variations.size(); ++iv)
    {
      int vartype = iv % 2; //  1: Up, 0: Down
      ErrorUp[iv].resize(Regions.size());
      ErrorLow[iv].resize(Regions.size());
      for (unsigned ir = 0; ir < Regions.size(); ++ir)
      {
        ErrorUp[iv][ir].resize(Observables.size());
        ErrorLow[iv][ir].resize(Observables.size());
        for (unsigned io = 0; io < Observables.size(); ++io) {
          vector<double> errup, errlow;
          errup.clear();
          errlow.clear();
          if (iv == 0) StatError(SumMCHists[iv][ir][io], errup, errlow,0);
          else SystError(SumMCHists[0][ir][io], SumMCHists[iv][ir][io], errup, errlow, vartype);
          ErrorUp[iv][ir][io] = errup;
          ErrorLow[iv][ir][io] = errlow;
        }
      }
    }
  }

  // void StatErrorSimPoisson(TH1F* hcentral, vector<double>& errup, vector<double>& errlow) {
  //   const double alpha = 1 - 0.6827;
  //   for (unsigned i = 0; i < hcentral->GetNbinsX(); ++i) {
  //     double N = hcentral->GetBinContent(i+1);
  //     double U =  ROOT::Math::gamma_quantile_c(alpha/2,N+1,1) ;
  //     double L = (N==0) ? 0  : (ROOT::Math::gamma_quantile(alpha/2,N,1.));
  //     errup.push_back(U-N);
  //     errlow.push_back(N-L);
  //   }
  // }

  void StatError(TH1F* hcentral, vector<double>& errup, vector<double>& errlow, int doPoisson = 0) {
    if (doPoisson == 1) hcentral->SetBinErrorOption(TH1::kPoisson);
    // else if (doPoisson == 2) {
    //   StatErrorSimPoisson(hcentral, errup, errlow);
    //   return;
    // }
    for (unsigned i = 0; i < hcentral->GetNbinsX(); ++i) {
      errup.push_back(hcentral->GetBinErrorUp(i+1));
      errlow.push_back(hcentral->GetBinErrorLow(i+1));
    }
  }

  void SystError(TH1F* hcentral, TH1F* hvar, vector<double>& errup, vector<double>& errlow, int type) { // type: 1: Up, 0: Down
    for (unsigned i = 0; i < hcentral->GetNbinsX(); ++i) {
      double diff = hvar->GetBinContent(i+1) - hcentral->GetBinContent(i+1);
      if (type == 1) {
        if (diff > 0) errup.push_back(diff);
        else errup.push_back(0);
      }
      else {
        if (diff < 0) errlow.push_back(-1.0 * diff);
        else errlow.push_back(0);
      }
    }
  }

  void CombineErrors() {
    ErrorUpCombined.clear();
    ErrorLowCombined.clear();
    ErrorUpCombined.resize(Regions.size());
    ErrorLowCombined.resize(Regions.size());
    for (unsigned ir = 0; ir < Regions.size(); ++ir) {
      ErrorUpCombined[ir].resize(Observables.size());
      ErrorLowCombined[ir].resize(Observables.size());
      for (unsigned io = 0; io < Observables.size(); ++io) {
        // bool dbg = false;
        // if (rm.Ranges[ir].b1 == 1251 && io == 0) dbg = true;
        ErrorUpCombined[ir][io].resize(nbins[io]);
        ErrorLowCombined[ir][io].resize(nbins[io]);
        for (unsigned ib = 0; ib < nbins[io]; ++ib) {
          double errupsq(0), errlowsq(0);
          for (unsigned iv = 0; iv < Variations.size(); ++iv) {
            if (iv == 0) {
              errupsq += ErrorUp[iv][ir][io][ib] * ErrorUp[iv][ir][io][ib];
              errlowsq += ErrorLow[iv][ir][io][ib] * ErrorLow[iv][ir][io][ib];
            }
            else {
              int vartype = iv % 2; //  1: Up, 0: Down
              if (vartype == 1) errupsq += ErrorUp[iv][ir][io][ib] * ErrorUp[iv][ir][io][ib];
              if (vartype == 0) errlowsq += ErrorLow[iv][ir][io][ib] * ErrorLow[iv][ir][io][ib];
            }
          }
          ErrorUpCombined[ir][io][ib] = sqrt(errupsq);
          ErrorLowCombined[ir][io][ib] = sqrt(errlowsq); 
        }
      }
    }
  }

  void CreateErrorGraphs() {
    ErrorGraphs.resize(Regions.size());
    for (unsigned ir = 0; ir < Regions.size(); ++ir) {
      ErrorGraphs[ir].resize(Observables.size());
      for (unsigned io = 0; io < Observables.size(); ++io) {
        double x[1000];
        double y[1000];
        int lastpoint = nbins[io] * 4 - 1;
        double binwidth = (xup[io] - xlow[io]) / nbins[io];
        for (unsigned i = 0; i < nbins[io]; ++i) {
          double lowedge = xlow[io] + (i + 0.0) * binwidth;
          double upedge = xlow[io] + (i + 1.0) * binwidth;
          x[2*i] = x[lastpoint-i*2] = lowedge;
          x[2*i+1] = x[lastpoint-i*2-1] = upedge;
          y[2*i] = y[2*i+1] = SumMCHists[0][ir][io]->GetBinContent(i + 1) + ErrorUpCombined[ir][io][i];
          y[lastpoint-2*i] = y[lastpoint-2*i-1] = SumMCHists[0][ir][io]->GetBinContent(i + 1) - ErrorLowCombined[ir][io][i];
        }
        ErrorGraphs[ir][io] = new TGraph(4 * nbins[io], x, y);
        ErrorGraphs[ir][io]->SetFillColor(1);
        ErrorGraphs[ir][io]->SetFillStyle(3002);
      }
    }
  }
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
  vector< vector< vector<TH1F*> > > SumMCHists; // [Variation][Region][Observable]
  vector< vector< vector< vector<double> > > > ErrorUp, ErrorLow; // [Variation][Region][Observable][Bin]
  vector< vector< vector<double> > > ErrorUpCombined, ErrorLowCombined; // [Region][Observable][Bin]
  vector< vector<TGraph*> > ErrorGraphs; // [Region][Observable]
};
#endif
