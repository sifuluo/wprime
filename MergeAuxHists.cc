#include <vector>
#include <string>
#include <algorithm>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

#include "Utilities/Configs.cc"

void MergeAuxHists(int isampleyear = 3) {
  // Eta and pT bins for Jet response 
  const vector<double> etabins{0., 1.3, 2.5, 3.0, 5.2}; // size 5, 4 bins, ieta top at 3;
  const vector<vector<double> > ptbins{
    {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,130., 150.,180.,220., 260., 300.,350.,400.,500.,1000.,10000.}, // 23 bins, 24 numbers
    {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150., 180.,220.,260., 300.,10000.}, // 18 bins
    {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150., 180.,220.,260.,10000.}, // 17 bins
    {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150.,10000.} //14 bins
  };

  Configs* conf = new Configs(isampleyear, 2, 0);
  
  vector<TFile*> JSInFiles;
  vector<double> JSNorms;
  for (unsigned ist = 0; ist < dlib.DatasetNames.size(); ++ist) {
    if (ist < 2) continue; // Data not considered
    TString JSInFileName = conf->AuxHistBasePath + "Scale_" + conf->SampleYear + "_" + dlib.DatasetNames[ist] + ".root";
    TFile *infile = new TFile(JSInFileName, "READ");
    if (infile->IsZombie() || !infile || infile->GetListOfKeys()->IsEmpty()) continue;
    JSInFiles.push_back(new TFile(JSInFileName, "READ"));
    JSNorms.push_back(dlib.GetNormFactor(dlib.DatasetNames[ist],isampleyear));
    cout << "Loading " << JSInFileName << endl;
  }
  TString JSOutFileName = conf->AuxHistBasePath + "Scale_" + conf->SampleYear + "_Merged.root";
  TFile *fJS = new TFile(JSOutFileName,"RECREATE");
  cout << "Output to " << JSOutFileName << endl;
  fJS->cd();
  vector<vector<TH1F*> > JSHists;
  vector<vector<TH1F*> > JSHists_mu;
  vector<vector<TH1F*> > JSHists_sig;
  JSHists.resize(etabins.size());
  JSHists_mu.resize(etabins.size());
  JSHists_sig.resize(etabins.size());
  for (unsigned ieta = 0; ieta < etabins.size() - 1; ++ieta) {
    // vector<TH1F*> jeseta;
    JSHists[ieta].resize(ptbins[ieta].size());
    JSHists_mu[ieta].resize(ptbins[ieta].size());
    JSHists_sig[ieta].resize(ptbins[ieta].size());
    for (unsigned ipt = 0; ipt < ptbins[ieta].size() - 1; ++ipt) {
      TString JSsn = Form("eta%d_pt%d", ieta, ipt);
      JSHists_mu[ieta][ipt] = new TH1F(JSsn + "_mu", JSsn + "_mu", 20, 0.5,1.5);
      JSHists_sig[ieta][ipt] = new TH1F(JSsn + "_sig", JSsn + "sig", 20, 0., 1.);
      for (unsigned ist = 0; ist < JSInFiles.size(); ++ist) {
        TH1F* h1 = (TH1F*) JSInFiles[ist]->Get(JSsn);
        h1->Scale(JSNorms[ist]);
        if (ist == 0) {
          JSHists[ieta][ipt] = (TH1F*) h1;
          h1->SetDirectory(fJS);
          // Can also just clone h1 as: JSHists[ieta][ipt] = (TH1F*) h1->Clone(). And Omit SetDirectory()
        }
        else {
          JSHists[ieta][ipt]->Add(h1);
        }
        JSHists_mu[ieta][ipt]->Fill(h1->GetMean(), h1->GetMaximum());
        JSHists_sig[ieta][ipt]->Fill(h1->GetStdDev(), h1->GetMaximum());
      }
    }
  }
  fJS->Write();
  fJS->Save();
  
  vector<TFile*> bTEInFiles;
  vector<double> bTENorms;
  for (unsigned ist = 0; ist < dlib.DatasetNames.size(); ++ist) {
    if (ist < 2) continue; // Data not considered
    TString bTEInFileName = conf->AuxHistBasePath + "bTagEff_" + conf->SampleYear + "_" + dlib.DatasetNames[ist] + ".root";
    TFile *infile = new TFile(bTEInFileName, "READ");
    if (infile->IsZombie() || !infile || infile->GetListOfKeys()->IsEmpty()) continue;
    bTEInFiles.push_back(new TFile(bTEInFileName, "READ"));
    bTENorms.push_back(dlib.GetNormFactor(dlib.DatasetNames[ist],isampleyear));
    cout << "Loading " << bTEInFileName << endl;
  }
  TString bTEOutFileName = conf->AuxHistBasePath + "bTagEff_" + conf->SampleYear + "_Merged.root";
  TFile *fbTE = new TFile(bTEOutFileName, "RECREATE");
  cout << "Output to " << bTEOutFileName << endl;
  fbTE->cd();
  vector<TString> bTEsn = {"BtagEff_L","BtagEff_M", "BtagEff_T","BtagPass_L","BtagPass_M", "BtagPass_T","TotalEvts"};
  vector<TH2F*> bTEHists;
  bTEHists.resize(bTEsn.size());
  for (unsigned ist = 0; ist < bTEInFiles.size(); ++ist) {
    vector<TH2F*> tmphists;
    tmphists.resize(bTEsn.size());
    for (unsigned ih = 3; ih < 7; ++ih) {
      TH2F* h1 = (TH2F*) bTEInFiles[ist]->Get(bTEsn[ih]);
      h1->Scale(bTENorms[ist]);
      tmphists[ih] = h1;
    }
    if (ist == 0) {
      bTEHists = tmphists;
      for (unsigned ih = 4; ih < 7; ++ih) bTEHists[ih]->SetDirectory(fbTE);
    }
    else {
      for (unsigned ih = 0; ih < 4; ++ih) bTEHists[ih]->Add((TH2F*) bTEInFiles[ist]->Get(bTEsn[ih]));
    }
  }
  for (unsigned ih = 0; ih < 3; ++ih) bTEHists[ih]->Divide(bTEHists[3]);

  fbTE->Write();
  fbTE->Save();




}