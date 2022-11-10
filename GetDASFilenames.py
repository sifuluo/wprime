import os

get_data = True
get_mc = False
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
      foutput = open(f"filenames/DAS_{ds}_{year}.txt","w")
      foutput_apv = open(f"filenames/DAS_{ds}_{year}apv.txt","w")
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
      if year != "2016": os.remove(f"filenames/DAS_{ds}_{year}apv.txt")
    os.remove("datasets.txt")

if get_mc:
  MC_datasets = []
  MC_datasets.append(("TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIISummer20UL16NanoAODv9-106X_mcRun2_asymptotic_v17-v1/NANOAODSIM","ttbar_2016"))
  MC_datasets.append(("TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIISummer20UL17NanoAODv9-106X_mc2017_realistic_v9-v1/NANOAODSIM","ttbar_2017"))
  MC_datasets.append(("TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIISummer20UL18NanoAODv9-106X_upgrade2018_realistic_v16_L1v1-v1/NANOAODSIM","ttbar_2018"))

  for ds, dn in MC_datasets:
    print(ds, dn)
    filename = "filenames/DAS_" + dn + ".txt"
    if os.path.exists(filename): os.remove(filename)
    os.system(f'dasgoclient -query="file dataset=/{ds}" >> {filename}')
