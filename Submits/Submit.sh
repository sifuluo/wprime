#!/bin/sh

cd /afs/cern.ch/work/s/siluo/wprime
root -l -b -q "Validation2.cc+($2,$3,$1)"
# root -l -b -q "CreatebTagEffHist.cc+($2,$3,$1)"
exit $?
