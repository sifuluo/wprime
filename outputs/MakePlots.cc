#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TString.h>

#include <iostream>
#include <vector>

#include "../Utilities/DataFormat.cc"

void MakePlots(int isy_ = 3) {
  string SampleYear = Constants::SampleYears[isy_];
  string SampleTrigger = "SM";

  vector<string> SampleTypes = {"SingleMuon","ttbar"};

  RegionIDSelection *rsel = new RegionIDSelection(1050,1999);

  // ==========================================================================
  //   Branch Leafs declearation
  int RegionIdentifier[9];
  float EventWeight[18];
  float PUWeight[7];

  double nTrueInt;
  int nPVGood;
  //   Branch Leafs declearation
  // ==========================================================================

  vector<TFile*> InFiles;
  vector<TH1F*> histograms; // Used to hold the pointers, so they won't be deleted out of scope

  string outname = "plots/" + SampleYear + "_" + SampleTrigger + ".root";
  TFile* fout = new TFile(outname.c_str(), "RECREATE");
  cout << "Creating plots for Year " << SampleYear << ", Trigger: " << SampleTrigger <<endl;

  for (unsigned i = 0; i < SampleTypes.size(); ++i) {
    string fn = SampleYear + "_" + SampleTypes[i] + "_" + SampleTrigger + "_0.root";
    TFile *f = TFile::Open(fn.c_str(),"READ");
    if (!f) continue;
    // TFile *f = new TFile(fn.c_str(),"READ"); // Alternative way to open a file
    // if (f->IsZombie()) continue; // And Test if it does exist.
    cout << "Reading from " <<fn.c_str() <<endl;
    InFiles.push_back(f);
    TString sn = SampleTypes[i];
    fout->cd();
    vector<TString> weightnames = {"noweight","nomweight","weightup","weightdown","weight0","weight1","weight2","weight3"};
    vector<TH1F*> hnpvsgood;
    vector<TH1F*> hnTrueInt;

    for (unsigned j = 0; j < weightnames.size(); ++j) {
      TString hn = sn + "_" + weightnames[j];
      TString hn1 = hn + "_nPVGood";
      TString hn2 = hn + "_nTrueInt";
      hnpvsgood.push_back(new TH1F(hn1, hn1,99,0,99));
      hnTrueInt.push_back(new TH1F(hn2, hn2, 99,0,99));
      // histograms.push_back(hnpvsgood.back());
      // histograms.push_back(hnTrueInt.back());
      if (i == 0) break;
    }

    TTree* t = (TTree*) f->Get("t");
    t->SetBranchAddress("RegionIdentifier",&RegionIdentifier);
    t->SetBranchAddress("EventWeight",&EventWeight);
    t->SetBranchAddress("PUWeight",&PUWeight);
    t->SetBranchAddress("nTrueInt",&nTrueInt);
    t->SetBranchAddress("nPVGood",&nPVGood);


    for (Long64_t ie = 0; ie < t->GetEntries(); ++ie) {
      t->GetEntry(ie);
      if (ie % 10000 == 0) cout << Form("\r %lli/ %lli, (%.1f%%) in %s", ie, t->GetEntries(), double(ie) / double(t->GetEntries()) * 100., sn.Data()) << flush;
      if (ie == t->GetEntries() - 1) cout << endl << sn << " Done" <<endl;
      double sf = EventWeight[0] / PUWeight[1];
      for (unsigned iw = 0; iw < weightnames.size(); ++iw) {
        double sf_ = sf;
        if (iw > 0) sf_ = sf_* PUWeight[iw - 1];
        if (i == 0) sf_ = 1;
        hnpvsgood[iw]->Fill(nPVGood, sf_);
        hnTrueInt[iw]->Fill(nTrueInt, sf_);
        if (i == 0) break;
      }

    }
    // fout->Write();
  }
  fout->Write();
  fout->Save();

  cout << "Saved As " << outname <<endl;
  gSystem->Exit(0);
}
