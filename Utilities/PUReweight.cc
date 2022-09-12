#ifndef PUREWEIGHT_CC
#define PUREWEIGHT_CC

#include <vector>
#include <string>
#include <iostream>

#include <TFile.h>
#include <TH1.h>
#include <TString.h>

// https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData#Recommended_cross_section
class PUReweight{
public:
  PUReweight(int isy_, int ix_ = 1) { // default is 69200
    iSampleYear = isy_;
    ixsec = ix_;
    Init();
  };

  void Init() {
    vector<string> pathyears{"16","16","17","18"};
    string basepath = "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions" + pathyears[iSampleYear] + "/13TeV/PileUp/UltraLegacy/";
    vector<string> xsecs{"66000","69200","72400","80000"};
    vector<string> SampleYears{"2016-preVFP","2016-postVFP","2017","2018"};
    string filename = "PileupHistogram-goldenJSON-13tev-" + SampleYears[iSampleYear] + "-" + xsecs[ixsec] + "ub-99bins.root";
    TString path = basepath + filename;
    pileupfile = new TFile(path,"READ");
    pileuphist = (TH1D*)pileupfile->Get("pileup");
    cout << "PUReweighting using histogram:" << endl;
    cout << path<<endl;
    norm = double(pileuphist->GetXaxis()->GetNbins()) / pileuphist->Integral();
    pileuphist->Scale(norm);
  }

  double GetWeight(int nPU) {
    return pileuphist->GetBinContent(nPU + 1);
  }


  int iSampleYear, ixsec;
  TFile *pileupfile;
  TH1D* pileuphist;
  double norm;
};


#endif
