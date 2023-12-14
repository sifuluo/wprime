#ifndef NANOAODREADER_CC
#define NANOAODREADER_CC
// Main interface takes raw data from MakeClass() class and dump them into physics objects defined in Datasets
// Calucate the event weights and region identifier in the meantime.

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <array>

#include "TChain.h"
#include "TFile.h"
#include "TLeaf.h"

#include "DataFormat.cc"
#include "BranchReader.cc"

#include "Configs.cc"
#include "bTag.cc"
#include "PUID.cc"

#include "XYMETCorrection_withUL17andUL18andUL16.h"

#include "ProgressBar.cc"

using namespace std;

class NanoAODReader {
public:
  NanoAODReader(Configs *conf_) {
    chain = new TChain("Events;2");

    conf = conf_;

    IsMC = conf->IsMC;

    R_BTSF = new bTagSFReader(conf);
    R_BTSF->ReadCSVFile();
    R_PUIDSF = new PUIDSFReader(conf);
    R_PUIDSF->ReadPUIDSF();
    
    if (conf->iFile >= 0 || (conf->InputFile == "All" && conf->iFile < 0)) { // batch mode

      vector<string> rootfiles = GetFileNames();
      // rootfiles = {"/eos/user/p/pflanaga/andrewsdata/skimmed_samples/ttbar/2018/B047029C-C11B-A54B-A5F3-91981AB3D5DC.root"};
      for (string rf : rootfiles) {
        chain->Add(TString(rf));
        cout << "Successfully loaded root file: " << rf << endl;
      }
      cout << "In total, " << rootfiles.size() << " files are loaded" << endl;
    }
    else if (conf->InputFile == "") cout << "iFile set to negative but a valid test InputFile name is missing" << endl;
    else {
      chain->Add(conf->InputFile);
      cout << "Loaded test InputFile: " << conf->InputFile << endl;
    }
    cout << "Running with SampleYear = " << conf->SampleYear << ", SampleType = " << conf->SampleType <<  endl;
    evts = new Events(chain, conf->SampleYear, IsMC);
    ChainEntries = GetEntries();
    if (conf->InputFile != "All" && conf->FilesPerJob == 1) cout << "This file contains " << GetEntries() << " events" <<endl;
    if (conf->FilesPerJob > 1.0 && conf->FilesPerJob != (int) conf->FilesPerJob) cout << "FilesPerJob should be integer if > 1.0!!!" << endl;
    EntryBegin = 0;
    EntryEnd = ChainEntries;
    if (conf->FilesPerJob < 1.0 && conf->iFile >= 0) {
      int EntriesPerJob = floor(((double) ChainEntries) * conf->FilesPerJob);
      double JobsPerFile = 1.0 / conf->FilesPerJob;
      if (JobsPerFile - floor(JobsPerFile) != 0) cout << "FilesPerJob divided by 1 is not an integer" << endl;
      int iSeg = conf->iFile % ((int)JobsPerFile);
      EntryBegin = iSeg * EntriesPerJob;
      EntryEnd = (iSeg + 1) * EntriesPerJob;
      if (iSeg + 1 == JobsPerFile) EntryEnd = ChainEntries;
      cout << "This Iteration Divided the file containing " << ChainEntries << " entries into " << JobsPerFile << " jobs, each containing " << EntriesPerJob << " entries" << endl;
    }
    EntriesMax = EntryEnd - EntryBegin;
    cout << "EntryBegin = " << EntryBegin << ", EntryEnd = " << EntryEnd << ". Running over " << EntriesMax << " events" << endl;
  };

  ~NanoAODReader() {
    cout << "NanoAODReader Destructed" << endl;
  }

  vector<string> GetFileNames() {
    vector<string> out;
    string basepath = "filenames/";
    if (conf->DASInput) basepath = basepath + "DASFileNames/DAS_";
    string filename = basepath + conf->SampleType + "_" + conf->SampleYear + ".txt";
    ifstream infile(filename);
    if (!infile) {
      cout << "Cannot read from file " << filename << endl;
      throw runtime_error("Cannot Read from file");
      return out;
    }
    else cout << "Reading from file " << filename << endl;

    int startfile = conf->iFile;
    int endfile = conf->iFile;
    if (conf->FilesPerJob >= 1.) {
      startfile = conf->iFile * conf->FilesPerJob;
      endfile = (conf->iFile + 1) * conf->FilesPerJob - 1;
    }
    else {
      startfile = endfile = floor(conf->iFile * conf->FilesPerJob);
    }
    string rootf;
    int counter = -1;
    while (getline(infile, rootf)) {
      ++counter;
      if (counter < startfile) continue;
      if (counter > endfile && !(conf->InputFile == "All" && conf->iFile < 0)) break;
      if (rootf.find("/store/") == 0) rootf = "root://cms-xrd-global.cern.ch/" + rootf;
      cout << "Loading root file " << rootf << endl;
      out.push_back(rootf);
    }
    return out;
  }

  Long64_t GetEntries() {
    ChainEntries = chain->GetEntries();
    return ChainEntries;
  }

  Long64_t GetEntriesFast() {
    ChainEntries = chain->GetEntriesFast();
    return ChainEntries;
  }

  Long64_t GetEntryBegin() {return EntryBegin;}
  Long64_t GetEntryEnd() {return EntryEnd;}
  Long64_t GetEntriesMax() {return EntriesMax;}

  void SetbTag(bTagEff* bt) {
    bTE = bt;
  }

  //function to determine lepton-jet overlaps, gives answer depending on PUID passing or not
  vector<bool> JetProximityCheck(Lepton ell_){
    vector<bool> out = {false, false, false};
    for(unsigned j = 0; j < Jets.size(); ++j) if(fabs(Jets[j].DeltaR(ell_)) < 0.4) {
      for (unsigned i = 0; i < 3; ++i) {
        out[i] = out[i] || Jets[j].PUIDpasses[i];
      }
    }
    return out;
  }

  int ReadEvent(Long64_t i) {
    PassedHEMCut = true;
    bTagWP = conf->bTagWP;
    PUIDWP = conf->PUIDWP;

    Long64_t evtcode = evts->LoadTree(i);
    if (evtcode < 0) return -1;
    iEvent = i;
    iEventInChain = iEvent + EntryBegin;
    evts->GetEntry(iEventInChain);
    if (ReadMETFilterStatus() == false) return -2; //skip events not passing MET filter flags
    run = evts->run;
    luminosityBlock = evts->luminosityBlock;
    if (!IsMC && (run < 0 || luminosityBlock < 0)) cout << "Run/LuminosityBlock number is negative" <<endl;
    
    if (IsMC) {
      ReadGenParts();
      ReadGenJets();
      ReadGenMET();
      ReadPileup();
    }
    ReadJets();
    if (conf->bTagEffHistCreation) return 0;
    ReadTriggers();
    ReadLeptons();
    ReadMET();
    ReadVertices();
    if (PV_npvsGood < 1) return -3; // Requires PV_npvsGood >= 1
    // if (!PassedHEMCut) return -3;
    RegionAssociations = RegionReader();
    EventWeights = CalcEventSFweights();
    return 0;
  }

  void ReadGenParts() {
    GenParts.clear();
    for (unsigned i = 0; i < evts->nGenPart; ++i) {
      GenPart tmp;
      tmp.SetPtEtaPhiM(evts->GenPart_pt[i],evts->GenPart_eta[i],evts->GenPart_phi[i],evts->GenPart_mass[i]);
      tmp.index = i;
      tmp.genPartIdxMother = evts->GenPart_genPartIdxMother[i];
      tmp.pdgId = evts->GenPart_pdgId[i];
      tmp.status = evts->GenPart_status[i];
      GenParts.push_back(tmp);
    }
  }

  void ReadGenJets() {
    GenJets.clear();
    for (unsigned i = 0; i < evts->nGenJet; ++i) {
      GenJet tmp;
      tmp.SetPtEtaPhiM(evts->GenJet_pt[i],evts->GenJet_eta[i],evts->GenJet_phi[i],evts->GenJet_mass[i]);
      tmp.index = i;
      tmp.partonFlavour = evts->GenJet_partonFlavour[i];
      tmp.hadronFlavour = evts->GenJet_hadronFlavour[i];
      GenJets.push_back(tmp);
    }
  }

  bool PassHEMCut(TLorentzVector &v) {
    if (conf->SampleYear != "2018") return true;
    if (v.Eta() < -3.) return true;
    if (v.Eta() > -1.3) return true;
    if (v.Phi() < -1.57) return true;
    if (v.Phi() > -0.87) return true;
    return false;
  }

  bool PassCommon(Jet& j) {
    bool pass = true;
    pass &= (j.MaxPt() >= 30.);
    pass &= (j.JetId >= 4);
    pass &= (j.Eta() < 5.0); //added to accommodate PU ID limits
    return pass;
  }

  void ReadJets() {
    Jets.clear();
    AllJets.clear();
    // if (!PassedHEMCut) return;
    int emptysequence = 0;
    if (evts->nJet > evts->nJetMax) cout << "Error: nJet = " << evts->nJet << " at iEvent = " << iEvent << endl;
    for (unsigned i = 0; i < evts->nJet; ++i) {
      if (emptysequence >= 3) break; // 3 empty jets in a row, skip the rest
      if (evts->Jet_pt_nom[i] == 0) emptysequence++;
      else emptysequence = 0;
      Jet tmp;
      tmp.index = i;
      tmp.SetPtEtaPhiM(evts->Jet_pt_nom[i],evts->Jet_eta[i],evts->Jet_phi[i],evts->Jet_mass_nom[i]); //the nominal here in MC contains JER while nanoAOD default does not
      if (!PassHEMCut(tmp)) {
        PassedHEMCut = false;
        // return;
      }
      tmp.JESup().SetPtEtaPhiM(evts->Jet_pt_jesTotalUp[i], evts->Jet_eta[i], evts->Jet_phi[i], evts->Jet_mass_jesTotalUp[i]);
      tmp.JESdown().SetPtEtaPhiM(evts->Jet_pt_jesTotalDown[i], evts->Jet_eta[i], evts->Jet_phi[i], evts->Jet_mass_jesTotalDown[i]);
      tmp.JERup().SetPtEtaPhiM(evts->Jet_pt_jerUp[i], evts->Jet_eta[i], evts->Jet_phi[i], evts->Jet_mass_jerUp[i]);
      tmp.JERdown().SetPtEtaPhiM(evts->Jet_pt_jerDown[i], evts->Jet_eta[i], evts->Jet_phi[i], evts->Jet_mass_jerDown[i]);
      tmp.JetId = evts->Jet_jetId[i];
      if (conf->AuxHistCreation) AllJets.push_back(tmp);
      if (!PassCommon(tmp)) continue;

      //set PUID flags
      // tmp.PUIDpasses = PUID(tmp.Pt(), fabs(tmp.Eta()), evts->Jet_puId[i], conf->SampleYear);
      if (tmp.Pt() >= 50) tmp.PUIDpasses = {true, true, true};
      else tmp.PUIDpasses = {(evts->Jet_puId[i]>= 4), (evts->Jet_puId[i]>=6), (evts->Jet_puId[i]==7)};

      //set PUID SFs
      if((IsMC || conf->Compare_PUIDSF) && evts->Jet_pt_nom[i] < 50. && evts->Jet_genJetIdx[i] >= 0){ //unlike other SFs, PU Jets and jets failing ID are not supposed to contribute to event weights
        vector<vector<float> > PUIDSFs = {{1,1,1},{1,1,1},{1,1,1}};
        if (conf->UseSkims_PUIDSF || conf->Compare_PUIDSF) {
          if(tmp.PUIDpasses[0]){
            PUIDSFs[0][0] = evts->Jet_puIdScaleFactorLoose[i];
            PUIDSFs[1][0] = evts->Jet_puIdScaleFactorLooseUp[i];
            PUIDSFs[2][0] = evts->Jet_puIdScaleFactorLooseDown[i];
          }
          if(tmp.PUIDpasses[1]){
            PUIDSFs[0][1] = evts->Jet_puIdScaleFactorMedium[i];
            PUIDSFs[1][1] = evts->Jet_puIdScaleFactorMediumUp[i];
            PUIDSFs[2][1] = evts->Jet_puIdScaleFactorMediumDown[i];
          }
          if(tmp.PUIDpasses[2]){
            PUIDSFs[0][2] = evts->Jet_puIdScaleFactorTight[i];
            PUIDSFs[1][2] = evts->Jet_puIdScaleFactorTightUp[i];
            PUIDSFs[2][2] = evts->Jet_puIdScaleFactorTightDown[i];
          }
        }
        if (conf->Compare_PUIDSF) {
          R_PUIDSF->CompareScaleFactors(tmp, PUIDSFs);
          conf->Compare_PUIDSF--;
        }
        if (!conf->UseSkims_PUIDSF) {
          PUIDSFs = R_PUIDSF->GetScaleFactors(tmp);
        }
        tmp.PUIDSFweights = PUIDSFs;
      }

      //set generator information
      if (IsMC) {
        tmp.genJetIdx = evts->Jet_genJetIdx[i];
        tmp.hadronFlavour = evts->Jet_hadronFlavour[i];
        tmp.partonFlavour = evts->Jet_partonFlavour[i];
      }

      //set btagging flags
      tmp.Jet_btagDeepFlavB = evts->Jet_btagDeepFlavB[i];
      tmp.bTagPasses = bTag(evts->Jet_btagDeepFlavB[i], conf->SampleYear);
      

      //set btagging SFs
      tmp.bJetSFweightsCorr = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}};
      tmp.bJetSFweightsUncorr = {{1.,1.,1.}, {1.,1.,1.}, {1.,1.,1.}};
      if (IsMC || conf->Compare_bTagSF) {
        //FIXME: Need b-tagging efficiency per sample at some point, see https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagSFMethods#b_tagging_efficiency_in_MC_sampl
        vector<vector<float> > bTSFsCorr = {{1,1,1},{1,1,1},{1,1,1}};
        vector<vector<float> > bTSFsUncorr = {{1,1,1},{1,1,1},{1,1,1}};
        if (conf->UseSkims_bTagSF || conf->Compare_bTagSF) {
          bTSFsCorr[0] = {evts->Jet_bTagScaleFactorLoose[i], evts->Jet_bTagScaleFactorMedium[i], evts->Jet_bTagScaleFactorTight[i]};
          bTSFsCorr[1] = {evts->Jet_bTagScaleFactorLooseUpCorrelated[i], evts->Jet_bTagScaleFactorMediumUpCorrelated[i], evts->Jet_bTagScaleFactorTightUpCorrelated[i]};
          bTSFsCorr[2] = {evts->Jet_bTagScaleFactorLooseDownCorrelated[i], evts->Jet_bTagScaleFactorMediumDownCorrelated[i], evts->Jet_bTagScaleFactorTightDownCorrelated[i]};
          bTSFsUncorr[0] = {1, 1, 1};
          bTSFsUncorr[1] = {evts->Jet_bTagScaleFactorLooseUpUncorrelated[i], evts->Jet_bTagScaleFactorMediumUpUncorrelated[i], evts->Jet_bTagScaleFactorTightUpUncorrelated[i]};
          bTSFsUncorr[2] = {evts->Jet_bTagScaleFactorLooseDownUncorrelated[i], evts->Jet_bTagScaleFactorMediumDownUncorrelated[i], evts->Jet_bTagScaleFactorTightDownUncorrelated[i]};
        }
        if (conf->Compare_bTagSF) {
          R_BTSF->CompareScaleFactors(tmp, bTSFsCorr);
          conf->Compare_bTagSF--;
        }
        if (!conf->UseSkims_bTagSF) {
          bTSFsCorr = R_BTSF->GetScaleFactors(tmp);
        }
        
        if (IsMC) {
          vector<float> bTag_Eff = bTE->GetEff(tmp);
          for (unsigned iv = 0; iv < 3; ++iv) {
            for (unsigned iwp = 0; iwp < 3; ++iwp) {
              if (tmp.bTagPasses[iwp]) {
                tmp.bJetSFweightsCorr[iv][iwp] = bTSFsCorr[iv][iwp];
                tmp.bJetSFweightsUncorr[iv][iwp] = bTSFsUncorr[iv][iwp];
              }
              else {
                tmp.bJetSFweightsCorr[iv][iwp] = (1. - bTag_Eff[iwp] * bTSFsCorr[iv][iwp]) / (1. - bTag_Eff[iwp]);
                tmp.bJetSFweightsUncorr[iv][iwp] = (1. - bTag_Eff[iwp] * bTSFsUncorr[iv][iwp]) / (1. - bTag_Eff[iwp]);
              }
              if (tmp.Pt() < 20 || tmp.Pt() > 1000 || fabs(tmp.Eta()) >= 2.4999) {
                tmp.bJetSFweightsCorr[iv][iwp] = 1.0;
                tmp.bJetSFweightsUncorr[iv][iwp] = 1.0;
                continue;
              }
              if (conf->bTagEffHistCreation) continue;
              if (tmp.bJetSFweightsCorr[iv][iwp] < 0 || tmp.bJetSFweightsCorr[iv][iwp] != tmp.bJetSFweightsCorr[iv][iwp]) {
                cout << "In bTag WP " << iwp << " , " << iv << " variation, ";
                if (tmp.bTagPasses[iwp]) cout << " bTagged ";
                else cout << " Non-bTagged ";
                cout << Form("Jet %i (pT = %f, eta = %f) has unexpected bJetSFweightsCorr. Eff = %f, SF = %f", i, tmp.Pt(), tmp.Eta(), bTag_Eff[iwp], bTSFsCorr[iv][iwp]) << endl;
              }
              if (tmp.bJetSFweightsUncorr[iv][iwp] < 0 || tmp.bJetSFweightsUncorr[iv][iwp] != tmp.bJetSFweightsUncorr[iv][iwp]) {
                cout << "In bTag WP " << iwp << " , " << iv << " variation, ";
                if (tmp.bTagPasses[iwp]) cout << " bTagged ";
                else cout << " Non-bTagged ";
                cout << Form("Jet %i (pT = %f, eta = %f) has unexpected bJetSFweightsUncorr. Eff = %f, SF = %f", i, tmp.Pt(), tmp.Eta(), bTag_Eff[iwp], bTSFsUncorr[iv][iwp]) << endl;
              }
              if (tmp.bJetSFweightsCorr[iv][iwp] == 0) tmp.bJetSFweightsCorr[iv][iwp] = 1.0; // Final safeguard
              if (tmp.bJetSFweightsUncorr[iv][iwp] == 0) tmp.bJetSFweightsUncorr[iv][iwp] = 1.0; // Final safeguard
            }
          }
        }
      }
      Jets.push_back(tmp);
    }
  }

  void ReadTriggers() {
    Triggers.clear();
    // if (!PassedHEMCut) return;
    isolated_electron_trigger = evts->isolated_electron_trigger;
    isolated_muon_trigger = evts->isolated_muon_trigger;
    isolated_muon_track_trigger = evts->isolated_muon_track_trigger;
    for (unsigned i = 0; i < evts->nTrigObj; ++i) {
      if (evts->TrigObj_id[i] == 13 || evts->TrigObj_id[i] == 11) {
        Trigger tmp;
        tmp.SetPtEtaPhiM(evts->TrigObj_pt[i], evts->TrigObj_eta[i], evts->TrigObj_phi[i],0);
        tmp.index = i;
        tmp.id = evts->TrigObj_id[i];
        tmp.filterBits = evts->TrigObj_filterBits[i];
        Triggers.push_back(tmp);
      }
    }
  }

  bool TriggerMatch(Electron e) {
    if (!evts->isolated_electron_trigger) return false;
    for (unsigned i = 0; i < Triggers.size(); ++i) {
      if (Triggers[i].id != 11) continue;
      if (e.DeltaR(Triggers[i]) > 0.1) continue;
      if (conf->SampleYear == "2017" && !(1024 & Triggers[i].filterBits)) continue;
      return true;
    }
    return false;
  }
  bool PassCommon(Electron e) {
    bool pass = true;
    float absEta = fabs(e.Eta());
    pass &= (absEta < 2.4);
    pass &= (absEta < 1.44 || absEta > 1.57);
    pass &= (e.MaxPt() >= 10.);
    return pass;
  }
  bool PassPrimary(Electron e, int iv = -1) { // iv: Variation. -1: MaxPt, 0-4, nominal and variations
    bool pass = true;
    pass &= e.TriggerMatched;
    if (iv < 0) pass &= (e.MaxPt() > 30.);
    else pass &= (e.v(iv).Pt() > 30.);
    // pass &= e.cutBasedHEEP;
    pass &= e.mva;
    return pass;
  }
  bool PassVeto(Electron e, int iv = -1) { // iv: Variation. -1: MaxPt, 0-4, nominal and variations
    bool pass = true;
    if (iv < 0) pass &= (e.MaxPt() > 10.);
    else pass &= (e.v(iv).Pt() > 10.);
    pass &= (e.cutBased >= 2);
    return pass;
  }
  bool PassVetoPick(Electron e, int iv = -1) { // iv: Variation. -1: MaxPt, 0-4, nominal and variations
    bool pass = true;
    pass &= e.TriggerMatched;
    if (iv < 0) pass &= (e.MaxPt() > 27.);
    else pass &= (e.v(iv).Pt() > 27.);
    pass &= (e.cutBased >= 2);
    return pass;
  }
  bool PassLoose(Electron e, int iv = -1) { // iv: Variation. -1: MaxPt, 0-4, nominal and variations
    bool pass = true;
    pass &= e.TriggerMatched;
    if (iv < 0) pass &= (e.MaxPt() > 30.);
    else pass &= (e.v(iv).Pt() > 30.);
    pass &= (e.cutBased >= 1);
    return pass;
  }

  bool TriggerMatch(Muon m) {
    if (!isolated_muon_trigger && !isolated_muon_track_trigger) return false;
    for (unsigned i = 0; i < Triggers.size(); ++i) {
      if (Triggers[i].id != 13) continue;
      if (m.DeltaR(Triggers[i]) > 0.1) continue;
      return true;
    }
    return false;
  }
  bool PassCommon(Muon m) {
    bool pass = true;
    float absEta = fabs(m.Eta());
    pass &= (absEta < 2.4);
    pass &= (m.MaxPt() >= 10.);
    return pass;
  }
  bool PassPrimary(Muon m) {
    bool pass = true;
    pass &= m.TriggerMatched;
    pass &= (m.Pt() > 27.);
    pass &= (m.relIso < 0.15);
    pass &= m.tightId;
    return pass;
  }
  bool PassVeto(Muon m) {
    bool pass = true;
    pass &= (m.Pt() > 10.);
    pass &= (m.relIso < 0.25);
    pass &= m.looseId;
    return pass;
  }
  bool PassVetoPick(Muon m) { // iv: Variation. -1: MaxPt, 0-4, nominal and variations
    bool pass = true;
    pass &= m.TriggerMatched;
    pass &= (m.Pt() > 27.);
    pass &= (m.relIso > 0.15);
    pass &= m.looseId;
    return pass;
  }
  bool PassLoose(Muon m) {
    bool pass = true;
    pass &= m.TriggerMatched;
    pass &= (m.Pt() > 27.);
    pass &= (m.relIso < 1.5 && m.relIso > 0.15);
    pass &= m.looseId;
    return pass;
  }

  void ReadLeptons() {
    Leptons.clear();
    Electrons.clear();
    Muons.clear();
    // if (!PassedHEMCut) return;
    int emptysequence = 0;
    for (unsigned i = 0; i < evts->nElectron; ++i) {
      if (emptysequence >= 3) break;
      if (evts->Electron_pt[i] == 0) {
        emptysequence++;
        continue;
      }
      else emptysequence = 0;
      Electron tmp;
      // cout << "Electron i = " << i << ", nElectron = " << evts->nElectron <<endl;
      tmp.SetPtEtaPhiM(evts->Electron_pt[i],evts->Electron_eta[i],evts->Electron_phi[i],evts->Electron_mass[i]);
      if (!PassHEMCut(tmp)) {
        PassedHEMCut = false;
        // return;
      }
      //set resolution variations (only matter in MC, will be ineffective in data)
      tmp.ResUp() = ((TLorentzVector) tmp) * ((tmp.E() - evts->Electron_dEsigmaUp[i]) / tmp.E());
      tmp.ResDown() = ((TLorentzVector) tmp) * ((tmp.E() - evts->Electron_dEsigmaDown[i]) / tmp.E());
      // set scale variations (only filled in data, should be applied to MC, for now FIXME inactive)
      // tmp.ScaleUp() = ((TLorentzVector) tmp) * (tmp.E() - evts->Electron_dEscaleUp[i]) / tmp.E();
      // tmp.ScaleDown() = ((TLorentzVector) tmp) * (tmp.E() - evts->Electron_dEscaleDown[i]) / tmp.E();
      tmp.ScaleUp() = tmp;
      tmp.ScaleDown() = tmp;
      tmp.index = i;
      tmp.charge = evts->Electron_charge[i];
      tmp.cutBased = evts->Electron_cutBased[i];
      tmp.cutBasedHEEP = evts->Electron_cutBased_HEEP[i];
      tmp.mva = evts->Electron_mvaFall17V2Iso_WP90[i];

      if (!PassCommon(tmp)) continue;

      //check for jet overlaps
      tmp.JetProximity = JetProximityCheck(tmp);
      tmp.TriggerMatched = TriggerMatch(tmp);      

      //set SF and variation for primary only, HEEP as in https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaRunIIRecommendations#HEEPV7_0
      tmp.SFs = {1., 1., 1.};
      if(PassPrimary(tmp,0) && IsMC){
        tmp.SFs[0] = evts->Electron_scaleFactor[i];
        tmp.SFs[1] = evts->Electron_scaleFactorUp[i];
        tmp.SFs[2] = evts->Electron_scaleFactorDown[i];
      }

      Electrons.push_back(tmp);
      Leptons.push_back(tmp);
    }

    emptysequence = 0;
    bool ZeroMuonErrOccurred = false;
    for (unsigned i = 0; i < evts->nMuon; ++i) {
      if (emptysequence >= 3) break; // [9] is the hard cap of Muon array size
      if (evts->Muon_pt[i] == 0) {
        emptysequence++;
        continue;
      }
      else emptysequence = 0;
      Muon tmp;
      tmp.SetPtEtaPhiM(evts->Muon_pt[i],evts->Muon_eta[i],evts->Muon_phi[i],evts->Muon_mass[i]);
      if (!PassHEMCut(tmp)) {
        PassedHEMCut = false;
        // return;
      }
      tmp.index = i;
      tmp.charge = evts->Muon_charge[i];
      tmp.tightId = evts->Muon_tightId[i];
      tmp.looseId = evts->Muon_looseId[i];
      tmp.relIso = evts->Muon_pfRelIso04_all[i];

      //Dummy for scale variations, not to be used without Rochester corrections (not compulsory)
      TLorentzVector dummy;
      dummy.SetPtEtaPhiM(evts->Muon_pt[i],evts->Muon_eta[i],evts->Muon_phi[i],evts->Muon_mass[i]);
      tmp.ResUp() = dummy;
      tmp.ResDown() = dummy;
      tmp.ScaleUp() = dummy;
      tmp.ScaleDown() = dummy;

      if (!PassCommon(tmp)) continue;

      //check for jet overlaps
      tmp.JetProximity = JetProximityCheck(tmp);
      tmp.TriggerMatched = TriggerMatch(tmp);    

      // //CommonSelectionBlock
      // float absEta = fabs(tmp.Eta());
      // bool passCommon = (fabs(tmp.Eta()) < 2.4);
      // passCommon &= (tmp.Pt() > 10.);
      // passCommon &= (evts->Muon_pfRelIso04_all[i] < 0.25);
      // passCommon &= !(tmp.JetProximity[conf->PUIDWP]);

      // //TripleSelectionBlock
      // bool passVeto = (evts->Muon_looseId[i]) && passCommon;
      // passCommon &= (tmp.Pt() > 35.); //change pT cut for non-veto muons to be on trigger plateau
      // bool passLoose = (evts->Muon_tightId[i]) && passCommon;
      // passCommon &= (evts->Muon_pfRelIso04_all[i] < 0.15); //change isolation cut for primary muons
      // bool passPrimary = (evts->Muon_tightId[i]) && passCommon;

      // tmp.IsPrimary = passPrimary;
      // tmp.IsLoose = passLoose;
      // tmp.IsVeto = passVeto;

      // if(!passVeto && !passLoose && !passPrimary) continue;

      //set SF and variation for primary only
      if(PassPrimary(tmp) && IsMC){
        if (evts->Muon_triggerScaleFactor[i] * evts->Muon_idScaleFactor[i] * evts->Muon_isoScaleFactor[i] == 0) {
          if (MuonSFZeroErrors < 100) cout << "Zero muon SF, triggerSF = " << evts->Muon_triggerScaleFactor[i] << " , idSF = " << evts->Muon_idScaleFactor[i] << " , isoSF = " << evts->Muon_isoScaleFactor[i] <<endl;
          MuonSFZeroErrors++;
          ZeroMuonErrOccurred = true;
        }
        else {
          tmp.triggerSFs[0] = evts->Muon_triggerScaleFactor[i];
          tmp.triggerSFs[1] = evts->Muon_triggerScaleFactorSystUp[i];
          tmp.triggerSFs[2] = evts->Muon_triggerScaleFactorSystDown[i];
          tmp.idSFs[0] = evts->Muon_idScaleFactor[i];
          tmp.idSFs[1] = evts->Muon_idScaleFactorSystUp[i];
          tmp.idSFs[2] = evts->Muon_idScaleFactorSystDown[i];
          tmp.isoSFs[0] = evts->Muon_isoScaleFactor[i];
          tmp.isoSFs[1] = evts->Muon_isoScaleFactorSystUp[i];
          tmp.isoSFs[2] = evts->Muon_isoScaleFactorSystDown[i];
        }
      }

      Muons.push_back(tmp);
      Leptons.push_back(tmp);
    }
    if (ZeroMuonErrOccurred) MuonSFZeroErrorEvts++;
    if (evts->nElectron > evts->nElectronMax) cout << "Error: nElectron = " << evts->nElectron << " at iEvent = " << iEvent << endl;
    if (evts->nMuon > evts->nMuonMax) cout << "Error: nMuon = " << evts->nMuon << " at iEvent = " << iEvent << endl;
  }

  void ReadMET() {
    Met.v() = TLorentzVector();

    //conversion of the constant SampleYear into the correction's year string where needed
    string convertedYear = "";
    if(conf->SampleYear      == "2016apv") convertedYear = "2016APV";
    else if(conf->SampleYear == "2016")    convertedYear = "2016nonAPV";
    else                                   convertedYear = conf->SampleYear;


    if(IsMC){
      TLorentzVector JESup, JESdown, JERup, JERdown;

      std::pair<double,double> METXYCorr = METXYCorr_Met_MetPhi(evts->MET_T1Smear_pt, evts->MET_T1Smear_phi, evts->run, convertedYear, IsMC, evts->PV_npvs, true, false);
      Met.SetPtEtaPhiM(METXYCorr.first, 0, METXYCorr.second, 0);

      std::pair<double,double> METXYCorr_JESup = METXYCorr_Met_MetPhi(evts->MET_T1Smear_pt_jesTotalUp, evts->MET_T1Smear_phi_jesTotalUp, evts->run, convertedYear, IsMC, evts->PV_npvs, true, false);
      JESup.SetPtEtaPhiM(METXYCorr_JESup.first, 0, METXYCorr_JESup.second, 0);
      Met.JESup() = JESup;

      std::pair<double,double> METXYCorr_JESdown = METXYCorr_Met_MetPhi(evts->MET_T1Smear_pt_jesTotalDown, evts->MET_T1Smear_phi_jesTotalDown, evts->run, convertedYear, IsMC, evts->PV_npvs, true, false);
      JESdown.SetPtEtaPhiM(METXYCorr_JESdown.first, 0, METXYCorr_JESdown.second, 0);
      Met.JESdown() = JESdown;

      std::pair<double,double> METXYCorr_JERup = METXYCorr_Met_MetPhi(evts->MET_T1Smear_pt_jerUp, evts->MET_T1Smear_phi_jerUp, evts->run, convertedYear, IsMC, evts->PV_npvs, true, false);
      JERup.SetPtEtaPhiM(METXYCorr_JERup.first, 0, METXYCorr_JERup.second, 0);
      Met.JERup() = JERup;

      std::pair<double,double> METXYCorr_JERdown = METXYCorr_Met_MetPhi(evts->MET_T1Smear_pt_jerDown, evts->MET_T1Smear_phi_jerDown, evts->run, convertedYear, IsMC, evts->PV_npvs, true, false);
      JERdown.SetPtEtaPhiM(METXYCorr_JERdown.first, 0, METXYCorr_JERdown.second, 0);
      Met.JERdown() = JERdown;
    }
    else{
      TLorentzVector dummy;

      std::pair<double,double> METXYCorr = METXYCorr_Met_MetPhi(evts->MET_T1_pt, evts->MET_T1_phi, evts->run, convertedYear, IsMC, evts->PV_npvs, true, false);
      Met.SetPtEtaPhiM(METXYCorr.first, 0, METXYCorr.second, 0);

      dummy.SetPtEtaPhiM(METXYCorr.first, 0, METXYCorr.second, 0);
      Met.JESup() = dummy;
      Met.JESdown() = dummy;
      Met.JERup() = dummy;
      Met.JERdown() = dummy;
    }
  }

  void ReadGenMET() {
    GenMet = TLorentzVector();
    GenMet.SetPtEtaPhiM(evts->GenMET_pt, 0, evts->GenMET_phi, 0);
  }

  void ReadPileup() {
    Pileup_nPU = evts->Pileup_nPU;
    Pileup_nTrueInt = evts->Pileup_nTrueInt;
  }

  void ReadVertices() {
    PV_npvs = evts->PV_npvs;
    PV_npvsGood = evts->PV_npvsGood;
  }

  bool ReadMETFilterStatus() {
    bool status = true;
    status = status && evts->Flag_goodVertices;
    status = status && evts->Flag_globalSuperTightHalo2016Filter;
    status = status && evts->Flag_HBHENoiseFilter;
    status = status && evts->Flag_HBHENoiseIsoFilter;
    status = status && evts->Flag_EcalDeadCellTriggerPrimitiveFilter;
    status = status && evts->Flag_BadPFMuonFilter;
    status = status && evts->Flag_BadPFMuonDzFilter;
    status = status && evts->Flag_eeBadScFilter;
    status = status && evts->Flag_ecalBadCalibFilter;
    // cout << "1 " << evts->Flag_goodVertices << " 2 " << evts->Flag_globalSuperTightHalo2016Filter << " 3 " << evts->Flag_HBHENoiseFilter <<endl;
    return status;
  }

  //function to determine all regions an event belongs to as a function of all object pT variations
  RegionID RegionReader(){
    RegionID rids;
    HasRegionsOfInterest = false;
    //loop over variations: nominal, e-scale up, e-scale down, e-res up, e-res down, JES up, JES down, JER up, JER down
    for(unsigned i = 0; i < rids.RegionCount; ++i){
      int RegionNumber = -1; //-1 no region , -2 not pass jet proximity, -3 not Pass HEMCut
      int iPrimaryLep(-1), iLooseLep(-1), iVetoLep(-1);
      //check lepton multiplicity
      int nev(0), nel(0), nep(0);
      for(unsigned j = 0; j<Electrons.size(); ++j){
        if (PassPrimary(Electrons[j], i)) {
          nep++;
          iPrimaryLep = j;
        }
        else if (PassVeto(Electrons[j], i)) {
          nev++;
          if (PassVetoPick(Electrons[j], i)) iVetoLep = j;
        }
        else if (PassLoose(Electrons[j], i)) {
          nel++;
          iLooseLep = j;
        }
      }

      int nmuv(0), nmul(0), nmup(0);
      for(unsigned j = 0; j<Muons.size(); ++j){//no scale variations foreseen as of yet, due to not applying Rochester corrections
        if (PassPrimary(Muons[j])) {
          nmup++;
          iPrimaryLep = j;
        }
        else if (PassVeto(Muons[j])) {
          nmuv++;
          if (PassVetoPick(Muons[j])) iVetoLep = j;
        }
        else if (PassLoose(Muons[j])) {
          nmul++;
          iLooseLep = j;
        }
      }

      if ( (nep + nmup == 1) && (nev + nmuv == 0)) { // Signal Region
        if (nep == 1) {
          RegionNumber = 2100;
          TheLepton = Electrons[iPrimaryLep];
        }
        else {
          RegionNumber = 1100;
          TheLepton = Muons[iPrimaryLep];
        }
      }
      else if ((nep + nmup == 0) && (nev + nmuv == 0) && (nel + nmul == 1)) { // Background Estimation Region 1
        if (nel == 1) {
          RegionNumber = 2200;
          TheLepton = Electrons[iLooseLep];
        }
        else {
          RegionNumber = 1200;
          TheLepton = Muons[iLooseLep];
        }
      }
      else if ((nep + nmup == 0) && (nev + nmuv == 1) && (nel + nmul == 0) && (iVetoLep != -1)) { // Background Estimation Region
        if (nev == 1) {
          RegionNumber = 2300;
          TheLepton = Electrons[iVetoLep];
        }
        else {
          RegionNumber = 1300;
          TheLepton = Muons[iVetoLep];
        }
      }
      else {
        if (conf->Debug("LeptonRegion") && i == 0) cout << Form("In Event %i, Variation %i, Electron (Primary, Veto, Loose) = (%i, %i, %i), Muon (Primary, Veto, Loose) = (%i, %i, %i)"
          , (int)iEvent, i, nep, nev, nel, nmup, nmuv, nmul) << endl;
        rids.Regions[i] = -1; // Lepton failed to fall into any region
        continue;
      }

      //check jet multiplicity
      int nj = 0;
      int nb = 0;
      for(unsigned j = 0; j<Jets.size(); ++j){
        if(Jets[j].v(i).Pt() < 30.) continue;
        if(!Jets[j].PUIDpasses[PUIDWP]) continue;//select working point for PUID to none by commenting this line out, loose by PUIDpasses 0, medium by 1, tight by 2
        nj++;
        if(Jets[j].bTagPasses[bTagWP]) nb++;//select working point for b-tagging by bTagPasses[0] = loose, 1 medium and 2 tight
      }
      // if(nj<5 || nj>6) {
      //   rids.Regions[i] = -3;
      //   continue; //in no region we're interested in
      // }
      RegionNumber += nj*10;
      RegionNumber += nb;
      rids.Regions[i]=RegionNumber;
      for (unsigned j = 0; j < conf->AcceptedRegions.size(); ++j) {
        if (RegionNumber != conf->AcceptedRegions[j]) continue;
        // When the raw region is within the region, we start to assign the error region codes.
        if (TheLepton.JetProximity[conf->PUIDWP]) {
          rids.Regions[i] = -2;
        }
        if (!PassedHEMCut) {
          rids.Regions[i] = -3;
        }
        if ((!PassedHEMCut) && (TheLepton.JetProximity[conf->PUIDWP])) {
          rids.Regions[i] = -4;
        }
      }
      for (unsigned j = 0; j < conf->AcceptedRegions.size(); ++j) {
        // Check if the iteration care about the error regions
        if (RegionNumber == conf->AcceptedRegions[j]) HasRegionsOfInterest = true;
      }
    }
    return rids;
  }

  //function to derive event weights from SFs on objects
  vector<pair<double, string> > CalcEventSFweights() {
    vector<EventWeight> SFweights;
    //set SF weights per object
    EventWeight electronW, muonTriggerW, muonIdW, muonIsoW, BjetWCorr, BjetWUncorr, PUIDW, L1PreFiringW, PUreweight, PDFWeight, LHEScaleW;
    electronW.source = "electron";
    muonTriggerW.source = "muonTrigger";
    muonIdW.source = "muonId";
    muonIsoW.source = "muonIso";
    BjetWCorr.source = "BjetTagCorr";
    BjetWUncorr.source = "BjetTagUncorr";
    PUIDW.source = "PUID";
    L1PreFiringW.source = "L1PreFiring";
    PUreweight.source = "PUreweight";
    PDFWeight.source = "PDF";
    LHEScaleW.source = "LHEScale";

    if (IsMC) {
      int modes[3]={0, +1, -1};
      for(unsigned i=0; i<3; ++i){
        for(unsigned j = 0; j < Electrons.size(); ++j) electronW.variations[i] *= Electrons[j].SFs[i];
        for(unsigned j = 0; j < Muons.size(); ++j) {
          muonTriggerW.variations[i] *= Muons[j].triggerSFs[i];
          muonIdW.variations[i] *= Muons[j].idSFs[i];
          muonIsoW.variations[i] *= Muons[j].isoSFs[i];
        }
        for(unsigned j = 0; j < Jets.size(); ++j){
          BjetWCorr.variations[i] *= Jets[j].bJetSFweightsCorr[i][bTagWP];
          BjetWUncorr.variations[i] *= Jets[j].bJetSFweightsUncorr[i][bTagWP];
          if (i > 0) BjetWUncorr.variations[i] *= 1. / Jets[j].bJetSFweightsCorr[0][bTagWP]; // Fix for Corr and Uncorr nominal are combined
          PUIDW.variations[i] *= Jets[j].PUIDSFweights[i][PUIDWP];
        }
      }
      if (BjetWCorr.variations[0] != BjetWCorr.variations[0]) cout << "Caught BjetWCorr nan" <<endl;
      if (BjetWUncorr.variations[0] != BjetWUncorr.variations[0]) cout << "Caught BjetWUncorr nan" <<endl;
      // return;
      string sampleyear;
      string sy = conf->SampleYear;
      if (sy == "2016apv") sampleyear = "2016preVFP";
      else if (sy == "2016") sampleyear = "2016postVFP";
      else if (sy == "2017") sampleyear = "2017";
      else if (sy == "2018") sampleyear = "2018";
      //L1PrefiringWeight
      if(sampleyear == "2016preVFP" || sampleyear == "2016postVFP" || sampleyear == "2017"){
        L1PreFiringW.variations[0] = evts->L1PreFiringWeight_Nom;
        L1PreFiringW.variations[1] = evts->L1PreFiringWeight_Up;
        L1PreFiringW.variations[2] = evts->L1PreFiringWeight_Down;
      }

      //PUreweight
      PUreweight.variations[0] = evts->Pileup_scaleFactor;
      PUreweight.variations[1] = evts->Pileup_scaleFactorUp;
      PUreweight.variations[2] = evts->Pileup_scaleFactorDown;

      // PDF
      // Quoted Percentile definition:
      // Before giving a general definition of all percentiles, we will define the 80th percentile of a collection of values to be the smallest value in the collection that is at least as large as 80% of all of the values.
      // The lowest element is only the 0th percentile, and cannot be anything else.
      vector<float> lhepdfws;
      lhepdfws.clear();
      for (unsigned i = 0; i < evts->nLHEPdfWeight; ++i) {
        lhepdfws.push_back(evts->LHEPdfWeight[i]);
      }
      sort(lhepdfws.begin(), lhepdfws.end());
      int le = ceil(0.1587 * evts->nLHEPdfWeight); // one sigma down
      int ne = ceil(0.5 * evts->nLHEPdfWeight); // nominal
      int ue = ceil(0.8413 * evts->nLHEPdfWeight); // one sigma up
      PDFWeight.variations[0] = lhepdfws[ne];
      PDFWeight.variations[1] = lhepdfws[ue];
      PDFWeight.variations[2] = lhepdfws[le];

      // LHEScale
      vector<float> lhescalews;
      for (unsigned i = 0; i < evts->nLHEScaleWeight; ++i) {
        lhescalews.push_back(evts->LHEScaleWeight[i]);
      }
      sort(lhescalews.begin(), lhescalews.end());
      LHEScaleW.variations[0] = 1.0;
      LHEScaleW.variations[1] = lhescalews.back();
      LHEScaleW.variations[2] = lhescalews.front();
    }

    SFweights.push_back(electronW);
    SFweights.push_back(muonTriggerW);
    SFweights.push_back(muonIdW);
    SFweights.push_back(muonIsoW);
    SFweights.push_back(BjetWCorr);
    SFweights.push_back(BjetWUncorr);
    SFweights.push_back(PUIDW);
    SFweights.push_back(L1PreFiringW);
    SFweights.push_back(PUreweight);
    SFweights.push_back(PDFWeight);
    SFweights.push_back(LHEScaleW);

    // for (unsigned i = 0; i < SFweights.size(); ++i) {
    //   SFweights[i].NanCheck();
    // }

    //determine nominal event weight
    vector<pair<double, string> > EventWeight_out;
    float CentralWeight = 1.;
    for(unsigned i = 0; i < SFweights.size(); ++i){
      CentralWeight *= SFweights[i].variations[0];
    }
    EventWeight_out.push_back(make_pair(CentralWeight, "Nominal"));

    //select source for up and down variations
    for(unsigned i = 0; i < SFweights.size(); ++i){

      //create variations with strings for later combine histograms
      float weightup = 1.;
      float weightdown = 1.;
      for(unsigned j = 0; j < SFweights.size(); ++j){
        if (i == j) {
          weightup *= SFweights[j].variations[1];
          weightdown *= SFweights[j].variations[2];
        }
        else {
          weightup *= SFweights[j].variations[0];
          weightdown *= SFweights[j].variations[0];
        }
      }
      EventWeight_out.push_back(make_pair(weightup, SFweights[i].source + "_up"));
      EventWeight_out.push_back(make_pair(weightdown, SFweights[i].source + "_down"));
      // EventWeight_out.push_back(make_pair(CentralWeight / SFweights[i].variations[0] * SFweights[i].variations[1], SFweights[i].source + "_up"));
      // EventWeight_out.push_back(make_pair(CentralWeight / SFweights[i].variations[0] * SFweights[i].variations[2], SFweights[i].source + "_down"));
    }
    return EventWeight_out;
  }
  
  void BranchSizeCheck() {
    if (!IsMC) return;
    vector<TString> varnames{"GenPart_pt","GenJet_pt","Jet_pt","Electron_pt","Muon_pt",
      "TrigObj_pt","Electron_scaleFactor","LHEPdfWeight","LHEScaleWeight"};
    vector<unsigned> cursizes{evts->nGenPartMax, evts->nGenJetMax, evts->nJetMax, evts->nElectronMax, evts->nMuonMax,
      evts->nTrigObjMax, evts->nSFMax, evts->nLHEPdfWeightMax, evts->nLHEScaleWeightMax};
    vector<unsigned> evtsizes{evts->nGenPart, evts->nGenJet, evts->nJet, evts->nElectron, evts->nMuon,
      evts->nTrigObj, 10, evts->nLHEPdfWeight, evts->nLHEScaleWeight};
    if (varnames.size() != cursizes.size()) cout << "Please double check the varnames and cursizes container contents. Lenghth is differnet" << endl;
    for (unsigned i = 0; i < varnames.size(); ++i) {
      unsigned newsize = chain->GetLeaf(varnames[i])->GetLen();
      if (newsize > cursizes[i]) cout << endl << varnames[i] << " has size = " << newsize << " exceeded current fixed size = " << cursizes[i] << endl;
      if (evtsizes[i] > cursizes[i]) cout << endl << varnames[i] << "has nsize = " << evtsizes[i] << "exceeded current fixed size = " << cursizes[i] <<endl;
    }
  }

  void RunEndSummary() {
    double em = ChainEntries;
    cout << endl << "NanoAODReader Summary:" << endl;
    if (MuonSFZeroErrors > 0) {
      double nerrevts = MuonSFZeroErrorEvts;
      cout << "Muon SF zeros occurance = " << MuonSFZeroErrors << " in " << nerrevts << " Events out of total Events " << ChainEntries;
      cout << ", Event rate = " << nerrevts / em << endl;
    }
  }

  Configs *conf;

  Long64_t iEvent, iEventInChain, EntryBegin, EntryEnd;
  Long64_t ChainEntries, EntriesMax;

  bTagSFReader *R_BTSF;
  PUIDSFReader *R_PUIDSF;

  bool IsMC;
  bool PassedHEMCut;
  bool HasRegionsOfInterest;
  int bTagWP, PUIDWP;
  TChain* chain;
  Events* evts;
  bTagEff* bTE;
  int run, luminosityBlock;
  vector<GenPart> GenParts;
  vector<GenJet> GenJets;
  vector<Jet> Jets, AllJets;
  vector<Trigger> Triggers;
  vector<Lepton> Leptons;
  Lepton TheLepton;
  vector<Electron> Electrons;
  vector<Muon> Muons;
  MET Met;
  GenMET GenMet;

  bool isolated_electron_trigger, isolated_muon_trigger, isolated_muon_track_trigger;
  int Pileup_nPU;
  float Pileup_nTrueInt;
  int PV_npvs, PV_npvsGood;

  int MuonSFZeroErrors = 0;
  int MuonSFZeroErrorEvts = 0;
  // nlohmann::json GoodSections;
  // bool LumiStatus;

  RegionID RegionAssociations;
  vector<pair<double, string> > EventWeights;
};


#endif
