#ifndef HISTSFORCOMBINE_CC
#define HISTSFORCOMBINE_CC

#include "TString.h"
#include "TH1.h"
#include "TFile.h"

#include <vector>
#include <string>

#include "DrawDataFormat.cc"

class HistsForCombine{
public:
  HistsForCombine(string fn = "SimpleShapes", string r = "1153", string year = "2018") {
    Region = r;
    Year = year;
    Init(fn);
  };

  void Init(string fn = "SimpleShapes") {
    MakeVariations();
    TString outfilename = "CombinedFiles/" + fn + "_Wprime" + Region + ".root";
    cout << "Combine Histograms will be saved to " << outfilename << endl;
    OutFile = new TFile(outfilename, "RECREATE");
    HasData = false;
  }

  void AddHist(string gn, TH1F* h_, int iv) {
    if (h_ == nullptr) return;
    if (gn == "Data") {
      gn = "data_obs";
      HasData = true;
    }
    TString hn = gn + "_Wprime" + Region + "_" + Variations[iv];
    TH1F* h = (TH1F*) h_->Clone();
    h->SetDirectory(OutFile);
    h->SetName(hn);
    OutHists.push_back(h);
    // cout << "Added " << hn << endl;
  }

  void MakeDummyData() {
    if (OutHists.size() == 0) return;
    AddHist("data_obs", OutHists[0], 0);
    OutHists[OutHists.size() - 1]->Reset();
  }

  void Done() {
    OutFile->Write();
    OutFile->Save();
    OutFile->Close();
    // for (unsigned i = 0; i < OutHists.size(); ++i) {
    //   if (OutHists[i] != nullptr) delete OutHists[i];
    // }
  }

  string UpPost = "Up";
  string DownPost = "Down";
  void AddVariationSource(string sr) {
    Variations.push_back(sr + UpPost);
    Variations.push_back(sr + DownPost);
  }
  void MakeVariations() { // Stay coordinated with Regions.cc: class RegionManager
    Variations = {""};
    AddVariationSource("electronScale"); // 1 2
    AddVariationSource("electronRes"); // 3 4
    AddVariationSource("JES"); // 5 6
    AddVariationSource("JER"); // 7 8
    AddVariationSource("electron"); // 9 10
    AddVariationSource("muonTrigger"); // 11 12
    AddVariationSource("muonId"); // 13 14
    AddVariationSource("muonIso"); // 15 16
    AddVariationSource("BjetTagCorr"); // 17 18
    AddVariationSource("BjetTagUncorr2018"); // 19 20
    AddVariationSource("PUID"); // 21 22 
    AddVariationSource("L1PreFiring"); // 23 24
    AddVariationSource("PUreweight"); // 25 26
    AddVariationSource("PDF"); // 27 28
    AddVariationSource("LHEScale"); // 29 30
    AddVariationSource("RwStat" + Year + Region); // 31 32
    // AddVariationSource("RwStat"); // 31 32
    // Change the card accordingly too
  }

  TFile* OutFile;
  string Region;
  string Year;
  vector<TH1F*> OutHists; // Container to hold all histograms.
  // vector< vector<TH1F*> > OutHists; //[iGroup][iVariation]
  bool HasData;
  vector<string> Variations = {""};
};

#endif