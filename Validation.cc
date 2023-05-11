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
  std::array<float,21> EventWeight;

  float LeptonPt, LeptonPt_SU, LeptonPt_SD, LeptonPt_RU, LeptonPt_RD, LeptonEta;

  vector<float> *JetPt, *JetPt_SU, *JetPt_SD, *JetPt_RU, *JetPt_RD, *JetEta;
  float METPt, METPt_SU, METPt_SD, METPt_RU, METPt_RD, METPhi;

  vector<float> *mT;

  int nPU;
  float nTrueInt;
  int nPV, nPVGood;

  vector<float> *WPrimeMassSimpleFL, *WPrimeMassSimpleLL;

  void BookBranches() {
    t->Branch("RegionIdentifier", &RegionIdentifier);
    t->Branch("EventWeight", &EventWeight);

    t->Branch("LeptonPt",&LeptonPt);
    t->Branch("LeptonPt_SU",&LeptonPt_SU);
    t->Branch("LeptonPt_SD",&LeptonPt_SD);
    t->Branch("LeptonPt_RU",&LeptonPt_RU);
    t->Branch("LeptonPt_RD",&LeptonPt_RD);
    t->Branch("LeptonEta",&LeptonEta);

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

    t->Branch("METPt",&METPt);
    t->Branch("METPt_SU",&METPt_SU);
    t->Branch("METPt_SD",&METPt_SD);
    t->Branch("METPt_RU",&METPt_RU);
    t->Branch("METPt_RD",&METPt_RD);
    t->Branch("METPhi", &METPhi);

    mT = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    mT->resize(9);
    t->Branch("mT", &mT);

    WPrimeMassSimpleFL = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    WPrimeMassSimpleLL = new vector<float>; // central , EleSU, EleSD, EleRU, EleRD, JetSU, JetSD, JetRU, JetRD
    WPrimeMassSimpleFL->resize(9);
    WPrimeMassSimpleLL->resize(9);
    t->Branch("WPrimeMassSimpleFL", &WPrimeMassSimpleFL);
    t->Branch("WPrimeMassSimpleLL", &WPrimeMassSimpleLL);

    t->Branch("nPU", &nPU);
    t->Branch("nTrueInt", &nTrueInt);

    t->Branch("nPV", &nPV);
    t->Branch("nPVGood", &nPVGood);
  }

  void FillBranchContent() {
    for(unsigned i = 0; i < 9; ++i){
      RegionIdentifier[i] = r->RegionAssociations.Regions[i];
    }

    for(unsigned i = 0; i < 21; ++i){
      EventWeight[i] = r->EventWeights[i].first;
    }

    LeptonPt = r->Leptons[0].Pt();
    LeptonPt_SU = r->Leptons[0].SU.Pt();
    LeptonPt_SD = r->Leptons[0].SD.Pt();
    LeptonPt_RU = r->Leptons[0].RU.Pt();
    LeptonPt_RD = r->Leptons[0].RD.Pt();
    LeptonEta = r->Leptons[0].Eta();

    JetPt->clear();
    JetPt_SU->clear();
    JetPt_SD->clear();
    JetPt_RU->clear();
    JetPt_RD->clear();
    JetEta->clear();
    for (Jet j : r->Jets) {
      JetPt->push_back(j.Pt());
      JetPt_SU->push_back(j.SU.Pt());
      JetPt_SD->push_back(j.SD.Pt());
      JetPt_RU->push_back(j.RU.Pt());
      JetPt_RD->push_back(j.RD.Pt());
      JetEta->push_back(j.Eta());
    }

    METPt = r->Met.Pt();
    METPt_SU = r->Met.SU.Pt();
    METPt_SD = r->Met.SD.Pt();
    METPt_RU = r->Met.RU.Pt();
    METPt_RD = r->Met.RD.Pt();
    METPhi = r->Met.Phi();

    for (unsigned i = 0; i < 9; ++i) {
      TLorentzVector vmT = r->Leptons[0].GetV(i) + r->Met.GetV(i);
      vmT.SetPz(0);
      mT->at(i) = vmT.M();
      TLorentzVector vWprimeFL = r->Jets[0].GetV(i) + r->Jets[1].GetV(i) + r->Jets[3].GetV(i) + r->Jets[4].GetV(i);
      TLorentzVector vWprimeLL = r->Jets[0].GetV(i) + r->Jets[1].GetV(i) + r->Leptons[0].GetV(i) + r->Met.GetV(i);
      WPrimeMassSimpleFL->at(i) = vWprimeFL.M();
      WPrimeMassSimpleLL->at(i) = vWprimeLL.M();
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

void Validation(int isampleyear = 3, int isampletype = 24, int ifile = 0) {
  Configs *conf = new Configs(isampleyear, isampletype, ifile);
  conf->LocalOutput = false;
  conf->PrintProgress = false;
  conf->ProgressInterval = 2000;
  // conf->EntryMax = 1000000;

  ThisAnalysis *a = new ThisAnalysis(conf);
  a->SetOutput("Validation");
  for (Long64_t iEvent = 0; iEvent < a->GetEntryMax(); ++iEvent) {
    if (!a->ReadEvent(iEvent)) continue;
    a->FillBranchContent();
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
}
