#include "Utilities/NanoAODReader.cc"

using namespace std;

void SampleValidator(int sampleyear = 3, int sampletype = 19, int ifile = 0) {
  Configs* conf = new Config(sampleyear, sampletype, ifile);
  conf->Debug = false;
  conf->LocalOutput = true;
  conf->PrintProgress = true;
  conf->ProgressInterval = 100;
  conf->Compare_bTagSF = true;
  conf->Compare_PUIDSF = true;

  NanoAODReader *r = new NanoAODReader(conf);
  bTagEff *bTE = new bTagEff(conf);
  r->SetbTag(bTE);
  Long64_t EntryMax = r->GetEntries();
  EntryMax = 1000;
  progress = new Progress(EntryMax, conf->ProgressInterval);

  for (Long64_t ievt = 0; ievt < EntryMax; ++ievt) {
    if (conf->PrintProgress) progress->Print(ievt);
    r->ReadEvent(ievt);
    if (!r->KeepEvent) continue;

  }

}