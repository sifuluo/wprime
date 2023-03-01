#ifndef FITTER_CC
#define FITTER_CC

#include <vector>

#include "TROOT.h"
#include "TLorentzVector.h"

//For Minimizer
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TError.h"

#include "JetScale.cc"
#include "bTag.cc"

using namespace std;

class Fitter{
public:
  Fitter(JetScale *JS_) {
    JS = JS_;
  };

  void SetbTag(bTagEff* bt_) {
    bTE = bt_;
  }

  void SetJets(vector<Jet> alljets_) {
    AllJets = alljets_;
  }

  void SetLep(TLorentzVector lep_) {
    Lep = lep_;
  }
  
  void SetMET(TLorentzVector met_) {
    MET = met_;
  }

  static double MinimizePFunc(const double *scales) {
    double Prob = JS->JetScaleLikelihood(Jets, Lep, MET, scales);
    return Prob;
  }

  double MinimizeP(vector<Jet>& Jets_) {
    func = ROOT::Math::Functor(&MinimizePFunc,4);
    mini->SetPrintLevel(0);
    if (debug) {
      mini->SetPrintLevel(3);
    }
    mini->SetStrategy(3);
    mini->SetMaxFunctionCalls(100000);
    mini->SetMaxIterations(10000);
    mini->SetTolerance(0.01);
    mini->SetErrorDef(0.5);
    mini->SetFunction(func);
    ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(100000);

    Jets.resize(4);
    for (unsigned i = 0; i < 4; ++i) {
      Jets[i] = (TLorentzVector) Jets_[i];
      pair<double,double> limits = JS->ScaleLimits(Jets[i].Eta(),Jets[i].Pt());
      mini->SetLimitedVariable(i,Form("Scale_%i",i),1.0,0.01,ThisLimits.first,ThisLimits.second);
    }
    mini->Minimize();

    double Prob = 0;
    if (!(mini->Status())) {
      Prob = (1. - mini->MinValue());
    }
    else {
      Prob = -1;
    }
    return Prob;
  }

  void MakePermutations(unsigned nj = 0) {
    Perms.clear();
    if (nj == 0) nj = AllJets.size();
    for (unsigned i0 = 0; i0 < nj - 1; ++i0) {
      for (unsigned i1 = i0 + 1; i1 < nj; ++i1) {
        for (unsigned i2 = 0; i2 < nj; ++i2) {
          if (i2 == i1 || i2 == i0) continue;
          for (unsigned i3 = 0; i3 < nj; ++i3) {
            if (i3 == i2 || i3 == i1 || i3 == i0) continue;
            Perms.push_back({i0,i1,i2,i3});
          }
        }
      }
    }
  }

  double Optimize(vector<Jet>& BestJets) {
    MakePermutations();
    double BestP = -1;
    vector<int> BestPerm = {0,0,0,0};
    for (unsigned ip = 0; ip < Perms.size(); ++ip) {
      vector<Jet> jets;
      for (unsigned ij = 0; ij < Perms[ip].size(); ++ij) {
        jets.push_back(AllJets[Perms[ip][ij]]);
      }
      double PbTag = bTE->GetLikelihood(jets);
      double PPerm = MinimizeP(jets);
      double ThisP = PbTag * PPerm;
      if (ThisP > BestP) {
        BestP = ThisP;
        BestPerm = Perms[ip];
      }
    }
    BestJets.clear();
    if (BestP > 0) {
      for (unsigned ij = 0; ij < BestPerm.size(); ++ij) {
        BestJets.push_back(AllJets[BestPerm[ij]]);
      }
    }
    return BestP;
  }

  bTagEff* bTE;
  vector<Jet> AllJets;
  vector< vector<int> > Perms;

  //Minimizer components
  static ROOT::Math::Minimizer* mini;
  static ROOT::Math::Functor func;

  static JetScale* JS;
  static vector<TLorentzVector> Jets;
  static TLorentzVector Lep;
  static TLorentzVector MET;
}

JetScale* Fitter::JS;
ROOT::Math::Minimizer* Fitter::mini = ROOT::Math::Factory::CreateMinimizer("TMinuit");
ROOT::Math::Functor Fitter::func;
vector<TLorentzVector> Fitter::Jets;
vector<TLorentzVector> Fitter::Lep;
vector<TLorentzVector> Fitter::MET;

#endif