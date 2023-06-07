import os, sys
import ROOT

#define the input names
signalNames = ["M$MASS"] #$MASS gets replaced by runing combine with -m option, supports only 1 signal mass at a time
bgrNames = ["ttbar", "wjets", "single_top", "diboson"]
binNumber = 1152 #this needs to be set
yearNumber = 2018
binName = "Wprime" + str(binNumber)

#First, generate the shape variation histograms
#print("Starting processing of intermediate files")
#if os.path.isdir("TestHistograms"):
#  os.system("rm TestHistograms/*.root") #reset 
#else:
#  os.system("mkdir TestHistograms") #make directory, if it doesn't exist
#os.system("root -l -b -q 'runCombineHistogramDumpster.C+(" + str(binNumber) + ", " + str(yearNumber) + ")'") #This does not yet provide a year, as only 2018 is processed
#os.system("hadd -f SimpleShapes_" + binName + ".root TestHistograms/*.root") #hadd all histograms to a convenient combined file

#define all the systematic names, types, and values
systNames = ["lumi", "electron", "muonTrigger", "muonId", "muonIso", "BjetTag", "PUID", "L1PreFiring", "PUreweight", "PDF",  "LHEScale", "electronScale", "electronRes", "JES",  "JER"]
systTypes = ["lnN",  "shape"   , "shape"      , "shape",  "shape",   "shape",   "shape","shape",       "shape",      "shape","shape",    "shape",         "shape",       "shape","shape"] 
systVals  = ["1.025","1"       , "1",           "1",      "1",       "1",       "1",    "1",           "1",          "1",    "1",        "1",             "1",           "1",    "1"]

#write the actual combine card
print("Creating Combine card file")
f = open("Test_" + binName + ".txt","w")
f.write("imax " + str(1) + "\n") #number of channels
f.write("jmax " + str(len(bgrNames)) + "\n") #number of backgrounds
f.write("kmax " + str(len(systNames)) + "\n") #number of nuisance parameters
f.write("----------\n")
f.write("shapes * * SimpleShapes_" + binName + ".root $PROCESS_$CHANNEL $PROCESS_$CHANNEL_$SYSTEMATIC\n")
f.write("----------\n")
f.write("bin         " + binName + "\n")

#load ROOT file, find observation number
print("Reading observed events numbers")
r = ROOT.TFile.Open("SimpleShapes_" + binName + ".root", "read")
h = r.Get("data_obs_" + binName)
observed = h.Integral()
f.write("observation " + str(observed) + "\n")
f.write("----------\n")

##assemble strings for lines
print("Assembling lines for Combine card")
#sysetmatic lines first, as this is an entire column to be processed, later, and they are longest
systLines = []
maxLength = 0
for i in range(0, len(systNames)): #assemble systematic names
  systLines.append(systNames[i])
  maxLength = max(maxLength, len(systLines[i]))

maxLength2 = 0
for i in range(0, len(systLines)): #align systematic names, then assemble systematic types
  while len(systLines[i]) < (maxLength + 3):
    systLines[i] += " "
  systLines[i] += systTypes[i]
  maxLength2 = max(maxLength2, len(systLines[i]))

for i in range(0, len(systLines)): #align syst types
  while len(systLines[i]) < (maxLength2 + 5):
    systLines[i] += " "

maxLength2 = len(systLines[0])

#assemble bin and process block
allNames = signalNames + bgrNames
allNumbers = []
for i in range(-len(signalNames)+1, 1): #negative and zero numbers for signals
  allNumbers.append(i)
for i in range (1, len(bgrNames)+1): #positive nonzero numbers for backgrounds
  allNumbers.append(i)
binLine      = "bin     "
processLine1 = "process "
processLine2 = "process "
rateLine     = "rate    "

#align bin, process, and rate lines with systematic line length
while len(binLine) < maxLength2:
  binLine += " "
while len(processLine1) < maxLength2:
  processLine1 += " "
while len(processLine2) < maxLength2:
  processLine2 += " "
while len(rateLine) < maxLength2:
  rateLine += " "

for i in range(0, len(allNames)): #assemble bin, process, rate, and systematic line entries, then align
  binLine += binName
  processLine1 += allNames[i]
  processLine2 += str(allNumbers[i])
  rateLine += "-1" #this option makes Combine read the rate from the histogram integrals

  currentLength = max(len(binLine), len(processLine1), len(processLine2), len(rateLine))
  
  for j in range(0, len(systLines)): #assemble systematic values
    systLines[j] += systVals[j]
    currentLength = max(currentLength, len(systLines[j]))

  #align all lines with extra space
  currentLength += 5

  while len(binLine) < currentLength:
    binLine += " "

  while len(processLine1) < currentLength:
    processLine1 += " "

  while len(processLine2) < currentLength:
    processLine2 += " "

  while len(rateLine) < currentLength:
    rateLine += " "

  for j in range(0, len(systLines)):
    while len(systLines[j]) < currentLength:
      systLines[j] += " "

print("Writing Combine Card values")
f.write(binLine + "\n")
f.write(processLine1 + "\n")
f.write(processLine2 + "\n")
f.write(rateLine + "\n")
f.write("----------\n")

for i in range(0, len(systLines)):
  f.write(systLines[i] + "\n")

