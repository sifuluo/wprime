#include <TString.h>
// #include <TVector2.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
// #include <TFitResult.h>
// #include <TClonesArray.h>
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TProfile.h>
#include <TEfficiency.h>


// std
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>

#include "Utilities/NanoAODReader.cc"
// #include "Events.C"

// class ThisAnalysis : public Analyzer {
// public:
//   ThisAnalysis(Configs *conf_) : Analyzer(conf_) {
//     // Reimplementation of Analyzer to customize the branches to save
//   };

//   // int nPU;
//   double nTrueInt;
//   // int nPV;
//   int nPVGood;
//   double PUWeight;
//   // TH1F* nPVGoodBeforePUReweight, *nPVGoodAfterPUReweight;

//   void BookBranches() {
//     // t->Branch("PassedSelections",&PassedSelections);
//     t->Branch("EventScaleFactor",&EventScaleFactor);
//     // // t->Branch("nPU", &nPU);
//     // t->Branch("nTrueInt", &nTrueInt);
//     // // t->Branch("nPV", &nPV);
//     t->Branch("nPVGood", &nPVGood);
//     t->Branch("PUweight", &PUWeight);
//     // nPVGoodBeforePUReweight = new TH1F("nPVGoodBeforePUReweight","nPVGoodBeforePUReweight", 99,0,99);
//     // nPVGoodAfterPUReweight = new TH1F("nPVGoodAfterPUReweight","nPVGoodAfterPUReweight", 99,0,99);
//   }

//   void FillEventCounter() {
//     return;
//   }

//   void FillBranchContent() {
//     PUWeight = 1;
//     if (IsMC) {
//       // nPU = r->Pileup_nPU;
//       nTrueInt = r->Pileup_nTrueInt;
//       // PUWeight = GetEventPUWeight();
//     }
//     else {
//       // nPU = 0;
//       nTrueInt = 0;
//       PUWeight = 1;
//     }
//     // nPV = r->PV_npvs;
//     nPVGood = r->PV_npvsGood;
//     // nPVGoodBeforePUReweight->Fill(nPVGood, EventScaleFactor);
//     // nPVGoodAfterPUReweight->Fill(nPVGood, EventScaleFactor * PUWeight);
//   }
// };
// // From NanoAODReader
// void PileupDistFromReader(int isampleyear = 3, int isampletype = 2, int itrigger = 1, int ifile = 1) {
//   Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
//   conf->Debug = false;
//   conf->PUEvaluation = true;
//   // conf->DASInput = true;
//   // conf->PrintProgress = true;
//   // conf->FilesPerJob = 100;
//   conf->EntryMax = 10000;
//   conf->SetSwitch("LocalOutput",true);
//   // conf->InputFile = "All";
//   NanoAODReader *r = new NanoAODReader(conf);
//   TFile *fout = new TFile("outputs/test.root","RECREATE");
//   TH1F* nPVGoodBeforePUReweight = new TH1F("nPVGoodBeforePUReweight","nPVGoodBeforePUReweight", 99,0,99);
//   TH1F* nPVGoodAfterPUReweight = new TH1F("nPVGoodAfterPUReweight","nPVGoodAfterPUReweight", 99,0,99);
//   int nPVGood;
//   for (Long64_t iEvent = 0; iEvent < conf->EntryMax; ++iEvent) {
//     r->ReadEvent(iEvent);
//     nPVGood = r->PV_npvsGood;
//     nPVGoodBeforePUReweight->Fill(nPVGood);
//   }
//   fout->Write();
//   fout->Save();
// }

// From BranchReader
void PileupDistFromReader() {
  TChain *chain = new TChain("Events");
  chain->Add("/eos/user/p/pflanaga/andrewsdata/skimmed_samples/ttbar/2018/014BDC65-BE41-D14F-B57A-ABD0054C28FE.root");
  Events *evts = new Events(chain,"2018",true);
  // Events *evts = new Events(chain);
  // TFile *fout = new TFile("outputs/test.root","RECREATE");
  // TH1F* nPVGoodBeforePUReweight = new TH1F("nPVGoodBeforePUReweight","nPVGoodBeforePUReweight", 99,0,99);
  for (Long64_t iEvent = 0; iEvent < 10; ++iEvent) {
    Long64_t ientry = evts->LoadTree(iEvent);
      if (ientry < 0) break;
    evts->GetEntry(iEvent);
    // nPVGoodBeforePUReweight->Fill(evts->PV_npvsGood);
    cout << "flag1 " << evts->Flag_goodVertices << endl;
    cout << "flag2 " << evts->isolated_muon_trigger << endl;
    cout << "jetpt = " << evts->Jet_pt[0] <<endl;
    cout << "nJet = " << evts->nJet <<endl;
  }
  // fout->Write();
  // fout->Save();
}

// // From Raw Chain
// void PileupDistFromReader() {
//   TChain *chain = new TChain("Events");
//   chain->Add("/eos/user/p/pflanaga/andrewsdata/skimmed_samples/ttbar/2018/014BDC65-BE41-D14F-B57A-ABD0054C28FE.root");
//   int PV_npvsGood;
//   chain->SetBranchAddress("PV_npvsGood", &PV_npvsGood);
//   TFile *fout = new TFile("outputs/test.root","RECREATE");
//   TH1F* nPVGoodBeforePUReweight = new TH1F("nPVGoodBeforePUReweight","nPVGoodBeforePUReweight", 99,0,99);
//   for (Long64_t iEvent = 0; iEvent < 10000; ++iEvent) {
//     chain->GetEntry(iEvent);
//     nPVGoodBeforePUReweight->Fill(PV_npvsGood);
//   }
//   fout->Write();
//   fout->Save();
// }
