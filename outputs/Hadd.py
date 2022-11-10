import os

EOSFolderName = "Validation"
basepath = "/eos/user/s/siluo/WPrimeAnalysis/" + EOSFolderName + "/"
for d in os.listdir(basepath) :
  sourcepath = basepath + d + "/"
  if not len(os.listdir(sourcepath)): continue
  for f in os.listdir(sourcepath):
    filepath = sourcepath + f
    if os.stat(filepath).st_size < 1024:
      os.remove(filepath)
  if os.path.exists(d+".root"):
    os.remove(d+".root")
  command = "hadd " + d + ".root " + sourcepath + "*"
  print("Running "+ command)
  os.system(command)
