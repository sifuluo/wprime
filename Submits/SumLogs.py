import os

RemarkableErrlogs = []
remarklogsize = 250
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
      fsize = os.stat(ferr).st_size
      if fsize > 0:
        # os.path.getsize(ferr)
        line = "Error log: " + ferr + " size = " + str(fsize)
        print(line)
        haserr += 1
        if fsize > remarklogsize: RemarkableErrlogs.append(line)
        continue
      # else:
        # os.remove(flog)
        # os.remove(ferr)
        # os.remove(fout)
  if haserr > 0:
    print("There are " + str(haserr) + " error processes in above folder " + dd)
    print("-------------------------------")
if len(RemarkableErrlogs) > 0:
  print("\nRemarkable Error logs: (size > "+ str(remarklogsize) +")")
  for line in RemarkableErrlogs:
    print(line)