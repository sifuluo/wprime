#ifndef UNCERTAINTIES_CC
#define UNCERTAINTIES_CC

#include "TString.h"
#include "TH1.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

class Uncertainties{
public:
  Uncertainties() {

  };

  vector<double> AddCentral(TH1F* h) {
    if (h == nullptr) return {};
    // hcentral = (TH1F*)h->Clone();
    hcentral = h;
    MinY = 0;
    nbins = hcentral->GetNbinsX();
    xlow = hcentral->GetXaxis()->GetXmin();
    xup = hcentral->GetXaxis()->GetXmax();
    if (nbins == 0) return {};
    center.clear();
    errup.clear();
    errlow.clear();
    center.resize(nbins,0);
    errup.resize(nbins,0);
    errlow.resize(nbins,0);
    bool doreport = false;
    bool reported = false;
    double StatErrorIntegralNom(0), StatErrorIntegralUp(0), StatErrorIntegralDown(0);
    hcentral->SetBinErrorOption(TH1::kPoisson);
    for (unsigned i = 0; i < nbins; ++i) {
      if (hcentral->GetBinErrorUp(i + 1) != hcentral->GetBinErrorUp(i + 1)) {
        if (!reported && doreport) {
          cout << "Stat has val of nan for " << hcentral->GetName() << endl;
          reported = true;
        }
        continue;
      }
      center[i] = hcentral->GetBinContent(i + 1);
      errup[i] = errup[i] + hcentral->GetBinErrorUp(i + 1) * hcentral->GetBinErrorUp(i + 1);
      errlow[i] = errlow[i] + hcentral->GetBinErrorLow(i + 1) * hcentral->GetBinErrorLow(i + 1);
      StatErrorIntegralNom += center[i];
      StatErrorIntegralUp += (center[i] + hcentral->GetBinErrorUp(i + 1));
      StatErrorIntegralDown += (center[i] - hcentral->GetBinErrorLow(i + 1));
    }
    if (StatErrorIntegralNom != hcentral->Integral()) cout << "StatErrorIntegralNom = " << StatErrorIntegralNom << ", Integral = " << hcentral->Integral() << endl;
    vector<double> out = {StatErrorIntegralNom, StatErrorIntegralUp, StatErrorIntegralDown};
    return out;
  }

  vector<double> AddVarianceSet(TH1F* hvarup, TH1F* hvarlow) {
    if (hcentral == nullptr) return {};
    if (hvarup->Integral() / hcentral->Integral() > 2. || hvarup->Integral() / hcentral->Integral() < 0.5) cout << hvarup->GetName() << " ratio to central " << hcentral->GetName() << " = " <<hvarup->Integral() / hcentral->Integral() << endl;
    if (hvarlow->Integral() / hcentral->Integral() > 2. || hvarlow->Integral() / hcentral->Integral() < 0.5) cout << hvarlow->GetName() << " ratio to central " << hcentral->GetName() << " = " <<hvarlow->Integral() / hcentral->Integral() << endl;
    bool doreport = false;
    bool reportedup = false;
    bool reportedlow = false;
    double SystErrorIntegralNom(0), SystErrorIntegralUp(0), SystErrorIntegralDown(0);
    // cout << errup[5] <<"," << errlow[5] <<endl;
    for (unsigned i = 0; i < nbins; ++i) {
      double diffup(0), difflow(0);
      if (hvarup != nullptr) diffup = hvarup->GetBinContent(i + 1) - center[i];
      if (hvarlow != nullptr) difflow = hvarlow->GetBinContent(i + 1) - center[i];
      if (diffup != diffup) {
        diffup = 0;
        if (!reportedup && doreport) {
          cout << "Diff up has  val of nan for " << hvarup->GetName() << " and " << hcentral->GetName() << endl;
          reportedup = true;
        }
      }
      if (difflow != difflow) {
        difflow = 0;
        if (!reportedlow && doreport) {
          cout << "Diff low has  val of nan for " << hvarlow->GetName() << " and " << hcentral->GetName() << endl;
          reportedlow = true;
        }
      }
      double eu = max(max(diffup,difflow),0.0);
      double el = min(min(diffup,difflow),0.0);
      // if (i == 5) cout << "diffup = " << diffup << ", difflow = " << difflow <<endl;
      // if (i == 5) cout << "eu = " << eu << ", el = " << el <<endl;
      errup[i] = errup[i] + eu * eu;
      errlow[i] = errlow[i] + el * el;
      SystErrorIntegralNom += center[i];
      SystErrorIntegralUp += eu;
      SystErrorIntegralDown += el;
    }
    if (doreport) cout << errup[5] <<"," << errlow[5] <<endl;
    vector<double> out = {SystErrorIntegralNom, SystErrorIntegralUp, SystErrorIntegralDown};
    return out;
  }

  TGraph* CreateErrorGraph(int ErrorBandFillStyle = 3002) {
    double x[1000];
    double y[1000];
    int lp = nbins * 4 - 1;
    for (unsigned i = 0; i < nbins; ++i) {
      x[2*i] = x[lp-2*i] = hcentral->GetXaxis()->GetBinLowEdge(i+1);
      x[2*i+1] = x[lp-2*i-1] = hcentral->GetXaxis()->GetBinUpEdge(i+1);
    }
    for (unsigned i = 0; i < nbins; ++i) {
      y[2*i] = y[2*i+1] = y[lp-2*i] = y[lp-2*i-1] = MinY;
      double cent = hcentral->GetBinContent(i+1);
      if (cent <= MinY) continue;
      y[2*i] = y[2*i+1] = cent + sqrt(errup[i]);
      y[lp-2*i] = y[lp-2*i-1] = cent - sqrt(errlow[i]);
    }
    ErrorBand = new TGraph(nbins * 4,x,y);
    ErrorBand->SetLineWidth(0);
    ErrorBand->SetFillStyle(ErrorBandFillStyle);
    return ErrorBand;
  }

  /* Block specially designed for MCReweighting*/
  
  // void Minus(Uncertainties* u) {
  //   for (unsigned i = 0; i < nbins; ++i) {
  //     center[i] = center[i] - u->center[i];
  //     errup[i] = errup[i] + u->errup[i];
  //     errlow[i] = errlow[i] + u->errlow[i];
  //   }
  // }

  // void Divide(Uncertainties* u) {
  //   for (unsigned i = 0; i < nbins; ++i)
  // }

  TH1F* hcentral;
  TGraph *ErrorBand;
  unsigned nbins;
  double xlow, xup, MinY;
  vector<double> center, errup, errlow;
};

#endif