import os

def GetLocalDataset(names, iy):
  outname = names[0]
  inname = names[0]
  if len(names) == 2:
    inname = names[1]
  basepath = "/eos/user/p/pflanaga/andrewsdata/skimmed_samples/"
  # basepath = "/eos/user/d/doverton/skimmed_samples"
  inyears = ["2016","2016_APV","2017","2018"]
  outyears = ["2016","2016apv","2017","2018"]
  if inname == "SingleElectron" and iy == 3: inname = "EGamma/"
  else: inname = inname + "/"

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

  datasets.append(["SingleElectron"])
  datasets.append(["SingleMuon"])
  datasets.append(["ttbar"])

  datasets.append(["wjets_HT_70_100"])
  datasets.append(["wjets_HT_100_200"])
  datasets.append(["wjets_HT_200_400"])
  datasets.append(["wjets_HT_400_600"])
  datasets.append(["wjets_HT_600_800"])
  datasets.append(["wjets_HT_800_1200"])
  datasets.append(["wjets_HT_1200_2500"])
  datasets.append(["wjets_HT_2500_inf"])
  datasets.append(["wjets_inclusive"])
  datasets.append(["single_antitop_tchan"])
  datasets.append(["single_antitop_tw"])
  datasets.append(["single_top_schan"])
  datasets.append(["single_top_tchan"])
  datasets.append(["single_top_tw"])

  for im in ["300","400","500","600","700","800","900","1000","1100"]:
    inname = "wprime_" + im + "_former_leptonic"
    outname = "FL" + im
    datasets.append([outname, inname])
    
  for im in ["300","400","500","600","700","800","900","1000","1100"]:
    inname = "wprime_" + im + "_latter_leptonic"
    outname = "LL" + im
    datasets.append([outname, inname])
  
  return datasets

def GetSampleTypes():
  sts = []
  for ds in GetDatasetNames():
    sts.append(ds[0])
  return sts

CreateFileNames = False
if CreateFileNames:
  for iy in range(4):
    for ids in GetDatasetNames():
      GetLocalDataset(ids, iy)
