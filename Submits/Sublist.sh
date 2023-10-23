#!/bin/sh
for i in ttbar wjets_HT_70_100 wjets_HT_100_200 wjets_HT_200_400 wjets_HT_400_600 wjets_HT_600_800 wjets_HT_800_1200 wjets_HT_1200_2500 wjets_HT_2500_inf single_top_schan single_top_tchan single_antitop_tchan single_top_tw single_antitop_tw WW WZTo1L1Nu2Q WZTo1L3Nu WZTo2Q2L WZTo3LNu
# ZZ is not in the list
do 
  echo Submitting 2016 ${i}
  condor_submit 2016_${i}.sub
  echo Submitting 2017 ${i}
  condor_submit 2017_${i}.sub
  echo Submitting 2018 ${i}
  condor_submit 2018_${i}.sub
done
