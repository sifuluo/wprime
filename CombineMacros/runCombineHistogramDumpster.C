#include "CombineHistogramDumpster.C"

void runCombineHistogramDumpster(int bin = 1152, int year = 2018){
  for(unsigned i = 0; i < 40; ++i){
    if(bin/1000 == 1 && i == 0) continue;
    else if(bin/2000 == 1 && i == 1) continue;
    CombineHistogramDumpster D(0, i, 1152, TString::Format("%d", year)); //don't supply a year, for now, there are no options
    D.Loop();
  }
}
