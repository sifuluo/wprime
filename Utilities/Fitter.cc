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
#include "Hypothesis.cc"
#include "Permutations.cc"

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

  void SetPermutation(Permutations *p_) {
    PermEval = p_;
  }

  void SetJets(vector<Jet>& ajs, int iregion = 0) {
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

  // Functor to determine likelihood of a set of scales
  // return 1.0 - likelihood. Such that the return value will be positive
  // And the minimized return value correspond to the highest likelihood
  static double MinimizePFunc(const double *scales) {
    ScaledHypo = BaseHypo; // Init the hypothesis in current scale set with unscaled jets, leptons and met
    ScaledHypo.ScaleJets(scales); // Scaled the hypothesis
    vector<TLorentzVector> Neus;
    double PNeu = JS->SolveNeutrinos(ScaledHypo.Lep, ScaledHypo.MET, Neus); // Neus is passed as Reference, so it will be altered.
    // If a Neutrino solution is not possible, the negative radical is set to PNeu.
    // So the return value will be always > 1.0
    // Otherwise if at least one neutrino solution is reached, PNeu will always be = 1
    if (PNeu < 0) return (-1.0 * PNeu + 1.);
    // Comparing likelihood of recontructing a top with either neutrino solutions
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
    // GetPFitter = PScale * PLep * PHadW * PHadT;
    double p = ScaledHypo.GetPFitter();
    if (p < 0 || p > 1.) cout << Form("PScale = %f, PHadW = %f, PHadT = %f, PLep = %f", ScaledHypo.PScale, ScaledHypo.PHadW, ScaledHypo.PHadT, ScaledHypo.PLep) << endl;

    FitRecords.push_back(ScaledHypo.MakeRecord());  // Debug tool
    // return 1-p. So minimizer will try to get higher p, while the return value is expected to be > 0
    return (-1.0 * p + 1.);
  }

  // Likelihood of scales based on the jet responses.
  static double ScaleLikelihood(const double *scales) {
    double p = 1.0;
    for (unsigned i = 0; i < 4; ++i) {
      p = p * JS->JetScaleLikelihood(BaseHypo.Jets[i].Eta(), BaseHypo.Jets[i].Pt(), scales[i]);
    }
    return p;
  }

  // Minimize the return value of the functor MinimizePFunc()
  // Then return likelihood as 1. - minimized functor; or 1. - (1. - likelihood)
  double MinimizeP() {
    // Initializing the minimizer with our funtor MinimizePFunc() defined above
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
      // The variating range is limited to +- 2 sigma of the fitted jet response distribution
      pair<double,double> limits = JS->ScaleLimits(BaseHypo.Jets[i].Eta(),BaseHypo.Jets[i].Pt());
      mini->SetLimitedVariable(i,Form("Scale_%i",i),1.0,0.01,limits.first,limits.second);
      // mini->SetLimitedVariable(i,Form("Scale_%i",i),1.0,0.01,0.99,1.01);
    }
    mini->Minimize();

    double Prob = 0;
    if (!(mini->Status())) {
      // Revert the 1-p to p itself.
      Prob = (1. - mini->MinValue());
    }
    else {
      Prob = -1;
    }
    return Prob;
  }

  // Makes permutations using the number of jets and target size of each permutation
  void MakePermutations(unsigned nj = 0, unsigned target = 5) {
    Perms.clear();
    if (nj == 0) nj = AllJets.size();
    vector<unsigned> ThisPerm = vector<unsigned>(target);
    AppendPerm(ThisPerm, nj);
  }

  void AppendPerm(vector<unsigned> thisperm, unsigned nj, unsigned pos = 0) {
    for (unsigned i = 0; i < nj; ++i) {
      bool taken = false;
      for (unsigned j = 0; j < pos; ++j) {
        if (i == thisperm[j]) taken = true;
      }
      if (taken) continue;
      if (pos == 1 && i < thisperm[0]) continue;
      thisperm[pos] = i;
      if (pos == thisperm.size() - 1) Perms.push_back(thisperm); // Last digit filled. A complete permutation can be saved
      else AppendPerm(thisperm, nj, pos + 1); // Moving on to next digit position of a permutation
    }
  }

  // Check if the reconstructed W and tops can have a likelihood at least = MinPMass. To avoid redundant fitting on insane permutations.
  bool PermutationPreFitCheck() {
    double MinPMass = 0.01;
    double lowlimit[4];
    double uplimit[4];
    for (unsigned i = 0; i < 4; ++i) {
      pair<double,double> limits = JS->ScaleLimits(BaseHypo.Jets[i].Eta(),BaseHypo.Jets[i].Pt());
      lowlimit[i] = limits.first;
      uplimit[i] = limits.second;
    }
    Hypothesis LowScaledHypo = BaseHypo;
    LowScaledHypo.ScaleJets(lowlimit);
    Hypothesis HighScaledHypo = BaseHypo;
    HighScaledHypo.ScaleJets(uplimit);
    if (max(max(HighScaledHypo.HadW().M(), LowScaledHypo.HadW().M()), BaseHypo.HadW().M()) < JS->GetWMassLowLimits(MinPMass)) return false;
    if (min(min(HighScaledHypo.HadW().M(), LowScaledHypo.HadW().M()), BaseHypo.HadW().M()) > JS->GetWMassUpLimits(MinPMass)) return false;
    if (max(max(HighScaledHypo.HadT().M(), LowScaledHypo.HadT().M()), BaseHypo.HadT().M()) < JS->GetTopMassLowLimits(MinPMass)) return false;
    if (min(min(HighScaledHypo.HadT().M(), LowScaledHypo.HadT().M()), BaseHypo.HadT().M()) > JS->GetTopMassUpLimits(MinPMass)) return false;
    if (max(max(HighScaledHypo.LepT().M(), LowScaledHypo.LepT().M()), BaseHypo.LepT().M()) < JS->GetTopMassLowLimits(MinPMass)) return false;
    if (min(min(HighScaledHypo.LepT().M(), LowScaledHypo.LepT().M()), BaseHypo.LepT().M()) > JS->GetTopMassUpLimits(MinPMass)) return false;
    return true;
  }
  
  // Optimize event over all permutations
  double Optimize() {
    if (AllJets.size() < 5) return -2;
    MakePermutations();
    double BestP = -1;
    BestPerm = {0,0,0,0};
    // Debug block
    double BestPFitter = 0;
    vector<FitRecord> BestFitRecords;
    // End of Debug Block
    for (unsigned ip = 0; ip < Perms.size(); ++ip) { // Loop over permutations
      FitRecords.clear();
      BaseHypo.ResetJets();
      BaseHypo.SetJetsFromPerm(AllJets, Perms[ip]);
      if (!PermutationPreFitCheck()) continue;
      double PFitter = MinimizeP();
      if (PFitter < 0) continue;

      // Obtaining the scales set from minimizer which should yield the minimized likelihodd
      double ThisScale[4];
      for (unsigned isc = 0; isc < 4; ++isc) ThisScale[isc] = mini->X()[isc];
      MinimizePFunc(ThisScale); // Using the scales set trying to reproduce the ScaledHypos
      double massh = ScaledHypo.WPH().M();
      double massl = ScaledHypo.WPL().M();
      // Evaluating permutation likelihood based on their pT order and bTagging status.
      double PbTag_h = PermEval->GetbTagPermLikelihood(AllbTags, Perms[ip], massh, 0);
      double PPtPerm_h = PermEval->GetPtPermLikelihood(AllJets, Perms[ip], massh, 0);
      double PbTag_l = PermEval->GetbTagPermLikelihood(AllbTags, Perms[ip], massl, 1);
      double PPtPerm_l = PermEval->GetPtPermLikelihood(AllJets, Perms[ip], massl, 1);
      if (PbTag_h * PPtPerm_h > PbTag_l * PPtPerm_l) {
        ScaledHypo.PbTag = PbTag_h;
        ScaledHypo.PPtPerm = PPtPerm_h;
        ScaledHypo.WPType = 0;
      }
      else {
        ScaledHypo.PbTag = PbTag_l;
        ScaledHypo.PPtPerm = PPtPerm_l;
        ScaledHypo.WPType = 1;
      }
      
      double ThisP = ScaledHypo.PbTag * ScaledHypo.PPtPerm * PFitter;
      if (ThisP > 0 && ThisP > BestP) {
        BestP = ThisP;
        BestPerm = Perms[ip];
        BestHypo = ScaledHypo;
        //Debug block
        BestPFitter = PFitter;
        BestFitRecords = FitRecords;
      }
    }
    // FIXME PFitter gave different min value from the value calculated by the same functor with the scale set given by the fitter.
    if (BestP > 0 && fabs((BestPFitter - BestHypo.GetPFitter()) / BestPFitter) > 0.01) {
      cout << "Last few trials of fitter: " << endl;
      for (unsigned i = BestFitRecords.size() - 5; i < BestFitRecords.size(); ++i) BestFitRecords[i].Print("    Trials: ");
      BestHypo.MakeRecord().Print("Reproduced: ");
      cout << "Minimizer gave PFitter = " << BestPFitter << ", Reproduced PFitter = " << BestHypo.GetPFitter() << endl;
    }
    return BestP;
  }

  Configs* conf;
  bool PUIDWP;
  bool bTagWP;

  Permutations* PermEval;
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
  static Hypothesis BaseHypo;
  static Hypothesis ScaledHypo;

  static vector<FitRecord> FitRecords;
};

JetScale* Fitter::JS;
ROOT::Math::Minimizer* Fitter::mini = ROOT::Math::Factory::CreateMinimizer("TMinuit");
ROOT::Math::Functor Fitter::func;

Hypothesis Fitter::BaseHypo;
Hypothesis Fitter::ScaledHypo;

vector<FitRecord> Fitter::FitRecords;

#endif