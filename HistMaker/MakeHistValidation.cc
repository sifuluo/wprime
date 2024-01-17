#include "Utilities/TreeClass/ValidationTree.C"
#include "Utilities/DrawDataFormat.cc"
#include "Utilities/MCReweight.cc"
#include "../Utilities/ProgressBar.cc"
#include "../Utilities/ErrorLogDetector.cc"
// #include "../Utilities/Configs.cc" // For checkpoints

#include <math.h>

#include "TChain.h"
#include "TFile.h"

using namespace std;

class InterTree : public ValidationTree {
public:
  InterTree(TTree* t) : ValidationTree(t) {

  };

  void FillHistograms() {}
};

int MakeHistValidation(int isampleyear = 3, int isampletype = 0, int ifile = -1, bool DoMCReweight = false, bool DrawMCReweight = false) {
  if (isampletype != 2 && DoMCReweight) return 0;
  if (ErrorLogDetected(isampleyear, isampletype, ifile) == 0) return 0;
  rm.TightOnlyInit();
  string basepath = "/eos/user/s/siluo/WPrimeAnalysis/Validation/";
  string itpath = "";
  string SampleYear = dlib.SampleYears[isampleyear];
  // string HistFilePath = "outputs/";
  string HistFilePath = basepath;
  string HistFilePrefix = SampleYear + "_Validation";
  Histograms HistCol;
  vector<string> SampleTypes = dlib.DatasetNames;
  string SampleType = SampleTypes[isampletype];
  HistFilePath = HistFilePath + "Hists/";
  if (ifile > -1) HistFilePath = HistFilePath + SampleYear + "_" + SampleType + "/";
  if (isampletype != -1) {
    
  }
  if (DoMCReweight) HistFilePrefix += "_RW";
  else if (SampleType == "ttbar") HistFilePrefix += "_NRW";
  if (SampleType == "ZZ") return 0;

  string MCRWVar = "ST";
  double MCRWVal = 0.; // Need to be assigned later
  MCReweightManager *mcrm = new MCReweightManager(MCRWVar); // MCReweight derive variable
  mcrm->Verbose = false;
  if ((DoMCReweight && (SampleType == "ttbar" || SampleType == "")) || DrawMCReweight) {
    mcrm->Init();
    string SourcePath = basepath + "Hists/";
    string SourcePrefix = SampleYear + "_Validation";
    // TString rwfn = StandardNames::HistFileName(basepath + "Hists/", HistFilePrefix, "ReweightSF");
    if (DrawMCReweight || !mcrm->ReadFromFile(SourcePath, SourcePrefix)) {
      mcrm->ReadFromFiles(SourcePath, SourcePrefix);
      mcrm->SaveToFile(SourcePath, SourcePrefix);
    }
  }

  if (DrawMCReweight) return 0;

  HistCol.SetSampleTypes(SampleTypes);
  HistCol.AddObservable("LeptonPt",50,0,500);
  HistCol.AddObservable("LeptonEta",90,-4.5,4.5);
  HistCol.AddObservable("LeptonPhi",90,-4.5,4.5);
  for (unsigned ij = 0; ij < 5; ++ij) {
    HistCol.AddObservable(Form("Jet%iPt",ij),100,0,1000);
    HistCol.AddObservable(Form("Jet%iEta",ij),90,-4.5,4.5);
    HistCol.AddObservable(Form("Jet%iPhi",ij),90,-4.5,4.5);
    for (unsigned ij2 = ij + 1; ij2 < 5; ++ij2) {
      string ob = Form("dR(Jet%i,Jet%i)",ij, ij2);
      HistCol.AddObservable(ob, 45, 0, 45);
    }
  }
  HistCol.AddObservable("METPt",500,0,10000);
  HistCol.AddObservable("METPhi",64,-3.2,3.2);
  HistCol.AddObservable("dPhiMetLep",90,-4.5,4.5);
  HistCol.AddObservable("mT",500,0,10000);
  HistCol.AddObservable("HT",500,0,10000); 
  HistCol.AddObservable("ST",500,0,10000);
  HistCol.AddObservable("WPrimeMassSimpleFL",500,0,10000);
  HistCol.AddObservable("WPrimeMassSimpleLL",500,0,10000);
  // HistCol.AddObservable("WPrimeMass",100,0,2000);
  // HistCol.AddObservable("WPrimeMassFL",100,0,2000);
  // HistCol.AddObservable("WPrimeMassLL",100,0,2000);
  // HistCol.AddObservable("Likelihood",100,-10,0);
  HistCol.CreateHistograms(HistFilePath, HistFilePrefix, SampleType, ifile);
  Progress* progress = new Progress(0,10000);


  TChain* t = new TChain("t");
  float NormFactor = dlib.GetNormFactor(SampleType, isampleyear);
  cout << endl << "Start processing " << SampleType << endl;
  string SamplePath = SampleYear + "_" + SampleType + "/" + SampleYear + "_" + SampleType;
  if (ifile != -1) SamplePath += Form("_%i.root",ifile);
  else SamplePath += "*.root";
  TString InFilePath = basepath + itpath + SamplePath;
  cout << "The InputFile path is " << InFilePath << endl;
  t->Add(InFilePath);
  InterTree *r = new InterTree(t);
  Long64_t EntryMax = t->GetEntries();
  // Long64_t EntryMax = t->GetEntriesFast();
  // Long64_t EntryMax = 2000000;
  progress->SetEntryMax(EntryMax);
  int n_nan_weight = 0;
  for (Long64_t ievt = 0; ievt < EntryMax; ++ievt) {
    r->GetEntry(ievt);
    progress->Print(ievt);
    // checkpoint(0);

    bool hasnan = false;
    for (unsigned iv = 9; iv < HistCol.Variations.size(); ++iv) {
      if (r->EventWeight[iv-8] != r->EventWeight[iv-8]) hasnan = true;
    }
    if (hasnan) {
      n_nan_weight++;
      continue;
    }
    // checkpoint(1);

    for (unsigned iv = 0; iv < HistCol.Variations.size(); ++iv) {
      float EventWeight = r->EventWeight[0];
      if (iv > 8) EventWeight = r->EventWeight[iv-8];
      EventWeight *= NormFactor;
      int RegionIdentifier = r->RegionIdentifier[0];
      if (RegionIdentifier / 10 % 10 == 0) continue;
      if (iv < 9) RegionIdentifier = r->RegionIdentifier[iv];
  
      //Start of customize part
      float LeptonPt = r->LeptonPt;
      float LeptonEta = r->LeptonEta;
      float LeptonPhi = r->LeptonPhi;
      vector<TLorentzVector> Jets;
      Jets.clear();
      for (unsigned ij = 0; ij < 5; ++ij) {
        TLorentzVector j;
        j.SetPtEtaPhiM(r->JetPt->at(ij), r->JetEta->at(ij), r->JetPhi->at(ij), 0);
        Jets.push_back(j);
      }
      float METPt = r->METPt;
      float METPhi = r->METPhi;
      float dPhiMetLep = r->dPhiMetLep;
      float mT = r->mT->at(0);
      float HT = 0;
      for (unsigned ij = 0; ij < r->JetPt->size(); ++ij) {
        HT += r->JetPt->at(ij);
      }
      float ST = HT;
      ST += LeptonPt;
      ST += METPt;

      float WPrimeMassSimpleFL = r->WPrimeMassSimpleFL->at(0);
      float WPrimeMassSimpleLL = r->WPrimeMassSimpleLL->at(0);
      // float WPrimeMass = r->WPrimeMass->at(0);
      // float Likelihood = r->Likelihood->at(0);
      // int   WPType = r->WPType->at(0);
      if (iv > 0 && iv < 9) { // Variations on Physical quantities
        // "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown"
        if (iv == 1) LeptonPt = r->LeptonPt_SU;
        else if (iv == 2) LeptonPt = r->LeptonPt_SD;
        else if (iv == 3) LeptonPt = r->LeptonPt_RU;
        else if (iv == 4) LeptonPt = r->LeptonPt_RD;
        else {
          for (unsigned ij = 0; ij < 5; ++ij) {
            float TarPt = 0;
            if (iv == 5) TarPt = r->JetPt_SU->at(ij);
            if (iv == 6) TarPt = r->JetPt_SD->at(ij);
            if (iv == 7) TarPt = r->JetPt_RU->at(ij);
            if (iv == 8) TarPt = r->JetPt_RD->at(ij);
            Jets[ij] = Jets[ij] * (TarPt / Jets[ij].Pt());
          }
        }
        mT = r->mT->at(iv);
        WPrimeMassSimpleFL = r->WPrimeMassSimpleFL->at(iv);
        WPrimeMassSimpleLL = r->WPrimeMassSimpleLL->at(iv);
      }
      if (SampleType == "ttbar" && DoMCReweight) {
        MCRWVal = ST;
        float mcrweight = mcrm->GetSF1DF(MCRWVal, RegionIdentifier);
        EventWeight *= mcrweight;
        if ((mcrweight > 3.0 || mcrweight < 0.3)) {
          cout << "Extreme reweight = " << mcrweight << ", at var = " << ST <<endl;
        }
      }
      HistCol.SetCurrentFill(isampletype, iv, RegionIdentifier, EventWeight);
      HistCol.Fill("LeptonPt", LeptonPt);
      HistCol.Fill("LeptonEta",LeptonEta);
      HistCol.Fill("LeptonPhi",LeptonPhi);
      for (unsigned ij = 0; ij < 5; ++ij) {
        HistCol.Fill(Form("Jet%iPt", ij), Jets[ij].Pt());
        HistCol.Fill(Form("Jet%iEta", ij), Jets[ij].Eta());
        HistCol.Fill(Form("Jet%iPhi", ij), Jets[ij].Phi());
        for (unsigned ij2 = ij + 1; ij2 < 5; ++ij2) {
          string ob = Form("dR(Jet%i,Jet%i)",ij, ij2);
          HistCol.Fill(ob, Jets[ij].DeltaR(Jets[ij2]));
        }
      }
      HistCol.Fill("METPt",METPt);
      HistCol.Fill("METPhi",METPhi);
      HistCol.Fill("dPhiMetLep",dPhiMetLep);
      HistCol.Fill("mT",mT);
      HistCol.Fill("HT",HT);
      HistCol.Fill("ST",ST);
      HistCol.Fill("WPrimeMassSimpleFL",WPrimeMassSimpleFL);
      HistCol.Fill("WPrimeMassSimpleLL",WPrimeMassSimpleLL);
      // if (Likelihood > 0) {
      //   HistCol.Fill("WPrimeMass",WPrimeMass);
      //   if (WPType == 0) HistCol.Fill("WPrimeMassFL", WPrimeMass);
      //   else if (WPType == 1) HistCol.Fill("WPrimeMassLL", WPrimeMass);
      //   else cout << "Wrong WPType read : " << WPType << endl;
      //   HistCol.Fill("Likelihood", log10(Likelihood));
      // }
    }
    // checkpoint(2);
  }
  cout << "In SampleType: " << SampleTypes[isampletype];
  cout << ", Number of events with nan weight = " << n_nan_weight << endl;

  HistCol.PostProcess();
  return ErrorLogDetected(isampleyear, isampletype, ifile);
  // cout << "Auto exiting" << endl;
  // gApplication->Terminate();
}