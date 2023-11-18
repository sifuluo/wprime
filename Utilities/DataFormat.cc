#ifndef DATAFORMAT_CC
#define DATAFORMAT_CC
// As title suggests, all data format for physics objects are defined here

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <chrono>
// #include "TChain.h"
#include "TLorentzVector.h"
#include "TString.h"
// #include "Configs.cc"

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
  TString PrintPtEtaPhiM() const{
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
    double mp = Pt();
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
  vector<vector<float> > bJetSFweightsCorr = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}
  vector<vector<float> > bJetSFweightsUncorr = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}}; // {nominal, up, down} x {loose, medium, tight}
  vector<float> bTagEffs = {0.9, 0.7, 0.5};
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
  vector<bool> JetProximity = {false, false, false}; //{PUID: loose, medium, tight}
};

struct Electron : Lepton {
  Electron(TLorentzVector v_ = TLorentzVector()) : Lepton(v_) {};
  vector<float> SFs = {1.,1.,1.}; // {nominal, up, down}
  int cutBased;
  bool cutBasedHEEP;
  bool mva;
};

struct Muon: Lepton {
  Muon(TLorentzVector v_ = TLorentzVector()) :Lepton(v_) {};
  vector<float> triggerSFs = {1.,1.,1.};
  vector<float> idSFs = {1.,1.,1.};
  vector<float> isoSFs = {1.,1.,1.};
  bool tightId;
  bool looseId;
  float relIso;
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
    return 0;
  }
};

struct RegionID{
  unsigned RegionCount = 9;
  //identifiers are -1: not assigned, -2: Lepton Failed, -3: Trigger Failed, -4: nJet failed
  int Regions[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
  //Central; Electron scale up, down; Electron resolution up, down; Jet Energy Scale up, down; Jet Energy resolution up, down
  string RegionNames[9] = {"central", "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown"};
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

struct FitRecord{
  vector<double> Scales = {1,1,1,1}; // LJ0, LJ1, Hadb, Lepb
  vector<double> M = {0,0,0}; // HadW, HadT, LepT
  vector<double> P = {1,1,1,1,1}; // HadW, HadT, LepT, Scale, Total
  void Print(TString pre1, TString pre2 = "") {
    if (pre2 == "") pre2 = pre1;
    cout << pre1;
    cout << Form("Scales: %f, %f, %f, %f, PScale = %f, Final P = %f", Scales[0], Scales[1], Scales[2], Scales[3], P[3], P[4]) << endl;
    cout << pre2;
    cout << Form("Mass(Prob) -- HadW: %f(%f), HadT: %f(%f), LepT: %f(%f)", M[0], P[0], M[1], P[1], M[2], P[2]) << endl;
  }
};

struct FitterStatus{
  int Status = -1;
  double NCalls = 0;
  double NIterations = 0;
  double SecondsTaken = 0;
  double NCount = 0;
  void Reset() {
    Status = -1;
    NCalls = 0;
    NIterations = 0;
    SecondsTaken = 0;
    NCount = 0;
  }
  void Add(FitterStatus& o) {
    NCalls += o.NCalls;
    NIterations += o.NIterations;
    SecondsTaken += o.SecondsTaken;
    NCount += o.NCount;
  }
  void Print(string tt) {
    cout << tt << " Contains " << NCount << " Perms. NCalls = " << NCalls << ", SecondsTaken = " << SecondsTaken << ", Seconds per call = " << SecondsTaken / NCalls;
    if (NIterations != NCalls) cout << ", NIterations = " << NIterations;
    cout << endl;
  }
};

struct StopWatch{
  chrono::system_clock::time_point StartTime, CheckedTime, CurrentTime;
  chrono::duration<double, std::milli> d;
  void Start() {
    StartTime = std::chrono::system_clock::now();
    CheckedTime = StartTime;
  }
  double End() {
    CurrentTime = chrono::system_clock::now();
    d = CurrentTime - StartTime;
    double dd = d.count();
    CheckedTime = CurrentTime;
    return dd;
  }
  double Check() {
    CurrentTime = chrono::system_clock::now();
    d = CurrentTime - CheckedTime;
    double dd = d.count();
    CheckedTime = CurrentTime;
    return dd;
  }
};

namespace StandardNames {
  TString HistName(string sampletype, string observable, string regionrange, string variation) { // eg. ttbar_WPrimeMassFL_1152_PUIDWup
    TString histname = "=SampleType=_=Observable=_=RegionRange=_=Variation=";
    if (variation == "") histname = "=SampleType=_=Observable=_=RegionRange=";
    histname.ReplaceAll("=SampleType=", sampletype);
    histname.ReplaceAll("=Observable=", observable);
    histname.ReplaceAll("=RegionRange=", regionrange);
    if (variation != "") histname.ReplaceAll("=Variation=", variation);
    return histname;
  }
  TString HistFileName(string path, string prefix, string observable, string sampletype = "", int ifile = -1) {// eg. path/2018_Validation_LeptonPt.root, where 2018_Validation is the prefix
    TString hfn = path;
    if (hfn != "" && !(hfn.EndsWith("/"))) hfn += "/";
    // if (sampletype != "" || ifile != -1) hfn += "batch/";
    hfn += "=Prefix=_=Observable=";
    hfn.ReplaceAll("=Prefix=", prefix);
    hfn.ReplaceAll("=Observable=", observable);
    if (sampletype != "") hfn += "_" + sampletype;
    if (ifile != -1) hfn += Form("_%i",ifile);
    hfn += ".root";
    return hfn;
  }
}

#endif
