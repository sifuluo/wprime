#include "Utilities/wpm.C"
#include "Utilities/RatioPlot.cc"
#include "Utilities/DrawDataFormat.cc"
#include "../Utilities/ProgressBar.cc"
#include <math.h>

using namespace std;

class HistMaker : public wpm {
public:
  HistMaker(TTree* t) : wpm(t) {

  };

  void FillHistograms() {}
};

void WprimeMass(int isampleyear = 3) {
  string basepath = "../outputs/";
  string itpath = "";
  string SampleYear = dlib.SampleYears[isampleyear];
  TString OutFilePath = "outputs/" + SampleYear + "_WPMasses.root";
  TFile* fout = new TFile(OutFilePath,"RECREATE");
  fout->cd();
  Histograms HistCol;
  vector<string> SampleTypes = {"FL500"};

  HistCol.SetSampleTypes(SampleTypes);
  HistCol.AddObservable("SimpleWprimeFL",200,0,2000);
  HistCol.AddObservable("SimpleWprimeLL",200,0,2000);
  HistCol.AddObservable("WprimeFL",200,0,2000);
  HistCol.AddObservable("WprimeLL",200,0,2000);
  HistCol.AddObservable("WprimeFLPicked",200,0,2000);
  HistCol.AddObservable("WprimeLLPicked",200,0,2000);
  HistCol.AddObservable("Likelihood",200,-19,1);
  HistCol.CreateHistograms();
  Progress* progress = new Progress(0,10000);
  
  for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
    string SampleType = SampleTypes[ist];
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
    int n_nan_weight = 0;
    int n_nan_val = 0;
    for (Long64_t ievt = 0; ievt < t->GetEntries(); ++ievt) {
      // if (ievt > 1000) continue;
      r->GetEntry(ievt);
      progress->Print(ievt);

      bool hasnan = false;
      for (unsigned iv = 9; iv < rm.Variations.size(); ++iv) {
        if (r->EventWeight[iv-8] != r->EventWeight[iv-8]) hasnan = true;
      }
      // if (hasnan) {
      //   cout << "In event " << ievt << " nominal " << r->EventWeight[0];
      //   for (unsigned iv = 9; iv < rm.Variations.size(); ++iv) {
      //     if( (iv-9) % 4 == 0 ) cout <<endl;
      //     cout << "    . " << iv << ", " << rm.Variations[iv]<< " " << r->EventWeight[iv-8];
      //   }
      //   cout << endl;
      // }
      if (hasnan) {
        n_nan_weight++;
        continue;
      }

      for (unsigned iv = 0; iv < rm.Variations.size(); ++iv) {
        // vector<string> Variations = {"central"
        // , "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown"
        // , "EleSFup", "EleSFdown", "MuonSFup", "MuonSFdown", "BjetTagSFup", "BjetTagSFdown",
        // , "PUIDSFup", "PUIDSFdown", "L1PreFiringSFup", "L1PreFiringSFdown", "PUreweightSFup","PUreweightSFdown"
        // };
        // Indices for each line: 0; 1-8; 9-14; 15-20;
        double EventWeight = r->EventWeight[0];
        if (iv > 8) EventWeight = r->EventWeight[iv-8];
        int RegionIdentifier = r->RegionIdentifier[0];
        if (iv < 9) RegionIdentifier = r->RegionIdentifier[iv];
    
        //Start of customize part

        // if (iv > 0 && iv < 9) {}; // No physical observable changes now.
        double SimpleWprimeFL = r->SimpleWprimeFL;
        double SimpleWprimeLL = r->SimpleWprimeLL;
        double WprimeFL = r->WprimeFL;
        double WprimeLL = r->WprimeLL;
        double WprimeType = r->WprimeType;

        HistCol.Fill(ist, iv, RegionIdentifier, "SimpleWprimeFL", r->SimpleWprimeFL, EventWeight);
        HistCol.Fill(ist, iv, RegionIdentifier, "SimpleWprimeLL", r->SimpleWprimeLL, EventWeight);
        HistCol.Fill(ist, iv, RegionIdentifier, "WprimeFL", r->WprimeFL, EventWeight);
        HistCol.Fill(ist, iv, RegionIdentifier, "WprimeLL", r->WprimeLL, EventWeight);
        HistCol.Fill(ist, iv, RegionIdentifier, "Likelihood", r->Likelihood, EventWeight);
        if (!WprimeType) HistCol.Fill(ist, iv, RegionIdentifier, "WprimeFLPicked", r->WprimeFL, EventWeight);
        else HistCol.Fill(ist, iv, RegionIdentifier, "WprimeLLPicked", r->WprimeLL, EventWeight);
        // if (EventWeight != EventWeight) {
        //   cout << "iv = " << iv << ", " << HistCol.Variations[iv] << " has nan EventWeight: ";
        //   cout << r->EventWeight[iv-8] <<endl;
        // }

      }
    }
    cout << "In SampleType: " << SampleTypes[ist];
    cout << ", Number of events with nan weight = " << n_nan_weight << endl;
  }
  fout->Write();
  fout->Save();
}