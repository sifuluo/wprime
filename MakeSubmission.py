import os

years = ["2016apv","2016","2017","2018"]
SampleTypes = ["SingleElectron","SingleMuon","ttbar"]
Triggers = ["SE","SM"]

if not os.path.exists("Submits"):
  os.makedirs("Submits")

MCBaseFolder = "/eos/user/d/doverton/"
for iy, year in enumerate(years):
  for isa, sampletype in enumerate(SampleTypes):
    for itr, trigger in enumerate(Triggers):
      # filenamesfolder = "/afs/cern.ch/work/s/siluo/wprime/datafiles/" if (sampletype == "SingleElectron" or sampletype == "SingleMuon") else "/afs/cern.ch/work/s/siluo/wprime/mcfiles/"
      if isa + itr == 1: continue
      filenamesfolder = "/afs/cern.ch/work/s/siluo/wprime/filenames/"
      filenamesfile = (sampletype + "_" + year + ".txt")
      runname = sampletype+"_"+year+ "_" +trigger
      fnfile = filenamesfolder + filenamesfile
      if not os.path.exists(fnfile): continue
      if (sampletype == "SingleElectron" and trigger == "SM") or (sampletype == "SingleMuon" and trigger == "SE"): continue

      nf = len(open(fnfile).readlines())
      lines = []
      lines.append("Proxy_path   = /afs/cern.ch/user/s/siluo/x509up\n")
      lines.append("arguments    = $(Proxy_path) $(ProcID) "+str(iy)+ " " + str(isa) + " " + str(itr) + "\n")
      lines.append("executable   = Submit.sh\n")
      lines.append("max_retries  = 10\n")
      lines.append("+JobBatchName= " + runname +"\n")
      lines.append("output       = log"+runname+"/$(ClusterID)_$(ProcID).out\n")
      lines.append("error        = log"+runname+"/$(ClusterID)_$(ProcID).err\n")
      lines.append("log          = log"+runname+"/$(ClusterID)_$(ProcID).log\n")
      lines.append("universe     = vanilla\n")
      lines.append('Requirements = (OpSysAndVer =?= "CentOS7")\n')
      lines.append('+JobFlavour  = "longlunch"\n')
      lines.append("RequestCpus  = 2\n")
      lines.append("periodic_release =  (NumJobStarts < 10) && ((CurrentTime - EnteredCurrentStatus) > (5*60))\n")
      lines.append("queue "+str(nf)+"\n")

      fn = "Submits/" + runname + ".sub"
      f = open(fn, "w")
      f.writelines(lines)

      if not os.path.exists("Submits/log" + runname):
        os.makedirs("Submits/log" + runname)
