#ifndef DATAFORMAT_CC
#define DATAFORMAT_CC

#include <vector>
// #include "TChain.h"
#include "TLorentzVector.h"
#include "TString.h"

using namespace std;

struct PO : TLorentzVector {
  PO(TLorentzVector v_ = TLorentzVector()) : TLorentzVector(v_), index(-1) {};
  // operator TLorentzVector() {return v;};
  int index;
  TString printXYZT() {
    TString a = Form("X = %f, Y = %f, Z = %f, T = %f", X(), Y(), Z(), T());
    return a;
  }
  TString printXYZM() {
    TString a = Form("X = %f, Y = %f, Z = %f, M = %f", X(), Y(), Z(), M());
    return a;
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

struct Jet : PO {
  Jet(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
    // jetId==1 means: pass loose ID, fail tight, fail tightLepVeto
    // jetId==3 means: pass loose and tight ID, fail tightLepVeto
    // jetId==7 means: pass loose, tight, tightLepVeto ID.
  int jetId;
  int puId;
  int genJetIdx;
  int hadronFlavour;
  int partonFlavour;
  bool btag;
  // float btagDeepB;
  float btagDeepFlavB;
};

struct Lepton : PO {
  Lepton(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  int charge;
  // float jetRelIso;
  // int pdgId;
  // int jetIdx;
  // int genPartIdx;
  // int type; // 0 for electron, 1 for muon
};

struct Electron : Lepton {
  Electron(TLorentzVector v_ = TLorentzVector()) : Lepton(v_) {};
};

struct Muon: Lepton {
  Muon(TLorentzVector v_ = TLorentzVector()) :Lepton(v_) {};
  int tightId;
  double relIso;
};

struct MET : PO {
  MET(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
};

struct GenMET : PO {
  GenMET(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
};


#endif
