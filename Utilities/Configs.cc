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

  ~Configs() {
    cout << "Configs Destructed" << endl;
  }

  // Indices and items are emumerated in Constants.cc
  int iSampleYear; 
  string SampleYear;
  int iSampleType;
  string SampleType;
  bool IsMC;
  int iTrigger;
  string Trigger;

  // iFile >=0 will be index of files in the text file containing the path to files
  // if iFile < 0, a InputFile must be specified to be run on.
  int iFile; 
  // InputFile override. If InputFile = "All", all files in the text file will be processed.
  TString InputFile = "";

  //Number of entry to process
  Long64_t EntryMax;
  //Number of files to process
  int FilesPerJob = 1;
  // int Btag_WP = 2;

  // Idealy the container for all bool configs, practically only the ones not used extensively should rest in here.
  map<string,bool> Switches;

  bool Debug = false;

  // In batch mode, it is suggested to turn off, or will result in a large log file.
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
