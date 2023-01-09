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

		//set b-tagging working point for calculation -> move this to general configuration later
		Bwp = 2; //0: loose, 1: medium, 2: tight
	}

	//FIXME: Need to introduce up and down variations of Electron ID, need trigger SF implementation
	double GetElectronSF(Electron& e, int mode) {
		return Electron_SF->evaluate({sampleyear, "sf", "wp90iso", e.Eta(), e.Pt()});
	}

	//FIXME: Need actual implementation of the three different types of muon scale factor sources
	double GetMuonTriggerSF(Muon& m, int mode) {
		return Muon_Med_SF->evaluate({sampleyear + "_UL", fabs(m.Eta()), m.Pt(), "sf"});
	}

	double GetMuonIDSF(Muon& m, int mode) {
		return Muon_Med_SF->evaluate({sampleyear + "_UL", fabs(m.Eta()), m.Pt(), "sf"});
	}

	double GetMuonIsoSF(Muon& m, int mode) {
		return Muon_Med_SF->evaluate({sampleyear + "_UL", fabs(m.Eta()), m.Pt(), "sf"});
	}

	double GetBJetSFcontribution(Jet& j, int mode) {
		//select 0=loose, 1=medium, 2=tight
		string wp;
		if(Bwp == 0) wp = "L";
		else if(Bwp == 1) wp = "M";
		else if(Bwp == 2) wp = "T";
		if (fabs(j.Eta()) < 2.5){
			double SF = 0;
			if(mode == 0)       BJet_SF->evaluate({"central", wp, j.hadronFlavour, fabs(j.Eta()), j.Pt()});
			else if(mode == -1) BJet_SF->evaluate({"up", wp, j.hadronFlavour, fabs(j.Eta()), j.Pt()});
			else if(mode == +1) BJet_SF->evaluate({"down", wp, j.hadronFlavour, fabs(j.Eta()), j.Pt()});
			if(j.bTagPasses[Bwp]) return SF;
			else{
				//using a dummy efficiency for now, this will have to be sample-specific later
				double eff = 0.1;
				return (1.-eff*SF)/(1.-eff);
			}
		}
		else return 1.;
	}

	vector<EventWeight> CalcEventSFweights() {
		vector<EventWeight> out;
		// Electron_SFs.clear();
		// Muon_SFs.clear();
		// BJet_SFs.clear();
		EventWeight electronIDW, muonIDW, muonIsoW, muonTriggerW, BjetW, L1PreFiringW;
		electronIDW.source = "ElectronID";
		muonIDW.source = "muonID";
		muonIsoW.source = "muonIso";
		muonTriggerW.source = "muonTrigger";
		BjetW.source = "BjetTag";
		L1PreFiringW.source = "L1PreFiring";
		int modes[3]={-1,0,+1};
		for(unsigned i=0; i<3; ++i){ //loop over down, central, up variations of all event weights
			//electron ID SF = event weight, since there is only one electron, if any. Events with two electrons should be veto'd by default
			if(r->Electrons.size() == 1){
				electronIDW.IsActive = true;
				electronIDW.variations[i] = GetElectronSF(r->Electrons[0], modes[i]);
			}
			else{
				electronIDW.IsActive = false;
			}

			//muon ID and Iso SF = event weight, since there is only one muon, if any. Events with two muons should be veto'd by default
			if(r->Muons.size() == 1){
				muonTriggerW.variations[i] = GetMuonTriggerSF(r->Muons[0], modes[i]);
				muonIDW.variations[i] = GetMuonIDSF(r->Muons[0], modes[i]);
				muonIsoW.variations[i] = GetMuonIsoSF(r->Muons[0], modes[i]);
			}
			else{
				muonTriggerW.IsActive = false;
				muonIDW.IsActive = false;
				muonIsoW.IsActive = false;
			}

			//jet Btag SF implementation
			//FIXME: needs some reference to actual PUID selection later
			float BtagsW = 1.;
			for (unsigned i = 0; i < r->Jets.size(); ++i) BtagsW *= GetBJetSFcontribution(r->Jets[i], modes[i]);
			BjetW.IsActive = true;
			BjetW.variations[i] = BtagsW;

			//L1PrefiringWeight
			if(sampleyear == "2016preVFP" || sampleyear == "2016postVFP" || sampleyear == "2017"){
				L1PreFiringW.IsActive = true;
			}
			else L1PreFiringW.IsActive = false;
		}

		//L1PrefiringWeight
		if(sampleyear == "2016preVFP" || sampleyear == "2016postVFP" || sampleyear == "2017"){
			L1PreFiringW.IsActive = true;
			L1PreFiringW.variations[0] = r->L1PreFiringWeight_Dn;
			L1PreFiringW.variations[1] = r->L1PreFiringWeight_Nom;
			L1PreFiringW.variations[2] = r->L1PreFiringWeight_Up;
		}
		else L1PreFiringW.IsActive = false;

		//put all sources together
		out.push_back(electronIDW);
		out.push_back(muonIDW);
		out.push_back(muonIsoW);
		out.push_back(muonTriggerW);
		out.push_back(BjetW);
		out.push_back(L1PreFiringW);

		return out;
	}

	vector<pair<double, string> > WeightVariations(vector<EventWeight> sources_) {
		vector<pair<float, string> > out;

		//determine nominal weight
		float CentralWeight = 1.;
		for(unsigned i = 0; i < sources_.size(); ++i){
			if(!sources_[i].IsActive) continue; //skip inactive sources
			CentralWeight *= sources_[i].variations[1];
		}
		out.push_back(make_pair(CentralWeight, "Nominal");

		//select source for up and down variations
		for(unsigned i = 0; i < sources_.size(); ++i){
			if(!sources_[i].IsActive) continue; //skip inactive sources

			//create variations with strings for later combine histograms
			out.push_back(make_pair(CentralWeight / sources_[i].variations[1] * sources_[i].variations[0], sources_[i].source + "_down"));
			out.push_back(make_pair(CentralWeight / sources_[i].variations[1] * sources_[i].variations[2], sources_[i].source + "_up"));
		}

		return out;
	}


	NanoAODReader *r;
	string sampleyear;
	std::shared_ptr<const correction::Correction> Electron_SF;
	std::shared_ptr<const correction::Correction> Muon_Med_SF;
	std::shared_ptr<const correction::Correction> Muon_High_SF;
	std::shared_ptr<const correction::Correction> BJet_SF;

	unsigned Bwp;
};

#endif
