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
  "single_antitop_tchan","single_antitop_tw","single_top_schan","single_top_tchan","single_top_tw"};
  // 11                        12                  13                 14                 15

  const vector<string> Triggers{"SE","SM"};
}

#endif
