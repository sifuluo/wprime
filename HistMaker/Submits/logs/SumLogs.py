import os

for d in os.listdir("."):
  if not os.path.isdir(d): continue
  files = os.listdir(d)
  print("In folder " + d)
  for f in files:
    if not ".log" in f: continue
    flog = d + "/" + f
    ferr = d + "/" + f.replace(".log",".err")
    fout = d + "/" + f.replace(".log",".out")
    if os.path.exists(ferr):
      if os.stat(ferr).st_size > 0:
        # os.path.getsize(ferr)
        print("Error log: " + ferr)
        continue
      # else:
        # os.remove(flog)
        # os.remove(ferr)
        # os.remove(fout)