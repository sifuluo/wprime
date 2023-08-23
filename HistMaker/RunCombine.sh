#!/bin/sh

pp=/afs/cern.ch/work/s/siluo/HiggsAnalysis/CombinedLimit
# equivalent to CMSSW 11_3_0
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/gcc/9.3.0/etc/profile.d/init.sh
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/lcg/root/6.22.08-ljfedo/etc/profile.d/init.sh
# this also provides py3-six
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/py2-six/1.15.0-ljfedo/etc/profile.d/init.sh
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/py3-pandas/1.2.2/etc/profile.d/init.sh
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/gsl/2.6-ljfedo/etc/profile.d/init.sh
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/tbb/2020_U3-ljfedo/etc/profile.d/init.sh
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/cms/vdt/0.4.0-ghbfee/etc/profile.d/init.sh
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/boost/1.75.0-ljfedo/etc/profile.d/init.sh
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/pcre/8.43-bcolbf/etc/profile.d/init.sh
. /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/eigen/011e0db31d1bed8b7f73662be6d57d9f30fa457a/etc/profile.d/init.sh
export PATH=${PATH}:${pp}/build/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${pp}/build/lib
export PYTHONPATH=${PYTHONPATH}:${pp}/build/lib/python

obs=2018_WPrimeMassSimpleFL

for ir in 1153 1163 2153 2163
do
  for im in {3..11}
  do
    combine -m ${im}00 CombineCard_${obs}_${ir}.txt
  done
  hadd Limits_${obs}_${ir}.root higgsCombineTest.AsymptoticLimits.mH*.root
  rm higgsCombineTest.AsymptoticLimits.mH*.root
done