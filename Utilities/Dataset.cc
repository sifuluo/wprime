#ifndef DATASET_CC
#define DATASET_CC

#include "TString.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>

using namespace std;

// namespace Constants {
//   const vector<string> SampleYears{"2016apv","2016","2017","2018"};
//   const vector<string> SampleTypes{"SingleElectron","SingleMuon", // 0,1
//   "ttbar", // 2,
//   "wjets_HT_70_100", "wjets_HT_100_200", "wjets_HT_200_400", "wjets_HT_400_600",
//   // 3                  4                     5                    6
//   "wjets_HT_600_800", "wjets_HT_800_1200", "wjets_HT_1200_2500", "wjets_HT_2500_inf", "wjets_inclusive"
//   // 7                  8                     9                    10                    11
//   "single_antitop_tchan","single_antitop_tw","single_top_schan","single_top_tchan","single_top_tw",
//   // 12                        13                  14                 15                 16
//   "FL300","FL400","FL500","FL600","FL700","FL800","FL900","FL1000","FL1100",
//   // 17     18      19       20      21       22     23      24       25
//   "LL300","LL400","LL500","LL600","LL700","LL800","LL900","LL1000","LL1100"
//   // 26     27      28       29      30      31      32       33      34
//   };
//   const vector<double> CMSLumiYears{19.52, 16.81, 41.48, 59.83};
// }

struct Dataset {
  string Name;
  string GroupName;
  int Index;
  int Type; // 0: Data, 1: MC, 2: Signal
  int Color;
  double CrossSection; // Unit in fb
  vector<double> Size;

  void Print() {
    vector<string> types{"Data","MC","Signal"};
    vector<string> colors{"white","black","red","green","blue","yellow","magenta","cyan","lush green","purple"};
    //                       0       1      2      3      4       5         6        7        8          9
    cout << "Dataset: " << Name << " ,Index = " << Index << ", (" << types[Type] << ")" <<endl;
    cout << "         CrossSection = " << CrossSection << ", Sample Size  = ";
    for (unsigned i = 0; i < Size.size(); ++i) cout << Size[i] << ", ";
    if (Size.size() != 4) cout << "Not well defined";
    cout << endl;
    cout << "         Plot group is " << GroupName << ", Colored " << Color;
    if (Color < colors.size()) cout << " (" << colors[Color] << ") ";
    cout << endl;
  }
};

class DatasetsLib {
public:
  DatasetsLib() {
    Debug = false;
    Init();
  };
  void Init() {
    Datasets.clear();
    DatasetNames.clear();
    // Add Dataset with parameters as Name, Group Name, Type(0:Data, 1:MC, 2:Signal), Color, Xsection, SampleSize
    //                Name                      Group Name     Type Color  Xsection   SampleSizes for each year
    AddDataset_NGTCXS("SingleElectron"         ,"Data"          , 0, 1 , 1., {1,1,1,1}); // 0
    AddDataset_NGTCXS("SingleMuon"             ,"Data"          , 0, 1 , 1., {1,1,1,1}); // 1

    AddDataset_NGTCXS("ttbar"                  ,"ttbar"         , 1, 2 , 365974.4, {1, 144722000, 346052000, 476408000}); // 2

    AddDataset_NGTCXS("wjets_HT_70_100"        ,"wjets"         , 1, 3 , 1353000, {1, 19439931, 44576510, 66220256}); // 3
    AddDataset_NGTCXS("wjets_HT_100_200"       ,"wjets"         , 1, 3 , 1627450, {1, 19753958, 47424468, 51408967}); // 4
    AddDataset_NGTCXS("wjets_HT_200_400"       ,"wjets"         , 1, 3 , 435237,  {1, 15067621, 42281979, 58225632}); // 5
    AddDataset_NGTCXS("wjets_HT_400_600"       ,"wjets"         , 1, 3 , 59181,   {1, 2115509, 5468473, 7444030}); // 6
    AddDataset_NGTCXS("wjets_HT_600_800"       ,"wjets"         , 1, 3 , 14581,   {1, 2251807, 5545298, 7718765}); // 7
    AddDataset_NGTCXS("wjets_HT_800_1200"      ,"wjets"         , 1, 3 , 6656,    {1, 2132228, 5088483, 7306187}); // 8
    AddDataset_NGTCXS("wjets_HT_1200_2500"     ,"wjets"         , 1, 3 , 1608,    {1, 2090561, 4752118, 6481518}); // 9
    AddDataset_NGTCXS("wjets_HT_2500_inf"      ,"wjets"         , 1, 3 , 39,      {1, 709514, 1185699, 2097648}); // 10
    AddDataset_NGTCXS("wjets_inclusive"        ,"wjets"         , 1, 3 , 0,       {0,0,0,0});                     // 11

    AddDataset_NGTCXS("single_antitop_tchan"   ,"single_top"    , 1, 4 , 69090,  {1,30609000,69793000,395627000}); // 12 // 9562700 events
    AddDataset_NGTCXS("single_antitop_tw"      ,"single_top"    , 1, 4 , 34970,  {1,3654510,8433998,10949620}); // 13
    AddDataset_NGTCXS("single_top_schan"       ,"single_top"    , 1, 4 , 3740,   {1,5471000,13620000,19365999}); // 14
    AddDataset_NGTCXS("single_top_tchan"       ,"single_top"    , 1, 4 , 115300, {1,63073000,129903000,178336000}); // 15
    AddDataset_NGTCXS("single_top_tw"          ,"single_top"    , 1, 4 , 34910,  {1,3368375,8507203,11270430}); // 16

    // McM page
    // https://cms-pdmv.cern.ch/mcm/requests?range=B2G-RunIISummer20UL16wmLHEGEN-03230,B2G-RunIISummer20UL16wmLHEGEN-03247&page=0&shown=127
    // DAS request dataset wildcard
    // dataset=/btWprimeToBottomTop_*Leptonic_M-*_TuneCP5_13TeV-madgraphMLM-pythia8/*/NANOAODSIM
    // Example command to get the sample size:
    // for i in 3 4 5 6 7 8 9 10 11; do dasgoclient -query="dataset=/btWprimeToBottomTop_LatterLeptonic_M-${i}00_TuneCP5_13TeV-madgraphMLM-pythia8/RunIISummer20UL16NanoAODAPVv9-106X_mcRun2_asymptotic_preVFP_v11-v1/NANOAODSIM | grep dataset.nevents"; done
    AddDataset_NGTCXS("FL300"                  ,"M300"            , 2, 5 , 683.8,  {542974, 451706, 989541, 973312}); // 17
    AddDataset_NGTCXS("FL400"                  ,"M400"            , 2, 5 , 321.7,  {542917, 457628, 1009763, 1005777}); // 18
    AddDataset_NGTCXS("FL500"                  ,"M500"            , 2, 5 , 161.1,  {539889, 472634, 990910, 995846}); // 19
    AddDataset_NGTCXS("FL600"                  ,"M600"            , 2, 5 , 85.92,  {538626, 459533, 1002905, 1013268}); // 20
    AddDataset_NGTCXS("FL700"                  ,"M700"            , 2, 5 , 48.84,  {0, 458032, 993657, 1007434}); // 21
    AddDataset_NGTCXS("FL800"                  ,"M800"            , 2, 5 , 29.81,  {544804, 457557, 989717, 994770}); // 22
    AddDataset_NGTCXS("FL900"                  ,"M900"            , 2, 5 , 18.33,  {537197, 459185, 996690, 1014207}); // 23
    AddDataset_NGTCXS("FL1000"                 ,"M1000"           , 2, 5 , 11.73,  {533034, 443584, 999049, 999380}); // 24
    AddDataset_NGTCXS("FL1100"                 ,"M1100"           , 2, 5 , 7.683,  {467268, 466768, 1001850, 986599}); // 25

    AddDataset_NGTCXS("LL300"                  ,"M300"            , 2, 5 , 708.3,  {533339, 463438, 1016545, 979122}); // 26
    AddDataset_NGTCXS("LL400"                  ,"M400"            , 2, 5 , 336.1,  {527708, 451102, 991505, 990858}); // 27
    AddDataset_NGTCXS("LL500"                  ,"M500"            , 2, 5 , 165.3,  {548554, 462578, 990701, 1001088}); // 28
    AddDataset_NGTCXS("LL600"                  ,"M600"            , 2, 5 , 85.82,  {530042, 467804, 997101, 995847}); // 29
    AddDataset_NGTCXS("LL700"                  ,"M700"            , 2, 5 , 47.47,  {539337, 454955, 999882, 993510}); // 30
    AddDataset_NGTCXS("LL800"                  ,"M800"            , 2, 5 , 27.73,  {0, 462499, 978958, 988361}); // 31
    AddDataset_NGTCXS("LL900"                  ,"M900"            , 2, 5 , 16.49,  {0, 464288, 1015927, 1006881}); // 32
    AddDataset_NGTCXS("LL1000"                 ,"M1000"           , 2, 5 , 10.25,  {540970, 457909, 998840, 1008866}); // 33
    AddDataset_NGTCXS("LL1100"                 ,"M1100"           , 2, 5 , 6.546,  {535810, 461040, 990631, 1010305}); // 34

    // AddDataset_NGTCXS("Private_FL_M500"        ,""              , 2, 7 , 161.1,  {1,1,1,189291});
  }
  // Adding Dataset with parameters as Name, Group name, Type (0:Data,1:MC,2:Signal), Color, Xsection, SampleSize
  void AddDataset_NGTCXS(string name, string gname, int type, int color, double xsec, vector<double> size) {
    Dataset ds;
    ds.Name = name;
    ds.Index = DatasetNames.size();
    ds.Type = type;
    ds.Color = color;
    ds.CrossSection = xsec;
    ds.Size = size;
    if (gname == "") ds.GroupName = name;
    else ds.GroupName = gname;
    DatasetNames.push_back(name);
    Datasets[name] = ds;
  }

  void AddDataset(Dataset ds) {
    DatasetNames.push_back(ds.Name);
    Datasets[ds.Name] = ds;
  }

  bool CheckExist(string ds, string funcname = "") {
    if (!Debug) return true;
    bool found = (Datasets.find(ds) != Datasets.end());
    if (!found) {
      string msg =  "Dataset " + ds + " no found while running function " + funcname;
      cout << msg <<endl;
      throw runtime_error(msg);
    }
    return found;
  }

  Dataset& GetDataset(string ds) {
    CheckExist(ds,"GetDataset");
    return Datasets[ds];
  }

  double GetCMSLumi(int i) {
    return CMSLumiYears[i];
  }

  int GetIndex(string ds) {
    CheckExist(ds,"GetIndex");
    return Datasets[ds].Index;
  }

  string GetGroup(string ds) {
    CheckExist(ds,"GetGroup");
    return Datasets[ds].GroupName;
  }

  int GetType(string ds) {
    CheckExist(ds, "GetType");
    return Datasets[ds].Type;
  }

  int GetColor(string ds) {
    CheckExist(ds, "GetColor");
    return Datasets[ds].Color;
  }

  double GetXsec(string ds) {
    CheckExist(ds, "GetXsec");
    return Datasets[ds].CrossSection;
  }

  double GetSize(string ds, int y) {
    CheckExist(ds, "GetSize");
    return Datasets[ds].Size[y];
  }

  double GetNormFactor(string ds, int y) {
    if (GetType(ds) == 0) return 1;
    return CMSLumiYears[y] * GetXsec(ds) / GetSize(ds,y);
  }


  bool Debug;
  const vector<string> SampleYears{"2016apv","2016","2017","2018"};
  const vector<double> CMSLumiYears{19.52, 16.81, 41.48, 59.83};
  vector<string> DatasetNames;
  map<string,Dataset> Datasets;

};

DatasetsLib dlib;

#endif
