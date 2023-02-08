#ifndef SRPLOT_CC
#define SRPLOT_CC

#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TPad.h"
#include "TLegend.h"

#include <vector>
#include <string>
#include <iostream>

#include "CMSStyle.cc"

class SRPlot {
public:
  SRPlot(TVirtualPad* p_) {
    Pad = p_;
    Logy = true;
    Init();
  };

  void SetLogy(bool l = false) {
    Logy = l;
  }

  void Init() {
    setTDRStyle();
    Pad->cd();
    Pad->Draw();
    Pad->SetTopMargin(gStyle->GetPadTopMargin());
    Pad->SetBottomMargin(gStyle->GetPadBottomMargin());
    Pad->SetLogy(Logy);
  }

  void SetXTitle(TString xt) {
    XTitle = xt;
  }

  void SetYTitle(TString yt) {
    YTitle = yt;
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
    if (type_ == 0) return;
    else if (type_ == 1) AddMC(n_, h_);
    else if (type_ == 2) AddSig(n_, h_);
  }

  void Legend(double x1 = 0.65, double y1 = 0.65, double x2 = 0.9, double y2 = 0.9) {
    if (x2 == -1) x2 = 1. - Pad->GetRightMargin();
    if (y2 == -1) y2 = 1. - Pad->GetTopMargin();
    leg = new TLegend(x1,y1,x2,y2,"","NDC");
    leg->SetBorderSize(1);
    leg->SetNColumns(2);
  }

  void Legend(vector<double> lpos) { // In RatioPlot, calculations will be applied to make legend there of the same size as here
    double x1 = lpos[0];
    double y1 = lpos[1];
    double x2 = lpos[2];
    double y2 = lpos[3];
    Legend(x1,y1,x2,y2);
  }

  void DrawPlot(TString fn, int year = 0) {
    Pad->cd();
    TString utitle = ";" + XTitle + ";" + YTitle;
    TString stackname = Pad->GetName() + (TString)"_MCStack";
    MCStack = new THStack(stackname,utitle);
    for (unsigned ih = 0; ih < MCHists.size(); ++ih) {
      MCStack->Add(MCHists[ih]);
      // leg->AddEntry(MCHists[ih],MCNames[ih],"f");
    }
    // for (unsigned ih = 0; ih < SigHists.size(); ++ih) {
    //   leg->AddEntry(SigHists[ih],SigNames[ih],"l");
    // }
    MCStack->Draw("hist");
    for (unsigned ih = 0; ih < SigHists.size(); ++ih) {
      SigHists[ih]->Draw("samehist");
    }
    leg->Draw();
    MCStack->GetYaxis()->SetTitleSize(gStyle->GetTitleSize() * 0.7);
    MCStack->GetYaxis()->SetTitleOffset(gStyle->GetTitleOffset() * 1.0);
    MCStack->GetYaxis()->SetLabelSize(gStyle->GetLabelSize() * 0.7);
    MCStack->GetYaxis()->SetLabelOffset(gStyle->GetLabelOffset() * 0.7);
    MCStack->GetXaxis()->CenterTitle();
    MCStack->GetXaxis()->SetTitleSize(gStyle->GetTitleSize() * 0.7);
    MCStack->GetXaxis()->SetTitleOffset(gStyle->GetTitleOffset());
    MCStack->GetXaxis()->SetLabelSize(gStyle->GetLabelSize() * 0.7);
    MCStack->GetXaxis()->SetLabelOffset(gStyle->GetLabelOffset() * 0.7);
    CMSFrame(Pad,year);
    
    if (fn != "") Pad->SaveAs(fn);
  }


  bool Logy;

  TVirtualPad* Pad;

  TLegend* leg;
  TString XTitle, YTitle;

  vector<TH1F*> MCHists;
  vector<TString> MCNames;
  THStack* MCStack;

  vector<TH1F*> SigHists;
  vector<TString> SigNames;
};

#endif
