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
  conf->bTagEffHistCreation = false;
  conf->AuxHistCreation = true;
  // conf->ProgressInterval = 1;
  conf->InputFile = infile;
  // conf->EntryMax = 1000;

  NanoAODReader* r = new NanoAODReader(conf);
  bTagEff* bTE = new bTagEff(conf);
  r->SetbTag(bTE);
  JetScale *JS = new JetScale(conf);
  GenHypothesis *gh = new GenHypothesis(conf->WPType);
  Permutations *PM = new Permutations(conf);
  
  Long64_t nentries = r->GetEntries();
  if (conf->EntryMax > 0 && conf->EntryMax < nentries) nentries = conf->EntryMax;
  Progress* progress = new Progress(nentries,conf->ProgressInterval);
  for (unsigned ievt = 0; ievt < nentries; ++ievt) {
    progress->Print(ievt);
    if (r->ReadEvent(ievt) < 0) continue;
    r->BranchSizeCheck();
    if (conf->EntryMax > 0 && ievt == conf->EntryMax) break;
    //check jet multiplicity
    int nj = r->RegionAssociations.GetNJets(0); // Nominal region
    int nb = r->RegionAssociations.GetbTags(0); // Nominal region
    for (unsigned ij = 0; ij < r->Jets.size(); ++ij) {
      if (r->Jets[ij].genJetIdx != -1 && ((nj >= 5 && nb >= 3) || (nj >= 6 && nb >= 3))) { // Fitter takes SR inputs
        JS->FillJet(r->Jets[ij],r->GenJets[r->Jets[ij].genJetIdx], r->EventWeights[0].first);
      }
    }
    if (conf->Type != 2) continue;

    if (nj != 5 && nj != 6) continue;
    gh->SetGenParts(r->GenParts);
    gh->MatchToJets(r->GenJets, r->Jets);
    if (!gh->ValidReco || !gh->ValidGen) continue;
    gh->MatchToLeptons(r->Leptons);
    gh->CreateHypothesisSet(r->TheLepton, r->Met);
    PM->FillPerm(gh->OutJets, r->EventWeights[0].first);
    Hypothesis Tar = gh->TarRecoHypo;
    Tar.Neu = gh->OutParts[6]; // Use Gen-Level neutrino as to replace the met.
    JS->FillHypo(Tar, r->EventWeights[0].first);
  }
  JS->PostProcess();
  JS->Clear();
  PM->PostProcess();
  PM->Clear();
  gh->Summarize();
  
  progress->JobEnd();

  gApplication->Terminate();

}
