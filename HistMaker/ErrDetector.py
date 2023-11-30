import sys
import os
import ROOT as r
# sys.path.insert(1,"../")

sampleyears = ["2016apv","2016","2017","2018"]
sampletypes = []
sampletypes.extend(["SingleElectron", "SingleMuon", "ttbar"])
sampletypes.extend(["wjets_HT_70_100", "wjets_HT_100_200", "wjets_HT_200_400", "wjets_HT_400_600", "wjets_HT_600_800", "wjets_HT_800_1200", "wjets_HT_1200_2500", "wjets_HT_2500_inf"])
sampletypes.extend(["single_top_schan", "single_top_tchan", "single_antitop_tchan", "single_top_tw", "single_antitop_tw"])
sampletypes.extend(["WW", "ZZ", "WZTo1L1Nu2Q", "WZTo1L3Nu", "WZTo2Q2L", "WZTo3LNu"])
sampletypes.extend(["FL300", "FL400", "FL500", "FL600", "FL700", "FL800", "FL900", "FL1000", "FL1100"])
sampletypes.extend(["LL300", "LL400", "LL500", "LL600", "LL700", "LL800", "LL900", "LL1000", "LL1100"])
LogsDir = "Submits/logs/"
OutFilesDir = "/eos/user/s/siluo/WPrimeAnalysis/ValidationFitted/"
SampleYear = sampleyears[int(sys.argv[1])]
SampleType = sampletypes[int(sys.argv[2])]

errfilepath = LogsDir + SampleYear + "_" + SampleType + "/"
errfilename = SampleYear + "_" + SampleType + "_" + str(sys.argv[3]) + ".err"
logerr = errfilepath + errfilename
logout = logerr.replace(".err",".out")

outfilepath = OutFilesDir + SampleYear + "_" + SampleType + "/"
outfilename = SampleYear + "_" + SampleType + "_" + str(sys.argv[3]) + ".root"
outfile = outfilepath + outfilename
print("Python ErrDetector Checking output file: " + outfile)
if not(os.path.exists(outfile)):
  print("(Python ErrDetector First Run) No output detected")
  sys.exit(2)
else:
  print("(Python ErrDetector Checking...) Output .root file exists")

f = r.TFile(outfile,"READ")
if f.IsZombie():
  print("(Python ErrDetector Failed Run) Zombie output .root file detected")
  sys.exit(1)
else:
  print("(Python ErrDetector Checking...) Output .root file is not a zombie")

print("Python ErrDetector Checking err file: " + logerr)
if not (os.path.exists(logerr) and os.path.exists(logout)):
  print("(Python ErrDetector First Run) No error log file detected")
  sys.exit(2)
# errlines = open(logerr).readlines()
# if errlines[0] == "===Job is done===": emptyerr = True
errsize = os.stat(logerr).st_size
outsize = os.stat(logout).st_size

if errsize == 0 and outsize > 100:
  print("(Python ErrDetector Successful Run) Error log is empty and Out log size = " + str(outsize))
  sys.exit(0)
if errsize == 0 and outsize < 100:
  print("(Python ErrDetector First Run) Empty error log file detected but also empty out log")
  sys.exit(2)
if errsize > 0:
  print("(Python ErrDetector Failed Run) None empty error log detected")
  with open(logerr,"r") as fferr:
    print(fferr.read())
  sys.exit(1)