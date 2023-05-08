#include <TChain.h>
#include <TString.h>
#include <vector>
#include <TLorentzVector.h>
#include <iostream>
#include <cmath>
#include <array>

#include "Utilities/NanoAODReader.cc"
#include "Utilities/JetScale.cc"

void CreateAuxHists(int sampleyear = 3, int sampletype = 1, int ifile = -1, string infile = "All") {
  // infile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/wprime_500/2017/003C756A-BC7B-5A48-8F2E-A61D3CDC7C32.root";
  // sampletype = dlib.AddDataset_NGTCXS("WP500");
  Configs* conf = new Configs(sampleyear, sampletype, ifile);
  conf->Debug = false;
  conf->DASInput = false;
  conf->bTagEffCreation = true;
  conf->JetScaleCreation = true;
  conf->InputFile = infile;

  NanoAODReader* r = new NanoAODReader(conf);
  bTagEff* bTE = new bTagEff(conf);
  JetScale *JS = new JetScale(conf);
  r->SetbTag(bTE);
  
  Long64_t nentries = r->GetEntries();
  Progress* progress = new Progress(nentries,conf->ProgressInterval);
  for (unsigned ievt = 0; ievt < nentries; ++ievt) {
    progress->Print(ievt);
    if (!(r->ReadEvent(ievt))) break;
    // if (ievt == 100000) break;
    for (unsigned ij = 0; ij < r->Jets.size(); ++ij) {
      bTE->FillJet(r->Jets[ij]);
      if (r->Jets[ij].genJetIdx != -1) JS->FillJet(r->Jets[ij],r->GenJets[r->Jets[ij].genJetIdx]);
    }
  }
  bTE->PostProcess();
  bTE->Clear();
  JS->PostProcess();
  JS->Clear();
  
  progress->JobEnd();

}
