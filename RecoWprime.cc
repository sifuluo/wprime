#include "Utilities/Analyzer.cc"

class ThisAnalyzer : public Analyzer {
public:
  ThisAnalyzer(Configs *conf_): Analyzer(conf_) {

  };
  std::array<int,9> RegionIdentifier;
  std::array<float,18> EventWeight;
  float SimpleWprimeLL;
  float SimpleWprimeFL;
  int SimpleWprimeType;
  float FLdRtb;
  float LLdRtb;
  float WprimeFL;
  float WprimeLL;
  int  WprimeType; // 0 for FL, 1 for LL
  float Likelihood;

  void BookBranches() {
    t->Branch("RegionIdentifier", &RegionIdentifier);
    t->Branch("EventWeight", &EventWeight);
    t->Branch("SimpleWprimeFL",&SimpleWprimeFL);
    t->Branch("SimpleWprimeLL",&SimpleWprimeLL);
    t->Branch("SimpleWprimeType",&SimpleWprimeType);
    t->Branch("WprimeFL",&WprimeFL);
    t->Branch("WprimeLL",&WprimeLL);
    t->Branch("WprimeType",&WprimeType);
    t->Branch("Likelihood", &Likelihood);
  }

  void FitEvent() {
    Ftr->SetJets(r->Jets);
    if (r->RegionAssociations.Regions[0] / 1000 == 1) Ftr->SetLep(r->Muons[0]);
    else if (r->RegionAssociations.Regions[0] / 1000 == 2) Ftr->SetLep(r->Electrons[0]);
    Ftr->SetMET(r->Met);
    Likelihood = Ftr->Optimize();
    // cout << "Likelihood = " << Likelihood <<endl;
    if (Likelihood > 0) {
      Ftr->BestWPrime();
      WprimeFL = Ftr->BestHypo.FLWP().M();
      WprimeLL = Ftr->BestHypo.LLWP().M();
      WprimeType = Ftr->BestHypo.Type;
    }
    else {
      WprimeFL = 0;
      WprimeLL = 0;
      WprimeType = -1;
    }
  }

  void FillBranchContent() {
    for(unsigned i = 0; i < 9; ++i){
      RegionIdentifier[i] = 0;
      //RegionName[i] = "";
    }
    for(unsigned i = 0; i < 18; ++i){
      EventWeight[i] = 0.;
      //EventWeightName[i] = "";
    }

    SimpleWprimeFL = 0.;
    SimpleWprimeLL = 0.;
    SimpleWprimeType = -1;
    WprimeFL = 0.;
    WprimeLL = 0.;
    WprimeType = -1;

    for(unsigned i = 0; i < r->RegionAssociations.RegionCount; ++i){
      RegionIdentifier[i] = r->RegionAssociations.Regions[i];
    }
    for(unsigned i = 0; i < r->EventWeights.size(); ++i){
      EventWeight[i] = r->EventWeights[i].first;
      //EventWeightName[i] = r->EventWeights[i].second;
    }

    TLorentzVector vWprimeLL, vWprimeFL;
    // SimpleWprime assume Jets[0] to be the W' b. And Jet[1] from the top decayed from the W'
    // Jets[3] Jets[4] are light jets.
    vWprimeFL = r->Jets[0] + r->Jets[1] + r->Jets[3] + r->Jets[4]; // Jets 2 is most likely from the top not produced by W'
    float dr = r->Jets[0].DeltaR(r->Jets[1] + r->Jets[3] + r->Jets[4]);
    if(RegionIdentifier[0]/1000 == 1) vWprimeLL = r->Muons[0] + r->Met + r->Jets[0] + r->Jets[1];
    else if(RegionIdentifier[0]/1000 == 2) vWprimeLL = r->Electrons[0] + r->Met + r->Jets[0] + r->Jets[1];
    if ((vWprimeLL - r->Jets[0]).DeltaR(r->Jets[0]) > dr) SimpleWprimeType = 1;
    else SimpleWprimeType = 0;

    SimpleWprimeLL = vWprimeLL.M();
    SimpleWprimeFL = vWprimeFL.M();
    FitEvent();

  }

};

void RecoWprime(int sampleyear = 3, int sampletype = 19, int ifile = 0) {
  Configs* conf = new Configs(sampleyear, sampletype, ifile);
  conf->Debug = false;
  conf->LocalOutput = true;
  conf->PrintProgress = true;
  conf->ProgressInterval = 100;
  // conf->InputFile = "All";
  // conf->EntryMax = 123900;
  ThisAnalyzer *a = new ThisAnalyzer(conf);
  a->SetOutput("RecoWprime");
  cout << "Begin Loop" <<endl;
  for (Long64_t ievt = 0; ievt < a->GetEntryMax(); ++ievt) {
    // if (ievt > 100) break;
    if (!a->ReadEvent(ievt)) continue;
    if (a->r->RegionAssociations.Regions[0] <=0) continue;
    if (a->r->RegionAssociations.GetNJets(0) < 5) {
      // cout << "Prefiltering at main code" <<endl;
      // for (unsigned i = 1; i < 9; ++i) {
      //   if (a->r->RegionAssociations.GetNJets(i) >= 5) cout << "In region " << i << ", 5 Jets is achieved" <<endl;
      // }
      continue;
    }
    a->FillBranchContent();
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();


}