#ifndef HYPOTHESIS_CC
#define HYPOTHESIS_CC

#include "DataFormat.cc"

using namespace std;

class Hypothesis {
public:
  Hypothesis() {

  };

  void Reset() {
    PbTag = PScale = PLep = PHadW = PHadT = 1.0;
    Jets.clear();
  }

  void SetJetsFromPerm(vector<Jet> alljets, vector<unsigned> perm){
    if (Jets.size() < perm.size()) Jets.resize(perm.size());
    for (unsigned i = 0; i < perm.size(); ++i) {
      Jets[i] = (TLorentzVector) alljets[perm[i]];
    }
  }
  void SetWPb(TLorentzVector j) {
    if (Jets.size() == 4) Jets.push_back(j);
    else if (Jets.size() == 5) Jets[4] = j;
  }

  void ScaleJets(const double *scales) {
    for (unsigned i = 0; i < 4; ++i) {
      MET = MET + Jets[i] - Jets[i] * scales[i];
      Jets[i] = Jets[i] * scales[i];
    }
  }

  TLorentzVector LepT() {
    return Lep + Neu + Jets[3];
  }
  TLorentzVector HadW() {
    return Jets[0] + Jets[1];
  }
  TLorentzVector HadT() {
    return Jets[0] + Jets[1] + Jets[2];
  }
  TLorentzVector FLWP() {
    return HadT() + Jets[4];
  }
  TLorentzVector LLWP() {
    return LepT() + Jets[4];
  }
  TLorentzVector WP() {
    if (Type == 0) return FLWP();
    else if (Type == 1) return LLWP();
    return TLorentzVector();
  }

  double GetPFitter() {
    return PScale * PLep * PHadW * PHadT;
  }

  int Type; // 0 for FL, 1 for LL;
  double PbTag, PScale, PLep, PHadW, PHadT;
  vector<TLorentzVector> Jets;
  TLorentzVector Lep, MET, Neu;
};

#endif