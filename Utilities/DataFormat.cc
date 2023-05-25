#ifndef DATAFORMAT_CC
#define DATAFORMAT_CC

#include <vector>
#include <map>
#include <string>
#include <iostream>
// #include "TChain.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "Configs.cc"

using namespace std;

struct PO : TLorentzVector {
  PO(TLorentzVector v_ = TLorentzVector()) : TLorentzVector(v_), index(-1) {};
  int index;
  TString PrintXYZT() {
    TString a = Form("X = %f, Y = %f, Z = %f, T = %f", X(), Y(), Z(), T());
    return a;
  }
  TString PrintXYZM() {
    TString a = Form("X = %f, Y = %f, Z = %f, M = %f", X(), Y(), Z(), M());
    return a;
  }
  TString PrintPtEtaPhiM() {
    TString a = Form("Pt = %f, Eta = %f, Phi = %f, M = %f", Pt(), Eta(), Phi(), M());
    return a;
  }
};

struct VarPO : PO {
  VarPO(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  TLorentzVector SU, SD, RU, RD;
  TLorentzVector& ScaleUp(){return SU;}
  TLorentzVector& ScaleDown(){return SD;}
  TLorentzVector& ResUp(){return RU;}
  TLorentzVector& ResDown(){return RD;}
  TLorentzVector& JESup(){return SU;}
  TLorentzVector& JESdown(){return SD;}
  TLorentzVector& JERup(){return RU;}
  TLorentzVector& JERdown(){return RD;}

  TLorentzVector& GetV(int iv = 0) {
    if (iv == 1) return SU;
    else if (iv == 2) return SD;
    else if (iv == 3) return RU;
    else if (iv == 4) return RD;
    else return *this;
  }

  float MaxPt() {
    float mp = Pt();
    mp = max(mp,SU.Pt());
    mp = max(mp,SD.Pt());
    mp = max(mp,RU.Pt());
    mp = max(mp,RD.Pt());
    return mp;
  }
  
};

struct GenPart : PO {
  GenPart(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  int genPartIdxMother;
  int pdgId;
  int status;
  // int statusFlags;
};

struct GenJet : PO {
  GenJet(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  int partonFlavour;
  int hadronFlavour;
};

struct Jet : VarPO {
  Jet(TLorentzVector v_ = TLorentzVector()) : VarPO(v_) {};
  TLorentzVector& v ( int ir = 0) { return GetV(ir - 4); } // Jet variation index is 5 to 8 in region id.
  vector<bool> PUIDpasses = {false, false, false}; // {loose, medium, tight}
  vector<vector<float> > PUIDSFweights = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}
  int JetId;
  int genJetIdx;
  int hadronFlavour;
  int partonFlavour;
  vector<bool> bTagPasses = {false, false, false}; // {loose, medium, tight}
  vector<vector<float> > bJetSFweights = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}
  vector<float> bTagEffs = {0.9, 0.7, 0.5};

  bool PassCommon() {
    bool pass = true;
    pass &= (MaxPt() >= 30.);
    pass &= (JetId >= 4);
    pass &= (Eta() < 5.0) //added to accommodate PU ID limits
    return pass;
  }
};

struct Trigger : PO {
  Trigger(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  int id;
  int filterBits;
};

struct Lepton : VarPO {
  Lepton(TLorentzVector v_ = TLorentzVector()) : VarPO(v_) {};
  TLorentzVector& v(int ir = 0) { return GetV(ir);} // Ele variations index is 1 to 4
  int charge;
  bool IsPrimary;
  bool IsLoose;
  bool IsVeto;
  bool TriggerMatched;
  vector<bool> OverlapsJet = {false, false, false}; //{PUID: loose, medium, tight}
};

struct Electron : Lepton {
  Electron(TLorentzVector v_ = TLorentzVector()) : Lepton(v_) {};
  vector<float> SFs = {1.,1.,1.}; // {nominal, up, down}
  int cutBased;
  bool cutBasedHEEP;

  bool TriggerMatch(bool trg, vector<Trigger>& tobs, bool Is2017 = false) {
    if (!trg) return false;
    for (unsigned i = 0; i < tobs.size(); ++i) {
      if (tobs[i].id != 11) continue;
      if (DeltaR(tobs[i]) > 0.4) continue;
      if (Is2017 && !(1024 & tobs[i].filterBits)) continue;  
      return true;
    }
    return false;
  }

  bool PassCommon() {
    bool pass = true;
    float absEta = fabs(Eta());
    pass &= (absEta < 2.4);
    pass &= (absEta < 1.44 || absEta > 1.57);
    pass &= (MaxPt() >= 10.);
    return pass;
  }

  bool PassPrimary(int iv = -1) {
    bool pass = true;
    pass &= TriggerMatched;
    if (iv < 0) pass &= (MaxPt() > 30.);
    else pass &= (GetV(iv).Pt() > 30.);
    pass &= cutBasedHEEP;
    return pass
  }

  bool PassVeto(int iv = -1) {
    bool pass = true;
    if (iv < 0) pass &= (MaxPt() > 10.);
    else pass &= (GetV(iv).Pt() > 10.);
    pass &= (cutBased >= 2);
    return pass;
  }

  bool PassLoose(int iv = -1) {
    bool pass = true;
    pass &= Triggermatched;
    if (iv < 0) pass &= (MaxPt() > 30.);
    else pass &= (GetV(iv).Pt() > 30.);
    pass &= (cutBased >= 1);
    return pass;
  }

  int GetCategory(int iv = -1) {
    if (!PassCommon(iv)) return 0;
    if (PassPrimary(iv)) return 1;
    if (PassVeto(iv)) return 2;
    if (PassLoose(iv)) return 3;
  }
};

struct Muon: Lepton {
  Muon(TLorentzVector v_ = TLorentzVector()) :Lepton(v_) {};
  vector<float> triggerSFs = {1.,1.,1.};
  vector<float> idSFs = {1.,1.,1.};
  vector<float> isoSFs = {1.,1.,1.};
  bool tightId;
  bool looseId;
  float relIso;

  bool TriggerMatch(bool trg, vector<Trigger>& tobs, bool Is2017 = false) {
    if (!trg) return false;
    for (unsigned i = 0; i < tobs.size(); ++i) {
      if (tobs[i].id != 13) continue;
      if (DeltaR(tobs[i]) > 0.4) continue;
      return true;
    }
    return false;
  }

  bool PassCommon() {
    bool pass = true;
    float absEta = fabs(Eta());
    pass &= (absEta < 2.4);
    pass &= (MaxPt() >= 10.);
    return pass;
  }

  bool PassPrimary(int iv = -1) {
    bool pass = true;
    pass &= TriggerMatched;
    pass &= (Pt() > 27.);
    pass &= (relIso < 0.15);
    pass &= tightId;
    return pass
  }

  bool PassVeto(int iv = -1) {
    bool pass = true;
    pass &= (Pt() > 10.);
    pass &= (relIso < 0.25);
    pass &= looseId;
    return pass;
  }

  bool PassLoose(int iv = -1) {
    bool pass = true;
    pass &= Triggermatched;
    pass &= (Pt() > 27.);
    pass &= (relIso < 1.5 && relIso > 0.15);
    pass &= looseId;
    return pass;
  }

  int GetCategory(int iv = -1) {
    if (!PassCommon()) return 0;
    if (PassPrimary()) return 1;
    if (PassVeto()) return 2;
    if (PassLoose()) return 3;
  }
};

struct MET : VarPO {
  MET(TLorentzVector v_ = TLorentzVector()) : VarPO(v_) {};
  TLorentzVector& v ( int ir = 0) { return GetV(ir - 4); } // Same as Jet
};

struct GenMET : VarPO {
  GenMET(TLorentzVector v_ = TLorentzVector()) : VarPO(v_) {};
  TLorentzVector& v ( int ir = 0) { return GetV(ir - 4); }
};

struct EventWeight{
  string source;
  // bool IsActive;
  vector<float> variations = {1.,1.,1.}; // {nominal, up, down}
  int NanCheck() {
    if (variations[0] == 0.) {
      cout << "Event Weight from source of " << source << " is 0. , which can lead to nan in the total weight." <<endl;
      return 1;
    }
    for (unsigned i = 0; i < 3; ++i) {
      if (variations[i] != variations[i]) {
        cout << "Event Weight from source of " << source << ", variation " << i << ", is nan." <<endl;
      return 2+i;
      }
    }
  }
};

struct RegionID{
  unsigned RegionCount = 9;
  //identifiers are -1: not assigned, -2: Lepton Failed, -3: Trigger Failed, -4: nJet failed
  int Regions[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
  //Central; Electron scale up, down; Electron resolution up, down; Jet Energy Scale up, down; Jet Energy resolution up, down
  string RegionNames[9] = {"central", "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown"};
  bool KeepEvent() {
    for (unsigned i = 0; i < RegionCount; ++i) {
      if (Regions[i] > 0) return true;
    }
    return false;
  }
  vector<int> V() { // output a vector type of RegionIDs
    vector<int> out;
    out.clear();
    for (unsigned i = 0; i < RegionCount; ++i) out.push_back(Regions[i]);
    return out;
  }

  int GetLepType(unsigned ir = 0) {return Regions[ir] / 1000;}
  int GetLepReq(unsigned ir = 0) {return Regions[ir] / 100 % 10;}
  int GetNJets(unsigned ir = 0) {return Regions[ir] / 10 % 10;}
  int GetbTags(unsigned ir = 0) {return Regions[ir] % 10;}

};
//region identifier key: 1xyz muon region, 2xyz electron region; x=1 primary, x=2 loose; y=jet multiplicity; z=b-tag multiplicity

#endif
