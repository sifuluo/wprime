#ifndef SCALEFACTOR_CC
#define SCALEFACTOR_CC

#include "DataFormat.cc"
#include "NanoAODReader.cc"

#include "correction.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class ScaleFactor {
public:
  ScaleFactor(NanoAODReader* r_) { // Needs to be 16preVFP 16postVFP 17 or 18
    r = r_;
    Init();
  };

  void Init() {
    string sy = r->conf->SampleYear;
    if (sy == "16apv" || sy == "2016apv") sampleyear = "2016preVFP";
    else if (sy == "16" || sy == "2016") sampleyear = "2016postVFP";
    else if (sy == "17" || sy == "2017") sampleyear = "2017";
    else if (sy == "18" || sy == "2018") sampleyear = "2018";
    else cout << "Wrong sample year: " << sy << ", Needs to be 16apv, 16, 17, 18" <<endl;

    string basepath = "/cvmfs/cms.cern.ch/rsync/cms-nanoAOD/jsonpog-integration/POG";
    string elecpath = basepath + "/EGM/" + sampleyear + "_UL/electron.json.gz";
    string muonpath = basepath + "/MUO/" + sampleyear + "_UL/muon_Z.json.gz";
    string jercpath = basepath + "/JME/" + sampleyear + "_UL/jet_jerc.json.gz";
    string bjetpath = basepath + "/BTV/" + sampleyear + "_UL/btagging.json.gz";

    // std::unique_ptr<CorrectionSet> is the auto below
    auto eleccs = correction::CorrectionSet::from_file(elecpath);
    Electron_SF = eleccs->at("UL-Electron-ID-SF");

    auto muoncs = correction::CorrectionSet::from_file(muonpath);
    Muon_Med_SF = muoncs->at("NUM_TrackerMuons_DEN_genTracks");

    auto bjetcs = correction::CorrectionSet::from_file(bjetpath);
    BJet_SF = bjetcs->at("deepJet_comb");
  }

  double GetElectronSF(Electron& e) {
    return Electron_SF->evaluate({sampleyear, "sf", "wp90iso", e.Eta(), e.Pt()});
  }

  double GetMuonSF(Muon& m) {
    return Muon_Med_SF->evaluate({sampleyear + "_UL", fabs(m.Eta()), m.Pt(), "sf"});
  }

  double GetBJetSF(Jet& j) {
    if (abs(j.hadronFlavour) && fabs(j.Eta()) < 2.5) return BJet_SF->evaluate({"central","T",j.hadronFlavour, fabs(j.Eta()), j.Pt()});
    else return 1.;
  }

  double CalcEventSF() {
    Electron_SFs.clear();
    Muon_SFs.clear();
    BJet_SFs.clear();
    double out = 1.0;
    // cout << "ele"<<endl;
    for (unsigned i = 0; i < r->Electrons.size(); ++i) {
      Electron_SFs.push_back(GetElectronSF(r->Electrons[i]));
      out *= GetElectronSF(r->Electrons[i]);
      // cout << " Adding Electron ScaleFactor = " << GetElectronSF(r->Electrons[i]) <<endl;
    }
    // cout << "muon" <<endl;
    for (unsigned i = 0; i < r->Muons.size(); ++i) {
      Muon_SFs.push_back(GetMuonSF(r->Muons[i]));
      out *= GetMuonSF(r->Muons[i]);
      // cout << " Adding Muon ScaleFactor = " << GetMuonSF(r->Muons[i]) << endl;
    }
    // cout << "jets" <<endl;
    for (unsigned i = 0; i < r->Jets.size(); ++i) {
      if (r->Jets[i].btag){
        BJet_SFs.push_back(GetBJetSF(r->Jets[i]));
        out *= GetBJetSF(r->Jets[i]);
        // cout << "Adding B-jet ScaleFactor = " << GetBJetSF(r->Jets[i]) <<endl;
      }
    }
    // cout << "Total Scale Factor of this event  = " << out << endl;
    if (out != MultiplySFs()) cout << "???" << endl;
    return out;
  }

  double MultiplySFs() {
    double out = 1.;
    for (double sf : Electron_SFs) out *= sf;
    for (double sf : Muon_SFs) out *= sf;
    for (double sf : BJet_SFs) out *= sf;
    return out;
  }


  NanoAODReader *r;
  string sampleyear;
  std::shared_ptr<const correction::Correction> Electron_SF;
  std::shared_ptr<const correction::Correction> Muon_Med_SF;
  std::shared_ptr<const correction::Correction> Muon_High_SF;
  std::shared_ptr<const correction::Correction> BJet_SF;
  vector<double> Electron_SFs, Muon_SFs, BJet_SFs;
};

#endif
