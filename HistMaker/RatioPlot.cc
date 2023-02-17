#ifndef RATIOPLOT_CC
#define RATIOPLOT_CC

#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraph.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm> // max,min

#include "CMSStyle.cc"

using namespace std;

class RatioPlot {
public:
  RatioPlot(TString pn, bool IsSR_ = false, TString xt = "", TString yt = "Number of Entries")
  {
    PlotName = pn;
    Logy = true;
    IsSR = IsSR_;
    if (IsSR) UTitle = ";" + xt + ";" + yt;
    else {
      UTitle = ";;" + yt;
      LTitle = ";" + xt + ";Data/MC";
    }
    TString stackname = PlotName + (TString)"_MCStack";
    MCStack = new THStack(stackname,UTitle);
    nbins = 0;
  };

  void SetLogy(bool l = true) {
    Logy = l;
    // UPad->SetLogy(l);
  }

  void SetTitles(TString xt, TString yt = "Number of Entries") {
    if (IsSR) UTitle = ";" + xt + ";" + yt;
    else {
      UTitle = ";;" + yt;
      LTitle = ";" + xt + ";Data/MC";
    }
    MCStack->SetTitle(UTitle);
  }

  void SetVariationTypes(vector<string> vars) {
    Variations = vars;
    VarSize = Variations.size();
    MCSummed.clear();
    SigHists.clear();
    MCSummed.resize(VarSize);
    //SigHists will be dynamically sized since it is [iSig][iVar] structure.
  }

  void AddData(TH1F* h_) {
    DataHist = (TH1F*)h_->Clone();
    DataHist->SetBinErrorOption(TH1::kPoisson);
    DataHist->SetLineStyle(1);
    DataHist->SetLineColor(1);
    DataHist->SetMarkerStyle(20);
    DataHist->SetTitle(UTitle); // Not necessary because Stack will be drawn first
  }

  void AddMC(TH1F* h_, int iv) {
    if (iv == 0) {
      h_->SetLineStyle(1);
      MCHists.push_back(h_);
      MCStack->Add(h_);
    }
    if (MCSummed[iv] == nullptr) MCSummed[iv] = (TH1F*) h_->Clone();
    else MCSummed[iv]->Add(h_);
  }

  void AddSig(TString n_, TH1F* h_, int iv) {
    if (iv == 0) {
      h_->SetLineStyle(2);
      h_->SetLineWidth(2);
    }
    int idx = -1;
    for (unsigned i = 0; i < SigNames.size(); ++i) {
      if (SigNames[i] == n_) idx = i;
    }
    if (idx == -1) {
      idx = SigNames.size();
      SigNames.push_back(n_);
      SigHists.push_back(vector<TH1F*>(VarSize)); // Potentially result in unfilled slot being nullptr
    }
    SigHists[idx][iv] = h_;
  }

  void AddHist(TString n_, TH1F* h_, int type_, int iv) {
    if (h_ == nullptr) return;
    if (type_ == 0) AddData(h_);
    else if (type_ == 1) AddMC(h_, iv);
    else if (type_ == 2) AddSig(n_, h_, iv);
    if (nbins == 0) {
      nbins = h_->GetNbinsX();
      xlow = h_->GetXaxis()->GetXmin();
      xup = h_->GetXaxis()->GetXmax();
    }
    else if (h_->GetNbinsX() != nbins) cout << "Inconsistent histogram nbins for " << n_ <<endl;
    else if (h_->GetXaxis()->GetXmin() != xlow) cout << "Inconsistent histogram xlow for " << n_ <<endl;
    else if (h_->GetXaxis()->GetXmax() != xup) cout << "Inconsistent histogram xup for " << n_ <<endl;
  }

  void StatError(TH1F* hcentral, vector<double>& errup, vector<double>& errlow) {
    if (hcentral == nullptr) return;
    hcentral->SetBinErrorOption(TH1::kPoisson);
    for (unsigned i = 0; i < nbins; ++i) {
      errup[i] = errup[i] + hcentral->GetBinErrorUp(i + 1) * hcentral->GetBinErrorUp(i + 1);
      errlow[i] = errlow[i] + hcentral->GetBinErrorLow(i + 1) * hcentral->GetBinErrorLow(i + 1);
    }
  }

  void SystError(TH1F* hcentral, TH1F* hvarup, TH1F* hvarlow, vector<double>& errup, vector<double>& errlow) {
    if (hcentral == nullptr) return;
    for (unsigned i = 0; i < nbins; ++i) {
      double diffup(0), difflow(0);
      if (hvarup != nullptr) diffup = hvarup->GetBinContent(i + 1) - hcentral->GetBinContent(i + 1);
      if (hvarlow != nullptr) difflow = hvarlow->GetBinContent(i + 1) - hcentral->GetBinContent(i + 1);
      double eu = max(max(diffup,difflow),0.0);
      double el = min(min(diffup,difflow),0.0);
      errup[i] = errup[i] + eu * eu;
      errlow[i] = errlow[i] + el * el;
    }
  }

  void ErrorCalc() {
    MCErrUp.clear();
    MCErrLow.clear();
    SigErrUp.clear();
    SigErrLow.clear();
    MCErrUp.resize(nbins, 0);
    MCErrLow.resize(nbins, 0);
    SigErrUp.resize(SigNames.size());
    SigErrLow.resize(SigNames.size());
    for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
      SigErrUp[isig].resize(nbins, 0);
      SigErrLow[isig].resize(nbins, 0);
    }
    for (unsigned iv = 0; iv < (VarSize+1)/2; ++iv) {
      if (iv == 0) {
        StatError(MCSummed[0],MCErrUp,MCErrLow);
        for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
          StatError(SigHists[isig][0], SigErrUp[isig], SigErrLow[isig]);
        }
      }
      else {
        SystError(MCSummed[0], MCSummed[iv*2-1],MCSummed[iv*2], MCErrUp, MCErrLow);
        for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
          SystError(SigHists[isig][0], SigHists[isig][iv*2-1], SigHists[isig][iv*2], SigErrUp[isig], SigErrLow[isig]);
        }
      }
    }
    for (unsigned ib = 0; ib < nbins; ++ib) {
      MCErrUp[ib] = sqrt(MCErrUp[ib]);
      MCErrLow[ib] = sqrt(MCErrLow[ib]);
      for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
        SigErrUp[isig][ib] = sqrt(SigErrUp[isig][ib]);
        SigErrLow[isig][ib] = sqrt(SigErrLow[isig][ib]);
      }
    }
  }

  void CreateErrorGraphs() {
    ErrorCalc();
    double bw = (xup - xlow) / (nbins + 0.0); // Ensure nbins double
    int lp = nbins * 4 - 1;
    double x[1000]; // A large enough size larger than any nbins
    double y[1000];
    for (unsigned i = 0; i < nbins; ++i) {
      x[2*i] = x[lp-2*i] = xlow + (i + 0.0) * bw;
      x[2*i+1] = x[lp-2*i-1] = xlow + (i + 1.0) * bw;
      y[2*i] = y[2*i+1] = y[lp-2*i] = y[lp-2*i-1] = MinY;
      double cent = MCSummed[0]->GetBinContent(i+1);
      if (cent <= MinY) continue;
      y[2*i] = y[2*i+1] = cent + MCErrUp[i];
      y[lp-2*i] = y[lp-2*i-1] = cent - MCErrLow[i];
    }
    MCErrorGraph = new TGraph(nbins * 4, x, y);
    MCErrorGraph->SetLineWidth(0);
    MCErrorGraph->SetFillColor(1);
    MCErrorGraph->SetFillStyle(ErrorBandFillStyle);

    SigErrorGraphs.resize(SigNames.size());
    for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
      for (unsigned i = 0; i < nbins; ++i) {
        y[2*i] = y[2*i+1] = y[lp-2*i] = y[lp-2*i-1] = MinY;
        double cent = SigHists[isig][0]->GetBinContent(i+1);
        if (cent <= MinY) continue;
        y[2*i] = y[2*i+1] = cent + SigErrUp[isig][i];
        y[lp-2*i] = y[lp-2*i-1] = cent - SigErrLow[isig][i];
      }
      SigErrorGraphs[isig] = new TGraph(nbins * 4, x, y);
      SigErrorGraphs[isig]->SetLineWidth(0);
      SigErrorGraphs[isig]->SetFillColor(SigHists[isig][0]->GetLineColor());
      SigErrorGraphs[isig]->SetFillStyle(ErrorBandFillStyle);
    }

    if (IsSR) return;
    if (RatioHist == nullptr) CreateRatioHist();
    // RatioHist->SetMarkerStyle(6); // Make it small;
    for (unsigned i = 0; i < nbins; ++i) {
      y[2*i] = y[2*i+1] = y[lp-2*i] = y[lp-2*i-1] = MinY;
      double cent = DataHist->GetBinContent(i+1);
      if (cent <= MinY) continue;
      if (MCSummed[0]->GetBinContent(i+1) - MCErrLow[i] > 0) {
        y[2*i] = y[2*i+1] = cent / (MCSummed[0]->GetBinContent(i+1) - MCErrLow[i]);
      }
      if (MCSummed[0]->GetBinContent(i+1) + MCErrUp[i] > 0) {
        y[lp-2*i] = y[lp-2*i-1] = cent / (MCSummed[0]->GetBinContent(i+1) + MCErrUp[i]);
      } 
    }
    RatioErrorGraph = new TGraph(nbins * 4, x, y);
    RatioErrorGraph->SetLineWidth(0);
    RatioErrorGraph->SetFillColor(1);
    RatioErrorGraph->SetFillStyle(ErrorBandFillStyle);
    int i = 51;
  }

  void Legend(vector<double> lpos) { // Todo: Dynamic legend position and compress the yaxis if necessary
    double x1 = lpos[0];
    double y1 = lpos[1];
    double x2 = lpos[2];
    double y2 = lpos[3];
    leg = new TLegend(x1,y1,x2,y2,"","NDC");
    leg->SetBorderSize(1);
    leg->SetNColumns(2);
  }

  void CreateRatioHist() {
    if (IsSR) return;
    RatioHist = (TH1F*)DataHist->Clone();
    RatioHist->Divide(MCSummed[0]);
    RatioHist->SetTitle(LTitle);
    RatioHist->GetYaxis()->SetRangeUser(0,2);
    RatioHist->GetYaxis()->SetNdivisions(505);
  }

  double GetMaximum() {
    if (IsSR) return MCStack->GetMaximum();
    if (DataHist->GetMaximum() > MCStack->GetMaximum()) return DataHist->GetMaximum();
    else return MCStack->GetMaximum();
  }

  void SetMaximum(double max) {
    MCStack->SetMaximum(max);
  }

  double GetSensitivity(TString SensSig = "M500") {
    double SigIntegral = 0;
    for (unsigned i = 0; i < SigNames.size(); ++i) {
      if (SigNames[i] == SensSig) SigIntegral = SigHists[i][0]->Integral();
    }
    double MCIntegral = MCSummed[0]->Integral();
    double sens = SigIntegral / sqrt(SigIntegral + MCIntegral);
    return sens;
  }

  // void ScaleSignal(int ss = 1) {
  //  // ScaleSignal < 0: auto scale; 1 >= ScaleSignal >= 0: Scale by that ; ScaleSignal = 0: do not scale
  //   for (unsigned ih = 0; ih < SigHists.size(); ++ih) {
  //     TString signame = SigNames[ih];
  //     if ((SigHists[ih]->GetMaximum() * 10.< maximum && ss < 0) || ss > 1) {
  //       double scale = ss;
  //       if (scale < 0) scale = SignalScaleCalc(SigHists[ih]->GetMaximum(), maximum);
  //       SigHists[ih]->Scale(scale);
  //       signame = Form("%s*%i",signame.Data(),(int)scale);
  //     }
  //   }
  // }

  void SetPad(TVirtualPad* p_) {
    setTDRStyle();
    Pad = p_;
    Pad->cd();
    Pad->UseCurrentStyle();
    if (IsSR) {
      UPad = Pad;
      UPad->SetTopMargin(gStyle->GetPadTopMargin());
      UPad->SetBottomMargin(gStyle->GetPadBottomMargin());
      UPad->SetLogy(Logy);
      UPad->Draw();
    }
    else {
      TString uppadname = PlotName + (TString)"_upper";
      TString lowpadname = PlotName + (TString)"_lower";
      UPad = new TPad(uppadname,uppadname,0,0.3,1,1);
      UPad->SetTopMargin(gStyle->GetPadTopMargin()/0.7);
      UPad->SetBottomMargin(0.0);
      UPad->SetLogy(Logy);
      UPad->Draw();
      LPad = new TPad(lowpadname,lowpadname,0,0,1,0.3);
      LPad->Draw();
      LPad->SetTopMargin(0.1);
      LPad->SetTopMargin(gStyle->GetPadTopMargin()*0.3);
      LPad->SetBottomMargin(gStyle->GetPadBottomMargin()/0.3);
      LPad->SetGridy();
    }
  }

  void DrawPlot(int year) {
    UPad->cd();
    MCStack->Draw("hist");
    if (MCErrorGraph != nullptr) MCErrorGraph->Draw("samef");
    if (!IsSR) DataHist->Draw("E1same");
    for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
      if (SigHists[isig][0]->GetEntries() == 0) continue;
      SigHists[isig][0]->Draw("samehist");
      if (SigErrorGraphs.size() == SigNames.size()) SigErrorGraphs[isig]->Draw("samef");
    }
    Pad->cd();
    leg->Draw();
    // The coefficients are tried out and tested to be placed at same location on canvas
    if (IsSR) {
      MCStack->GetYaxis()->SetTitleSize(gStyle->GetTitleSize() * 0.7);
      MCStack->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset() / 0.7);
      MCStack->GetYaxis()->SetLabelSize(gStyle->GetLabelSize() * 0.7);
      MCStack->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset() * 0.7);
      MCStack->GetXaxis()->CenterTitle();
      MCStack->GetXaxis()->SetTitleSize(gStyle->GetTitleSize() * 0.7);
      MCStack->GetXaxis()->SetTitleOffset(gStyle->GetTitleOffset());
      MCStack->GetXaxis()->SetLabelSize(gStyle->GetLabelSize() * 0.7);
      MCStack->GetXaxis()->SetLabelOffset(gStyle->GetLabelOffset() * 0.3);
    }
    else {
      MCStack->GetYaxis()->SetTitleSize(gStyle->GetTitleSize());
      MCStack->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset());
      MCStack->GetYaxis()->SetLabelSize(gStyle->GetLabelSize());
      MCStack->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset());

      LPad->cd();
      RatioHist->GetXaxis()->CenterTitle();
      RatioHist->GetXaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3 * 0.7);
      RatioHist->GetXaxis()->SetTitleOffset(gStyle->GetTitleOffset());
      RatioHist->GetXaxis()->SetLabelSize(gStyle->GetLabelSize() / 0.3 * 0.7);
      RatioHist->GetXaxis()->SetLabelOffset(gStyle->GetLabelOffset());

      RatioHist->GetYaxis()->CenterTitle();
      RatioHist->GetYaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3 * 0.7);
      RatioHist->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset() * 0.5 );
      RatioHist->GetYaxis()->SetLabelSize(gStyle->GetLabelSize()/ 0.3 * 0.7);
      RatioHist->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset());

      RatioHist->Draw("");
      if (RatioErrorGraph != nullptr) RatioErrorGraph->Draw("samef");
    }
    CMSFrame(UPad,year);
  }

  bool Logy, IsSR;

  TVirtualPad* Pad;
  TVirtualPad* UPad;
  TVirtualPad* LPad;
  TLegend* leg;

  TString PlotName, UTitle, LTitle;

  vector<string> Variations;
  int VarSize;
  int nbins;
  double xlow, xup;
  double MinY = 0;

  TH1F* DataHist;

  vector<TH1F*> MCHists;
  vector<TH1F*> MCSummed; // [iVariation]
  THStack* MCStack;
  TH1F* RatioHist;

  vector<vector<TH1F*> > SigHists; // [iSig][iVariation]
  vector<TString> SigNames;

  vector<double> MCErrUp, MCErrLow; // [nBins]
  vector< vector<double> > SigErrUp, SigErrLow; //[iSig][nBins]

  int ErrorBandFillStyle = 3002;
  TGraph* MCErrorGraph;
  vector<TGraph*> SigErrorGraphs;
  TGraph* RatioErrorGraph;
};



#endif
