#ifndef DATASET_CC
#define DATASET_CC
// As title suggests, dataset information library

#include "TString.h"
#include "TH1.h"
#include "TLegend.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>

using namespace std;

struct Dataset {
  string Name;
  string GroupName;
  int Index;
  int Type; // 0: Data, 1: MC, 2: Signal
  int Color;
  double CrossSection; // Unit in fb
  vector<double> Size;

  void Print(int verbosity = 0) {
    vector<string> types{"Data","MC","Signal"};
    vector<string> colors{"white","black","red","green","blue","yellow","magenta","cyan","lush green","purple"};
    //                       0       1      2      3      4       5         6        7        8          9
    cout << "Dataset: " << Name << " ,Index = " << Index << ", (" << types[Type] << ")" <<endl;
    if (verbosity < 1) return;
    cout << "         CrossSection = " << CrossSection << ", Sample Size  = ";
    for (unsigned i = 0; i < Size.size(); ++i) cout << Size[i] << ", ";
    if (Size.size() != 4) cout << "Not well defined";
    cout << endl;
    if (verbosity < 2) return;
    cout << "         Plot group is " << GroupName << ", Colored " << Color;
    if (Color < (int) colors.size()) cout << " (" << colors[Color] << ") ";
    cout << endl;
  }
};

struct DatasetGroup {
  string GroupName;
  int Color;
  int Type;
  vector<string> DatasetNames;
  TH1F* dummy;
  void MakeDummyHist() {
    
    dummy->SetLineColor(Color);
  }
  void AddLegend(TLegend *l) {
    TString gname = GroupName;
    if (dummy == nullptr) dummy = new TH1F(gname + "_dummy","dummy",2,0,2);
    dummy->SetLineColor(Color);
    if (Type == 0) {
      dummy->SetLineStyle(1);
      dummy->SetMarkerStyle(20);
      l->AddEntry(dummy, gname, "p");
    }
    else if (Type == 1) {
      dummy->SetLineStyle(1);
      dummy->SetFillColor(Color);
      l->AddEntry(dummy, gname, "f");
    }
    else if (Type == 2) {
      dummy->SetLineStyle(2);
      dummy->SetLineWidth(2);
      l->AddEntry(dummy, gname, "l");
    }
  }
};

class DatasetsLib {
public:
  DatasetsLib() {
    Debug = false;
    Init();
    SortGroups();
  };

  void Init() {
    Datasets.clear();
    DatasetNames.clear();
    Groups.clear();
    GroupNames.clear();
    // Add Dataset with parameters as Name, Group Name, Type(0:Data, 1:MC, 2:Signal), Color, Xsection(In fb), SampleSize
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

    AddDataset_NGTCXS("single_top_schan"       ,"single_top"    , 1, 4 , 3740,   {1,5471000,13620000,19365999}); // 11
    AddDataset_NGTCXS("single_top_tchan"       ,"single_top"    , 1, 4 , 115300, {1,63073000,129903000,178336000}); // 12
    AddDataset_NGTCXS("single_antitop_tchan"   ,"single_top"    , 1, 4 , 69090,  {1,30609000,69793000,95627000}); // 13
    AddDataset_NGTCXS("single_top_tw"          ,"single_top"    , 1, 4 , 34910,  {1,3368375,8507203,11270430}); // 14
    AddDataset_NGTCXS("single_antitop_tw"      ,"single_top"    , 1, 4 , 34970,  {1,3654510,8433998,10949620}); // 15

    AddDataset_NGTCXS("WW"                     ,"diboson"       , 1, 5 , 51650,  {1, 19976139, 39931603, 40272013}); // 16
    AddDataset_NGTCXS("ZZ"                     ,"diboson"       , 1, 5 , 12170,  {1, 1151000, 2706000, 3526000}); // 17
    AddDataset_NGTCXS("WZTo1L1Nu2Q"            ,"diboson"       , 1, 5 , 9119,   {1, 3690271, 7345742, 7395487}); // 18
    AddDataset_NGTCXS("WZTo1L3Nu"              ,"diboson"       , 1, 5 , 3414,   {1, 2468727, 2481654, 2497292}); // 19
    AddDataset_NGTCXS("WZTo2Q2L"               ,"diboson"       , 1, 5 , 6565,   {1, 13526954, 29091996, 28576996}); // 20
    AddDataset_NGTCXS("WZTo3LNu"               ,"diboson"       , 1, 5 , 4430,   {1, 20810003, 10339582, 38624209}); // 21

    // McM page
    // https://cms-pdmv.cern.ch/mcm/requests?range=B2G-RunIISummer20UL16wmLHEGEN-03230,B2G-RunIISummer20UL16wmLHEGEN-03247&page=0&shown=127
    // DAS request dataset wildcard
    // dataset=/btWprimeToBottomTop_*Leptonic_M-*_TuneCP5_13TeV-madgraphMLM-pythia8/*/NANOAODSIM
    // Example command to get the sample size:
    // for i in 3 4 5 6 7 8 9 10 11; do dasgoclient -query="dataset=/btWprimeToBottomTop_LatterLeptonic_M-${i}00_TuneCP5_13TeV-madgraphMLM-pythia8/RunIISummer20UL16NanoAODAPVv9-106X_mcRun2_asymptotic_preVFP_v11-v1/NANOAODSIM | grep dataset.nevents"; done
    AddDataset_NGTCXS("FL300"                  ,"M300"            , 2, 4 , 683.8,  {542974, 451706, 989541, 973312}); // 22
    AddDataset_NGTCXS("FL400"                  ,"M400"            , 2, 5 , 321.7,  {542917, 457628, 1009763, 1005777}); // 23
    AddDataset_NGTCXS("FL500"                  ,"M500"            , 2, 6 , 161.1,  {539889, 472634, 990910, 995846}); // 24
    AddDataset_NGTCXS("FL600"                  ,"M600"            , 2, 7 , 85.92,  {538626, 459533, 1002905, 1013268}); // 25
    AddDataset_NGTCXS("FL700"                  ,"M700"            , 2, 8 , 48.84,  {536088, 458032, 993657, 1007434}); // 26
    AddDataset_NGTCXS("FL800"                  ,"M800"            , 2, 9 , 29.81,  {544804, 457557, 989717, 994770}); // 27
    AddDataset_NGTCXS("FL900"                  ,"M900"            , 2, 30, 18.33,  {537197, 459185, 996690, 1014207}); // 28
    AddDataset_NGTCXS("FL1000"                 ,"M1000"           , 2, 40, 11.73,  {533034, 443584, 999049, 999380}); // 29
    AddDataset_NGTCXS("FL1100"                 ,"M1100"           , 2, 46, 7.683,  {467268, 466768, 1001850, 986599}); // 30

    AddDataset_NGTCXS("LL300"                  ,"M300"            , 2, 4 , 708.3,  {533339, 463438, 1016545, 979122}); // 31
    AddDataset_NGTCXS("LL400"                  ,"M400"            , 2, 5 , 336.1,  {527708, 451102, 991505, 990858}); // 32
    AddDataset_NGTCXS("LL500"                  ,"M500"            , 2, 6 , 165.3,  {548554, 462578, 990701, 1001088}); // 33
    AddDataset_NGTCXS("LL600"                  ,"M600"            , 2, 7 , 85.82,  {530042, 467804, 997101, 995847}); // 34
    AddDataset_NGTCXS("LL700"                  ,"M700"            , 2, 8 , 47.47,  {539337, 454955, 999882, 993510}); // 35
    AddDataset_NGTCXS("LL800"                  ,"M800"            , 2, 9 , 27.73,  {541417, 462499, 978958, 988361}); // 36
    AddDataset_NGTCXS("LL900"                  ,"M900"            , 2, 30, 16.49,  {534297, 464288, 1015927, 1006881}); // 37
    AddDataset_NGTCXS("LL1000"                 ,"M1000"           , 2, 40, 10.25,  {540970, 457909, 998840, 1008866}); // 38
    AddDataset_NGTCXS("LL1100"                 ,"M1100"           , 2, 46, 6.546,  {535810, 461040, 990631, 1010305}); // 39
  }

  void AppendAndrewDatasets() {
    AddDataset_NGTCXS("wjets_inclusive"        ,"wjets"           , 1, 3 , 0,      {0,0,0,0}); // 40
    AddDataset_NGTCXS("dy_HT_70_100"           ,"dy"              , 1, 3 , 208977, {1, 12618142, 12205958, 17004433}); // 3 41
    AddDataset_NGTCXS("dy_HT_100_200"          ,"dy"              , 1, 3 , 181302, {1, 17886393, 18648544, 26202328}); // 4 42
    AddDataset_NGTCXS("dy_HT_200_400"          ,"dy"              , 1, 3 , 50418,  {1, 11516413, 12451701, 18455718}); // 5 43
    AddDataset_NGTCXS("dy_HT_400_600"          ,"dy"              , 1, 3 , 6984,   {1, 5208308, 5384252, 8682257}); // 6 44
    AddDataset_NGTCXS("dy_HT_600_800"          ,"dy"              , 1, 3 , 1681,   {1, 4981503, 5118706, 7035971}); // 7 45
    AddDataset_NGTCXS("dy_HT_800_1200"         ,"dy"              , 1, 3 , 775,    {1, 4805067, 4347168, 6554679}); // 8 46
    AddDataset_NGTCXS("dy_HT_1200_2500"        ,"dy"              , 1, 3 , 186,    {1, 4160521, 4725936, 5966661}); // 9 47
    AddDataset_NGTCXS("dy_HT_2500_inf"         ,"dy"              , 1, 3 , 4,      {1, 1418215, 1480047, 1978203}); // 10 48
    AddDataset_NGTCXS("dy_inclusive"           ,"dy"              , 1, 3 , 0,      {0,0,0,0}); // 49
  }

  // Adding Dataset with parameters as Name, Group name, Type (0:Data,1:MC,2:Signal), Color, Xsection, SampleSize
  int AddDataset_NGTCXS(string name, string gname = "", int type = 2, int color = 3, double xsec = 1.0, vector<double> size = {}) {
    if (Datasets.find(name) != Datasets.end()) {
      return Datasets[name].Index;
    }
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
    return ds.Index;
  }

  void ListDatasets() {
    cout << endl;
    for (unsigned i = 0; i < DatasetNames.size(); ++i) {
      Datasets[DatasetNames[i]].Print();
    }
  }

  void SortGroups() {
    // By default, sort the Groups by MC backgroup, Signal and Data comes last.
    vector<int> typeorder{1,2,0};
    for (unsigned i = 0; i < 3; ++i) {
      for (unsigned ids = 0; ids < DatasetNames.size(); ++ids) {
        Dataset& ds = GetDataset(DatasetNames[ids]);
        if (ds.Type != typeorder[i]) continue;
        if (Groups[ds.GroupName].DatasetNames.size() == 0) { // First occurance of a GroupName
          GroupNames.push_back(ds.GroupName);
          Groups[ds.GroupName].Color = ds.Color;
          Groups[ds.GroupName].Type = ds.Type;
          Groups[ds.GroupName].GroupName = ds.GroupName;
        }
        Groups[ds.GroupName].DatasetNames.push_back(ds.Name);
      }
    }
  }

  void GroupOrder() { // When need to override the order in which the hist groups are drawn
    GroupNames = {"ttbar","wjets","single_top","diboson","M300","M400","M500","M600","M700","M800","M900","M1000","M1100"};
    GroupNames.push_back("Data");
  }

  void AddLegend(TLegend *leg, bool IsSR) {
    for (unsigned i = 0; i < GroupNames.size(); ++i) {
      if (IsSR && GroupNames[i] == "Data") continue;
      Groups[GroupNames[i]].AddLegend(leg);
    }
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

  void PrintDatasets(int verbose = 0) {
    for (unsigned i = 0; i < DatasetNames.size(); ++i) {
      Dataset &ds = GetDataset(DatasetNames[i]);
      ds.Print();
    }
  } 

  Dataset& GetDataset(string ds) {
    CheckExist(ds,"GetDataset");
    return Datasets[ds];
  }

  Dataset& GetDataset(unsigned i) {
    if (i >= Datasets.size()) {
      string msg = "Index exceeding the size of all Datasets";
      cout << msg << endl;
      throw runtime_error(msg);
    }
    return Datasets[DatasetNames[i]];
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
  
  int GetGroupIndexFromDatasetName(string ds) {
    CheckExist(ds,"GetGroupIndexFromDatasetName");
    return GetGroupIndexFromGroupName(GetGroup(ds));
  }

  int GetGroupIndexFromGroupName(string gp) {
    for (unsigned ig = 0; ig < GroupNames.size(); ++ig) {
      if (gp == GroupNames[ig]) return ig;
    }
    return -1;
  }

  bool DatasetInList(int ist, vector<string> list) {
    for (unsigned i = 0; i < list.size(); ++i) {
      if (DatasetNames[ist] == list[i]) return true;
    }
    return false;
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
  vector<string> DatasetNames, GroupNames;
  map<string,DatasetGroup> Groups;
  map<string,Dataset> Datasets;

};

DatasetsLib dlib;

#endif
