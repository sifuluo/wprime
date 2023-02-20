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
  fout->cd();
  Histograms HistCol;
  HistCol.AddObservable("SimpleWprime",100,0,1000);
  HistCol.AddObservable("LeadingJetPt",100,0,1000);
  HistCol.CreateHistograms();

  Progress* progress = new Progress(0,10000);

  for (unsigned ist = 0; ist < dlib.DatasetNames.size(); ++ist) {
    string SampleType = dlib.DatasetNames[ist];
    double NormFactor = dlib.GetNormFactor(SampleType, isampleyear);
    cout << endl << "Start processing " << SampleType << endl;
    string SamplePath = SampleYear + "_" + SampleType + ".root";
    TString InFilePath = basepath + itpath + SamplePath;
    cout << "File Path: " << InFilePath << endl;
    TFile *fin = new TFile(InFilePath,"READ");
    TTree* t = (TTree*) fin->Get("t");
    if (!t) continue;
    HistMaker *r = new HistMaker(t);
    fout->cd();
    progress->SetEntryMax(t->GetEntries());
    for (Long64_t ievt = 0; ievt < t->GetEntries(); ++ievt) {
      r->GetEntry(ievt);
      progress->Print(ievt);
      for (unsigned iv = 0; iv < rm.Variations.size(); ++iv) {
        double EventWeight = r->EventWeight[0];
        double SimpleWprime = r->SimpleWprime;
        double LeadingJetPt = r->Jet_pt[0];
        int RegionIdentifier = r->RegionIdentifier[0];
        if (rm.Variations[iv] == "SFup") EventWeight = r->EventWeight[1];
        else if (rm.Variations[iv] == "SFdown") EventWeight = r->EventWeight[2];
        else {
          // Variations that affects physical quantities and RegionID
          RegionIdentifier = r->RegionIdentifier[iv];
          // the Quantities Variations are not implemented. Use nominal now.
          SimpleWprime = r->SimpleWprime;
          LeadingJetPt = r->Jet_pt[0];
        }
        
        if (SampleType == "SingleMuon" || SampleType == "SingleElectron") EventWeight = 1.;
        EventWeight = 1.; // Due to negative event weight
        EventWeight *= NormFactor;
        
        HistCol.Fill(ist, iv, RegionIdentifier, "SimpleWprime", SimpleWprime, EventWeight);
        HistCol.Fill(ist, iv, RegionIdentifier, "LeadingJetPt", LeadingJetPt, EventWeight);
      }
    }
  }

  fout->Write();
  fout->Save();
}

// void MakeHist(int isampleyear = 3) {
//   // MakeHistOneWP(isampleyear, 0, 0);
//   for (unsigned iPUWP = 0; iPUWP < 3; ++iPUWP) {
//     for (unsigned ibWP = 0;  ibWP < 3; ++ibWP) {
//       MakeHistOneWP(isampleyear, iPUWP, ibWP);
//     }
//   }
// }
