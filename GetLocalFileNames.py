import os

def GetLocalDataset(names, iy):
  outname = names[0]
  inname = names[0]
  if len(names) == 2:
    inname = names[1]
  # basepath = "/eos/user/d/doverton/skimmed_samples"
  pbasepath = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/"
  ebasepath = "/eos/user/e/eusebi/andrewsdata/skimmed_samples/"
  b2gbasepath = "/eos/cms/store/group/phys_b2g/wprime/skimmed_samples/"
  inyears = ["2016_APV","2016","2017","2018"]
  outyears = ["2016apv","2016","2017","2018"]
  if inname == "SingleElectron" and iy == 3: inname = "EGamma/"
  else: inname = inname + "/"

  if iy == 3: basepath = ebasepath
  elif iy == 2: basepath = b2gbasepath
  elif iy == 1: basepath = b2gbasepath
  else: basepath = b2gbasepath
  infile = basepath + inname + inyears[iy] + "/*"
  if not os.path.exists("filenames/"):
    os.makedirs("filenames")
  outfile = "filenames/" + outname + "_" + outyears[iy] + ".txt"
  if os.path.exists(outfile): os.remove(outfile)
  os.system("ls -d " + infile + " >> " + outfile)

def GetDatasetNames():
  datasets = []
  # Each dataset entry is a list of two strings
  # The first string is the name to store the .txt file in filenames
  # The second string is the folder name in the skimmed dataset folder.
  # For example, EGamma is the folder name in skimmed dataset, but I saved it into SingleElectron in filenames/
  # Such that SingleElectron samples across all years can be accessed the same way.
  # If the input name is the same as the output name, it can be omitted and only providing one variable

  # remember to check consistency with Utilities/Dataset.cc
  datasets.append(["SingleElectron"]) # 0
  datasets.append(["SingleMuon"]) # 1

  datasets.append(["ttbar"]) # 2

  datasets.append(["wjets_HT_70_100"]) # 3
  datasets.append(["wjets_HT_100_200"]) # 4
  datasets.append(["wjets_HT_200_400"]) # 5
  datasets.append(["wjets_HT_400_600"]) # 6
  datasets.append(["wjets_HT_600_800"]) # 7
  datasets.append(["wjets_HT_800_1200"]) # 8
  datasets.append(["wjets_HT_1200_2500"]) # 9
  datasets.append(["wjets_HT_2500_inf"]) # 10
  # datasets.append(["wjets_inclusive"]) # 11

  datasets.append(["single_top_schan"]) # 11
  datasets.append(["single_top_tchan"]) # 12
  datasets.append(["single_antitop_tchan"]) # 13
  datasets.append(["single_top_tw"]) # 14
  datasets.append(["single_antitop_tw"]) # 15

  datasets.append(["WW"]) # 16
  datasets.append(["ZZ"]) # 17
  datasets.append(["WZTo1L1Nu2Q"]) # 18
  datasets.append(["WZTo1L3Nu"]) # 19
  datasets.append(["WZTo2Q2L"]) # 20
  datasets.append(["WZTo3LNu"]) # 21

  for im in ["300","400","500","600","700","800","900","1000","1100"]: # 22 23 24 25 26 27 28 29 30
    inname = "wprime_" + im + "_former_leptonic"
    outname = "FL" + im
    datasets.append([outname, inname])
    
  for im in ["300","400","500","600","700","800","900","1000","1100"]: # 31 32 33 34 35 36 37 38 39
    inname = "wprime_" + im + "_latter_leptonic"
    outname = "LL" + im
    datasets.append([outname, inname])
  
  datasets.append(["wjets_inclusive"])
  datasets.append(["dy_HT_70_100"]) # 3
  datasets.append(["dy_HT_100_200"]) # 4
  datasets.append(["dy_HT_200_400"]) # 5
  datasets.append(["dy_HT_400_600"]) # 6
  datasets.append(["dy_HT_600_800"]) # 7
  datasets.append(["dy_HT_800_1200"]) # 8
  datasets.append(["dy_HT_1200_2500"]) # 9
  datasets.append(["dy_HT_2500_inf"]) # 10
  datasets.append(["dy_inclusive"]) # 11


  return datasets

def GetSampleTypes():
  sts = []
  for ds in GetDatasetNames():
    sts.append(ds[0])
  return sts

CreateFileNames = True
if CreateFileNames:
  for iy in range(4):
    # if iy != 3: continue # currenlty only 2018 available
    for ids in GetDatasetNames():
      GetLocalDataset(ids, iy)
