// ROOT
// #include <TROOT.h>
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

#include "Utilities/Analyzer.cc"
#include "Utilities/ErrorLogDetector.cc"

class ThisAnalysis : public Analyzer {
public:
  ThisAnalysis(Configs *conf_)
  : Analyzer(conf_) {
    // Reimplementation of Analyzer to customize the branches to save
  };

  std::array<int,9> RegionIdentifier;
  std::array<float,30> EventWeight; // Size of Array subject to change
  float HEMWeight;

  float LeptonPt, LeptonPt_SU, LeptonPt_SD, LeptonPt_RU, LeptonPt_RD, LeptonEta, LeptonPhi;

  vector<float> *JetPt, *JetPt_SU, *JetPt_SD, *JetPt_RU, *JetPt_RD, *JetEta, *JetPhi, *JetM, *JetM_SU, *JetM_SD, *JetM_RU, *JetM_RD;
  vector<float> *Jet_btagDeepFlavB;
  vector<bool> *JetbTag;
  
  float METPt, METPt_SU, METPt_SD, METPt_RU, METPt_RD, METPhi, METPhi_SU, METPhi_SD, METPhi_RU, METPhi_RD;
  float dPhiMetLep;

  vector<float> *mT;

  int nPU;
  float nTrueInt;
  int nPV, nPVGood;

  GenHypothesis *gh;
  vector<float> *WPrimeMassSimpleFL, *WPrimeMassSimpleLL;
  vector<int> TruePermVector;
  HypothesisBranches *BestHypo, *TrueHypo, *TTHypo;
  vector<float> *TruePermTrueScales;
  // float TruePermLikelihood, TruePermWPrimeMass;
  // vector<float> *PermScales,, *TruePermSolvedScales;
  // vector<int> *WPType, *Perm;
  // vector<float> *WPrimeMassSimpleFL, *WPrimeMassSimpleLL, *Likelihood, *WPrimeMass;
  int PermDiffCode;

  void BookBranches() {
    t->Branch("RegionIdentifier", &RegionIdentifier);
    t->Branch("EventWeight", &EventWeight);
    t->Branch("HEMWeight", &HEMWeight);

    t->Branch("LeptonPt",&LeptonPt);
    t->Branch("LeptonPt_SU",&LeptonPt_SU);
    t->Branch("LeptonPt_SD",&LeptonPt_SD);
    t->Branch("LeptonPt_RU",&LeptonPt_RU);
    t->Branch("LeptonPt_RD",&LeptonPt_RD);
    t->Branch("LeptonEta",&LeptonEta);
    t->Branch("LeptonPhi",&LeptonPhi);

    JetPt = new vector<float>;
    JetPt_SU = new vector<float>;
    JetPt_SD = new vector<float>;
    JetPt_RU = new vector<float>;
    JetPt_RD = new vector<float>;
    JetM = new vector<float>;
    JetM_SU = new vector<float>;
    JetM_SD = new vector<float>;
    JetM_RU = new vector<float>;
    JetM_RD = new vector<float>;
    t->Branch("JetPt",&JetPt);
    t->Branch("JetPt_SU",&JetPt_SU);
    t->Branch("JetPt_SD",&JetPt_SD);
    t->Branch("JetPt_RU",&JetPt_RU);
    t->Branch("JetPt_RD",&JetPt_RD);
    t->Branch("JetM",&JetM);
    t->Branch("JetM_SU",&JetM_SU);
    t->Branch("JetM_SD",&JetM_SD);
    t->Branch("JetM_RU",&JetM_RU);
    t->Branch("JetM_RD",&JetM_RD);
    JetEta = new vector<float>;
    t->Branch("JetEta",&JetEta);
    JetPhi = new vector<float>;
    t->Branch("JetPhi",&JetPhi);
    JetbTag = new vector<bool>;
    t->Branch("JetbTag", &JetbTag);
    Jet_btagDeepFlavB = new vector<float>;
    t->Branch("Jet_btagDeepFlavB", &Jet_btagDeepFlavB);


    t->Branch("METPt",&METPt);
    t->Branch("METPt_SU",&METPt_SU);
    t->Branch("METPt_SD",&METPt_SD);
    t->Branch("METPt_RU",&METPt_RU);
    t->Branch("METPt_RD",&METPt_RD);
    t->Branch("METPhi", &METPhi);
    t->Branch("METPhi_SU",&METPhi_SU);
    t->Branch("METPhi_SD",&METPhi_SD);
    t->Branch("METPhi_RU",&METPhi_RU);
    t->Branch("METPhi_RD",&METPhi_RD);

    t->Branch("dPhiMetLep", &dPhiMetLep);

    t->Branch("nPU", &nPU);
    t->Branch("nTrueInt", &nTrueInt);

    t->Branch("nPV", &nPV);
    t->Branch("nPVGood", &nPVGood);

    mT = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    mT->resize(9);
    t->Branch("mT", &mT);

    WPrimeMassSimpleFL = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    WPrimeMassSimpleLL = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    WPrimeMassSimpleFL->resize(9);
    WPrimeMassSimpleLL->resize(9);
    t->Branch("WPrimeMassSimpleFL", &WPrimeMassSimpleFL);
    t->Branch("WPrimeMassSimpleLL", &WPrimeMassSimpleLL);
    
    if (conf->Type == 2) gh = new GenHypothesis(conf->WPType);
    TruePermTrueScales = new vector<float>;
    t->Branch("TruePermTrueScales",&TruePermTrueScales);
    t->Branch("PermDiffCode", &PermDiffCode);

    BestHypo = new HypothesisBranches("Best");
    TrueHypo = new HypothesisBranches("True");
    TTHypo = new HypothesisBranches("TT");
    BestHypo->CreateBranches(t);
    TrueHypo->CreateBranches(t);
    TTHypo->CreateBranches(t);
  }

  void FillBranchContent() {
    for(unsigned i = 0; i < 9; ++i){
      RegionIdentifier[i] = r->RegionAssociations.Regions[i];
    }

    for(unsigned i = 0; i < r->EventWeights.size(); ++i){
      EventWeight[i] = r->EventWeights[i].first;
    }

    LeptonPt = r->TheLepton.Pt();
    LeptonPt_SU = r->TheLepton.SU.Pt();
    LeptonPt_SD = r->TheLepton.SD.Pt();
    LeptonPt_RU = r->TheLepton.RU.Pt();
    LeptonPt_RD = r->TheLepton.RD.Pt();
    LeptonEta = r->TheLepton.Eta();
    LeptonPhi = r->TheLepton.Phi();

    JetPt->clear();
    JetPt_SU->clear();
    JetPt_SD->clear();
    JetPt_RU->clear();
    JetPt_RD->clear();
    JetEta->clear();
    JetPhi->clear();
    JetbTag->clear();
    Jet_btagDeepFlavB->clear();
    for (Jet& j : r->Jets) {
      if (!j.PUIDpasses[conf->PUIDWP]) continue;
      JetPt->push_back(j.Pt());
      JetPt_SU->push_back(j.SU.Pt());
      JetPt_SD->push_back(j.SD.Pt());
      JetPt_RU->push_back(j.RU.Pt());
      JetPt_RD->push_back(j.RD.Pt());
      JetEta->push_back(j.Eta());
      JetPhi->push_back(j.Phi());
      JetbTag->push_back(j.bTagPasses[conf->bTagWP]);
      Jet_btagDeepFlavB->push_back(j.Jet_btagDeepFlavB);
    }

    METPt = r->Met.Pt();
    METPt_SU = r->Met.SU.Pt();
    METPt_SD = r->Met.SD.Pt();
    METPt_RU = r->Met.RU.Pt();
    METPt_RD = r->Met.RD.Pt();
    METPhi = r->Met.Phi();

    dPhiMetLep = r->Met.DeltaPhi(r->TheLepton);

    PermDiffCode = -1;
    TruePermVector = vector<int>(5,-1);
    TruePermTrueScales->clear();
    bool HasGenHypo = false;
    if (conf->Type == 2) {
      gh->SetGenParts(r->GenParts);
      TruePermVector = gh->MatchToJets(r->GenJets, r->Jets);
      HasGenHypo = gh->ValidReco && gh->ValidGen;
    }
    if (HasGenHypo) {
      gh->MatchToLeptons(r->Leptons);
      gh->CreateHypothesisSet(r->TheLepton, r->Met);
      if (TruePermVector.size() == 5) for (unsigned ipe = 0; ipe < 5; ++ipe) {
        TruePermTrueScales->push_back(gh->OutGenJets[ipe].Pt() / gh->OutJets[ipe].Pt());
      }
    }
    else {
      TruePermVector.clear();
      // cout << Form("OutParts size = %d, OutGenJets size = %d, OutJets size = %d", gh->OutParts.size(), gh->OutGenJets.size(), gh->OutJets.size() ) << endl;
    }
    BestHypo->Reset();
    TrueHypo->Reset();
    TTHypo->Reset();
    for (unsigned i = 0; i < 9; ++i) {
      TLorentzVector lepton = r->TheLepton.v(i);
      TLorentzVector met = r->Met.v(i);
      mT->at(i) = pow(pow(lepton.Pt()+met.Pt(),2)-pow(lepton.Px()+met.Px(),2)-pow(lepton.Py()+met.Py(),2),.5);
      TLorentzVector vWprimeFL = r->Jets[0].v(i) + r->Jets[1].v(i) + r->Jets[3].v(i) + r->Jets[4].v(i);
      TLorentzVector vWprimeLL = r->Jets[0].v(i) + r->Jets[1].v(i) + r->TheLepton.v(i) + r->Met.v(i);
      WPrimeMassSimpleFL->at(i) = vWprimeFL.M();
      WPrimeMassSimpleLL->at(i) = vWprimeLL.M();
      
      if (!conf->RunFitter) continue;
      
      Ftr->SetTruePerm(TruePermVector);
      SetEventFitter(i);
      if (Ftr->Optimize() < 0) continue;
      BestHypo->FillHypothesis(Ftr->BestHypo,i);
      TTHypo->FillHypothesis(Ftr->TTHypo,i);
      if (HasGenHypo && Ftr->TrueHypo.WPType > -1) {
        TrueHypo->FillHypothesis(Ftr->TrueHypo,i);
        if (i == 0) PermDiffCode = PM->ComparePerm(Ftr->BestHypo.Perm, Ftr->TrueHypo.Perm);
      }
      // if (i == 0) { // Only compare the truth against the nominal
      //   if (HasGenHypo && Ftr->TrueHypo.WPType > -1) {
      //     TruePermWPType = Ftr->TrueHypo.WPType;
      //     TruePermLikelihood = Ftr->TrueHypo.GetTotalP();
      //     TruePermWPrimeMass = Ftr->TrueHypo.WP().M();
      //     for (int j =0; j < 4; ++j) TruePermSolvedScales->at(j) = Ftr->TrueHypo.Scales[j];
      //     PermDiffCode = PM->ComparePerm(Ftr->BestPerm, TruePermVector);
      //   }
      //   else {
      //     TruePermWPType = -1;
      //     TruePermLikelihood = -1;
      //     TruePermWPrimeMass = -1;
      //     PermDiffCode = -1;
      //     // if (!HasGenHypo) cout << "Again, no true perm found" << endl;
      //     // if (Ftr->TrueHypo.WPType < 0) cout << "Ftr TrueHypo WPType = " << Ftr->TrueHypo.WPType << endl;
      //   }
      // }
      // if (i > 0 && Likelihood->at(0) > 0) { // Testing the frequency of variances having the different perm
      //   if (Perm->at(i) != Perm->at(0)) cout << "For variation " << i << " with Perm = " << Perm->at(i) << ", different with nominal perm: " << Perm->at(0) << endl;
      // }
    }

    nPU = 0;
    nTrueInt = 0;
    if (IsMC) {
      nPU = r->Pileup_nPU;
      nTrueInt = r->Pileup_nTrueInt;
    }
    nPV = r->PV_npvs;
    nPVGood = r->PV_npvsGood;
  }
};

int Validation(int isampleyear = 3, int isampletype = 24, int ifile = 0, double DoFitterFilesPerJob = 0, int TestRun = 0) {
  Configs *conf = new Configs(isampleyear, isampletype, ifile);
  // if (conf->ErrorRerun() == 0) return 0;
  // conf->InputFile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/SingleMuon/2018/2B07B4C0-852B-9B4F-83FA-CA6B047542D1.root";
  // conf->InputFile = "All";
  conf->PrintProgress = true;
  conf->UseMergedAuxHist = true;
  conf->AcceptRegions({1,2},{1},{5,6},{1,2,3,4});
  conf->AcceptRegions({-4,-3,-2});
  conf->TWMassMode = 0;
  // conf->DebugList = {"LeptonRegion"};
  if (DoFitterFilesPerJob) {
    conf->RunFitter = true;
    conf->FilesPerJob = DoFitterFilesPerJob;
  }
  if (TestRun >= 1) { // Benchmark run time
    conf->ProgressInterval = 1;
    conf->LocalOutput = true;
  }
  if (TestRun >= 2) conf->EntriesMax = 2000; // for debug
  
  ThisAnalysis *a = new ThisAnalysis(conf);
  if (!(a->SetOutput("ValidationFitted"))) return 0;
  int proceededEvts = 0;
  for (Long64_t iEvent = 0; iEvent < a->GetEntriesMax(); ++iEvent) {
    if (a->ReadEvent(iEvent) < 0) continue;
    if (!a->WithinROI()) continue;
    // a->r->BranchSizeCheck();
    proceededEvts++;
    a->FillBranchContent();
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
  cout << "Total Events processed: " << proceededEvts << endl;
  return conf->ErrorRerun();
}
