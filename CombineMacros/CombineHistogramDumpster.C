#define CombineHistogramDumpster_cxx
#include "CombineHistogramDumpster.h"
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>

void CombineHistogramDumpster::Loop()
{
//   In a ROOT session, you can do:
//      root> .L CombineHistogramDumpster.cc.C
//      root> CombineHistogramDumpster.cc t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch

  //define bin for analysis
  TString binS = TString::Format("Wprime%d", bin);
  TString gn = dset.GroupName;
  //define variations
  vector<TH1F*> WPrimeMass_FL;
  //first variations are all weight variations and map 1:1, region variations start at index 21
  vector<TString> variations = {"" // 0
  , "electronScaleUp", "electronScaleDown", "electronResUp", "electronResDown", "JESUp", "JESDown", "JERUp", "JERDown" // 1 - 8
  , "electronUp", "electronDown", "muonTriggerUp", "muonTriggerDown", "muonIdUp", "muonIdDown", "muonIsoUp", "muonIsoDown" // 9 - 16
  , "BjetTagCorrUp", "BjetTagCorrDown", "BjetTagUncorrUp", "BjetTagUncorrDown", "PUIDUp", "PUIDDown", "L1PreFiringUp", "L1PreFiringDown" // 17 - 24
  , "PUreweightUp", "PUreweightDown", "PDFUp", "PDFDown", "LHEScaleUp", "LHEScaleDown", // 25 - 30

  };
  for (unsigned i = 1; i < variations.size(); ++i) {
    variations[i] = gn + "_" + binS + "_" + variations[i];
  }

  // vector<TString> variations = {
  //   gn + "_" + binS,                             // 0
  //   gn + "_" + binS + "_" + "electronUp",        // 1
  //   gn + "_" + binS + "_" + "electronDown",      // 2
	// 	gn + "_" + binS + "_" + "muonTriggerUp",     // 3
	// 	gn + "_" + binS + "_" + "muonTriggerDown",   // 4
	// 	gn + "_" + binS + "_" + "muonIdUp",          // 5
	// 	gn + "_" + binS + "_" + "muonIdDown",        // 6
	// 	gn + "_" + binS + "_" + "muonIsoUp",         // 7
	// 	gn + "_" + binS + "_" + "muonIsoDown",       // 8
	// 	gn + "_" + binS + "_" + "BjetTagCorrUp",     // 9
	// 	gn + "_" + binS + "_" + "BjetTagCorrDown",   // 10
  //   gn + "_" + binS + "_" + "BjetTagUncorrUp",   // 11
	// 	gn + "_" + binS + "_" + "BjetTagUncorrDown", // 12
	// 	gn + "_" + binS + "_" + "PUIDUp",            // 13
	// 	gn + "_" + binS + "_" + "PUIDDown",          // 14
	// 	gn + "_" + binS + "_" + "L1PreFiringUp",     // 15
	// 	gn + "_" + binS + "_" + "L1PreFiringDown",
	// 	gn + "_" + binS + "_" + "PUreweightUp",
	// 	gn + "_" + binS + "_" + "PUreweightDown",
	// 	gn + "_" + binS + "_" + "PDFUp",
	// 	gn + "_" + binS + "_" + "PDFDown",
	// 	gn + "_" + binS + "_" + "LHEScaleUp",
	// 	gn + "_" + binS + "_" + "LHEScaleDown",
	// 	gn + "_" + binS + "_" + "electronScaleUp",
	// 	gn + "_" + binS + "_" + "electronScaleDown",
	// 	gn + "_" + binS + "_" + "electronResUp",
	// 	gn + "_" + binS + "_" + "electronResDown",
	// 	gn + "_" + binS + "_" + "JESUp",
	// 	gn + "_" + binS + "_" + "JESDown",
	// 	gn + "_" + binS + "_" + "JERUp",
	// 	gn + "_" + binS + "_" + "JERDown"
  // };
  for(unsigned i = 0; i < variations.size(); ++i) WPrimeMass_FL.push_back(new TH1F(variations[i],"W' mass, simplified, FL case; m_{W'} [GeV/c^{2}]; Events", 400, 0., 2000.));

  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntriesFast();

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    //set sample weight
    int Year = 0;
    float Lumi = 0.;
    if(YearType == "2016_APV")  {Year = 0; Lumi = 0.;}
    else if(YearType == "2016") {Year = 1; Lumi = 41.58;}
    else if(YearType == "2017") {Year = 2; Lumi = 49.81;}
    else if(YearType == "2018") {Year = 3; Lumi = 67.86;}
    float SampleWeight = 1.;
    if(dset.Type != 0) SampleWeight = Lumi * dset.CrossSection / dset.Size[Year];
    //variations of selections
    for(unsigned i = 0; i < 9; ++ i){
      if(RegionIdentifier[i] != bin) continue;
      string HistName;
      WPrimeMass_FL[i]->Fill(WPrimeMassSimpleFL->at(i),EventWeight[0]*SampleWeight);
    }
    //Muon type variations, for a test
    if(RegionIdentifier[0] == bin){
      //EventWeight variations
      for(unsigned i = 9; i < variations.size(); ++i){
        string HistName;
        WPrimeMass_FL[i]->Fill(WPrimeMassSimpleFL->at(0),EventWeight[i]*SampleWeight);
      }
    }
    
  }
  //save all the W' variation histograms into a file
  TFile *savefile;
  //if(!Iterator) savefile = new TFile("TestHistograms/SimpleShapes.root","RECREATE");
  //else savefile = new TFile("TestHistograms/SimpleShapes.root","WRITE");
  savefile = new TFile(TString::Format("TestHistograms/SimpleShapes%d.root",Iterator),"RECREATE");
  savefile->cd();
  for(unsigned i = 0; i < WPrimeMass_FL.size(); ++i){
    if(dset.Type == 0){
      if(i>0) continue;
      if(Iterator == 1) WPrimeMass_FL[i]->Write("data_obs_" + binS);
      else continue;
    }
    else WPrimeMass_FL[i]->Write(WPrimeMass_FL[i]->GetName());
  }
  savefile->Close();
}
