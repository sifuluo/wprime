#ifndef CONFIGS_CC
#define CONFIGS_CC

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "TString.h"

struct Configs {
  Configs(int isy_ = 2, int ist_ = 2, int itr_ = 0, int ifile_ = 0) {
    iSampleYear = isy_;
    iSampleType = ist_;
    iTrigger = itr_;
    iFile = ifile_;
    SampleYear = Constants::SampleYears[isy_];
    SampleType = Constants::SampleTypes[ist_];
    Trigger = Constants::Triggers[itr_];
  };

  int iSampleYear;
  string SampleYear;
  int iSampleType;
  string SampleType;
  int iTrigger;
  string Trigger;
  int iFile;

  TString InputFile = ""; // InputFile override

  int FilePerJob = 1;
  int Btag_WP = 2;

  map<string,bool> Switches;
  bool Debug = false;

  bool PrintProgress = false;
  int ProgressInterval = 1000;
  bool PUEvaluation = false;
  bool DASInput = false;

  void SetSwitch(string sw, bool b) {
    Switches[sw] = b;
  }

  bool GetSwitch(string sw) {
    return Switches[sw];
  }

};

void checkpoint(int i) {
  cout << "----Debug checkpoint >>> " << i << " <<< reached----" <<endl;
}

void checkpoint(string i) {
  cout << "----Debug checkpoint >>> " << i << " <<< reached----" <<endl;
}

#endif
