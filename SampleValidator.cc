#include "Utilities/NanoAODReader.cc"
#include "CreateAuxHists.cc"
using namespace std;

bool ValidLV(TLorentzVector lv) {
  if (lv.Pt() == 0) return false;
  return true;
}

int InValidVarPO(VarPO p) { // Jet, Lepton, MET are all VarPO
  if (!ValidLV(p)) return 5;
  if (!ValidLV(p.SU)) return 1;
  if (!ValidLV(p.SD)) return 2;
  if (!ValidLV(p.RU)) return 3;
  if (!ValidLV(p.RD)) return 4;
  return 0;
}

bool IsNan(float n) {return n != n;}
bool IsNan(double n) {return n != n;}

int InValidJet(Jet j) {
  if (InValidVarPO(j)) return 1;
  for (unsigned iv = 0; iv < 3; ++iv) {
    for (unsigned iwp = 0; iwp < 3; ++iwp) {
      if (IsNan(j.PUIDSFweights[iv][iwp])) return 2;
      if (IsNan(j.bJetSFweights[iv][iwp])) return 3;
    }
  }
  return 0;
}

int InValidLep(Lepton l) {
  if (InValidVarPO(l)) return 1;
  for (unsigned iv = 0; iv < 3; ++iv) {
    if (IsNan(l.SFs[iv])) return 2;
  }
  return 0;
}

void SampleValidation(int sampleyear = 2, int sampletype = 1, int ifile = -1, string infile = "All") {
  // infile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/wprime_500/2017/003C756A-BC7B-5A48-8F2E-A61D3CDC7C32.root";
  // sampletype = dlib.AddDataset_NGTCXS("WP500");
  Configs* conf = new Configs(sampleyear, sampletype, ifile);
  conf->Debug = false;
  conf->LocalOutput = true;
  conf->PrintProgress = true;
  conf->ProgressInterval = 1;
  conf->UseSkims_bTagSF = true;
  conf->UseSkims_PUIDSF = true;
  conf->Compare_bTagSF = 100; // Lines of outputs to print
  if (conf->IsMC) conf->Compare_PUIDSF = 100;
  // ifile needs to be negative to let the InputFile to work.
  // conf->InputFile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/wprime_500_latter_leptonic/2018/05E4564C-20A7-324B-BCCD-1D869EB6CE77.root";
  conf->InputFile = infile;
  NanoAODReader *r = new NanoAODReader(conf);
  bTagEff *bTE = new bTagEff(conf);
  r->SetbTag(bTE);
  Long64_t EntryMax = r->GetEntries();
  Long64_t StartEntry = 0;
  // EntryMax = 1000 + StartEntry;
  Progress* progress = new Progress(EntryMax, conf->ProgressInterval);

  for (Long64_t ievt = StartEntry; ievt < EntryMax; ++ievt) {
    if (conf->PrintProgress) progress->Print(ievt);
    r->ReadEvent(ievt);
    // if (!r->KeepEvent) continue;
    unsigned nJets = r->evts->nJet;
    if (nJets > 20) cout << "Excessive nJets = " << nJets << endl;
    for (unsigned i = 0; i < r->Jets.size(); ++i) {
      int err = InValidJet(r->Jets[i]);
      if (!err) continue;
      if (err == 1 && InValidVarPO(r->Jets[i]) != 5 && !conf->IsMC) continue; // Data ignore zero variations
      cout << "Invalid Jet detected: ";
      if (err == 1) {
        vector<string> lvs{"","SU","SD","RU","RD","Nom"};
        cout << lvs[InValidVarPO(r->Jets[i])] << " Pt = 0";
      }
      else if (err == 2) cout << "Nan PUIDSFweights";
      else if (err == 3) cout << "Nan bJetSFweights";
      cout << endl;
    }

    unsigned nElectrons = r->evts->nElectron;
    if (nElectrons > 10) cout << "Excessive nElectrons = " << nElectrons << endl;
    for (unsigned i = 0; i < r->Electrons.size(); ++i) {
      int err = InValidLep(r->Electrons[i]);
      if (!err) continue;
      cout << "Invalid Electron detected: ";
      if (err == 1) cout << "Pt = 0";
      else if (err == 2) cout << "Nan ScaleFactor";
      cout << endl;
    }

    unsigned nMuons = r->evts->nMuon;
    if (nMuons > 10) cout << "Excessive nMuons = " << nMuons << endl;
    for (unsigned i = 0; i < r->Muons.size(); ++i) {
      int err = InValidLep(r->Muons[i]);
      if (!err) continue;
      cout << "Invalid Muon detected: ";
      if (err == 1) cout << "Pt = 0";
      else if (err == 2) cout << "Nan ScaleFactor";
      cout << endl;
    }
  }
}

void SampleValidator(int sampleyear = 3, int iter = 1) {
  string infile = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/";
  int sampletype;
  if (iter == 0) {
    if (sampleyear == 3) infile += "wprime_500/2018/172299C4-2D12-FA48-8A1F-9117F69514FA.root";
    if (sampleyear == 2) infile += "wprime_500/2017/4CBF8DAC-6BFD-4F44-BCDE-57AC816614EB.root";
    if (sampleyear == 1) infile += "wprime_500/2016/4766B675-2F69-CA4D-8FE3-5C57DBB0BB99.root";
    if (sampleyear == 0) infile += "wprime_500/2016_APV/DEF70539-4F28-3848-8AC9-29062814D542.root";
    sampletype = dlib.AddDataset_NGTCXS("WP500");
  }
  
  if (iter == 1) {
    if (sampleyear == 3) infile += "SingleMuon/2018/0A7B4EA1-E44D-E64A-B269-22A132D5E72E.root";
    if (sampleyear == 2) infile += "SingleMuon/2017/0A7BEB92-DC6C-D74B-B814-A7165C2419FF.root";
    if (sampleyear == 1) infile += "SingleMuon/2016/0A4230E2-0C75-604D-890F-A4CE5E5C164E.root";
    if (sampleyear == 0) infile += "SingleMuon/2016_APV/0ACEDA1E-BB5E-6E4C-A9E1-DD3B86880F97.root";
    sampletype = dlib.AddDataset_NGTCXS("SingleMuon");
  }

  if (iter == 2) {
    if (sampleyear == 3) infile += "EGamma/2018/0A021BC8-7D79-4044-9A4C-611BADEE7798.root";
    if (sampleyear == 2) infile += "SingleElectron/2017/0A485330-ED63-DE43-91EB-B7D65DD2CC5D.root";
    if (sampleyear == 1) infile += "SingleElectron/2016/0D19EF2A-904C-5347-A8FA-8DCA87868F37.root";
    if (sampleyear == 0) infile += "SingleElectron/2016_APV/0A50C081-DE8C-3A47-A708-B678749AD664.root";
    sampletype = dlib.AddDataset_NGTCXS("SingleElectron");
  }
  
  if (dlib.GetDataset(sampletype).Type != 0) {
    cout << "Creating AuxHist for " << dlib.DatasetNames[sampletype] <<endl;
    CreateAuxHists(sampleyear, sampletype,-1,infile);
  }
  cout << "Validating for dataset " << dlib.DatasetNames[sampletype] <<endl;
  SampleValidation(sampleyear, sampletype,-1,infile);
}