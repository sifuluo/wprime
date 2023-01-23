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

#include "Utilities/ProgressBar.cc"

// From NanoAODReader
void PileupDistFromReader(int isampleyear = 3, int isampletype = 2, int itrigger = 1, int ifile = 1) {
  Configs *conf = new Configs(isampleyear, isampletype, itrigger, ifile);
  conf->Debug = false;
  // conf->PUEvaluation = true;
  // conf->DASInput = true;
  conf->PrintProgress = true;
  // conf->FilesPerJob = 100;
  // conf->EntryMax = 10000;
  conf->SetSwitch("LocalOutput",true);
  // conf->InputFile = "All";
  NanoAODReader *r = new NanoAODReader(conf);
  TFile *fout = new TFile("outputs/test.root","RECREATE");
  TH1F* nPVGoodBeforePUReweight = new TH1F("nPVGoodBeforePUReweight","nPVGoodBeforePUReweight", 99,0,99);
  TH1F* nPVGoodAfterPUReweight = new TH1F("nPVGoodAfterPUReweight","nPVGoodAfterPUReweight", 99,0,99);
  int nPVGood;
  Progress *progress = new Progress(r->GetEntries(), 10);
  for (Long64_t iEvent = 0; iEvent < r->GetEntries(); ++iEvent) {
    if (conf->PrintProgress) progress->Print(iEvent);
    r->ReadEvent(iEvent);
    nPVGood = r->PV_npvsGood;
    nPVGoodBeforePUReweight->Fill(nPVGood);
  }
  fout->Write();
  fout->Save();
}

// // From BranchReader
// void PileupDistFromReader() {
//   TChain *chain = new TChain("Events");
//   chain->Add("/eos/user/p/pflanaga/andrewsdata/skimmed_samples/ttbar/2018/014BDC65-BE41-D14F-B57A-ABD0054C28FE.root");
//   // Events *evts = new Events(chain,"2018",true);
//   Events *evts = new Events(chain);
//   // TFile *fout = new TFile("outputs/test.root","RECREATE");
//   // TH1F* nPVGoodBeforePUReweight = new TH1F("nPVGoodBeforePUReweight","nPVGoodBeforePUReweight", 99,0,99);
//   for (Long64_t iEvent = 0; iEvent < 10; ++iEvent) {
//     Long64_t ientry = evts->LoadTree(iEvent);
//       if (ientry < 0) break;
//     evts->GetEntry(iEvent);
//     // nPVGoodBeforePUReweight->Fill(evts->PV_npvsGood);
//     cout << "flag1 " << evts->Flag_goodVertices << endl;
//     cout << "flag2 " << evts->isolated_muon_trigger << endl;
//     cout << "jetpt = " << evts->Jet_pt[0] <<endl;
//     cout << "nJet = " << evts->nJet <<endl;
//   }
//   // fout->Write();
//   // fout->Save();
// }

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
