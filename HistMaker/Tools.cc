#ifndef TOOLS_CC
#define TOOLS_CC

void toolscheckpoint(int i) {
  cout << "----Debug checkpoint >>> " << i << " <<< reached----" <<endl;
}

void toolscheckpoint(string i) {
  cout << "----Debug checkpoint >>> " << i << " <<< reached----" <<endl;
}

// Rebin factor such that a plot with too many bins can be rebinned to less than 100 bins.
float RebinCalc(double nbins, int target = 100) {
  float rb_ = 1;
  vector<float> inc{2.,2.5,2.}; // 2, 5, 10, 20, 50, 100 etc rebinning
  int ind = 0;
  while (nbins  / rb_ > target) { // rebin untill less than target bins in histogram;
    rb_ = inc[ind] * rb_;
    if (ind == 2) ind = 0;
    else ind++;
  }
  return rb_;
}

// Manual factor to scale too small signal up, such that it can be visible in comparison
double SignalScaleCalc(double sigmax, double targetmax) {
  double sc = 1.;
  while (sigmax * sc * 10. < targetmax) {
    sc *= 10.;
  }
  return sc;
}

#endif
