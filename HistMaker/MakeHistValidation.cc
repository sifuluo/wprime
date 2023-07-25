#include "Utilities/TreeClass/ValidationTree.C"
#include "Utilities/DrawDataFormat.cc"
#include "Utilities/MCReweight.cc"
#include "../Utilities/ProgressBar.cc"
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

void MakeHistValidation(int isampleyear = 3,bool DoMCReweight = false, int isampletype = -1, int ifile = -1) {
  rm.TightOnlyInit();
  string basepath = "/eos/user/s/siluo/WPrimeAnalysis/Validation/";
  string itpath = "";
  string SampleYear = dlib.SampleYears[isampleyear];
  // string HistFilePath = "outputs/";
  string HistFilePath = basepath;
  string HistFilePrefix = SampleYear + "_Validation";
  Histograms HistCol;
  vector<string> SampleTypes = dlib.DatasetNames;
  string IterSampleType = "";
  if (isampletype != -1) {
    IterSampleType = SampleTypes[isampletype];
    HistFilePath = HistFilePath + "Hists/";
    if (ifile > -1) HistFilePath = HistFilePath + "batch/";
  }
  if (DoMCReweight) HistFilePrefix += "_RW";
  if (IterSampleType == "ZZ") return;
  bool DrawMCReweight = DoMCReweight && (isampletype == -1 || isampletype == 24) && ifile < 1;

  HistCol.SetSampleTypes(SampleTypes);
  HistCol.AddObservable("LeptonPt",50,0,500);
  HistCol.AddObservable("LeptonEta",90,-4.5,4.5);
  HistCol.AddObservable("LeadingJetPt",100,0,1000);
  HistCol.AddObservable("LeadingJetEta",90,-4.5,4.5);
  HistCol.AddObservable("METPt",100,0,2000);
  HistCol.AddObservable("METPhi",64,-3.2,3.2);
  HistCol.AddObservable("mT",100,0,2000);
  HistCol.AddObservable("HT",200,0,2000);
  HistCol.AddObservable("WPrimeMassSimpleFL",100,0,2000);
  HistCol.AddObservable("WPrimeMassSimpleLL",100,0,2000);
  HistCol.CreateHistograms(HistFilePath, HistFilePrefix, IterSampleType, ifile);
  Progress* progress = new Progress(0,10000);

  MCReweight *mcr1161 = new MCReweight("1161");
  MCReweight *mcr1151 = new MCReweight("1151");
  if ((DoMCReweight && (IterSampleType == "ttbar" || IterSampleType == "")) || DrawMCReweight) {
    string idr1161 = rm.StringRanges[rm.GetRangeIndex(1161)];
    string idr1151 = rm.StringRanges[rm.GetRangeIndex(1151)];
    string SourceObs = "WPrimeMassSimpleFL";
    string SourcePath = "outputs/";
    string SourcePrefix = SampleYear + "_Validation";
    TString SourceFileName = StandardNames::HistFileName(SourcePath, SourcePrefix, SourceObs);
    TFile *fsource = new TFile(SourceFileName);
    cout << "Source File Name is " << SourceFileName <<endl;
    for (unsigned i = 0; i < dlib.DatasetNames.size(); ++i) {
      string ds = dlib.DatasetNames[i];
      TString histname1161 = StandardNames::HistName(ds, SourceObs, idr1161, rm.Variations[0]);
      TString histname1151 = StandardNames::HistName(ds, SourceObs, idr1151, rm.Variations[0]);
      TH1F* h1161_ = (TH1F*) fsource->Get(histname1161);
      TH1F* h1151_ = (TH1F*) fsource->Get(histname1151);
      if (dlib.Datasets[ds].Type == 0) {
        mcr1161->AddData(h1161_);
        mcr1151->AddData(h1151_);
      }
      else if (ds == "ttbar") {
        mcr1161->Addttbar(h1161_);
        mcr1151->Addttbar(h1151_);
      }
      else if (dlib.Datasets[ds].Type == 1) {
        mcr1161->AddMC(h1161_);
        mcr1151->AddMC(h1151_);
      }
    }
    mcr1161->CreateSF1DPlot();
    mcr1151->CreateSF1DPlot();
    if (DrawMCReweight) {
      TString fweightname = StandardNames::HistFileName(HistFilePath, HistFilePrefix, "ReweightSF");
      TFile *fweight = new TFile(fweightname,"RECREATE");
      fweight->cd();
      TH1F* mcr1161sf1d = (TH1F*) mcr1161->SF1D->Clone();
      TH1F* mcr1151sf1d = (TH1F*) mcr1151->SF1D->Clone();
      fweight->Write();
      fweight->Save();
      fweight->Close();
      cout << "Reweight file is saved at " << fweightname << endl;
    }
    else cout << "This iteration is not saving SF file" << endl;
    fsource->Close();
    delete fsource;
  }
  
  for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
    if (isampletype != -1 && (int)ist != isampletype) continue;
    TChain* t = new TChain("t");
    string SampleType = SampleTypes[ist];
    if (SampleType == "ZZ") continue;
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
        float LeadingJetPt = r->JetPt->at(0);
        float LeadingJetEta = r->JetEta->at(0);
        float METPt = r->METPt;
        float METPhi = r->METPhi;
        float mT = r->mT->at(0);
        float HT = 0;
        for (unsigned ij = 0; ij < r->JetPt->size(); ++ij) {
          HT += r->JetPt->at(ij);
        }
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
        if (SampleType == "ttbar" && DoMCReweight) {
          // cout << "Trying to get SF of WPrimeMassSimpleFL = " << WPrimeMassSimpleFL << endl;
          float mcrweight = 1.0;
          if (RegionIdentifier / 10 % 10 == 6) mcrweight = mcr1161->GetSF1DF(WPrimeMassSimpleFL);
          else if (RegionIdentifier / 10 % 10 == 5) mcrweight = mcr1151->GetSF1DF(WPrimeMassSimpleFL);
          EventWeight *= mcrweight;
          // cout << "SF = " << mcrweight <<endl;
        }
        HistCol.SetCurrentFill(ist, iv, RegionIdentifier, EventWeight);
        HistCol.Fill("LeptonPt", LeptonPt);
        HistCol.Fill("LeptonEta",LeptonEta);
        HistCol.Fill("LeadingJetPt",LeadingJetPt);
        HistCol.Fill("LeadingJetEta",LeadingJetEta);
        HistCol.Fill("METPt",METPt);
        HistCol.Fill("METPhi",METPhi);
        HistCol.Fill("mT",mT);
        HistCol.Fill("HT",HT);
        HistCol.Fill("WPrimeMassSimpleFL",WPrimeMassSimpleFL);
        HistCol.Fill("WPrimeMassSimpleLL",WPrimeMassSimpleLL);

      }
      // checkpoint(2);
    }
    cout << "In SampleType: " << SampleTypes[ist];
    cout << ", Number of events with nan weight = " << n_nan_weight << endl;
  }
  HistCol.PostProcess();
  cout << "Auto exiting" << endl;
  gApplication->Terminate();
}