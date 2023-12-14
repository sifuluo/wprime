#!/bin/sh

cd __cwd__
python ErrDetector.py $2 $3 $1
errcode=$?
if [ ${errcode} -eq 0 ]; then
  echo ErrorCode is ${errcode}, exiting
  exit ${errcode}
fi
root -l -b -q "Validation.cc+($2,$3,$1)"
# root -l -b -q "CreatebTagEffHist.cc+($2,$3,$1)"

echo The root exit code is $?
python ErrDetector.py $2 $3 $1
errcode=$?
echo ErrorCode is ${errcode}
exit ${errcode}
