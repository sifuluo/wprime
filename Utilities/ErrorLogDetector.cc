#ifndef ERRORLOGDETECTOR_CC
#define ERRORLOGDETECTOR_CC

#include <fstream>
#include "Dataset.cc"

using namespace std;

int ErrorLogDetected(string logpre) {
  string errlog = logpre + ".err";
  string outlog = logpre + ".out";
  fstream fe(errlog, fstream::in);
  fstream fo(outlog, fstream::in);
  if (!(fe.is_open())) {
    cout << "(First Run) No error log file detected for " << errlog << endl;
    return 2;
  }
  if (!(fo.is_open())) {
    cout << "(First Run) No output log file detected for " << errlog << endl;
    return 2;
  }
  if (fo.peek() == EOF) {
    cout << "(First Run) Output log is empty for " << errlog << endl;
    return 2;
  }
  if (fe.peek() == EOF) {
    cout << "(Successful Run) Error log is empty for " << errlog << endl;
    return 0;
  }
  else {
    cout << "(Failed Run) None empty error log detected for " << errlog << endl;
    string errtext;
    int counter = 0;
    while (getline(fe, errtext) && counter < 20) {
      ++counter;
      cout << errtext << endl;
    }
    return 1;
  }
}

int ErrorLogDetected(int isampleyear, int isampletype, int ifile = 0){
  string dsname = dlib.SampleYears[isampleyear] + "_" + dlib.DatasetNames[isampletype];
  string errlogpath = Form("Submits/logs/%s/%s_%i", dsname.c_str(), dsname.c_str(), ifile);
  cout << "Checking log file: " << errlogpath << ".out/.err" << endl;
  return ErrorLogDetected(errlogpath);
}

#endif