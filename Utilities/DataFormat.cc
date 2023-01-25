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
  vector<bool> PUIDpasses = {false, false, false}; // {loose, medium, tight}
  vector<vector<float> > PUIDSFweights = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}


  int genJetIdx;
  int hadronFlavour;
  int partonFlavour;
  vector<bool> bTagPasses = {false, false, false}; // {loose, medium, tight}
  vector<vector<float> > bJetSFweights = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}
};

struct Lepton : PO {
  Lepton(TLorentzVector v_ = TLorentzVector()) : PO(v_) {};
  TLorentzVector ScaleUp, ScaleDown, ResUp, ResDown;
  int charge;
  bool IsPrimary;
  bool IsLoose;
  bool IsVeto;
  vector<bool> OverlapsJet = {false, false, false}; //{PUID: loose, medium, tight}
  vector<float> SFs = {1.,1.,1.}; // {nominal, up, down}
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
  vector<float> variations = {1.,1.,1.}; // {nominal, up, down}
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
};
//region identifier key: 1xyz muon region, 2xyz electron region; x=1 primary, x=2 loose; y=jet multiplicity; z=b-tag multiplicity

class RegionIDSelection {
public:
  RegionIDSelection(int b1_, int b2_) {
    b1 = b1_;
    b2 = b2_;
  };
  int b1 = 0;
  int b2 = 0;

  bool InRange(int x_, int digit) {
    x_ = x_ / digit % 10;
    int b1_ = b1 / digit % 10;
    int b2_ = b2 / digit % 10;
    return (x_ - b1_) * (x_ - b2_) <= 0;
  }
  bool PassTrigger(int id) {
    return InRange(id, 1000);
  }
  bool PassLepton(int id) {
    return InRange(id, 100);
  }
  bool PassnJet(int id) {
    return InRange(id, 10);
  }
  bool PassbTag(int id) {
    return InRange(id, 1);
  }
  bool Pass(int id) {
    bool p = PassTrigger(id) && PassLepton(id) && PassnJet(id) && PassbTag(id);
    return p;
  }
  vector<bool> Pass(RegionID id) {
    vector<bool> p;
    for (unsigned i = 0; i < id.RegionCount; ++i) p.push_back(Pass(id.Regions[i]));
    return p;
  }
  bool PassAny(RegionID id) {
    bool p = false;
    for (unsigned i = 0; i < id.RegionCount; ++i) p = p || Pass(id.Regions[i]);
    return p;
  }
};

#endif
