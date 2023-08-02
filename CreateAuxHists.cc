#include <TChain.h>
#include <TString.h>
#include <vector>
#include <TLorentzVector.h>
#include <iostream>
#include <cmath>
#include <array>

#include <TApplication.h>

#include "Utilities/NanoAODReader.cc"
#include "Utilities/JetScale.cc"
#include "Utilities/Permutations.cc"
#include "Utilities/Hypothesis.cc"

void CreateAuxHists(int sampleyear = 3, int sampletype = 2, int ifile = -1, string infile = "All") {
  dlib.AppendAndrewDatasets();
  // infile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/wprime_500/2017/003C756A-BC7B-5A48-8F2E-A61D3CDC7C32.root";
  // sampletype = dlib.AddDataset_NGTCXS("WP500");
  if (sampletype < 0) {
    dlib.ListDatasets();
    cout << "Please enter the index for a dataset:" <<endl;
    cin >> sampletype;
  }
  Configs* conf = new Configs(sampleyear, sampletype, ifile);
  conf->DASInput = false;
  conf->AuxHistCreation = true;
  // conf->ProgressInterval = 1;
  conf->InputFile = infile;
  conf->EntryMax = 2000000;

  NanoAODReader* r = new NanoAODReader(conf);
  bTagEff* bTE = new bTagEff(conf);
  JetScale *JS = new JetScale(conf);
  GenHypothesis *gh = new GenHypothesis();
  TString st = conf->SampleType;
  if (st.Contains("FL")) gh->Type = 0;
  else if (st.Contains("LL")) gh->Type = 1;
  Permutations *PM = new Permutations(conf);
  r->SetbTag(bTE);
  
  Long64_t nentries = r->GetEntries();
  if (conf->EntryMax > 0 && conf->EntryMax < nentries) nentries = conf->EntryMax;
  Progress* progress = new Progress(nentries,conf->ProgressInterval);
  for (unsigned ievt = 0; ievt < nentries; ++ievt) {
    progress->Print(ievt);
    if (!(r->ReadEvent(ievt))) break;
    r->BranchSizeCheck();
    if (conf->EntryMax > 0 && ievt == conf->EntryMax) break;
    //check jet multiplicity
    int nj = 0;
    int nb = 0;
    for(unsigned j = 0; j<r->Jets.size(); ++j){
      if(r->Jets[j].Pt() < 30.) continue;
      if(!r->Jets[j].PUIDpasses[conf->PUIDWP]) continue;//select working point for PUID to none by commenting this line out, loose by PUIDpasses 0, medium by 1, tight by 2
      nj++;
      if(r->Jets[j].bTagPasses[conf->bTagWP]) nb++;//select working point for b-tagging by bTagPasses[0] = loose, 1 medium and 2 tight
    }
    for (unsigned ij = 0; ij < r->Jets.size(); ++ij) {
      bTE->FillJet(r->Jets[ij]);
      if (r->Jets[ij].genJetIdx != -1 && ((nj >= 5 && nb >= 2) || (nj >= 6 && nb >= 3))) { // Fitter takes SR inputs
        JS->FillJet(r->Jets[ij],r->GenJets[r->Jets[ij].genJetIdx]);
      }
    }
    if (conf->Type != 2) continue;

    if (nj != 5 && nj != 6) continue;
    gh->SetGenParts(r->GenParts);
    gh->FindGenHypothesis();
    vector<Jet> HypoJets = gh->GetTruthJets(r->GenJets, r->Jets);
    PM->FillPerm(HypoJets);
  }
  bTE->PostProcess();
  bTE->Clear();
  JS->PostProcess();
  JS->Clear();
  PM->PostProcess();
  PM->Clear();
  
  progress->JobEnd();

  gApplication->Terminate();

}
