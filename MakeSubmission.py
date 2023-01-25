import os
import GetLocalFileNames

SampleYears = ["2016apv","2016","2017","2018"]
SampleTypes = ["SingleElectron","SingleMuon"]
# SampleTypes.extend(["ttbar"])
# SampleTypes.extend(["wjets_HT_70_100", "wjets_HT_100_200", "wjets_HT_200_400", "wjets_HT_400_600","wjets_HT_600_800", "wjets_HT_800_1200", "wjets_HT_1200_2500", "wjets_HT_2500_inf"])
# SampleTypes.extend(["single_antitop_tchan","single_antitop_tw","single_top_schan","single_top_tchan","single_top_tw"])
# SampleTypes.extend(["Private_FL_M500"])
SampleTypes = GetLocalFileNames.GetSampleTypes()
print(SampleTypes)
Triggers = ["SE","SM"]


#Remove Running stuff.
RunningYears = ["2018"]
# RunningTypes = ["SingleMuon","ttbar"]
RunningTypes = SampleTypes
RunningTriggers = ["SM"]

EOSPath = "/eos/user/s/siluo/WPrimeAnalysis/"
EOSSubFolderName = "Validation"

with open("Utilities/UserSpecificsSkeleton.cc","r") as skeleton:
  EOSPathFile = "Utilities/UserSpecifics.cc"
  fout = open(EOSPathFile,"w")
  # EOSLines = skeleton.read()
  for el in skeleton:
    el = el.replace("Replacement_EOSBasePath",EOSPath)
    fout.write(el)

if not os.path.exists("Submits"):
  os.makedirs("Submits")


# for iy, year in enumerate(SampleYears):
#   # if not (year in RunningYears): continue
#   for isa, sampletype in enumerate(SampleTypes):
#     # if not (sampletype in RunningTypes): continue
#     for itr, trigger in enumerate(Triggers):
#       # if not (trigger in RunningTriggers): continue
#       if isa + itr == 1: continue
#       filenamesfolder = "filenames/"
#       filenamesfile = (sampletype + "_" + year + ".txt")
#       runname = year + "_" + sampletype + "_" +trigger
#       fnfile = filenamesfolder + filenamesfile
#       if not os.path.exists(fnfile): continue
#       if (sampletype == "SingleElectron" and trigger == "SM") or (sampletype == "SingleMuon" and trigger == "SE"): continue

#       nf = len(open(fnfile).readlines())
#       lines = []
#       # lines.append("Proxy_path   = /afs/cern.ch/user/s/siluo/x509up\n")
#       # lines.append("arguments    = $(Proxy_path) $(ProcID) "+str(iy)+ " " + str(isa) + " " + str(itr) + "\n")
#       lines.append("arguments    = $(ProcID) "+str(iy)+ " " + str(isa) + " " + str(itr) + "\n")
#       lines.append("executable   = Submit.sh\n")
#       lines.append("max_retries  = 10\n")
#       # lines.append("+JobBatchName= " + runname +"\n")
#       lines.append("batch_name   = " + runname +"\n")
#       lines.append("output       = logs/"+runname+"/$(ClusterID)_$(ProcID).out\n")
#       lines.append("error        = logs/"+runname+"/$(ClusterID)_$(ProcID).err\n")
#       lines.append("log          = logs/"+runname+"/$(ClusterID)_$(ProcID).log\n")
#       lines.append("universe     = vanilla\n")
#       lines.append('Requirements = (OpSysAndVer =?= "CentOS7")\n')
#       # lines.append('+JobFlavour  = "workday"\n')
#       lines.append('+JobFlavour  = "longlunch"\n')
#       lines.append("RequestCpus  = 2\n")
#       lines.append("periodic_release =  (NumJobStarts < 10) && ((CurrentTime - EnteredCurrentStatus) > (5*60))\n")
#       lines.append("queue "+str(nf)+"\n")

#       fn = "Submits/" + runname + ".sub"
#       f = open(fn, "w")
#       f.writelines(lines)

#       logpath = "Submits/logs/" + runname
#       if not os.path.exists("Submits/logs/" + runname):
#         os.makedirs("Submits/logs/" + runname)

#       eossubfolder = EOSSubFolderName + "/"
#       jobsubfolder = year + "_" + sampletype + "_" + trigger + "/"
#       jobpath = EOSPath + eossubfolder + jobsubfolder
#       if not os.path.exists(jobpath):
#         os.makedirs(jobpath)
