#ifndef CONFIGS_CC
#define CONFIGS_CC

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "TString.h"

#include "Constants.cc"

struct Configs {
  Configs(int isy_ = 2, int ist_ = 2, int itr_ = 0, int ifile_ = 0) {
    iSampleYear = isy_;
    iSampleType = ist_;
    iTrigger = itr_;
    iFile = ifile_;
    SampleYear = Constants::SampleYears[isy_];
    SampleType = Constants::SampleTypes[ist_];
    Trigger = Constants::Triggers[itr_];
    IsMC = iSampleType > 1;
    EntryMax = 0;
  };

  int iSampleYear;
  string SampleYear;
  int iSampleType;
  string SampleType;
  bool IsMC;
  int iTrigger;
  string Trigger;
  int iFile;
  Long64_t EntryMax;

  TString InputFile = ""; // InputFile override

  int FilesPerJob = 1;
  // int Btag_WP = 2;

  // Idealy the container for all bool configs, practically only the ones not used extensively should rest in here.
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

  void TestSwitch() {
    bool out = false;
    for (unsigned i = 0; i < 100; ++i) {
      string ss = Form("%d",i);
      out = out || GetSwitch(ss);
    }
    if (out) throw runtime_error("Switch initialized as true");
  }

};

void checkpoint(int i) {
  cout << "----Debug checkpoint >>> " << i << " <<< reached----" <<endl;
}

void checkpoint(string i) {
  cout << "----Debug checkpoint >>> " << i << " <<< reached----" <<endl;
}

#endif
