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
    Init();
    PlotName = pn;
    Logy = true;
    IsSR = IsSR_;
    UTitle = ";;" + yt;
    LTitle = ";" + xt + ";Obs./Exp.";
    TString stackname = PlotName + (TString)"_MCStack";
    MCStack = new THStack(stackname,UTitle);
  };

  void Init() {
    MCHists.clear();
    MCNames.clear();
    MCSummed.clear();
    latex.SetNDC();
    latex.SetTextSize(0.035);
    latex.SetTextAlign(23);
  }

  void SetLogy(bool l = true) {
    Logy = l;
    // UPad->SetLogy(l);
  }

  void SetTitles(TString xt, TString yt = "Number of Entries") {
    UTitle = ";;" + yt;
    LTitle = ";" + xt + ";Data/MC";
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
    if (DataHist->GetMaximum() > TrueMaximum) TrueMaximum = DataHist->GetMaximum();
    HasData = true;
  }

  void AddMC(TString n_, TH1F* h_, int iv) {
    if (iv == 0) {
      h_->SetLineStyle(1);
      MCHists.push_back(h_); // Saved into vector, since THStack class doesn't own the histograms.
      MCNames.push_back(n_);
      MCStack->Add(h_);
      HasMC = true;
    }
    if (MCSummed[iv] == nullptr) {
      MCSummed[iv] = (TH1F*) h_->Clone();
      MCSummed[iv]->SetLineColor(1);
    }
    else MCSummed[iv]->Add(h_);
    if (MCSummed[iv]->GetMaximum() > TrueMaximum) TrueMaximum = MCSummed[iv]->GetMaximum();
  }

  void AddSig(TString n_, TH1F* h_, int iv) {
    if (iv == 0) {
      h_->SetLineStyle(2);
      h_->SetLineWidth(2);
      HasSig = true;
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
    // Signal index in RatioPlot is dynamically incremented,
    // thus is not always the same as it is in Dataset.cc
    SigHists[idx][iv] = h_;
    if (h_->GetMaximum() > TrueMaximum) TrueMaximum = h_->GetMaximum();
  }

  void AddHist(TString n_, TH1F* h_, int type_, int iv) {
    if (h_ == nullptr) return;
    if (type_ == 0) AddData(h_);
    else if (type_ == 1) AddMC(n_, h_, iv);
    else if (type_ == 2) AddSig(n_, h_, iv);
    if (nbins == 0) {
      nbins = h_->GetNbinsX();
      xlow = h_->GetXaxis()->GetXmin();
      xup = h_->GetXaxis()->GetXmax();
    }
    else if (h_->GetNbinsX() != nbins) cout << "Inconsistent histogram nbins for " << n_ <<endl;
    else if (h_->GetXaxis()->GetXmin() != xlow) cout << "Inconsistent histogram xlow for " << n_ <<endl;
    else if (h_->GetXaxis()->GetXmax() != xup) cout << "Inconsistent histogram xup for " << n_ <<endl;
    // cout << "Adding Histogram " << n_  << " of variation " << iv <<endl;
  }

  void PrepHists() {
    if (!HasMC) {
      StackDummy = new TH1F("","",nbins,xlow, xup);
      // StackDummy->SetLineWidth(0);
      MCStack->Add(StackDummy);
      double x[2] = {xlow, xup};
      double y[2] = {1.,1.};
      MCErrorRatioGraph = new TGraph(2,x,y);
    }
    if (!HasMC && !HasData && HasSig) {
      for (unsigned i = 0; i < SigNames.size(); ++i) {
        SigHists[i][0]->SetLineStyle(1);
      }
    }
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
        if (HasMC) StatError(MCSummed[0],MCErrUp,MCErrLow);
        for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
          StatError(SigHists[isig][0], SigErrUp[isig], SigErrLow[isig]);
        }
      }
      else {
        if (HasMC) SystError(MCSummed[0], MCSummed[iv*2-1],MCSummed[iv*2], MCErrUp, MCErrLow);
        for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
          SystError(SigHists[isig][0], SigHists[isig][iv*2-1], SigHists[isig][iv*2], SigErrUp[isig], SigErrLow[isig]);
        }
      }
    }
    for (unsigned ib = 0; ib < nbins; ++ib) {
      if (HasMC) {
        MCErrUp[ib] = sqrt(MCErrUp[ib]);
        MCErrLow[ib] = sqrt(MCErrLow[ib]);
      }
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
    }

    if (HasMC) {
      for (unsigned i = 0; i < nbins; ++i) {
        y[2*i] = y[2*i+1] = y[lp-2*i] = y[lp-2*i-1] = MinY;
        double cent = MCSummed[0]->GetBinContent(i+1);
        if (cent <= MinY) continue;
        y[2*i] = y[2*i+1] = cent + MCErrUp[i];
        y[lp-2*i] = y[lp-2*i-1] = cent - MCErrLow[i];
      }
      MCErrorGraph = new TGraph(nbins * 4, x, y);
      MCErrorGraph->SetLineWidth(0);
      MCErrorGraph->SetLineColor(0);
      MCErrorGraph->SetFillColor(1);
      MCErrorGraph->SetFillStyle(ErrorBandFillStyle);
    }
    
    SigErrorGraphs.resize(SigNames.size()); // SigNames size will prevent signal absent case crash the code
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

    if (HasMC) {
      for (unsigned i = 0; i < nbins; ++i) {
        y[2*i] = y[2*i+1] = y[lp-2*i] = y[lp-2*i-1] = 1.0;
        double cent = MCSummed[0]->GetBinContent(i+1);
        if (cent <= MinY) continue;  // Prevent -nan value in graph
        y[2*i] = y[2*i+1] = (cent + MCErrUp[i]) / cent;
        y[lp-2*i] = y[lp-2*i-1] = (cent - MCErrLow[i]) / cent;
      }
      MCErrorRatioGraph = new TGraph(nbins * 4, x, y);
    }
    MCErrorRatioGraph->SetLineWidth(0);
    MCErrorRatioGraph->SetLineColor(0);
    MCErrorRatioGraph->SetFillColor(1);
    MCErrorRatioGraph->SetFillStyle(ErrorBandFillStyle);
  }

  void CreateRatioPlots() {
    if (!HasMC) return;
    ExpOverMC.resize(SigNames.size());
    for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
      ExpOverMC[isig] = (TH1F*)SigHists[isig][0]->Clone();
      ExpOverMC[isig]->Add(MCSummed[0]);
      ExpOverMC[isig]->Divide(MCSummed[0]);
    }
    if (!IsSR && HasData) {
      DataOverMC = (TH1F*)DataHist->Clone();
      DataOverMC->SetTitle(LTitle); // Not necessary because ErrorGraph will be drawn first
      DataOverMC->Divide(MCSummed[0]);
    }
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

  void CreateLegendEntries() {
    if (HasMC) {
      for (unsigned i = 0; i < MCNames.size(); ++i) {
        leg->AddEntry(MCHists[i],MCNames[i],"f");
      }
    }
    if (HasSig) {
      for (unsigned i = 0; i < SigNames.size(); ++i) {
        leg->AddEntry(SigHists[i][0],SigNames[i],"l");
      }
    }
    if (HasData) {
      leg->AddEntry(DataHist, "Data", "p");
    }
    
  }

  double GetMaximum() {
    return TrueMaximum;
  }

  void SetMaximum(double max) {
    CanvasMaximum = max;
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

  void SetPad(TVirtualPad* p_) {
    setTDRStyle();
    Pad = p_;
    Pad->cd();
    Pad->UseCurrentStyle();
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

  bool DrawPlot(int year) { // return false if nothing to draw
    if (!HasData && !HasMC && !SigNames.size()) return false;
    UPad->cd();
    
    if (CanvasMaximum > 0) MCStack->SetMaximum(CanvasMaximum);
    else MCStack->SetMaximum(TrueMaximum);
    MCStack->Draw("hist");

    if (MCErrorGraph != nullptr && HasMC) {
      MCErrorGraph->Draw("samef");
      leg->AddEntry(MCErrorGraph,"Bkg. Unc.","f");
    }

    if (!IsSR && HasData) DataHist->Draw("E1same");

    for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
      if (SigHists[isig][0]->GetEntries() == 0) continue;
      SigHists[isig][0]->Draw("samehist");
      if (SigErrorGraphs.size() == SigNames.size()) SigErrorGraphs[isig]->Draw("f");
    }
    
    // The coefficients are tried out and tested to be placed at same location on canvas
    MCStack->GetYaxis()->SetTitleSize(gStyle->GetTitleSize());
    MCStack->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset());
    MCStack->GetYaxis()->SetLabelSize(gStyle->GetLabelSize());
    MCStack->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset());

    // Pad->cd();
    leg->Draw();

    LPad->cd();
    MCErrorRatioGraph->SetTitle(LTitle);
    MCErrorRatioGraph->GetXaxis()->SetRangeUser(xlow, xup);
    MCErrorRatioGraph->GetYaxis()->SetRangeUser(0, 2.4);
    MCErrorRatioGraph->GetYaxis()->SetNdivisions(505);

    MCErrorRatioGraph->GetXaxis()->CenterTitle();
    MCErrorRatioGraph->GetXaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3 * 0.7);
    MCErrorRatioGraph->GetXaxis()->SetTitleOffset(gStyle->GetTitleOffset());
    MCErrorRatioGraph->GetXaxis()->SetLabelSize(gStyle->GetLabelSize() / 0.3 * 0.7);
    MCErrorRatioGraph->GetXaxis()->SetLabelOffset(gStyle->GetLabelOffset());

    MCErrorRatioGraph->GetYaxis()->CenterTitle();
    MCErrorRatioGraph->GetYaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3 * 0.7);
    MCErrorRatioGraph->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset() * 0.5 );
    MCErrorRatioGraph->GetYaxis()->SetLabelSize(gStyle->GetLabelSize()/ 0.3 * 0.7);
    MCErrorRatioGraph->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset());
    MCErrorRatioGraph->Draw("af");

    if (HasMC) {  
      if (!IsSR && HasData) DataOverMC->Draw("same");
      for (unsigned isig = 0; isig < SigNames.size(); ++isig) {
        ExpOverMC[isig]->Draw("same hist ][");
      }
    }
    
    CMSFrame(UPad,year);
    return true;
  }

  void DrawLatex(TString st, int iline = 0, double x = -1., double y = -1., int al = 23) {
    UPad->cd(); // Remember to reverse to previous pad after drawing latex!
    latex.SetTextAlign(al);
    if (x == -1) x = (leg->GetX1() + leg->GetX2()) / 2.;
    if (y == -1) y = leg->GetY1() - 0.025;
    y -= 1.15 * latex.GetTextSize() * iline;
    latex.DrawLatex(x,y,st);
  }

  bool Logy, IsSR;

  TVirtualPad* Pad;
  TVirtualPad* UPad;
  TVirtualPad* LPad;
  TLegend* leg = nullptr;
  TLatex latex;

  TString PlotName, UTitle, LTitle;

  vector<string> Variations;
  int VarSize = 0;
  int nbins = 0;
  double xlow, xup;
  double MinY = 0;
  double CanvasMaximum = 0;
  double TrueMaximum = 0;

  bool HasData = false;
  bool HasMC = false;
  bool HasSig = false;

  TH1F* DataHist = nullptr;

  vector<TH1F*> MCHists;
  vector<TString> MCNames;
  vector<TH1F*> MCSummed; // [iVariation]
  THStack* MCStack;
  TH1F* StackDummy = nullptr;
  TH1F* DataOverMC = nullptr;

  vector<vector<TH1F*> > SigHists; // [iSig][iVariation]
  vector<TString> SigNames;
  vector<TH1F*> ExpOverMC; // [iSig];

  vector<double> MCErrUp, MCErrLow; // [nBins]
  vector< vector<double> > SigErrUp, SigErrLow; //[iSig][nBins]

  int ErrorBandFillStyle = 3002;
  TGraph* MCErrorGraph = nullptr;
  vector<TGraph*> SigErrorGraphs;
  TGraph* MCErrorRatioGraph = nullptr;

};



#endif
