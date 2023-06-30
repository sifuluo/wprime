#include "Utilities/TreeClass/PermTree.C"
#include "Utilities/DrawDataFormat.cc"
#include "../Utilities/ProgressBar.cc"

#include <math.h>

#include "TChain.h"

using namespace std;

class InterTree : public PermTree {
public:
  InterTree(TTree* t) : PermTree(t) {

  };

  void FillHistograms() {}
};

void MakeHistPerm(int isampleyear = 3) {
  string basepath = "/afs/cern.ch/work/s/siluo/wprime/outputs/";
  string itpath = "";
  string SampleYear = dlib.SampleYears[isampleyear];
  string HistFilePath = "outputs/";
  string HistFilePrefix = SampleYear + "_Perm";
  Histograms HistCol;
  vector<string> SampleTypes = dlib.DatasetNames;
  SampleTypes.clear();
  for (unsigned i = 0; i < 18; ++i) {
    SampleTypes.push_back(dlib.DatasetNames[i + 22]);
  }
  vector<string> Variations = {"central"};
  rm.InclusiveInit();
  HistCol.Regions = rm.StringRanges;
  

  HistCol.SetVariations(Variations);
  HistCol.SetSampleTypes(SampleTypes);
  HistCol.AddObservable("PtPerm",60,0,60);
  HistCol.AddObservable("bTagPerm",40,0,40);
  HistCol.CreateHistograms(HistFilePath, HistFilePrefix);
  Progress* progress = new Progress(0,10000);
  

  for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
    TChain* t = new TChain("t");
    string SampleType = SampleTypes[ist];
    if (dlib.Datasets[SampleType].Type != 2) continue;
    float NormFactor = dlib.GetNormFactor(SampleType, isampleyear);
    cout << endl << "Start processing " << SampleType << endl;
    string SamplePath = SampleYear + "_" + SampleType + "_-1.root";
    TString InFilePath = basepath + itpath + SamplePath;
    t->Add(InFilePath);
    InterTree *r = new InterTree(t);
    // progress->SetEntryMax(t->GetEntries());
    // Long64_t EntryMax = 2000000;
    Long64_t EntryMax = t->GetEntries();
    progress->SetEntryMax(EntryMax);
    int n_nan_weight = 0;
    for (Long64_t ievt = 0; ievt < t->GetEntries(); ++ievt) {
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
        if (RegionIdentifier%100/10 == 0) continue;
        if (iv < 9) RegionIdentifier = r->RegionIdentifier[iv];
    
        //Start of customize part
        int PtPerm = r->PtPerm;
        int bTagPerm = r->bTagPerm;

        HistCol.SetCurrentFill(ist, iv, RegionIdentifier, EventWeight);
        HistCol.Fill("PtPerm", PtPerm);
        HistCol.Fill("bTagPerm",bTagPerm);


      }
      // checkpoint(2);
    }
    cout << "In SampleType: " << SampleTypes[ist];
    cout << ", Number of events with nan weight = " << n_nan_weight << endl;
  }
  HistCol.PostProcess();
}