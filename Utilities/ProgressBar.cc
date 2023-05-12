#ifndef PROGRESSBAR_CC
#define PROGRESSBAR_CC

#include <cmath>
#include <iostream>
#include <string>
#include <ctime>
#include <TString.h>

using namespace std;

class Progress {
public:
  Progress(Long64_t itotal, unsigned div = 1000) {
    time(&starttime);
    SetEntryMax(itotal);
    dividend = div;
    barlength = 50;
    cout << "Process started at " << ctime(&starttime);
    bar = "";
    for (unsigned i = 0; i < barlength; ++i) bar += " ";
  };

  void SetEntryMax(Long64_t itotal) {
    EntryMax = itotal - 1;
    time(&checkedtime);
  };

  void Print(unsigned ie) {
    if (EntryMax == 0) return;
    if (ie % dividend == 0 || ie == EntryMax) {
      rate = float(ie) / float(EntryMax);
      per = rate * 100.;
      barloc = floor(rate * float(barlength));
      if (barloc < barlength) bar[barloc] = '>';
      if (barloc > 0) {
        if (bar[barloc - 1] != '=') bar[barloc - 1] = '=';
      }
      time(&currenttime);
      ptime = difftime(currenttime, starttime);
      pcheckedtime = difftime(currenttime, checkedtime);
      rtime = pcheckedtime / rate - pcheckedtime;
      ptime_s = Form("Elapsed: %i:%i:%i",int(ptime/3600), int(ptime/60) % 60, int(ptime) % 60);
      rtime_s = Form("Remaining: %i:%i:%i",int(rtime/3600), int(rtime/60) % 60, int(rtime) % 60);
      if (pcheckedtime < 5) rtime_s = "";
      //This is the style with the bar. Best used in dryrun.
      // cout <<Form("\r[%s] %i / %lli, (%.1f%%), %s, %s",bar.Data(), ie, EntryMax, per, ptime_s.c_str(), rtime_s.c_str()) <<flush;
      //This is the style withou the bar. Best used in batch, because flushed text are also in the log.
      cout <<Form("\r %i / %lli, (%.1f%%), %s, %s          ",ie, EntryMax, per, ptime_s.c_str(), rtime_s.c_str()) <<flush;
      // if (ie % 100 == 0) cout <<endl;
    }
  }

  void JobEnd() {
    time_t endtime;
    time(&endtime);
    cout << endl <<  "All Done at  " << ctime(&endtime) << ", Total " << ptime_s << endl;
  }

private:
  time_t starttime;
  time_t checkedtime;
  time_t currenttime;
  Long64_t EntryMax;
  unsigned dividend, barloc, barlength;
  float rate, per;
  double ptime, rtime, pcheckedtime;
  string ptime_s, rtime_s;
  TString bar;
};

void PrintProgress (int entry, int entries, int dividen = 100) {
  entries--;
  if (entry % dividen ==0 || entry == entries ) {
    float per = float(entry) / float(entries);
    cout << Form("\rProgress: %i / %i , (%.1f %% )", entry, entries, per) <<flush;
  }
  if (entry == entries){
    cout <<endl << "Done."<<endl;
  }
}

#endif
