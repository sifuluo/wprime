#ifndef CONSTANTS_CC
#define CONSTANTS_CC

#include <vector>
#include <cmath>
#include <string>


namespace Constants {
  const vector<string> SampleYears{"2016apv","2016","2017","2018"};
  const vector<string> SampleTypes{"SingleElectron","SingleMuon", // 0,1
  "ttbar", // 2,
  "wjets_HT_70_100", "wjets_HT_100_200", "wjets_HT_200_400", "wjets_HT_400_600",
  // 3                  4                     5                    6
  "wjets_HT_600_800", "wjets_HT_800_1200", "wjets_HT_1200_2500", "wjets_HT_2500_inf",
  // 7                  8                     9                    10
  "single_antitop_tchan","single_antitop_tw","single_top_schan","single_top_tchan","single_top_tw",
  // 11                        12                  13                 14                 15
  "Private_FL_M500"};
  // 16
  const vector<string> Triggers{"SE","SM"};

  // BTaggings
  //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16preVFP
  const vector<float> DeepJetCuts_2016apv{0.0508,0.2598,0.6502};
  const vector<float> DeepJetEff_2016apv {0.873 ,0.733 ,0.575};
  const vector<float> DeepJetMR_2016apv  {0.1   ,0.01  ,0.001};
  // const vector<float> DeepJetCuts_16post{0.0816,0.3657,0.7565};
  // const vector<float> DeepJetEff_16post {0.863 ,0.714 ,0.547};
  // const vector<float> DeepJetMR_16post  {0.1   ,0.01  ,0.001};
  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL16postVFP
  const vector<float> DeepJetCuts_2016{0.0480,0.2489,0.6377};
  const vector<float> DeepJetEff_2016 {0.885 ,0.756 ,0.604};
  const vector<float> DeepJetMR_2016  {0.1   ,0.01  ,0.001};

  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL17
  const vector<float> DeepJetCuts_2017{0.0532,0.3040,0.7476};
  const vector<float> DeepJetEff_2017 {0.910 ,0.791 ,0.616};
  const vector<float> DeepJetMR_2017  {0.1   ,0.01  ,0.001};

  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL18
  const vector<float> DeepJetCuts_2018{0.0490,0.2783,0.7100};
  const vector<float> DeepJetEff_2018 {0.929 ,0.819 ,0.652};
  const vector<float> DeepJetMR_2018  {0.1   ,0.01  ,0.001};
}

#endif
