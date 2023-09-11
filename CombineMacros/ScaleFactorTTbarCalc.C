#include <TROOT.h>
#include "../Utilities/Dataset.cc"
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <vector>
#include <TString.h>
#include <TFitResult.h> 
#include <TMatrixD.h>
#include <TMath.h>

//derive ttbar SF from 2-tag regions of same multiplicity and lepton flavour, then propagate stat uncertainty envelope bin-by-bin and do syst variation histograms
void ScaleFactorTTbarCalc(int bin=1152, int year=2018){
  
  vector<TString> variations = {"" // 0
    , "electronScaleUp", "electronScaleDown", "electronResUp", "electronResDown", "JESUp", "JESDown", "JERUp", "JERDown" // 1 - 8
    , "electronUp", "electronDown", "muonTriggerUp", "muonTriggerDown", "muonIdUp", "muonIdDown", "muonIsoUp", "muonIsoDown" // 9 - 16
    , "BjetTagCorrUp", "BjetTagCorrDown", "BjetTagUncorrUp", "BjetTagUncorrDown", "PUIDUp", "PUIDDown", "L1PreFiringUp", "L1PreFiringDown" // 17 - 24
    , "PUreweightUp", "PUreweightDown", "PDFUp", "PDFDown", "LHEScaleUp", "LHEScaleDown", // 25 - 30
  };

  TH1F dataHist;
  vector<TH1F> ttbarHists, NonTtbarHists;

  //loop over samples, organizing data, ttbar, and non-ttbar with variations
  for(unsigned i = 0; i < 40; ++i){
    if(bin/1000 == 1 && i == 0) continue;
    if(bin/2000 == 1 && i == 1) continue;
    TFile *infile = new TFile(TString::Format("TestHistograms/SimpleShapes_Bin%d_%d.root",bin,i),"READ");
    Dataset dset = dlib.GetDataset(i);
    TString gn = dset.GroupName;
    if(i<=1) dataHist = *(TH1F*)(infile->Get(TString::Format("data_obs_Wprime%d",bin)))->Clone("dataHist");
    else if(i==2){
      for(unsigned j = 0; j < variations.size(); ++j) ttbarHists.push_back(*(TH1F*)(infile->Get(TString::Format(gn+"_Wprime%d_"+variations[j],bin)))->Clone(TString::Format("ttbar_%d",j)));
    }
   else if (i==3){
     for(unsigned j = 0; j < variations.size(); ++j) NonTtbarHists.push_back(*(TH1F*)(infile->Get(TString::Format(gn+"_Wprime%d_"+variations[j],bin)))->Clone(TString::Format("nonTtbar_%d",j)));
   }
   else{
     for(unsigned j = 0; j < variations.size(); ++j) NonTtbarHists[j].Add((TH1F*)(infile->Get(TString::Format(gn+"_Wprime%d_"+variations[j],bin))));
   }
  }

  //use variations gathered to calculate central SF histogram and variation SF histograms
  vector<TH1F> SFhists, SFs;
  for(unsigned i = 0; i < variations.size(); ++i){
    SFhists.push_back(*(TH1F*)dataHist.Clone("SF_"+variations[i]));
    SFhists[i].Add(&NonTtbarHists[i],-1.);
    SFhists[i].Divide(&ttbarHists[i]);
    SFs.push_back(*(TH1F*)SFhists[i].Clone("SFcalc_"+variations[i]));
  }

  //define fit function
  TF1 *fitFunction = new TF1("fitFunction","[0]/x+[1]*x+[2]*x*x+[3]", 150., 2000.);

  //fit nominal variant with statistical uncertainties only, get covariance matrix, calculate statistical envelope
  TFitResultPtr fr = SFhists[0].Fit(fitFunction,"SRF");
  TMatrixD cov = fr->GetCovarianceMatrix();
  for(unsigned i = 0; i < SFhists[0].GetNbinsX(); ++i){
    float bc = SFhists[0].GetBinCenter(i+1);
    float Deriv0 = 1./bc;
    float Deriv1 = bc;
    float Deriv2 = bc*bc;
    float Deriv3 = 1.;
    float FinalEnvelope = 0.;
    //scan covariance matrix
    std::cout<<"_____"<<bc<<"_____"<<std::endl;
    for(unsigned x = 0; x < 4; ++x){
      for(unsigned y = 0; y < 4; ++y){
        float Component = 1.;
        if(x == 0) Component *= Deriv0;
        else if (x == 1) Component *= Deriv1;
        else if (x == 2) Component *= Deriv2;
        else if (x == 3) Component *= Deriv3;
        if(y == 0) Component *= Deriv0;
        else if (y == 1) Component *= Deriv1;
        else if (y == 2) Component *= Deriv2;
        else if (y == 3) Component *= Deriv3;
	std::cout<<Component<<std::endl;
	Component *= cov(x,y);
	std::cout<<Component<<std::endl;
	FinalEnvelope += Component;
	std::cout<<"-----"<<FinalEnvelope<<"-----"<<std::endl;
      }
    }
    SFs[0].SetBinContent(i+1, fitFunction->Eval(bc));
    SFs[0].SetBinError(i+1, TMath::Sqrt(FinalEnvelope));
  }

  //fit systematic variations
  for(unsigned i = 1; i < SFhists.size(); ++i){
    SFhists[i].Fit(fitFunction,"R");
    for(unsigned j = 0; j < SFhists[i].GetNbinsX(); ++j){
      SFs[i].SetBinContent(j+1, fitFunction->Eval(SFhists[i].GetBinCenter(j+1)));
      SFs[i].SetBinError(j+1, 0.);
    }
  }

  TFile *savefile = new TFile(TString::Format("TestHistograms/SF_Bin%d_%d.root",bin,year),"RECREATE");
  for(unsigned i = 0; i < SFhists.size(); ++i){
    SFhists[i].Write();
    SFs[i].Write();
  }
} 
