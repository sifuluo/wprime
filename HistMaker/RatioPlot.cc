#ifndef RATIOPLOT_CC
#define RATIOPLOT_CC

#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TPad.h"
#include "TLegend.h"

#include <vector>
#include <string>
#include <iostream>

#include "CMSStyle.cc"

using namespace std;

class RatioPlot {
public:
  RatioPlot(TString fn, bool IsSR_ = false) {
    PlotName = fn;
    Logy = true;
    IsSR = IsSR_;
  };

  void SetLogy(bool l = true) {
    Logy = l;
    // UPad->SetLogy(l);
  }

  void SetXTitle(TString xt) {
    XTitle = xt;
  }

  void SetYTitle(TString yt) {
    YTitle = yt;
  }

  void AddData(TH1F* h_) {
    DataHist = (TH1F*)h_->Clone();
    DataHist->SetLineStyle(1);
    DataHist->SetLineColor(1);
    DataHist->SetMarkerStyle(20);
  }

  void AddMC(TString n_, TH1F* h_) {
    h_->SetLineStyle(1);
    MCHists.push_back(h_);
    MCNames.push_back(n_);
  }

  void AddSig(TString n_, TH1F* h_) {
    h_->SetLineStyle(2);
    SigHists.push_back(h_);
    SigNames.push_back(n_);
  }

  void AddHist(TString n_, TH1F* h_, int type_) {
    if (h_ == nullptr) return;
    if (type_ == 0) AddData(h_);
    else if (type_ == 1) AddMC(n_, h_);
    else if (type_ == 2) AddSig(n_, h_);
  }

  void Legend(vector<double> lpos, TString reg) {
    double x1 = lpos[0];
    double y1 = lpos[1];
    double x2 = lpos[2];
    double y2 = lpos[3];
    leg = new TLegend(x1,y1,x2,y2,reg,"NDC");
    leg->SetBorderSize(1);
    leg->SetNColumns(2);
  }

  void PrepHists() {
    TString utitle = ";;" + YTitle;
    if (IsSR) utitle = ";" + XTitle + ";" + YTitle;
    TString stackname = PlotName + (TString)"_MCStack";
    MCStack = new THStack(stackname,utitle);
    for (unsigned ih = 0; ih < MCHists.size(); ++ih) {
      if (ih == 0) MCSummed = (TH1F*) MCHists[0]->Clone();
      else MCSummed->Add(MCHists[ih]);
      MCStack->Add(MCHists[ih]);
    }

    if (IsSR) return;
    DataHist->SetTitle(utitle);

    TString ltitle = ";" + XTitle + ";Data/MC";
    RatioHist = (TH1F*)DataHist->Clone();
    RatioHist->Divide(MCSummed);
    RatioHist->SetTitle(ltitle);
    RatioHist->GetYaxis()->SetRangeUser(0,2);
    RatioHist->GetYaxis()->SetNdivisions(505);
  }

  double GetMaximum() {
    if (IsSR) return MCStack->GetMaximum();
    if (DataHist->GetMaximum() > MCStack->GetMaximum()) return DataHist->GetMaximum();
    else return MCStack->GetMaximum();
  }

  // void ScaleSignal(int ss = 1) { // ScaleSignal < 0: auto scale; 1 >= ScaleSignal >= 0: Scale by that ; ScaleSignal = 0: do not scale
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
      TString upname = PlotName + (TString)"_upper";
      TString lowname = PlotName + (TString)"_lower";
      UPad = new TPad(upname,upname,0,0.3,1,1);
      UPad->SetTopMargin(gStyle->GetPadTopMargin()/0.7);
      UPad->SetBottomMargin(0.0);
      UPad->SetLogy(Logy);
      UPad->Draw();
      LPad = new TPad(lowname,lowname,0,0,1,0.3);
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
    if (!IsSR) DataHist->Draw("E1same");
    for (unsigned ih = 0; ih < SigHists.size(); ++ih) {
      SigHists[ih]->Draw("samehist");
    }
    Pad->cd();
    leg->Draw();

    if (IsSR) {
      MCStack->GetYaxis()->SetTitleSize(gStyle->GetTitleSize() * 0.7);
      MCStack->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset() * 1.0);
      MCStack->GetYaxis()->SetLabelSize(gStyle->GetLabelSize() * 0.7);
      MCStack->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset() * 0.7);
      MCStack->GetXaxis()->CenterTitle();
      MCStack->GetXaxis()->SetTitleSize(gStyle->GetTitleSize() * 0.7);
      MCStack->GetXaxis()->SetTitleOffset(gStyle->GetTitleOffset());
      MCStack->GetXaxis()->SetLabelSize(gStyle->GetLabelSize() * 0.7);
      MCStack->GetXaxis()->SetLabelOffset(gStyle->GetLabelOffset() * 0.7);
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
      RatioHist->GetYaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3*0.7);
      RatioHist->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset() * 0.5 );
      RatioHist->GetYaxis()->SetLabelSize(gStyle->GetLabelSize()/ 0.3 * 0.7);
      RatioHist->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset());

      RatioHist->Draw();
    }
    CMSFrame(UPad,year);
  }

  // void DrawLPlot() {
  //   if (IsSR) return;
  //   LPad->cd();
  //   RatioHist->GetXaxis()->CenterTitle();
  //   RatioHist->GetXaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3 * 0.7);
  //   RatioHist->GetXaxis()->SetTitleOffset(gStyle->GetTitleOffset());
  //   RatioHist->GetXaxis()->SetLabelSize(gStyle->GetLabelSize() / 0.3 * 0.7);
  //   RatioHist->GetXaxis()->SetLabelOffset(gStyle->GetLabelOffset());

  //   RatioHist->GetYaxis()->CenterTitle();
  //   RatioHist->GetYaxis()->SetTitleSize(gStyle->GetTitleSize() / 0.3*0.7);
  //   RatioHist->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset() * 0.5 );
  //   RatioHist->GetYaxis()->SetLabelSize(gStyle->GetLabelSize()/ 0.3 * 0.7);
  //   RatioHist->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset());

  //   RatioHist->Draw();
  // }

  // void DrawPlot(int year) {
  //   DrawUPlot(year);
  //   DrawLPlot();
  // }

  // void SavePlot(TString fn) {
  //   if (fn != "") {
  //     fn = "plots/" + fn + ".pdf";
  //     Pad->SaveAs(fn);
  //   }
  // }

  bool Logy, IsSR;

  TVirtualPad* Pad;
  TVirtualPad* UPad;
  TVirtualPad* LPad;
  TLegend* leg;

  TString XTitle, YTitle, PlotName;

  TH1F* DataHist;

  vector<TH1F*> MCHists;
  vector<TString> MCNames;
  THStack* MCStack;
  TH1F* MCSummed;
  TH1F* RatioHist;

  vector<TH1F*> SigHists;
  vector<TString> SigNames;
};



#endif
