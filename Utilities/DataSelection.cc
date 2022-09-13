#ifndef DATASELECTION_CC
#define DATASELECTION_CC

#include <TString.h>

#include <fstream>
#include <string>
#include <vector>

#include "DataFormat.cc"
#include "nlohmann/json.hpp"

class DataSelection {
public:
  DataSelection(Configs *conf_) {
    conf = conf_;
    iSampleYear = conf->iSampleYear;
    ReadLumiJSON();
  };

  void ReadLumiJSON() {
    TString s;
    if (iSampleYear < 2){ // 2016 or 2016apv
      ifstream f("/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Legacy_2016/Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt");
      s.ReadFile(f);
    }
    if (iSampleYear == 2){ // 2017
      ifstream f("/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/Legacy_2017/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt");
      s.ReadFile(f);
    }
    if (iSampleYear == 3){
      ifstream f("/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions18/13TeV/Legacy_2018/Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt");
      s.ReadFile(f);
    }
    GoodSections = nlohmann::json::parse(s.Data());
  }

  bool GetDataSelection(int run, unsigned luminosityBlock) {
    string runstr = to_string(run);
    bool LumiStatus = GoodSections.contains(runstr);
    if (LumiStatus) {
      bool goodblock = false;
      vector<vector<unsigned>> blocks = GoodSections.at(runstr).get<vector<vector<unsigned>>>();
      for (vector<unsigned> block : blocks) {
        if (block[0] <= luminosityBlock && block[1] >= luminosityBlock) {
          goodblock = true;
          break;
        }
      }
      LumiStatus &= goodblock;
    }
    return LumiStatus;
  }

  Configs *conf;
  int iSampleYear;
  nlohmann::json GoodSections;
};

#endif
