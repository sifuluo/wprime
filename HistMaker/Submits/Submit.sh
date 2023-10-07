#!/bin/sh

cd /afs/cern.ch/work/s/siluo/wprime/HistMaker/
python ErrDetector.py $2 $3 $1
errcode=$?
if [ ${errcode} -eq 0 ]; then
  echo ErrorCode is ${errcode}, exiting
  exit ${errcode}
fi

root -l -b -q "MakeHistValidation.cc+($2,$3,$1,true)"
echo The root exit code is $?
python ErrDetector.py $2 $3 $1
errcode=$?
echo ErrorCode is ${errcode}
exit ${errcode}
