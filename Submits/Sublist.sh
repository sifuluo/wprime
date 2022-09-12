#!/bin/sh

for y in 2016 2017; do
  for t in SE SM; do
    for i in SingleElectron SingleMuon ttbar; do
      echo Submitting ${y}_${i}_${t}.sub
      condor_submit ${y}_${i}_${t}.sub
    done
    for i in 70_100 100_200 200_400 400_600 600_800 800_1200 1200_2500 2500_inf; do
      echo Submitting ${y}_wjets_HT_${i}_${t}.sub
      condor_submit ${y}_wjets_HT_${i}_${t}.sub
    done
    for i in antitop_tchan antitop_tw top_schan top_tchan top_tw; do
      echo Submitting ${y}_single_${i}_${t}.sub
      condor_submit ${y}_single_${i}_${t}.sub

  done
done
