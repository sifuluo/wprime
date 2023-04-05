#include "Utilities/NanoAODReader.cc"

using namespace std;

void SampleValidator(int sampleyear = 3, int sampletype = 28, int ifile = 0) {
  Configs* conf = new Configs(sampleyear, sampletype, ifile);
  conf->Debug = false;
  conf->LocalOutput = true;
  conf->PrintProgress = true;
  conf->ProgressInterval = 1;
  conf->UseSkims_bTagSF = false;
  conf->UseSkims_PUIDSF = false;
  // conf->Compare_bTagSF = 10; // Lines of outputs to print
  // conf->Compare_PUIDSF = 10;
  // ifile needs to be negative to let the InputFile to work.
  conf->InputFile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/wprime_500_latter_leptonic/2018/05E4564C-20A7-324B-BCCD-1D869EB6CE77.root";
  NanoAODReader *r = new NanoAODReader(conf);
  bTagEff *bTE = new bTagEff(conf);
  r->SetbTag(bTE);
  Long64_t EntryMax = r->GetEntries();
  Long64_t StartEntry = 107175;
  EntryMax = 10 + StartEntry;
  Progress* progress = new Progress(EntryMax, conf->ProgressInterval);

  for (Long64_t ievt = StartEntry; ievt < EntryMax; ++ievt) {
    if (conf->PrintProgress) progress->Print(ievt);
    r->ReadEvent(ievt);
    if (!r->KeepEvent) continue;
  }
}