#include <vector>

#include "TFile.h"
#include "TChain.h"
#include "TString.h"

#include "DenisTree.C"
#include "DrawDataFormat.cc"
#include "../Utilities/ProgressBar.cc"

using namespace std;



class HistMaker : public DenisTree {
public:
  HistMaker(TTree* t) : DenisTree(t) {
        
  };

  void FillHistograms() {

  }
};



void MakeHist(int isampleyear = 3, int PUWP = 0, int bWP = 0) {
  vector<string> PUWPs{"_PUloose","_PUmedium","_PUtight"};
  vector<string> bTWPs{"_bTagLoose","_bTagMedium","_bTagTight"};
  string basepath = "/eos/user/r/rathjd/WPrimeAnalysis/";
  string itpath = "WprimeRegions" + PUWPs[PUWP] + bTWPs[bWP] + "/";
  string SampleYear = dlib.SampleYears[isampleyear];

  TString OutFilePath = "outputs/" + SampleYear + PUWPs[PUWP] + bTWPs[bWP] + ".root";
  TFile* fout = new TFile(OutFilePath,"RECREATE");
  vector<RegionHistsByVariables> HistCol;
  TString NameTemplate = "=SampleType=_=Variable=_=RegionRange=";

  Progress* progress = new Progress(1000,10000);

  for (unsigned ist = 0; ist < dlib.DatasetNames.size(); ++ist) {
    string SampleType = dlib.DatasetNames[ist];
    cout << endl << "Start processing " << SampleType << endl;
    string SamplePath = SampleYear + "_" + SampleType + ".root";
    TString InFilePath = basepath + itpath + SamplePath;
    TFile *fin = new TFile(InFilePath,"READ");
    
    fout->cd();
    // TString NameTemplate_ = NameTemplate.ReplaceAll("=SampleType=",(TString)SampleType);
    TString NameTemplate_ = Replacement(NameTemplate, "=SampleType=",(TString)SampleType);
    HistCol.push_back(RegionHistsByVariables(NameTemplate_,NameTemplate_));
    HistCol.back().AddVariable("SimpleWprime",100,0,1000);
    HistCol.back().AddVariable("LeadingJetPt",100,0,1000);

    TTree* t = (TTree*) fin->Get("t");
    if (!t) continue;
    HistMaker *r = new HistMaker(t);
    progress->SetEntryMax(t->GetEntries());
    for (Long64_t ievt = 0; ievt < t->GetEntries(); ++ievt) {
      r->GetEntry(ievt);
      progress->Print(ievt);
      double EventWeight = r->EventWeight[0];
      if (SampleType == "SingleMuon" || SampleType == "SingleElectron") EventWeight = 1.;
      EventWeight = 1.; // Due to negative event weight
      HistCol.back().Fill("SimpleWprime",r->SimpleWprime, EventWeight, r->RegionIdentifier[0]);
      HistCol.back().Fill("LeadingJetPt",r->Jet_pt[0], EventWeight, r->RegionIdentifier[0]);
    }
  }

  fout->Write();
  fout->Save();
  
}

