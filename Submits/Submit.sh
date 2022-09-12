#!/bin/sh
export X509_USER_PROXY=$1
voms-proxy-info -all
voms-proxy-info -all -file $1

# dir=$(pwd)
cd /afs/cern.ch/work/s/siluo/CMSSW/CMSSW_12_5_0_pre1/src
export SCRAM_ARCH=slc7_amd64_gcc10
eval `scramv1 runtime -sh`
cd /afs/cern.ch/work/s/siluo/wprime/
root -l -b "Validation.cc+($3,$4,$5,$2)"
flagname=/afs/cern.ch/work/s/siluo/wprime/SuccessFlags/${3}_${4}_${5}_${2}.txt
checkedname=/afs/cern.ch/work/s/siluo/wprime/SuccessFlags/Checked_${3}_${4}_${5}_${2}.txt
if [ -e $flagname ]
then
  mv flagname checkedname
  exit 0
else
  echo "Job did not output the SuccessFlag. Now exiting 1 and retrying......"
  exit 1
fi
