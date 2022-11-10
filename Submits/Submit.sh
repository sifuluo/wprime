#!/bin/sh
export X509_USER_PROXY=$1
voms-proxy-info -all
voms-proxy-info -all -file $1

# dir=$(pwd)
cd /afs/cern.ch/work/s/siluo/CMSSW/CMSSW_12_5_0_pre1/src
export SCRAM_ARCH=slc7_amd64_gcc10
eval `scramv1 runtime -sh`
cd /afs/cern.ch/work/s/siluo/wprime/
root -l -b -q "Validation.cc+($3,$4,$5,$2)"
exit $?
