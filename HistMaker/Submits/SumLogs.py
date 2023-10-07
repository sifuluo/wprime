import os

for d in os.listdir("logs/"):
  dd = "logs/"+d
  if not os.path.isdir(dd): continue
  files = os.listdir(dd)
  haserr = 0
  # print("In folder " + dd)
  for f in files:
    if not ".log" in f: continue
    flog = dd + "/" + f
    ferr = dd + "/" + f.replace(".log",".err")
    fout = dd + "/" + f.replace(".log",".out")
    if os.path.exists(ferr):
      if os.stat(ferr).st_size > 0:
        # os.path.getsize(ferr)
        print("Error log: " + ferr)
        haserr += 1
        continue
      # else:
        # os.remove(flog)
        # os.remove(ferr)
        # os.remove(fout)
  if haserr > 0:
    print("There are " + str(haserr) + " error processes in above folder" + dd)
    print("-------------------------------")