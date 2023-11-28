import sys
import os
# sys.path.insert(1,"../")

LogsDir = "Submits/logs/"
sampleyears = ["2016","2016apv","2017","2018"]
SampleYear = sampleyears[int(sys.argv[1])]
sampletypes = ["SingleElectron", "SingleMuon", "ttbar"]
SampleType = sampletypes[int(sys.argv[2])]
errfilepath = LogsDir + SampleYear + "_" + SampleType + "/"
errfilename = SampleYear + "_" + SampleType + "_" + str(sys.argv[3]) + ".err"
ferr = errfilepath + errfilename
fout = ferr.replace(".err",".out")
print("Checking err file: " + ferr)
if not (os.path.exists(ferr) and os.path.exists(fout)):
  print("(First Run) No error log file detected")
  sys.exit(2)
# errlines = open(ferr).readlines()
# if errlines[0] == "===Job is done===": emptyerr = True
errsize = os.stat(ferr).st_size
outsize = os.stat(fout).st_size

if errsize == 0 and outsize > 100:
  print("(Successful Run) Error log is empty and Out log size = " + str(outsize))
  sys.exit(0)
if errsize == 0 and outsize < 100:
  print("(First Run) Empty error log file detected but also empty out log")
  sys.exit(2)
if errsize > 0:
  print("(Failed Run) None empty error log detected")
  with open(ferr,"r") as fferr:
    print(fferr.read())
  sys.exit(1)