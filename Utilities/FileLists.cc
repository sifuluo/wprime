#ifndef FILELISTS_CC
#define FILELISTS_CC

#include "TString.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "Constants.cc"

using namespace std;

// Need to stay consistent with MakeSubmission
vector<string> GetFileNames(int isy_, int ist_, int igroup = 0, int groupsize = 1) {
  vector<string> out;
  string basepath = "/afs/cern.ch/work/s/siluo/wprime/filenames/";
  string filename = basepath + Constants::SampleTypes[ist_] + "_" + Constants::SampleYears[isy_] + ".txt";
  ifstream infile(filename);
  if (!infile) {
    cout << "Cannot read from file " << filename << endl;
    throw runtime_error("Cannot Read from file");
    return out;
  }
  else cout << "Reading from file " << filename << endl;

  int startfile = igroup * groupsize;
  int endfile = (igroup + 1) * groupsize - 1;
  string rootf;
  int counter = -1;
  while (getline(infile, rootf)) {
    ++counter;
    if (counter < startfile) continue;
    if (counter > endfile) break;
    cout << "Loading root file " << rootf << endl;
    out.push_back(rootf);
  }
  return out;
}

#endif
