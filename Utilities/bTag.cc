#include <vector>
#include <string>

std::vector<bool> bTag(float bTagDisc_, string year_){
  std::vector<float> WPs; //loose, medium, tight
  std::vector<bool> out = {false, false, false};
  //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16preVFP
  if(year_ == "2016apv") WPs = {0.0508,0.2598,0.6502};
  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL16postVFP
  else if(year_ == "2016") WPs = {0.0480,0.2489,0.6377};
  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL17
  else if(year_ == "2017") WPs = {0.0532,0.3040,0.7476};
  //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL18
  else if(year_ == "2018") WPs = {0.0490,0.2783,0.7100};
  else return out;

  if(bTagDisc_ > WPs[2]) out = {true, true, true};
  else if(bTagDisc_ > WPs[1]) out = {true, true, false};
  else if(bTagDisc_ > WPs[0]) out = {true, false, false};

  return out;
}
