import os

get_data = False
get_mc = True
if get_data:
  DATA_datasets = []
  DATA_datasets.append(("SingleElectron","2016"))
  DATA_datasets.append(("SingleElectron","2017"))
  DATA_datasets.append(("EGamma","2018"))
  DATA_datasets.append(("SingleMuon","2016"))
  DATA_datasets.append(("SingleMuon","2017"))
  DATA_datasets.append(("SingleMuon","2018"))

  for ds,year in DATA_datasets:
    print(ds,year)
    os.system(f'dasgoclient -query="dataset=/{ds}/Run{year}*UL{year}_MiniAODv2_NanoAODv9-v*/NANOAOD" >> datasets.txt')

    with open(f"datasets.txt","r") as f:
      foutput = open(f"filenames/DASFileNames/DAS_{ds}_{year}.txt","w")
      foutput_apv = open(f"filenames/DASFileNames/DAS_{ds}_{year}apv.txt","w")
      for i,line in enumerate(f):
        os.system(f'dasgoclient -query="file dataset={line}" >> filelist.txt'.replace("\n",""))
        finput = open("filelist.txt")
        for line in finput:
          hipm = "hipm" in line.lower()
          if hipm and year == "2016":
            foutput_apv.write(line)
          else:
            foutput.write(line)
        os.remove("filelist.txt")
        finput.close()
      foutput.close()
      foutput_apv.close()
      if year != "2016": os.remove(f"filenames/DASFileNames/DAS_{ds}_{year}apv.txt")
    os.remove("datasets.txt")

if get_mc:
  MC_datasets = []
  MC_datasets.append(("/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8","ttbar"))
  MC_datasets.append(("/WJetsToLNu_HT-70To100_TuneCP5_13TeV-madgraphMLM-pythia8","wjets_HT_70_100"))
  MC_datasets.append(("/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8","wjets_HT_100_200"))
  MC_datasets.append(("/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8","wjets_HT_200_400"))
  MC_datasets.append(("/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8","wjets_HT_400_600"))
  MC_datasets.append(("/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8","wjets_HT_600_800"))
  MC_datasets.append(("/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8","wjets_HT_800_1200"))
  MC_datasets.append(("/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8","wjets_HT_1200_2500"))
  MC_datasets.append(("/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8","wjets_HT_2500_inf"))
  MC_datasets.append(("/ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo-pythia8","single_top_schan"))
  MC_datasets.append(("/ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8","single_top_tchan"))
  MC_datasets.append(("/ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8","single_antitop_tchan"))
  MC_datasets.append(("/ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8","single_top_tw"))
  MC_datasets.append(("/ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8","single_antitop_tw"))

  # dasgoclient -query="dataset=/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8"
  MC_postfix = []
  # 2016apv postfixes
  # ttbar:        /RunIISummer20UL16NanoAODAPVv9-106X_mcRun2_asymptotic_preVFP_v11-v1/NANOAODSIM
  # WJetsToLNu_HT:/RunIISummer20UL16NanoAODAPVv9-106X_mcRun2_asymptotic_preVFP_v11-v1/NANOAODSIM

  # MC_postfix.append(("/RunIISummer20UL16NanoAODAPVv9*asymptotic_preVFP_v*/NANOAODSIM","_2016apv"))
  MC_postfix.append(("/RunIISummer20UL16NanoAODv9-106X_mcRun2_asymptotic_v17-v1/NANOAODSIM","2016"))
  MC_postfix.append(("/RunIISummer20UL17NanoAODv9-106X_mc2017_realistic_v9-v1/NANOAODSIM","2017"))
  MC_postfix.append(("/RunIISummer20UL18NanoAODv9-106X_upgrade2018_realistic_v16_L1v1-v1/NANOAODSIM","2018"))

  for ds, dn in MC_datasets:
    for post, year in MC_postfix:
      postfixv = 1;
      while postfixv < 10:
        datasetname = ds + post
        fn = dn + "_" + year
        print("Working on " ,fn , datasetname)
        filename = "filenames/DASFileNames/DAS_" + fn + ".txt"
        if os.path.exists(filename): os.remove(filename)
        os.system(f'dasgoclient -query="file dataset={datasetname}" >> {filename}')
        if os.stat(filename).st_size > 10: break
        print("Dataset not found. Output file size = " + str(os.stat(filename).st_size) + " , postfixv = " + str(postfixv))
        os.remove(filename)
        post = post.replace("-v"+str(postfixv) + "/","-v"+str(postfixv + 1) + "/")
        postfixv = postfixv + 1
        # os.system(f'dasgoclient -query="dataset={datasetname}"| wc -l')
