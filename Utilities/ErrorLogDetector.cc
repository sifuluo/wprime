#ifndef ERRORLOGDETECTOR_CC
#define ERRORLOGDETECTOR_CC

#include <fstream>
#include "Dataset.cc"

using namespace std;

int ErrorLogDetected(string errlog) {
  fstream f(errlog, fstream::in);
  if (!(f.is_open())) {
    cout << "(First Run) No error log file detected for " << errlog << endl;
    return 2;
  }
  if (f.peek() == EOF) {
    cout << "(Successful Run) Error log is empty for " << errlog << endl;
    return 0;
  }
  else {
    cout << "(Failed Run) None empty error log detected for " << errlog << endl;
    return 1;
  }
}

int ErrorLogDetected(int isampleyear, int isampletype, int ifile = 0){
  string dsname = dlib.SampleYears[isampleyear] + "_" + dlib.DatasetNames[isampletype];
  string errlogpath = Form("Submits/logs/%s/%s_%i.err", dsname.c_str(), dsname.c_str(), ifile);
  cout << "Checking log file: " << errlogpath << endl;
  return ErrorLogDetected(errlogpath);
}

#endif