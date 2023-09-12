#include "CombineHistogramDumpster.C"
#include "ScaleFactorTTbarCalc.C"

void runCombineHistogramDumpster(int bin = 1153, int year = 2018){
  //first run CR for SF calculation
  int binCR = 0;
  if(bin/1000 == 1) binCR+=1100;
  else if(bin/2000 == 1) binCR+=2100;
  if((bin-binCR)/60 == 1) binCR+=62;
  else binCR+=52;
  for(unsigned i = 0; i < 22; ++i){//skip signal samples
    if(bin/1000 == 1 && i == 0) continue;
    else if(bin/2000 == 1 && i == 1) continue;
    CombineHistogramDumpster D(0, i, binCR, TString::Format("%d", year), 0);
    D.Loop();
  }

  //fit SFs and variations
  ScaleFactorTTbarCalc(binCR, year);

  //run actual variations and SF'd ttbar
  for(unsigned i = 0; i < 40; ++i){
    if(bin/1000 == 1 && i == 0) continue;
    else if(bin/2000 == 1 && i == 1) continue;
    CombineHistogramDumpster D(0, i, bin, TString::Format("%d", year), binCR); //don't supply a year, for now, there are no options
    D.Loop();
  }
}
