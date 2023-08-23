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

class ThisAnalysis : public Analyzer {
public:
  ThisAnalysis(Configs *conf_)
  : Analyzer(conf_) {
    // Reimplementation of Analyzer to customize the branches to save
  };

  std::array<int,9> RegionIdentifier;
  std::array<float,30> EventWeight; // Size of Array subject to change

  float LeptonPt, LeptonPt_SU, LeptonPt_SD, LeptonPt_RU, LeptonPt_RD, LeptonEta, LeptonPhi;

  vector<float> *JetPt, *JetPt_SU, *JetPt_SD, *JetPt_RU, *JetPt_RD, *JetEta, *JetPhi;
  float METPt, METPt_SU, METPt_SD, METPt_RU, METPt_RD, METPhi;
  float dPhiMetLep;

  vector<float> *mT;

  int nPU;
  float nTrueInt;
  int nPV, nPVGood;

  vector<int> *WPType;
  vector<float> *WPrimeMassSimpleFL, *WPrimeMassSimpleLL, *Likelihood, *WPrimeMass;

  void BookBranches() {
    t->Branch("RegionIdentifier", &RegionIdentifier);
    t->Branch("EventWeight", &EventWeight);

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
    t->Branch("JetPt",&JetPt);
    t->Branch("JetPt_SU",&JetPt_SU);
    t->Branch("JetPt_SD",&JetPt_SD);
    t->Branch("JetPt_RU",&JetPt_RU);
    t->Branch("JetPt_RD",&JetPt_RD);
    JetEta = new vector<float>;
    t->Branch("JetEta",&JetEta);
    JetPhi = new vector<float>;
    t->Branch("JetPhi",&JetPhi);

    t->Branch("METPt",&METPt);
    t->Branch("METPt_SU",&METPt_SU);
    t->Branch("METPt_SD",&METPt_SD);
    t->Branch("METPt_RU",&METPt_RU);
    t->Branch("METPt_RD",&METPt_RD);
    t->Branch("METPhi", &METPhi);

    t->Branch("dPhiMetLep", &dPhiMetLep);

    mT = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    mT->resize(9);
    t->Branch("mT", &mT);

    WPrimeMassSimpleFL = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    WPrimeMassSimpleLL = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    WPrimeMassSimpleFL->resize(9);
    WPrimeMassSimpleLL->resize(9);
    t->Branch("WPrimeMassSimpleFL", &WPrimeMassSimpleFL);
    t->Branch("WPrimeMassSimpleLL", &WPrimeMassSimpleLL);

    WPrimeMass = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    Likelihood = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    WPType = new vector<int>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    WPrimeMass->resize(9);
    Likelihood->resize(9);
    WPType->resize(9);
    if (conf->RunFitter) {
      t->Branch("WPrimeMass", &WPrimeMass);
      t->Branch("Likelihood", &Likelihood);
      t->Branch("WPType", &WPType);
    }

    t->Branch("nPU", &nPU);
    t->Branch("nTrueInt", &nTrueInt);

    t->Branch("nPV", &nPV);
    t->Branch("nPVGood", &nPVGood);
  }

  void FillBranchContent() {
    // cout << "Regions are: " << endl;
    for(unsigned i = 0; i < 9; ++i){
      RegionIdentifier[i] = r->RegionAssociations.Regions[i];
      // cout << RegionIdentifier[i] << ", ";
    }
    // cout <<endl;

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
    for (Jet& j : r->Jets) {
      if (!j.PUIDpasses[conf->PUIDWP]) continue;
      JetPt->push_back(j.Pt());
      JetPt_SU->push_back(j.SU.Pt());
      JetPt_SD->push_back(j.SD.Pt());
      JetPt_RU->push_back(j.RU.Pt());
      JetPt_RD->push_back(j.RD.Pt());
      JetEta->push_back(j.Eta());
      JetPhi->push_back(j.Phi());
    }

    METPt = r->Met.Pt();
    METPt_SU = r->Met.SU.Pt();
    METPt_SD = r->Met.SD.Pt();
    METPt_RU = r->Met.RU.Pt();
    METPt_RD = r->Met.RD.Pt();
    METPhi = r->Met.Phi();

    dPhiMetLep = r->Met.DeltaPhi(r->TheLepton);

    for (unsigned i = 0; i < 9; ++i) {
      TLorentzVector vmT = r->TheLepton.GetV(i) + r->Met.GetV(i);
      vmT.SetPz(0);
      mT->at(i) = vmT.M();
      TLorentzVector vWprimeFL = r->Jets[0].GetV(i) + r->Jets[1].GetV(i) + r->Jets[3].GetV(i) + r->Jets[4].GetV(i);
      TLorentzVector vWprimeLL = r->Jets[0].GetV(i) + r->Jets[1].GetV(i) + r->TheLepton.GetV(i) + r->Met.GetV(i);
      WPrimeMassSimpleFL->at(i) = vWprimeFL.M();
      WPrimeMassSimpleLL->at(i) = vWprimeLL.M();
      if (!conf->RunFitter) continue;
      if (i == 0) {
        SetEventFitter(i);
        Likelihood->at(i) = Ftr->Optimize();
        if (Likelihood->at(i) < 0) continue;
        WPType->at(i) = Ftr->BestHypo.WPType;
        WPrimeMass->at(i) = Ftr->BestHypo.WP().M();
      }
      else {
        Likelihood->at(i) = -1;
        WPType->at(i) = -1;
        WPrimeMass->at(i) = -1;
      }
    }

    // cout << "2" <<endl;

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

void Validation(int isampleyear = 3, int isampletype = 2, int ifile = 0) {
  Configs *conf = new Configs(isampleyear, isampletype, ifile);
  conf->InputFile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/SingleMuon/2018/2B07B4C0-852B-9B4F-83FA-CA6B047542D1.root";
  conf->LocalOutput = false;
  conf->PrintProgress = true;
  conf->RunFitter = false;
  conf->UseMergedAuxHist = true;
  conf->AcceptRegions({1,2},{1},{5,6},{0,1,2,3,4,5,6});
  // conf->DebugList = {"LeptonRegion"};
  // conf->ProgressInterval = 1;
  // conf->EntryMax = 20000;
  // conf->RerunList("2018","ttbar",{106, 167, 301, 322, 53,126, 254, 307, 332, 6,131, 259, 312, 350, 74,133, 281, 317, 352, 87,146, 287, 320, 388, 92,148, 288, 321, 3});
  // conf->RerunList("2018","FL700",{0});
  // conf->RerunList("2018","LL800",{0});
  // conf->RerunList("2018","SingleMuon",{320});
  
  ThisAnalysis *a = new ThisAnalysis(conf);
  if (!(a->SetOutput("Validation"))) return;
  for (Long64_t iEvent = 0; iEvent < a->GetEntryMax(); ++iEvent) {
    if (a->ReadEvent(iEvent)) continue;
    if (!a->WithinROI()) continue;
    // a->r->BranchSizeCheck();
    a->FillBranchContent();
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
}
