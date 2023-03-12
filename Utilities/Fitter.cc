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
#include "Hypothesis.cc"

using namespace std;

class Fitter{
public:
  Fitter(Configs *conf_) {
    conf = conf_;
    PUIDWP = conf->PUIDWP;
    bTagWP = conf->bTagWP;
  };

  void SetJetScale(JetScale* JS_) {
    JS = JS_;
  }
  void SetbTag(bTagEff* bt_) {
    bTE = bt_;
  }

  void SetJets(vector<Jet> ajs, int iregion = 0) {
    AllJets.clear();
    AllbTags.clear();
    for (unsigned i = 0; i < ajs.size(); ++i) {
      // Stay consistent with "check jet multiplicity" part at RegionReader() in NanoAODReader.cc
      if (ajs[i].v(iregion).Pt() < 30.) continue;
      if (!ajs[i].PUIDpasses[PUIDWP]) continue;
      AllJets.push_back(ajs[i].v(iregion));
      AllbTags.push_back(ajs[i].bTagPasses[bTagWP]);
    }
  }

  void SetLep(Lepton lep_, int iregion = 0) {
    BaseHypo.Lep = lep_.v(iregion);
  }
  
  void SetMET(MET met_, int iregion = 0) {
    BaseHypo.MET = met_.v(iregion);
  }

  static double MinimizePFunc(const double *scales) {
    ScaledHypo = BaseHypo;
    ScaledHypo.ScaleJets(scales);
    vector<TLorentzVector> Neus;
    double PNeu = JS->SolveNeutrinos(ScaledHypo.Lep, ScaledHypo.MET, Neus);
    if (PNeu < 0) return (-1.0 * PNeu + 1.);
    ScaledHypo.PLep = JS->EvalTop(ScaledHypo.Lep + Neus[0] + ScaledHypo.Jets[3]);
    ScaledHypo.Neu = Neus[0];
    double PLep1 = JS->EvalTop(ScaledHypo.Lep + Neus[1] + ScaledHypo.Jets[3]);
    if (ScaledHypo.PLep < PLep1) {
      ScaledHypo.PLep = PLep1;
      ScaledHypo.Neu = Neus[1];
    }
    ScaledHypo.PHadW = JS->EvalW(ScaledHypo.HadW());
    ScaledHypo.PHadT = JS->EvalTop(ScaledHypo.HadT());

    ScaledHypo.PScale = ScaleLikelihood(scales);

    double p = ScaledHypo.GetPFitter();
    if (p < 0 || p > 1.) cout << Form("PScale = %f, PHadW = %f, PHadT = %f, PLep = %f", ScaledHypo.PScale, ScaledHypo.PHadW, ScaledHypo.PHadT, ScaledHypo.PLep) << endl;
    return (-1.0 * p + 1.);
  }

  // static vector<TLorentzVector> ScaleJets(const double *scales) {
  //   vector<TLorentzVector> sjs;
  //   sjs.clear();
  //   ScaledMET = MET;
  //   for (unsigned i = 0; i < 4; ++i) {
  //     TLorentzVector sj = Jets[i] * scales[i];
  //     ScaledMET = ScaledMET + Jets[i] - sj;
  //     sjs.push_back(sj);
  //   }
  //   return sjs;
  // }

  static double ScaleLikelihood(const double *scales) {
    double p = 1.0;
    for (unsigned i = 0; i < 4; ++i) {
      p = p * JS->JetScaleLikelihood(BaseHypo.Jets[i].Eta(), BaseHypo.Jets[i].Pt(), scales[i]);
    }
    return p;
  }

  double MinimizeP() {
    func = ROOT::Math::Functor(&MinimizePFunc,4);
    mini->SetPrintLevel(0);
    // mini->SetPrintLevel(3);
    mini->SetStrategy(3);
    mini->SetMaxFunctionCalls(100000);
    mini->SetMaxIterations(10000);
    mini->SetTolerance(0.01);
    mini->SetErrorDef(0.5);
    mini->SetFunction(func);
    ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(100000);

    for (unsigned i = 0; i < 4; ++i) {
      pair<double,double> limits = JS->ScaleLimits(BaseHypo.Jets[i].Eta(),BaseHypo.Jets[i].Pt());
      mini->SetLimitedVariable(i,Form("Scale_%i",i),1.0,0.01,limits.first,limits.second);
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

  double Optimize() {
    if (AllJets.size() < 5) cout << "This event has only " << AllJets.size() << " jets, skipped" <<endl;
    if (AllJets.size() < 5) return -2;
    MakePermutations();
    double BestP = -1;
    BestPerm = {0,0,0,0};
    for (unsigned ip = 0; ip < Perms.size(); ++ip) {
      BaseHypo.ResetJets();
      BaseHypo.SetJetsFromPerm(AllJets, Perms[ip]);
      double PbTag = bTE->GetLikelihood(AllJets, AllbTags, Perms[ip]);
      double PPerm = MinimizeP();
      double ThisP = PbTag * PPerm;
      if (ThisP > BestP) {
        BestP = ThisP;
        BestPerm = Perms[ip];
        for (unsigned isc = 0; isc < 4; ++isc) {
          BestScales[isc] = mini->X()[isc];
        }
      }
    }
    if (BestP > 0) {
      BaseHypo.SetJetsFromPerm(AllJets, BestPerm);
      MinimizePFunc(BestScales);
      BestHypo = ScaledHypo;
      BestHypo.PbTag = bTE->GetLikelihood(AllJets, AllbTags, BestPerm);
      // if (BestP > 1.0) {
      //   cout << "Event BestP = " << BestP << ", Recalculated P = " << BestHypo.PbTag * BestHypo.GetPFitter() <<endl;
      //   cout << Form("PScale = %f, PLep = %f, PHadW = %f, PHadT = %f, PbTag = %f", BestHypo.PScale, BestHypo.PLep, BestHypo.PHadW, BestHypo.PHadT, BestHypo.PbTag) <<endl;
      // }
    }
    
    return BestP;
  }

  TLorentzVector BestWPrime() {
    if (BestP < 0) return TLorentzVector();
    TLorentzVector hadt = BestHypo.HadT();
    TLorentzVector lept = BestHypo.LepT();
    vector<TLorentzVector> wpbcands;
    for (unsigned i = 0; i < AllJets.size(); ++i) {
      bool used = (i == BestPerm[0] || i == BestPerm[1] || i == BestPerm[2] || i == BestPerm[3]);
      if (!used) {
        wpbcands.push_back(AllJets[i]);
      }
    }
    if (wpbcands.size() == 0) cout << "No W'b Candidate" <<endl;
    double dphiwpbt = 0;
    for (unsigned ib = 0; ib < wpbcands.size(); ++ib) {
      float dphilep = fabs(wpbcands[ib].DeltaPhi(lept));
      float dphihad = fabs(wpbcands[ib].DeltaPhi(hadt));
      if (dphilep > dphihad && dphilep > dphiwpbt) {
        BestHypo.SetWPb(wpbcands[ib]);
        BestHypo.Type = 1;
        dphiwpbt = dphilep;
      }
      else if (dphihad > dphilep && dphihad > dphiwpbt) {
        BestHypo.SetWPb(wpbcands[ib]);
        BestHypo.Type = 0;
        dphiwpbt = dphihad;
      }
    }
    return BestHypo.WP();
  }

  Configs* conf;
  bool PUIDWP;
  bool bTagWP;

  bTagEff* bTE;
  vector<TLorentzVector> AllJets;
  vector<bool> AllbTags;
  vector< vector<unsigned> > Perms;

  double BestP;
  double BestScales[4];
  vector<unsigned> BestPerm;
  Hypothesis BestHypo;


  //Minimizer components
  static ROOT::Math::Minimizer* mini;
  static ROOT::Math::Functor func;

  static JetScale* JS;
  // static vector<TLorentzVector> Jets;
  // static TLorentzVector Lep;
  // static TLorentzVector MET;
  static Hypothesis BaseHypo;

  // static vector<TLorentzVector> ScaledJets;
  // static TLorentzVector ScaledMET;
  // static TLorentzVector Neutrino;
  static Hypothesis ScaledHypo;
};

JetScale* Fitter::JS;
ROOT::Math::Minimizer* Fitter::mini = ROOT::Math::Factory::CreateMinimizer("TMinuit");
ROOT::Math::Functor Fitter::func;
// vector<TLorentzVector> Fitter::Jets;
// TLorentzVector Fitter::Lep;
// TLorentzVector Fitter::MET;
Hypothesis Fitter::BaseHypo;
Hypothesis Fitter::ScaledHypo;

// vector<TLorentzVector> Fitter::ScaledJets;
// TLorentzVector Fitter::ScaledMET;
// TLorentzVector Fitter::Neutrino;

#endif