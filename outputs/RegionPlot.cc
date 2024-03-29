#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>

#include <iostream>
#include <vector>

void RegionPlot(int isy_ = 3, int itr_ = 1) {
  const vector<string> SampleYears{"2016apv","2016","2017","2018"};
  const vector<string> SampleTypes{"SingleElectron","SingleMuon", // 0,1
  "ttbar", // 2,
  "wjets_HT_70_100", "wjets_HT_100_200", "wjets_HT_200_400", "wjets_HT_400_600",
  // 3                  4                     5                    6
  "wjets_HT_600_800", "wjets_HT_800_1200", "wjets_HT_1200_2500", "wjets_HT_2500_inf",
  // 7                  8                     9                    10
  "single_antitop_tchan","single_antitop_tw","single_top_schan","single_top_tchan","single_top_tw",
  // 11                        12                  13                 14                 15
  "Private_FL_M500"};
  // 16
  const unsigned MCSampleStartIndex = 2;
  const vector<string> Triggers{"SE","SM"};

  vector<string> RunningTypes{"SingleMuon","ttbar","Private_FL_M500"};
  RunningTypes = SampleTypes;

  string SampleYear = SampleYears[isy_];
  string SampleTrigger = Triggers[itr_];

  // ==========================================================================
  //   Branch Leafs declearation
  float EventScaleFactor;

  vector<int> *nBJets, *nNBJets; // {loose, medium, tight}
  nBJets = new vector<int>;
  nNBJets = new vector<int>;

  vector<double>* WPrimeMassSimple;
  WPrimeMassSimple = new vector<double>;
  //   Branch Leafs declearation
  // ==========================================================================

  vector<TFile*> InFiles;
  vector<TH1F*> histograms; // Used to hold the pointers, so they won't be deleted out of scope
  vector< vector< vector<TH1F*> > > WPMass;
  vector< vector<TH2F*> > JetMulti;

  string outname = "plots/" + SampleYear + "_" + SampleTrigger + ".root";
  TFile* fout = new TFile(outname.c_str(), "RECREATE");
  cout << "Creating plots for Year " << SampleYear << ", Trigger: " << SampleTrigger <<endl;

  for (unsigned i = 0; i < SampleTypes.size(); ++i) {
    string sn = SampleTypes[i];
    if (find(RunningTypes.begin(), RunningTypes.end(), SampleTypes[i]) == RunningTypes.end()) continue;
    if (i < MCSampleStartIndex && i != itr_) continue;

    string fn = SampleYear + "_" + SampleTypes[i] + "_" + SampleTrigger + ".root";
    TFile *f = TFile::Open(fn.c_str(),"READ");
    if (!f) continue;
    // TFile *f = new TFile(fn.c_str(),"READ"); // Alternative way to open a file
    // if (f->IsZombie()) continue; // And Test if it does exist.
    cout << "Reading from " <<fn.c_str() <<endl;
    InFiles.push_back(f);

    fout->cd();

    vector< vector<TH1F*> > wpmass;
    vector<TH2F*> jetmulti;

    TH1F* evtCounter = (TH1F*) f->Get("EventCounter")->Clone();
    evtCounter->SetName(TString(sn) + "_EventCounter");
    // double TotalEvents = evtCounter->GetBinContent(1);

    TTree* t = (TTree*) f->Get("t");

    t->SetBranchAddress("EventScaleFactor",&EventScaleFactor);
    t->SetBranchAddress("nBJets",&nBJets);
    t->SetBranchAddress("nNBJets",&nNBJets);
    t->SetBranchAddress("WPrimeMassSimple",&WPrimeMassSimple);

    for (unsigned bwp = 0; bwp < 3; ++bwp) {
      // wpmass[bwp].resize(3);
      vector<TH1F*> wpmasswp;
      TString plotname = Form("%s_%iwp_JetMultiplicity", sn.c_str(), bwp);
      TString plottitle= plotname + "; Number of Light Jets; Number of b jets";
      jetmulti.push_back(new TH2F(plotname, plottitle, 10,-0.5,9.5,10,-0.5,9.5));
      for (unsigned nb = 0; nb < 3; ++nb) {
        TString plotname2 = Form("%s_%i_bjets_%iwp_WPrimeMass", sn.c_str(), nb, bwp);
        TString plottitle2 = plotname2 + "; M_{W'}; Number of Entries";
        wpmasswp.push_back(new TH1F(plotname2, plottitle2, 1000,0,1000));
      }
      wpmass.push_back(wpmasswp);
    }

    for (Long64_t ie = 0; ie < t->GetEntries(); ++ie) {
      t->GetEntry(ie);
      double sf = EventScaleFactor;
      if (sf < 0) sf = 1.0;
      for (unsigned iwp = 0; iwp < 3; ++iwp) {
        jetmulti[iwp]->Fill(nNBJets->at(iwp), nBJets->at(iwp));
        for (unsigned inb = 0; inb < nBJets->at(iwp); ++inb) {
          unsigned inb_ = inb;
          if (inb_ > 2) inb_ = 2;
          wpmass[iwp][inb_]->Fill(WPrimeMassSimple->at(iwp), sf);
        }
      }
    }
    WPMass.push_back(wpmass);
    JetMulti.push_back(jetmulti);
  }
  fout->Write();
  fout->Save();

  cout << "Saved As " << outname <<endl;
  gSystem->Exit(0);
}
