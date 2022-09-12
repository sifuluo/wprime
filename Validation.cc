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
#include "Utilities/NanoAODReader.cc"
#include "Utilities/DataFormat.cc"
#include "Utilities/BTag.cc"
#include "Utilities/ScaleFactor.cc"
#include "Utilities/ProgressBar.cc"

void Validation(int isampleyear = 1, int isampletype = 2, int itrigger = 1, int ifile = 0) {
  // Indices details see Utilities/Constants.cc
  bool debug = false;
  Analyzer *a = new Analyzer(isampleyear, isampletype, itrigger, ifile, debug);
  a->SetOutput("Validation");
  // a->SetEntryMax(10000);
  for (Long64_t iEvent = 0; iEvent < a->GetEntryMax(); ++iEvent) {
    bool pass = a->ReadEvent(iEvent);
    if (pass) continue;
    a->FillTree();
  }
  a->SaveOutput();
  a->CloseOutput();
  a->SuccessFlag();
}
