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

  int genJetIdx;
  int hadronFlavour;
  int partonFlavour;
  vector<bool> bTagPasses = {false, false, false}; // {loose, medium, tight}
  vector<vector<float> > bJetSFweights = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}
  vector<float> bTagEffs = {0.9, 0.7, 0.5};
};

struct Lepton : VarPO {
  Lepton(TLorentzVector v_ = TLorentzVector()) : VarPO(v_) {};
  TLorentzVector& v(int ir = 0) { return GetV(ir);} // Ele variations index is 1 to 4
  int charge;
  bool IsPrimary;
  bool IsLoose;
  bool IsVeto;
  vector<bool> OverlapsJet = {false, false, false}; //{PUID: loose, medium, tight}
  // float jetRelIso;
  // int pdgId;
  // int jetIdx;
  // int genPartIdx;
  //int type; // 0 for electron, 1 for muon
};

struct Electron : Lepton {
  Electron(TLorentzVector v_ = TLorentzVector()) : Lepton(v_) {};
  vector<float> SFs = {1.,1.,1.}; // {nominal, up, down}
  //int cutBased;
  //bool cutBasedHEEP;
};

struct Muon: Lepton {
  Muon(TLorentzVector v_ = TLorentzVector()) :Lepton(v_) {};
  vector<float> triggerSFs = {1.,1.,1.};
  vector<float> idSFs = {1.,1.,1.};
  vector<float> isoSFs = {1.,1.,1.};
  //int tightId;
  //int looseId;
  //double relIso;
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
