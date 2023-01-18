#!/bin/sh

cd /afs/cern.ch/work/s/siluo/wprime/
root -l -b -q "Validation.cc+($2,$3,$4,$1)"
exit $?
