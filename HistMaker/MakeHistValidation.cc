#include "Utilities/TreeClass/ValidationTree.C"
#include "Utilities/DrawDataFormat.cc"
#include "../Utilities/ProgressBar.cc"
#include <math.h>

using namespace std;

class InterTree : public ValidationTree {
public:
  InterTree(TTree* t) : ValidationTree(t) {

  };

  void FillHistograms() {}
};

void MakeHistValidation(int isampleyear = 3) {
  string basepath = "/eos/user/s/siluo/WPrimeAnalysis/Validation/";
  string itpath = "";
  string SampleYear = dlib.SampleYears[isampleyear];
  string OutFilePath = "outputs/";
  string OutFilePrefix = SampleYear + "_Validation";
  Histograms HistCol;
  vector<string> SampleTypes = dlib.DatasetNames;
  // SampleTypes = {"FL500"};

  HistCol.SetSampleTypes(SampleTypes);
  HistCol.AddObservable("LeptonPt",100,0,500);
  HistCol.AddObservable("LeptonEta",60,0,6.0);
  HistCol.AddObservable("LeadingJetPt",200,0,2000);
  HistCol.AddObservable("LeadingJetEta",60,0,6.0);
  HistCol.AddObservable("METPt",200,0,2000);
  HistCol.AddObservable("METPhi",64,-3.2,3.2);
  HistCol.AddObservable("mT",200,0,2000);
  HistCol.AddObservable("WPrimeMassSimpleFL",200,0,2000);
  HistCol.AddObservable("WPrimeMassSimpleLL",200,0,2000);
  HistCol.CreateHistograms(OutFilePath, OutFilePrefix);
  Progress* progress = new Progress(0,10000);
  
  for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
    string SampleType = SampleTypes[ist];
    if (SampleType == "ZZ") continue;
    float NormFactor = dlib.GetNormFactor(SampleType, isampleyear);
    cout << endl << "Start processing " << SampleType << endl;
    string SamplePath = SampleYear + "_" + SampleType + ".root";
    TString InFilePath = basepath + itpath + SamplePath;
    cout << "File Path: " << InFilePath << endl;
    TFile *fin = new TFile(InFilePath,"READ");
    if (fin->IsZombie()) {
      cout << InFilePath << " is broken. Proceeding to next file" <<endl;
      continue;
    } 
    TTree* t = (TTree*) fin->Get("t");
    if (!t) {
      cout << InFilePath << " tree is broken. Proceeding to next file" << endl;
      continue;
    }
    InterTree *r = new InterTree(t);
    progress->SetEntryMax(t->GetEntries());
    int n_nan_weight = 0;
    for (Long64_t ievt = 0; ievt < t->GetEntries(); ++ievt) {
      // if (ievt > 1000) continue;
      r->GetEntry(ievt);
      progress->Print(ievt);

      bool hasnan = false;
      for (unsigned iv = 9; iv < HistCol.Variations.size(); ++iv) {
        if (r->EventWeight[iv-8] != r->EventWeight[iv-8]) hasnan = true;
      }
      if (hasnan) {
        n_nan_weight++;
        continue;
      }

      for (unsigned iv = 0; iv < HistCol.Variations.size(); ++iv) {
        float EventWeight = r->EventWeight[0];
        if (iv > 8) EventWeight = r->EventWeight[iv-8];
        EventWeight *= NormFactor;
        int RegionIdentifier = r->RegionIdentifier[0];
        if (iv < 9) RegionIdentifier = r->RegionIdentifier[iv];
    
        //Start of customize part
        float LeptonPt = r->LeptonPt;
        float LeptonEta = r->LeptonEta;
        float LeadingJetPt = r->JetPt->at(0);
        float LeadingJetEta = r->JetEta->at(0);
        float METPt = r->METPt;
        float METPhi = r->METPhi;
        float mT = r->mT->at(0);
        float WPrimeMassSimpleFL = r->WPrimeMassSimpleFL->at(0);
        float WPrimeMassSimpleLL = r->WPrimeMassSimpleLL->at(0);
        if (iv > 0 && iv < 9) { // Variations on Physical quantities
          // "EleScaleUp", "EleScaleDown", "EleResUp", "EleResDown", "JESup", "JESdown", "JERup", "JERdown"
          if (iv == 1) LeptonPt = r->LeptonPt_SU;
          if (iv == 2) LeptonPt = r->LeptonPt_SD;
          if (iv == 3) LeptonPt = r->LeptonPt_RU;
          if (iv == 4) LeptonPt = r->LeptonPt_RD;
          if (iv == 5) LeadingJetPt = r->JetPt_SU->at(0);
          if (iv == 6) LeadingJetPt = r->JetPt_SD->at(0);
          if (iv == 7) LeadingJetPt = r->JetPt_RU->at(0);
          if (iv == 8) LeadingJetPt = r->JetPt_RD->at(0);
          mT = r->mT->at(iv);
          WPrimeMassSimpleFL = r->WPrimeMassSimpleFL->at(iv);
          WPrimeMassSimpleLL = r->WPrimeMassSimpleLL->at(iv);
        }

        HistCol.SetCurrentFill(ist, iv, RegionIdentifier, EventWeight);
        HistCol.Fill("LeptonPt", LeptonPt);
        HistCol.Fill("LeptonEta",LeptonEta);
        HistCol.Fill("LeadingJetPt",LeadingJetPt);
        HistCol.Fill("LeadingJetEta",LeadingJetEta);
        HistCol.Fill("METPt",METPt);
        HistCol.Fill("METPhi",METPhi);
        HistCol.Fill("mT",mT);
        HistCol.Fill("WPrimeMassSimpleFL",WPrimeMassSimpleFL);
        HistCol.Fill("WPrimeMassSimpleLL",WPrimeMassSimpleLL);

      }
    }
    cout << "In SampleType: " << SampleTypes[ist];
    cout << ", Number of events with nan weight = " << n_nan_weight << endl;
  }
  HistCol.PostProcess();
}