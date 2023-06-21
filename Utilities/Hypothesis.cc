#ifndef HYPOTHESIS_CC
#define HYPOTHESIS_CC

#include "DataFormat.cc"

using namespace std;

class Hypothesis {
public:
  Hypothesis() {

  };

  void ResetJets() {
    PbTag = PScale = PLep = PHadW = PHadT = 1.0;
    Jets.clear();
  }

  void SetJetsFromPerm(vector<TLorentzVector> alljets, vector<unsigned> perm){
    if (Jets.size() < perm.size()) Jets.resize(perm.size());
    for (unsigned i = 0; i < perm.size(); ++i) {
      Jets[i] = alljets[perm[i]];
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

struct PartDecay{
  int first = -1;
  int last = -1;
  vector<int> d; // Daughters
  void Reset() {
    first = -1;
    last = -1;
    d.clear();
  }
  bool InValid(string ss = "") {
    if (first == -1) cout << ss << " not found" << endl;
    return (first == -1);
  }
};

class GenHypothesis{
public:
  GenHypothesis(int t_ = -1) {
    Type = t_;
  };

  void Reset() {
    WP.Reset();
    WPt.Reset();
    OTt.Reset();
    HadT.Reset();
    HadW.Reset();
    LepT.Reset();
    LepW.Reset();
    OutPartDecay = vector<PartDecay>(7,PartDecay());
    OutParts = vector<GenPart>(7, GenPart());
  }

  void SetGenParts(vector<GenPart>& gp_) {
    gp = &gp_;
    // If GenPartile.index and their index in vector is different, how the class holds genparticle collection has to be changed.
    for (unsigned i = 0; i < gp->size(); ++i) {
      if (gp->at(i).index != i) cout << "GenParticle index Inconsistent with its location in stored vector, please check" << endl;
    }
  }

  int pid(int i) {return gp->at(i).pdgId;}
  int abspid(int i) {return abs(pid(i));}
  int mother(int i) {return gp->at(i).genPartIdxMother;}
  int motherpid(int i) {return pid(mother(i));} // Very handy shortcuts

  double Pt(int i) {
    if (i < 0) return -1.;
    return gp->at(GetLast(i)).Pt();
  }

  int GetFirst(int i) {
    while(motherpid(i) == pid(i)) i = mother(i);
    return i;
  }
  int GetLast(int i) {
    for (unsigned j = 0; j < gp->size(); ++j) if (mother(j) == i && pid(j) == pid(i)) return GetLast(j);
    return i;
  }
  vector<int> GetDaughters(int i) {
    vector<int> d;
    for (unsigned j = 0; j < gp->size(); ++j) if (mother(j) == i) d.push_back(j);
    return d;
  }

  PartDecay CompletePart(int i) {
    PartDecay pd;
    pd.first = GetFirst(i);
    pd.last = GetLast(i);
    pd.d = GetDaughters(pd.last);
    return PartDecay;
  }

  void FindGenHypothesis() {
    Reset();
    for (unsigned i = 0; i < gp->size(); ++i) { // W'
      if (abspid(i) == 34) {
        if (WP.first != -1 && WP.first != GetFirst(i)) cout << "Multiple WP found" << endl;
        WP = CompletePart(i);
      }
    }
    if (WP.InValid("WP")) return;

    for (unsigned i = 0; i < WP.d.size(); ++i) {
      if (abspid(WP.d[i]) == 5) OutPartDecay[4] = CompletePart(WP.d[i]); // WPb
      else if (abspid(WP.d[i]) == 6) WPt = CompletePart(WP.d[i]); // WPt
    }
    
    for (unsigned i = 0; i < gp->size(); ++i) { // Other top share the same mother with the wprime
      if (mother(i) == WP.mother && abspid(i) == 6) {
        if (OTt.first != -1 && OTt.first != GetFirst(i)) cout << "Multiple OTt found" << endl;
        OTt = CompletePart(i);
      }
    }
    if (OutPartDecay[4].InValid("WPb") || WPt.InValid("WPt") || OTt.InValid("OTt")) return;

    if (Type == 0) {
      HadT = WPt;
      LepT = OTt;
    }
    else {
      HadT = OTt;
      LepT = WPt;
    }

    for (unsigned i = 0; i < HadT.d.size(); ++i) {
      if (abspid(HadT.d[i]) == 5) OutPartDecay[2] = CompletePart(HadT.d[i]); // Hadb
      else if (abspid(HadT.d[i] == 24)) HadW = CompletePart(HadT.d[i]); // HadW
    }
    if (OutPartDecay[2].InValid("Hadb") || HadW.InValid("HadW")) return;

    for (unsigned i = 0; i < HadW.d.size(); ++i) { // Light Jets ordered by Pt
      if (Pt(HadW.d[i]) > Pt(OutPartDecay[0])) {
        OutPartDecay[1] = OutPartDecay[0];
        OutPartDecay[0] = CompletePart(HadW.d[i]);
      }
      else if (Pt(HadW.d[i] > Pt(OutPartDecay[1]))) OutPartDecay[1] = CompletePart(HadW.d[i]);
    }
    if (OutPartDecay[0].Invalid("LJ0") || OutPartDecay[1].Invalid("LJ1")) return;

    for (unsigned i = 0; i < LepT.d.size(); ++i) {
      if (abspid(LepT.d[i]) == 5) OutPartDecay[3] = CompletePart(LepT.d[i]); // Lepb
      else if (abspid(LepT.d[i] == 24)) LepW = CompletePart(LepT.d[i]); // LepW
    }
    if (OutPartDecay[3].InValid("Lepb") || HadW.InValid("LepW")) return;

    for (unsigned i = 0; i < LepW.d.size(); ++i) { // Lepton and Neutrino
      if (abspid(LepW.d[i]) == 11 || abspid(LepW.d[i]) == 13) OutPartDecay[5] = CompletePart(LepW.d[i]);
      if (abspid(LepW.d[i]) == 12 || abspid(LepW.d[i]) == 14) OutPartDecay[6] = CompletePart(LepW.d[i]);
    }
    if (OutPartDecay[5].Invalid("Lep") || OutPartDecay[6].Invalid("Neu")) return;

    for (unsigned i = 0; i < OutPartDecay.size(); ++i) {
      OutParts.push_back(gp->at(OutPartDecay[i].last));
    }
  }

  vector<Jet> MatchToJets(const vector<GenJet>& gjs, const vector<Jet>& js) {
    vector<int> genout = vector<int> (5,-1);
    for (unsigned i = 0; i < 5; ++i) {
      for (unsigned j = 0; j < gjs.size(); ++j) {
        if (OutParts[i].DeltaR(gjs[j]) < 0.4) {
          if (abs(OutParts[i].pdgId) != abs(gjs[j].partonFlavour)) cout << "GenPart " << i << "Matched to a GenJet with wrong partonFlavour" <<endl;
          else genout[i] = j;
        }
      }
    }

    vector<Jet> out = vector<Jet> (5,Jet());
    for (unsigned i = 0; i < 5; ++i) {
      for (unsigned j = 0; j < js.size(); ++j) {
        if (js[j].genJetIdx == genout[i]) out[i] = js[j];
      }
    }
    return out;
  }

  vector<GenPart> GetOutParts() {return OutParts;}

  const vector<GenPart>* gp;
  int Type; // 0: FL, 1: LL
  PartDecay WP, HadT, HadW, LepT, LepW, WPt, OTt;
  vector<PartDecay> OutPartDecay; // LJ1, LJ2, Hadb, Lepb, WPb, Lep, Neu;
  vector<GenPart> OutParts;

};

#endif