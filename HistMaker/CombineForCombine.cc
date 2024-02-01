#include "Utilities/DrawDataFormat.cc"

void CombineForCombine(int isampleyear = 3) {
  string basepath = "/eos/user/s/siluo/WPrimeAnalysis/ValidationFitted/";
  string itpath = "";
  string SampleYear = dlib.SampleYears[isampleyear];
  string Observable = "WPrimeMass";
  TString OutFileName = "CombineFiles/" + SampleYear + "_" + Observable + ".root";
  TFile *OutFile = new TFile(OutFileName,"RECREATE");
  OutFile->cd();

  vector<string> SampleTypes = dlib.DatasetNames;
  vector<string> GroupNames = dlib.GroupNames;
  vector<string> Regions = rm.StringRanges;
  rm.AddVariationSource("RwStat");
  vector<string> Variations = rm.Variations;

  vector<vector<vector<TH1F*> > > OutHists; // OutHists[iGroup][iVariation][iRegion]
  OutHists.resize(GroupNames.size()); //
  for (unsigned ig = 0; ig < GroupNames.size(); ++ig) {
    OutHists[ig].resize(Variations.size());
    for (unsigned iv = 0; iv < Variations.size(); ++iv) {
      OutHists[ig][iv].resize(Regions.size());
    }
  }
  for (unsigned ist = 0; ist < SampleTypes.size(); ++ist) {
    // string HistFilePath = "outputs/"; // FIXME: Everytime after MakeHistValidtion.cc A run of Hadd.sh is needed to push histograms and reweight hists together.
    string SampleType = SampleTypes[ist];
    string HistFilePath = basepath + "Hists/";
    string HistFilePrefix = SampleYear + "_Validation";
    if (SampleType == "ttbar") HistFilePrefix += "_RW2On2";
    if (SampleType == "ZZ") continue;
    string GroupName = dlib.GetGroup(SampleType);
    int ig = dlib.GetGroupIndexFromGroupName(GroupName);
    if (ig < 0) {
      cout << "Cannot find the group for SampleType: " << SampleType << "Skipping the Dataset" <<endl;
      continue;
    }
    if (GroupName == "Data") {
      GroupName = "data_obs";
    }
    TString HistFileName = StandardNames::HistFileName(HistFilePath, HistFilePrefix, Observable, SampleType);
    TFile *HistFile = new TFile(HistFileName,"READ");
    if (!HistFile) continue;
    if (HistFile->IsZombie()) continue;
    for (unsigned ir = 0; ir < Regions.size(); ++ir) {
      string Region = Regions[ir];
      for (unsigned iv = 0; iv < Variations.size(); ++iv) {
        TString inhistname = StandardNames::HistName(SampleType, Observable, Region, Variations[iv]);
        TH1F *h = (TH1F*)HistFile->Get(inhistname);
        if (h == nullptr) {
          cout << "Trying but faild to get " << inhistname << endl;
          continue;
        }
        if (OutHists[ig][iv][ir] == nullptr) {
          OutHists[ig][iv][ir] = (TH1F*)h->Clone();
          TString outhistname = StandardNames::HistName(GroupName, Observable, Region, Variations[iv]);
          OutHists[ig][iv][ir]->SetName(outhistname);
          OutHists[ig][iv][ir]->SetDirectory(OutFile);
        }
        else {
          OutHists[ig][iv][ir]->Add(h);
        }
        delete h;
      }
    }
    HistFile->Close();
    delete HistFile;
  }

  OutFile->Write();
  OutFile->Save();

}