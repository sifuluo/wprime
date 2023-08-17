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
  // conf->EntryMax = 2000000;

  NanoAODReader* r = new NanoAODReader(conf);
  bTagEff* bTE = new bTagEff(conf);
  r->SetbTag(bTE);
  JetScale *JS = new JetScale(conf);
  GenHypothesis *gh = new GenHypothesis();
  TString st = conf->SampleType;
  if (st.Contains("FL")) gh->WPType = 0;
  else if (st.Contains("LL")) gh->WPType = 1;
  Permutations *PM = new Permutations(conf);
  
  Long64_t nentries = r->GetEntries();
  if (conf->EntryMax > 0 && conf->EntryMax < nentries) nentries = conf->EntryMax;
  Progress* progress = new Progress(nentries,conf->ProgressInterval);
  for (unsigned ievt = 0; ievt < nentries; ++ievt) {
    progress->Print(ievt);
    if (!(r->ReadEvent(ievt))) continue;
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
    gh->FindGenHypothesis();
    gh->MatchToJets(r->GenJets, r->Jets);
    PM->FillPerm(gh->OutJets, r->EventWeights[0].first);
    Hypothesis TargetHypo;
    TargetHypo.Jets.resize(5);
    for (unsigned i = 0; i < 7; ++i) { //FIXME, Lepton and MET not included in GenHypothesis class!!!
      TLorentzVector TrueReco = gh->OutJets[i].GetV(0);
      TLorentzVector TrueGen = gh->OutGenJets[i].GetV(0);
      if (i < 5) TargetHypo.Jets[i].SetPtEtaPhiM(TrueGen.Pt(), TrueReco.Eta(), TrueReco.Phi(), TrueReco.M());
      if (i == 5)
      if (i == 6) TargetHypo.MET.SetPtEtaPhiM(TrueGen.Pt(), TrueGen.Eta(), TrueReco.Phi(), TrueReco.M());
    }
    JS->HadWMass->Fill()
  }
  JS->PostProcess();
  JS->Clear();
  PM->PostProcess();
  PM->Clear();
  
  progress->JobEnd();

  gApplication->Terminate();

}
