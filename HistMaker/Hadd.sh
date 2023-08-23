#!/bin/sh

p=/eos/user/s/siluo/WPrimeAnalysis/Validation/Hists/
pre=2018_Validation_
for i in LeptonPt LeptonEta LeptonPhi LeadingJetPt LeadingJetEta LeadingJetPhi METPt METPhi dPhiMetLep mT HT WPrimeMassSimpleFL WPrimeMassSimpleLL WPrimeMass WPrimeMassFL WPrimeMassLL Likelihood
do
hadd -f outputs/${pre}${i}.root ${p}${pre}${i}* ${p}${pre}NRW_${i}*
hadd -f outputs/${pre}RW_${i}.root ${p}${pre}${i}* ${p}${pre}RW_${i}*
done
cp ${p}${pre}ReweightSF.root outputs/