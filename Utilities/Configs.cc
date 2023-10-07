#ifndef CONFIGS_CC
#define CONFIGS_CC
// Configuration file that is designed to be passed across the entire process

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "TString.h"

#include "Dataset.cc"
#include "UserSpecifics.cc"
#include "ErrorLogDetector.cc"

struct Configs {
  Configs(int isy_ = 2, int ist_ = 2, int ifile_ = 0) {
    iSampleYear = isy_;
    iSampleType = ist_;
    iFile = ifile_;
    SampleYear = dlib.SampleYears[isy_];
    SampleType = dlib.DatasetNames[ist_];
    Type = dlib.Datasets[SampleType].Type;
    IsMC = Type > 0;
    TString st = SampleType;
    if (st.Contains("FL")) WPType = 0;
    else if (st.Contains("LL")) WPType = 1;
    else WPType = -1;
  };

  ~Configs() {
    cout << "Configs Destructed" << endl;
  }

  // Indices and items are emumerated in Constants.cc
  int iSampleYear; 
  string SampleYear;
  int iSampleType;
  string SampleType; // Should be SampleName actually
  bool IsMC;
  int Type; // 0: Data, 1: MC, 2: Signal
  int WPType; // 0: FL(Hadronic) 1: LL(Leptonic)

  // iFile >=0 will be index of files in the text file containing the path to files
  // if iFile < 0, a InputFile must be specified to be run on.
  int iFile; 
  // InputFile override. If InputFile = "All", all files in the text file will be processed.
  TString InputFile = "";

  //Number of entry to process
  Long64_t EntryMax = 0;
  //Number of files to process
  int FilesPerJob = 1;

  int bTagWP = 2; //0 loose, 1 medium, 2 tight
  int PUIDWP = 2; //0 loose, 1 medium, 2 tight
  bool UseSkims_bTagSF = true;
  bool UseSkims_PUIDSF = true;
  int Compare_bTagSF = 0; // Lines to print out, -1 for infinite, 0 for not print out
  int Compare_PUIDSF = 0;

  // Idealy the container for all bool configs, practically only the ones not used extensively should rest in here.
  map<string,bool> Switches;

  bool LocalOutput = false;

  // In batch mode, it is suggested to turn off, or will result in a large log file.
  bool PrintProgress = false;
  int ProgressInterval = 1000;

  bool DASInput = false;

  bool bTagEffHistCreation = false;
  bool AuxHistCreation = false;
  string AuxHistBasePath = UserSpecifics::EOSBasePath + "AuxHists/";
  bool UseMergedAuxHist = false;

  bool RunFitter = false;
  bool UseMassDist = false;
  double JetScaleMinPMass = 0.01;

  vector<int> AcceptedRegions = {};
  void AcceptRegions(vector<int> a, vector<int> b, vector<int> c, vector<int> d) {
    AcceptedRegions.clear();
    for (int aa : a) for (int bb : b) for (int cc : c) for (int dd: d) if (dd <= cc) AcceptedRegions.push_back(aa * 1000 + bb * 100 + cc * 10 + dd);
  }
  bool IsAcceptedRegion(int r) {
    return true;
  }
  
  int ErrorRerun(){ // 0: Empty log (Succeeded run);  1: non-empty log (failed run);  2: no log (first run)
    return ErrorLogDetected(iSampleYear, iSampleType, iFile);
  } 
  bool InRerunList = false;
  bool RerunList(vector<int>& l) {
    for (unsigned i = 0; i < l.size(); ++i) if (l[i] == iFile) {
      InRerunList = true;
      return true;
    }
    return false;
  }
  bool RerunList(int y, int st, vector<int> l) {
    if (iSampleYear == y && iSampleType == st) return RerunList(l);
    return false;
  }
  bool RerunList(string y, string st, vector<int> l) {
    if (SampleYear == y && SampleType == st) return RerunList(l);
    return false;
  }

  vector<string> DebugList; // Look for "conf->Debug()" in modules to see candidates.
  bool Debug(string n) {
    for (string nl : DebugList) {
      if (n == nl) return true;
    }
    return false;
  }

};

void checkpoint(int i) {
  cout << "----Debug checkpoint >>> " << i << " <<< reached----" <<endl;
}

void checkpoint(string i) {
  cout << "----Debug checkpoint >>> " << i << " <<< reached----" <<endl;
}

#endif
