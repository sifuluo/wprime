#ifndef DRAWDATAFORMAT_CC
#define DRAWDATAFORMAT_CC

#include <vector>
#include <map>
#include <string>

#include "TString.h"
#include "TH1.h"
#include "TFile.h"
#include <TApplication.h>

#include "../../Utilities/DataFormat.cc"
#include "../../Utilities/Dataset.cc"
#include "Regions.cc"

class Histograms{
public:
  Histograms() {
    Init();
  };
  
  ~Histograms() {
    fouts.clear();
    Hists.clear();
  }

  void Init() {
    Hists.clear();
    SampleTypes = dlib.DatasetNames;
    Variations = rm.Variations;
    Regions = rm.StringRanges;
  }

  void SetSampleTypes(vector<string> sts) {SampleTypes = sts;}
  void SetVariations(vector<string> vars) {Variations = vars;}

  // TString GetHistName(int io, int ist, int iv, int ir) {
  //   TString histname = StandardNames::HistName;
  //   histname.ReplaceAll("=SampleType=", SampleTypes[ist]);
  //   histname.ReplaceAll("=Variation=", Variations[iv]);
  //   histname.ReplaceAll("=RegionRange=", Regions[ir]);
  //   histname.ReplaceAll("=Observable=", Observables[io]);
  //   return histname;
  // }

  // Creating Histograms
  void AddObservable(string ob, int nbins_, double xlow_, double xup_) {
    Observables.push_back(ob);
    nbins.push_back(nbins_);
    xlow.push_back(xlow_);
    xup.push_back(xup_);
  }

  void CreateHistograms(string path, string prefix, string st = "", int ifile = -1) {
    Hists.clear();
    Hists.resize(Observables.size());
    cout << "Output is saved as: " << StandardNames::HistFileName(path, prefix, "Obs", st, ifile) << endl;
    cout << "Creating Histograms for Observables: ";
    for (unsigned io = 0; io < Observables.size(); ++io) {
      cout  << Observables[io] << ", ";
      Hists[io].resize(SampleTypes.size());
      TString fn = StandardNames::HistFileName(path, prefix, Observables[io], st, ifile);
      fouts.push_back(new TFile(fn, "RECREATE"));
      fouts[io]->cd();
      for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
        if (st != "" && SampleTypes[ist] != st) continue;
        Hists[io][ist].resize(Variations.size());
        for (unsigned iv = 0; iv < Variations.size(); ++iv) {
          Hists[io][ist][iv].resize(Regions.size());
          for (unsigned ir = 0; ir < Regions.size(); ++ir) {
            // TString histname = GetHistName(io, ist, iv, ir);
            TString histname = StandardNames::HistName(SampleTypes[ist], Observables[io], Regions[ir], Variations[iv]);
            Hists[io][ist][iv][ir] = new TH1F(histname, histname, nbins[io], xlow[io], xup[io]);
          }
        }
      }
    }
    cout << endl;
  }

  int Fill(string ob, int ist, int iv, int rid, float x, float w) {
    if (x !=x ) {
      cout << Form("Skipping filling nan value to ob %s, ist %i, iv %i, region %i",ob.c_str(), ist, iv, rid) <<endl;
      return 1;
    }
    if (w != w) {
      cout << Form("Skipping filling nan weight to ob %s, ist %i, iv %i, region %i",ob.c_str(), ist, iv, rid) <<endl;
      return 2;
    }
    int io = -1;
    for (unsigned iob_ = 0; iob_ < Observables.size(); ++iob_) {
      if (Observables[iob_] == ob) {
        io = iob_;
        break;
      }
    }
    if (io < 0) {
      string msg = "Observable not found to fill: " + ob;
      cout << msg << endl;
      throw runtime_error(msg);
    }
    int ir = rm.GetRangeIndex(rid);
    if (ir < 0) return 3;
    Hists[io][ist][iv][ir]->Fill(x,w);
    return 0;
  }

  void SetCurrentFill(int ist, int iv, int rid, float w) {
    tmp_ist = ist;
    tmp_iv = iv;
    tmp_rid = rid;
    tmp_w = w;
  }

  int Fill(string ob, float x) {
    return Fill(ob, tmp_ist, tmp_iv, tmp_rid, x, tmp_w);
  }

  void PostProcess() {
    for (unsigned i = 0; i < Observables.size(); ++i) {
      fouts[i]->Write();
      fouts[i]->Save();
      fouts[i]->Close();
    }
    cout << "Done Post Processing" <<endl;
  }

  vector<TFile*> fouts;
  vector< vector< vector< vector<TH1F*> > > > Hists; // Hists[iobservable][isampletype][ivariation][irange]
  vector<string> SampleTypes, Variations, Regions, Observables;
  vector<int> nbins; // [nObservables]
  vector<double> xlow, xup; // [nObservables]

  int tmp_ist, tmp_iv, tmp_rid;
  float tmp_w;
};

#endif