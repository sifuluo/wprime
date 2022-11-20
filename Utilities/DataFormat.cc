#ifndef DATAFORMAT_CC
#define DATAFORMAT_CC

#include <vector>
#include <map>
#include <string>
// #include "TChain.h"
#include "TLorentzVector.h"
#include "TString.h"

#include "Constants.cc"
#include "Configs.cc"
using namespace std;

struct PO : TLorentzVector {
  PO(TLorentzVector v_ = TLorentzVector()) : TLorentzVector(v_), index(-1) {};
  // operator TLorentzVector() {return v;};
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
  vector<bool> PUIDpasses; // {loose, medium, tight}
  int genJetIdx;
  int hadronFlavour;
  int partonFlavour;
  vector<bool> bTagPasses; // {loose, medium, tight}
};

struct Lepton : PO {
  Lepton(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  int charge;
  bool IsPrimary;
  bool IsLoose;
  bool IsVeto;
  vector<bool> OverlapsJet; //{PUID: loose, medium, tight}
  // float jetRelIso;
  // int pdgId;
  // int jetIdx;
  // int genPartIdx;
  // int type; // 0 for electron, 1 for muon
};

struct Electron : Lepton {
  Electron(TLorentzVector v_ = TLorentzVector()) : Lepton(v_) {};
  //int cutBased;
  //bool cutBasedHEEP;
};

struct Muon: Lepton {
  Muon(TLorentzVector v_ = TLorentzVector()) :Lepton(v_) {};
  //int tightId;
  //int looseId;
  //double relIso;
};

struct MET : PO {
  MET(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
};

struct GenMET : PO {
  GenMET(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
};

struct EventWeight{
  string source;
  bool IsActive;
  float variations[3];
};

#endif
