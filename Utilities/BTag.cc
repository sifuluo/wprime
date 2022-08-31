#ifndef BTAG_CC
#define BTAG_CC

#include <vector>
#include "DataFormat.cc"
#include "Constants.cc"

using namespace std;

class BTag {
public:
  BTag(int wp_ = 2, int isy_ = 0) {
    wp = wp_;
    sample = Constants::SampleYears[isy_];
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16preVFP
    DeepJetCuts["2016apv"] = vector<float> {0.0508,0.2598,0.6502};
    DeepJetEff["2016apv"]  = vector<float> {0.873 ,0.733 ,0.575};
    DeepJetMR["2016apv"]   = vector<float> {0.1   ,0.01  ,0.001};
    // DeepJetCuts["16post"] = vector<float> {0.0816,0.3657,0.7565};
    // DeepJetEff["16post"]  = vector<float> {0.863 ,0.714 ,0.547};
    // DeepJetMR["16post"]   = vector<float> {0.1   ,0.01  ,0.001};
    //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL16postVFP
    DeepJetCuts["2016"] = vector<float> {0.0480,0.2489,0.6377};
    DeepJetEff["2016"]  = vector<float> {0.885 ,0.756 ,0.604};
    DeepJetMR["2016"]   = vector<float> {0.1   ,0.01  ,0.001};

    //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL17
    DeepJetCuts["2017"] = vector<float> {0.0532,0.3040,0.7476};
    DeepJetEff["2017"]  = vector<float> {0.910 ,0.791 ,0.616};
    DeepJetMR["2017"]   = vector<float> {0.1   ,0.01  ,0.001};

    //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL18
    DeepJetCuts["2018"] = vector<float> {0.0490,0.2783,0.7100};
    DeepJetEff["2018"]  = vector<float> {0.929 ,0.819 ,0.652};
    DeepJetMR["2018"]   = vector<float> {0.1   ,0.01  ,0.001};

    cut = DeepJetCuts[sample][wp];
    eff = DeepJetEff[sample][wp];
    mr  = DeepJetMR[sample][wp];

  };

  bool IsBtag(float p) {
    return (p > cut);
  }

  bool IsBtag(Jet& j) {
    return (j.btagDeepFlavB > cut);
  }

  void TagB(Jet& j) {
    j.btag = (j.btagDeepFlavB > cut);
  }

  int wp; // 0  for loose, 1 for medium, 2 for tight;
  string sample;
  float cut, eff, mr;
  map< string, vector<float> > DeepJetCuts, DeepJetEff, DeepJetMR;
};

#endif
