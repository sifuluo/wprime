#include <TChain.h>
#include <TString.h>
#include <vector>
#include <TLorentzVector.h>
#include <iostream>
#include <cmath>
#include <array>

#include <TApplication.h>

#include "Utilities/NanoAODReader.cc"
#include "Utilities/ErrorLogDetector.cc"

void CreatebTagEffHist(int sampleyear = 3, int sampletype = 2, int ifile = -1, string infile = "All") {
  dlib.AppendAndrewDatasets();
  if (ErrorLogDetected(sampleyear, sampletype, ifile) == 0) return;
  // infile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/wprime_500/2017/003C756A-BC7B-5A48-8F2E-A61D3CDC7C32.root";
  // sampletype = dlib.AddDataset_NGTCXS("WP500");
  if (sampletype < 0) {
    dlib.ListDatasets();
    cout << "Please enter the index for a dataset:" <<endl;
    cin >> sampletype;
  }
  Configs* conf = new Configs(sampleyear, sampletype, ifile);
  conf->DASInput = false;
  conf->bTagEffHistCreation = true;
  // conf->ProgressInterval = 1;
  conf->InputFile = infile;
  // conf->EntryMax = 2000000;

  NanoAODReader* r = new NanoAODReader(conf);
  bTagEff* bTE = new bTagEff(conf);
  r->SetbTag(bTE);
  
  Long64_t nentries = r->GetEntries();
  if (conf->EntryMax > 0 && conf->EntryMax < nentries) nentries = conf->EntryMax;
  Progress* progress = new Progress(nentries,conf->ProgressInterval);
  for (unsigned ievt = 0; ievt < nentries; ++ievt) {
    progress->Print(ievt);
    if (r->ReadEvent(ievt) < 0) {
      continue;
    }
    r->BranchSizeCheck();
    if (conf->EntryMax > 0 && ievt == conf->EntryMax) break;
    for (unsigned ij = 0; ij < r->Jets.size(); ++ij) {
      bTE->FillJet(r->Jets[ij]);
    }
  }
  bTE->PostProcess();
  bTE->Clear();
  
  progress->JobEnd();

  gApplication->Terminate();
}