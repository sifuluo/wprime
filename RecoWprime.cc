#include "Utilities/Analyzer.cc"

class ThisAnalyzer : public Analyzer {
public:
  ThisAnalyzer(Configs *conf_): Analyzer(conf_) {

  };
  std::array<int,9> RegionIdentifier;
  std::array<float,18> EventWeight;
  float SimpleWprime;
  float FLdRtb;
  float LLdRtb;
  float WprimeMass;
  float Likelihood;

  void BookBranches() {
    t->Branch("RegionIdentifier", &RegionIdentifier);
    t->Branch("EventWeight", &EventWeight);
    t->Branch("SimpleWprime",&SimpleWprime);
    t->Branch("WprimeMass",&WprimeMass);
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
      TLorentzVector BestWPrime = Ftr->BestWPrime();
      WprimeMass = BestWPrime.M();
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

    SimpleWprime = 0.;
    WprimeMass = 0.;

    for(unsigned i = 0; i < r->RegionAssociations.RegionCount; ++i){
      RegionIdentifier[i] = r->RegionAssociations.Regions[i];
    }
    for(unsigned i = 0; i < r->EventWeights.size(); ++i){
      EventWeight[i] = r->EventWeights[i].first;
      //EventWeightName[i] = r->EventWeights[i].second;
    }

    TLorentzVector WprimeLL;
    if(RegionIdentifier[0]/1000 == 1) WprimeLL = r->Muons[0] + r->Met + r->Jets[0] + r->Jets[1];
    else if(RegionIdentifier[0]/1000 == 2) WprimeLL = r->Electrons[0] + r->Met + r->Jets[0] + r->Jets[1];

    SimpleWprimeLL = WprimeLL.M();
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
  // conf->EntryMax = 100;
  ThisAnalyzer *a = new ThisAnalyzer(conf);
  a->SetOutput("RecoWprime");
  cout << "Begin Loop" <<endl;
  for (Long64_t ievt = 0; ievt < a->GetEntryMax(); ++ievt) {
    if (!a->ReadEvent(ievt)) continue;
    if (a->r->RegionAssociations.Regions[0] <=0) continue;
    a->FillBranchContent();
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();


}