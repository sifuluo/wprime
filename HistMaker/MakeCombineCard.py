import os, sys
import ROOT

#define the input names
SigNames = ["M$MASS"] #$MASS gets replaced by runing combine with -m option, supports only 1 signal mass at a time
BGNames = ["ttbar", "wjets", "single_top", "diboson"]
Regions = [1153, 1163, 2153, 2163]
# Regions.extend([1154,1164,2154,2164])
# Regions.extend([1155,1165,2155,2165])
SampleYear = 2018
obs = "WPrimeMassSimpleFL"
FilePath = "CombinedFiles/"

HistFileName = FilePath + str(SampleYear) + "_" + obs + ".root"

#define all the systematic names, types, and values
systNames = ["lumi", "EleSF", "MuonTrigger", "MuonId", "MuonIso", "BjetTagCorr", "BjetTagUncorr", "PUIDSF", "L1PreFiringSF", "PUreweightSF", "PDF",  "LHEScale", "EleScale", "EleRes", "JES",  "JER"]
systTypes = ["lnN",  "shape", "shape"      , "shape",  "shape",   "shape",       "shape",         "shape",  "shape",         "shape",        "shape","shape",    "shape",    "shape",  "shape","shape"] 
systVals  = ["1.025","1"    , "1",           "1",      "1",       "1",           "1",             "1",      "1",             "1",            "1",    "1",        "1",        "1",      "1",    "1"]

#write the actual combine card
for Region in Regions:
  Observable = obs + "_" + str(Region)
  CardName = "CombineCard_" + str(SampleYear) + "_" + Observable + ".txt"
  print("Creating Combine card file " + CardName)
  f = open(CardName,"w")
  f.write("imax " + str(1) + "\n") #number of channels
  f.write("jmax " + str(len(BGNames)) + "\n") #number of backgrounds
  f.write("kmax " + str(len(systNames)) + "\n") #number of nuisance parameters
  f.write("----------\n")
  f.write("shapes * * " + HistFileName + " $PROCESS_$CHANNEL $PROCESS_$CHANNEL_$SYSTEMATIC\n")
  f.write("----------\n")
  f.write("bin         " + Observable + "\n")

  #load ROOT file, find observation number
  print("Reading observed events numbers")
  r = ROOT.TFile.Open(HistFileName, "read")
  h = r.Get("data_obs_" + Observable)
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
  allNames = SigNames + BGNames
  allNumbers = []
  for i in range(-len(SigNames)+1, 1): #negative and zero numbers for signals
    allNumbers.append(i)
  for i in range (1, len(BGNames)+1): #positive nonzero numbers for backgrounds
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
    binLine += Observable
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
