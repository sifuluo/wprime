#include <TChain.h>
#include <TString.h>
#include <vector>
#include <TLorentzVector.h>
#include <iostream>
#include <cmath>
#include <array>

#include "Utilities/Analyzer.cc"

class ThisAnalyzer : public Analyzer {
public:
  ThisAnalyzer(Configs *conf_)
  : Analyzer(conf_) {
    //empty configuratioon and no idea what to write in here...
  };

  //event sorting and systematic weights
  std::array<int,9> RegionIdentifier;
  //std::array<string,9> RegionName;
  std::array<float,18> EventWeight;
  //std::array<string,18> EventWeightName;

  //basic variables
  std::array<float,6> Jet_pt, Jet_eta, Jet_phi;
  float Electron_pt, Electron_eta, Electron_phi;
  float Muon_pt, Muon_eta, Muon_phi;
  float MET_pt, MET_phi, MET_eta;

  //complex variables
  float SimpleWprime;

  //booking branches for intermediate ntuple
  void BookBranches(){
    t->Branch("RegionIdentifier", &RegionIdentifier);
    //t->Branch("RegionName", &RegionName);
    t->Branch("EventWeight", &EventWeight);
    //t->Branch("EventWeightName", &EventWeightName);
    
    t->Branch("Jet_pt", &Jet_pt);
    t->Branch("Jet_eta", &Jet_eta);
    t->Branch("Jet_phi", &Jet_phi);

    t->Branch("Electron_pt", &Electron_pt);
    t->Branch("Electron_eta", &Electron_eta);
    t->Branch("Electron_phi", &Electron_phi);

    t->Branch("Muon_pt", &Muon_pt);
    t->Branch("Muon_eta", &Muon_eta);
    t->Branch("Muon_phi", &Muon_phi);

    t->Branch("MET_pt", &MET_pt);
    t->Branch("MET_phi", &MET_phi);

    t->Branch("SimpleWprime", &SimpleWprime);
  }//end BookBranches

  void FillBranchContent(){
    //reset everything
    for(unsigned i = 0; i < 9; ++i){
      RegionIdentifier[i] = 0;
      //RegionName[i] = "";
    }

    for(unsigned i = 0; i < 18; ++i){
      EventWeight[i] = 0.;
      //EventWeightName[i] = "";
    }

    for(unsigned i = 0; i < 6; ++i){
      Jet_pt[i] = 0.;
      Jet_eta[i] = 0.;
      Jet_phi[i] = 0.;
    }

    Electron_pt = 0.;
    Electron_eta = 0.;
    Electron_phi = 0.;

    Muon_pt = 0.;
    Muon_eta = 0.;
    Muon_phi = 0.;

    MET_pt = 0.;
    MET_phi = 0.;

    SimpleWprime = 0.;

    for(unsigned i = 0; i < r->RegionAssociations.RegionCount; ++i){
      RegionIdentifier[i] = r->RegionAssociations.Regions[i];
      //RegionName[i] = r->RegionAssociations.RegionNames[i];
    }

    for(unsigned i = 0; i < r->EventWeights.size(); ++i){
      EventWeight[i] = r->EventWeights[i].first;
      //EventWeightName[i] = r->EventWeights[i].second;
    }

    for(unsigned i = 0; (i < r->Jets.size()) && (i < 6); ++i){
      Jet_pt[i] = r->Jets[i].Pt();
      Jet_eta[i] = r->Jets[i].Eta();
      Jet_phi[i] = r->Jets[i].Phi();
    }

    if(r->Muons.size()){
      Muon_pt = r->Muons[0].Pt();
      Muon_eta = r->Muons[0].Eta();
      Muon_phi = r->Muons[0].Phi();
    }

    if(r->Electrons.size()){
      Electron_pt = r->Electrons[0].Pt();
      Electron_eta = r->Electrons[0].Eta();
      Electron_phi = r->Electrons[0].Phi();
    }
  
    MET_pt = r->Met.Pt();
    MET_phi = r->Met.Phi();

    TLorentzVector Wprime;
    if(RegionIdentifier[0]/1000 == 1) Wprime = r->Muons[0] + r->Met + r->Jets[0] + r->Jets[1];
    else if(RegionIdentifier[0]/1000 == 2) Wprime = r->Electrons[0] + r->Met + r->Jets[0] + r->Jets[1];

    SimpleWprime = Wprime.M();
  }//end FillBranchContent
};//end ThisAnalyzer

void WprimeRegions(int sampleyear = 3, int sampletype = 16, int itrigger = 0, int ifile = -1){
  Configs *conf = new Configs(sampleyear, sampletype, itrigger, ifile);
  conf->Debug = false;
  conf->PUEvaluation = false;
  conf->DASInput = false;
  conf->InputFile = "All";
  if(sampleyear == 3) conf->SampleYear="2018";

  ThisAnalyzer *w = new ThisAnalyzer(conf);
  w->SetOutput("WprimeRegions");
  w->BookBranches();
  for(Long64_t iEvent = 0; iEvent < w->GetEntryMax(); ++iEvent){
    w->ReadEvent(iEvent);
    w->FillBranchContent();
    w->FillTree();
  }
  w->SaveOutput();
  w->CloseOutput();
}




