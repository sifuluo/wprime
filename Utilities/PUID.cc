#include <vector>
#include <string>

//working points for PU jet ID implemented as in https://twiki.cern.ch/twiki/bin/view/CMS/PileupJetIDUL
std::vector<bool> PUID(float pT_, float absEta_, float PUIDdisc_, string year_){
 std::vector<bool> output = {false, false, false};
  if(pT_ >= 50.) output = {true, true, true};
  else if(year_ == "2016apv" || year_ == "2016"){
    if(absEta_ < 2.5){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.94) output = {true, true, true};
        else if(PUIDdisc_ > 0.86) output = {true, true, false};
        else if(PUIDdisc_ > -0.71) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.97) output = {true, true, true};
        else if(PUIDdisc_ > 0.93) output = {true, true, false};
        else if(PUIDdisc_ > -0.42) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 2.75){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.24) output = {true, true, true};
        else if(PUIDdisc_ > -0.10) output = {true, true, false};
        else if(PUIDdisc_ > -0.36) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.48) output = {true, true, true};
        else if(PUIDdisc_ > 0.19) output = {true, true, false};
        else if(PUIDdisc_ > -0.09) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 3.0){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.05) output = {true, true, true};
        else if(PUIDdisc_ > -0.15) output = {true, true, false};
        else if(PUIDdisc_ > -0.29) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.26) output = {true, true, true};
        else if(PUIDdisc_ > 0.04) output = {true, true, false};
        else if(PUIDdisc_ > -0.14) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 5.0){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.10) output = {true, true, true};
        else if(PUIDdisc_ > -0.08) output = {true, true, false};
        else if(PUIDdisc_ > -0.23) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.29) output = {true, true, true};
        else if(PUIDdisc_ > 0.12) output = {true, true, false};
        else if(PUIDdisc_ > -0.02) output = {true, false, false};
        else output = {false, false, false};
      }
    }
  }
  else if(year_ == "2017" || year_ == "2018"){
    if(absEta_ < 2.5){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.96) output = {true, true, true};
        else if(PUIDdisc_ > 0.90) output = {true, true, false};
        else if(PUIDdisc_ > -0.63) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.98) output = {true, true, true};
        else if(PUIDdisc_ > 0.96) output = {true, true, false};
        else if(PUIDdisc_ > -0.19) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 2.75){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.82) output = {true, true, true};
        else if(PUIDdisc_ > 0.36) output = {true, true, false};
        else if(PUIDdisc_ > -0.18) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.92) output = {true, true, true};
        else if(PUIDdisc_ > 0.61) output = {true, true, false};
        else if(PUIDdisc_ > 0.22) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 3.0){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.20) output = {true, true, true};
        else if(PUIDdisc_ > -0.16) output = {true, true, false};
        else if(PUIDdisc_ > -0.43) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.47) output = {true, true, true};
        else if(PUIDdisc_ > 0.14) output = {true, true, false};
        else if(PUIDdisc_ > -0.13) output = {true, false, false};
        else output = {false, false, false};
      }
    }
    else if(absEta_ < 5.0){
      if(pT_ < 40. && pT_ >= 30.){
        if(PUIDdisc_ > 0.09) output = {true, true, true};
        else if(PUIDdisc_ > -0.09) output = {true, true, false};
        else if(PUIDdisc_ > -0.24) output = {true, false, false};
        else output = {false, false, false};
      }
      else if(pT_ < 50. && pT_ >= 40.){
        if(PUIDdisc_ > 0.29) output = {true, true, true};
        else if(PUIDdisc_ > 0.12) output = {true, true, false};
        else if(PUIDdisc_ > -0.03) output = {true, false, false};
        else output = {false, false, false};
      }
    }
  }
  return output;
}
