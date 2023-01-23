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
  TLorentzVector JESup, JESdown, JERup, JERdown;
  vector<bool> PUIDpasses = vector<bool>{false, false, false}; // {loose, medium, tight}
  vector<vector<float> > PUIDSFweights = vector<vector<float> >{{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}

  int genJetIdx;
  int hadronFlavour;
  int partonFlavour;
  vector<bool> bTagPasses = vector<bool>{false, false, false}; // {loose, medium, tight}
  vector<vector<float> > bJetSFweights = vector<vector<float> >{{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}
};

struct Lepton : PO {
  Lepton(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  TLorentzVector ScaleUp, ScaleDown, ResUp, ResDown;
  int charge;
  bool IsPrimary;
  bool IsLoose;
  bool IsVeto;
  vector<bool> OverlapsJet = vector<bool>{false, false, false}; //{PUID: loose, medium, tight}
  vector<float> SFs = vector<float> {1.,1.,1.};
  // float jetRelIso;
  // int pdgId;
  // int jetIdx;
  // int genPartIdx;
  //int type; // 0 for electron, 1 for muon
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
  TLorentzVector JESup, JESdown, JERup, JERdown;
};

struct GenMET : PO {
  GenMET(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  TLorentzVector JESup, JESdown, JERup, JERdown;
};

struct EventWeight{
  string source;
  // bool IsActive;
  vector<float> variations = vector<float>{1.,1.,1.}; // nominal, down, up
};

struct RegionID{
  unsigned RegionCount = 9;
  //identifiers are -1: no region; otherwise number of jets first digit number of btags 2nd digit
  int Regions[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
  //Central; Electron scale up, down; Electron resolution up, down; Jet Energy Scale up, down; Jet Energy resolution up, down
  string RegionNames[9] = {"central", "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown"};
};
//region identifier key: 1xyz muon region, 2xyz electron region; x=1 primary, x=2 loose; y=jet multiplicity; z=b-tag multiplicity

#endif
