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
  int pid = -1;
  int m = -1;
  vector<int> d, dpid; // Daughters
  void Reset() {
    first = -1;
    last = -1;
    pid = -1;
    m = -1;
    d.clear();
    dpid.clear();
  }
  bool HasDaughterPID(int a) {
    for (unsigned i = 0; i < dpid.size(); ++i) {
      if (abs(dpid[i]) == a) return true;
    }
    return false;
  }
  void PrintDaughterPID() {
    for (unsigned i = 0; i < dpid.size(); ++i) {
      cout << dpid[i] << " ";
    }
  }
  bool InValid(string ss = "") {
    if (first == -1) cout << ss << " not found" << endl;
    return (first == -1);
  }
  void Print(TString loc = "xx") {
    cout << Form("GenPart %s, (%i to %i), pid : %i, mother %i, Daughters: ", loc.Data(), first, last, pid, m);
    for (unsigned i = 0; i < d.size(); ++i) cout << d[i] << " ,";
    PrintDaughterPID();
    cout << endl;
  }
};

class GenHypothesis{
public:
  GenHypothesis(int t_ = -1) {
    Type = t_;
  };

  void Reset() {
    WPs.clear();
    Ts.clear();
    Ws.clear();

    WP.Reset();
    WPt.Reset();
    OTt.Reset();
    HadT.Reset();
    HadW.Reset();
    LepT.Reset();
    LepW.Reset();
    OutPartDecay = vector<PartDecay>(7,PartDecay());
    OutParts.clear();
  }

  void SetGenParts(vector<GenPart>& gp_) {
    gp = &gp_;
    // If GenPartile.index and their index in vector is different, how the class holds genparticle collection has to be changed.
    for (unsigned i = 0; i < gp->size(); ++i) {
      if (gp->at(i).index != (int)i) cout << "GenParticle index Inconsistent with its location in stored vector, please check" << endl;
    }
  }

  int pid(int i) {
    // if (i < 0) return 0;
    return gp->at(i).pdgId;
  }
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
    pd.pid = pid(i);
    pd.m = mother(pd.first);
    pd.d = GetDaughters(pd.last);
    for (unsigned i = 0; i < pd.d.size(); ++i) {
      pd.dpid.push_back(pid(pd.d[i]));
    }
    return pd;
  }
  
  bool IsDuplicated(int idx, vector<PartDecay> vpd) {
    for (unsigned i = 0; i < vpd.size(); ++i) {
      if (vpd[i].first == GetFirst(idx)) return true;
    }
    return false;
  }

  void FindGenHypothesis() {
    Reset();
    for (unsigned i = 0; i < gp->size(); ++i) {
      if (abspid(i) == 34 && !IsDuplicated(i,WPs)) {
        PartDecay p_ = CompletePart(i);
        if (p_.HasDaughterPID(5) && p_.HasDaughterPID(6)){
          WPs.push_back(p_);
        }
        else {
          cout << "Found WP with dpid = ";
          p_.PrintDaughterPID();
          cout << endl;
        }
      }
      else if (abspid(i) == 6 && !IsDuplicated(i, Ts)) {
        PartDecay p_ = CompletePart(i);
        if (p_.HasDaughterPID(5) && p_.HasDaughterPID(24)) {
          Ts.push_back(p_);
        }
        else {
          cout << "Found top with dpid = ";
          p_.PrintDaughterPID();
          cout << endl;
        }
      }
      else if (abspid(i) == 24 && !IsDuplicated(i, Ws)) {
        Ws.push_back(CompletePart(i));
      }
    }
    if (WPs.size() > 1) {
      return;
      cout << "More than 1 WP found" <<endl;
      for (unsigned iwp = 0; iwp < WPs.size(); ++iwp) {
        WPs[iwp].Print(Form("WP%i", iwp));
      }
    }
    if (Ts.size() > 2) {
      return;
      cout << "More than 2 t found" <<endl;
    }

    for (unsigned i = 0; i < gp->size(); ++i) { // W'
      if (abspid(i) == 34) {
        if (WP.first != -1 && WP.first != GetFirst(i)) {
          cout << "Multiple WP found" << endl;
        }
        WP = CompletePart(i);
      }
    }
    if (WP.InValid("WP")) return;
    else if (Debug) WP.Print("WP");

    for (unsigned i = 0; i < WP.d.size(); ++i) {
      if (abspid(WP.d[i]) == 5) OutPartDecay[4] = CompletePart(WP.d[i]); // WPb
      else if (abspid(WP.d[i]) == 6) WPt = CompletePart(WP.d[i]); // WPt
    }
    
    for (unsigned i = 0; i < gp->size(); ++i) { // Other top share the same mother with the wprime
      if (mother(i) == mother(WP.first) && abspid(i) == 6) {
        if (OTt.first != -1 && OTt.first != GetFirst(i)) cout << "Multiple OTt found" << endl;
        OTt = CompletePart(i);
      }
    }
    if (OutPartDecay[4].InValid("WPb") || WPt.InValid("WPt") || OTt.InValid("OTt")) return;
    else if (Debug) {
      OutPartDecay[4].Print("WPb");
      WPt.Print("WPt");
      OTt.Print("OTt");
    }

    if (Type == 0) {
      HadT = WPt;
      LepT = OTt;
    }
    else if (Type == 1){
      HadT = OTt;
      LepT = WPt;
    }
    else cout << "GenHypothesis Type is set incorrectly to " << Type << endl;

    for (unsigned i = 0; i < HadT.d.size(); ++i) {
      if (abspid(HadT.d[i]) == 5) OutPartDecay[2] = CompletePart(HadT.d[i]); // Hadb
      else if (abspid(HadT.d[i] == 24)) HadW = CompletePart(HadT.d[i]); // HadW
    }
    if (OutPartDecay[2].InValid("Hadb") || HadW.InValid("HadW")) return;
    else if (Debug) {
      OutPartDecay[2].Print("Hadb");
      HadW.Print("HadW");
    }

    for (unsigned i = 0; i < HadW.d.size(); ++i) { // Light Jets ordered by Pt
      if (Pt(HadW.d[i]) > Pt(OutPartDecay[0].last)) {
        OutPartDecay[1] = OutPartDecay[0];
        OutPartDecay[0] = CompletePart(HadW.d[i]);
      }
      else if (Pt(HadW.d[i]) > Pt(OutPartDecay[1].last)) OutPartDecay[1] = CompletePart(HadW.d[i]);
    }
    if (OutPartDecay[0].InValid("LJ0") || OutPartDecay[1].InValid("LJ1")) return;
    else if (Debug) {
      OutPartDecay[0].Print("LJ0");
      OutPartDecay[1].Print("LJ1");
    }

    for (unsigned i = 0; i < LepT.d.size(); ++i) {
      if (abspid(LepT.d[i]) == 5) OutPartDecay[3] = CompletePart(LepT.d[i]); // Lepb
      else if (abspid(LepT.d[i] == 24)) LepW = CompletePart(LepT.d[i]); // LepW
    }
    if (OutPartDecay[3].InValid("Lepb") || LepW.InValid("LepW")) return;
    else if (Debug) {
      OutPartDecay[3].Print("Lepb");
      LepW.Print("LepW");
    }

    for (unsigned i = 0; i < LepW.d.size(); ++i) { // Lepton and Neutrino
      if (abspid(LepW.d[i]) == 11 || abspid(LepW.d[i]) == 13) OutPartDecay[5] = CompletePart(LepW.d[i]);
      if (abspid(LepW.d[i]) == 12 || abspid(LepW.d[i]) == 14) OutPartDecay[6] = CompletePart(LepW.d[i]);
    }
    if (OutPartDecay[5].InValid("Lep") || OutPartDecay[6].InValid("Neu")) {
      cout << "LepW Daughter pid = " << pid(LepW.d[0]) << ", " << pid(LepW.d[1]) << endl;
      return;
    }
    else if (Debug) {
      OutPartDecay[5].Print("Lep");
      OutPartDecay[6].Print("Neu");
    }

    for (unsigned i = 0; i < OutPartDecay.size(); ++i) {
      // GenPart gp_ = gp->at(OutPartDecay[i].last);
      // cout << Form("%i th part decay pid = %i, part pid = %i", i, pid(OutPartDecay[i].last), gp_.pdgId) <<endl;
      OutParts.push_back(gp->at(OutPartDecay[i].last));
    }
  }

  vector<Jet> MatchToJets(const vector<GenJet>& gjs, const vector<Jet>& js) {
    vector<int> genout = vector<int> (5,-1);
    vector<Jet> out = vector<Jet> (5,Jet());
    for (unsigned i = 0; i < 5; ++i) {
      float mindr = 0.4;
      for (unsigned j = 0; j < gjs.size(); ++j) {
        float dr = OutParts[i].DeltaR(gjs[j]);
        if (dr < mindr) {
          mindr = dr;
          genout[i] = j;
        }
      }
      if (genout[i] == -1) {
        out.clear();
        return out;
      }
      if (abs(OutParts[i].pdgId) != abs(gjs[genout[i]].partonFlavour) && Debug) {
        cout <<Form("GenPart %i (%i), Matched to GenJet of Flavour %i, dR = %f, pT ratio = %f", i, OutParts[i].pdgId, gjs[genout[i]].partonFlavour, OutParts[i].DeltaR(gjs[genout[i]]), OutParts[i].Pt() / gjs[genout[i]].Pt()) <<endl;
      }
    }

    for (unsigned i = 0; i < 5; ++i) {
      for (unsigned j = 0; j < js.size(); ++j) {
        if (js[j].genJetIdx == genout[i]) out[i] = js[j];
      }
    }
    return out;
  }

  vector<GenPart> GetOutParts() {return OutParts;}

  bool Debug = false;
  const vector<GenPart>* gp;
  int Type; // 0: FL, 1: LL
  vector<PartDecay> WPs, Ts, Ws;
  vector<PartDecay> HadTDecay, LepTDecay;
  PartDecay WP, HadT, HadW, LepT, LepW, WPt, OTt;
  vector<PartDecay> OutPartDecay; // LJ1, LJ2, Hadb, Lepb, WPb, Lep, Neu;
  vector<GenPart> OutParts;

};

#endif