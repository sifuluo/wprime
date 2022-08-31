#include "Utilities/NanoAODEvents.C"
#include "Utilities/NanoAODReader.cc"
#include "Utilities/DataFormat.cc"
#include "Utilities/BTag.cc"
#include "Utilities/JESTools.cc"
#include "Utilities/ProgressBar.cc"

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

void MakeJES(string fn = "ttbar2018",int ifstart = 0, int ifend = 0) {
  NanoAODReader* r = new NanoAODReader(fn,ifstart,ifend);
  r->sample = "18";
  JESTools *jt = new JESTools();
  BTag* bt = new BTag(2,"18");
  r->SetBTagger(bt);
  jt->SetBTagger(bt);
  string outname = fn + "_JES.root";
  TFile* JESOutFile = new TFile(outname.c_str(),"RECREATE");
  JESOutFile->cd();
  jt->BookJESPlots();

  for (unsigned iEvent = 0; iEvent < r->GetEntries(); ++iEvent) {
    r->ReadEvent(iEvent);
    PrintProgress(iEvent, r->GetEntries(), 1000);
    for (unsigned ijet = 0; ijet < r->Jets.size(); ++ijet) {
      Jet& iJet = r->Jets[ijet];
      if (iJet.genJetIdx == -1) continue;
      float ratio = r->GenJets[iJet.genJetIdx].Pt() / iJet.Pt();
      jt->FillJESPlot(ratio, iJet.Eta(), iJet.Pt());
    }
  }
  JESOutFile->Write();
  JESOutFile->Save();
}
