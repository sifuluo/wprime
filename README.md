# WPrime
This is an analysis framework to read from the nanoAOD files, process the contents into a tree for plotting.

## Running the package

## Step 1: Listing files to run over
This package runs on Andrew's skimmed samples.
To create a file list. Running the python scipt ```python3 GetLocalFileNames.py```

Note the syntax to append a dataset is ```datasets.append(["in_name","out_name"])```, Because some datasets are simplified internally.


## Step 2: Creating folders and submission scripts
```python MakeSubmission.py``` takes care of this. And the products are intended to run on lxplus condor system.
The SampleTypes can be customized, and the default will be the list set in the previous code ```GetLocalFileNames.py```
Beware to mind the parameters ```EOSPath``` and ```EOSSubFolderName```, which direct the target location of the outputs, you may need to set it to your own EOS path.
Also since we are not using the fitter yet, check ```FileSplit``` value to be 0 to disable the fitter and file splitting function.


## Step 3: Create auxiliary hists
bTagging efficiency plots are always required for any further analysis.
By running ```root CreatebTagEffHist.cc+({iYear},{iSampleType})``` bTagging efficiency plots will be made and stored at EOSPath/AuxHists/
Note not to run on Data datasets, because those will not have any truth information and may result in crash.

For ttbar, where the number of events is extremely high, you can run the jobs on condor.
In path ```wprime/Submits/Submit.sh``` you may comment out the line that runs ```root -b -q "Validation.cc()"``` and uncomment the next line that runs ```CreatebTagEffHist.cc``` instead.
And it is ready to submit the ttbar job by cd into ```wprime/Submits/``` and do command like ```condor_submit 2018_ttbar.sub``` if 2018 is the year you are aiming for.
After running the batch job, remember to hadd those batch files at EOSPath/AuxHists/batch/ into one to stay consistent with the other datasets in EOSPath/AuxHists/

Next we need to merge those plots from different datasets into a merged files to be used by all later steps:
Simply cd into ```wprime/``` and do ```root MergeAuxHists.cc+({iYear},0)``` will merge the bTag Efficiency files by the datasets cross-section and luminosity.

If fitter is going to be activated in the analysis, jet response plots and permutation distribution plots must also be produced.
Similar to bTagging efficiencies, now the command to run is ```root CreateAuxHists.cc+({iYear},{iSampleType})```
However, unlike bTagging efficiencies, Jet responses is only designed to be run on signal samples. And considering their relatively small sample size, batch function is not added.

After producing the individual auxiliary plot for each dataset, merge them with ```root MergeAuxHists.cc+({iYear},1)```

Now the main code should be ready to roll.

## Step 4: Main analysis code
The main code is ```Validation.cc+({iYear},{iSampleType},{iFile},{FilesPerJob},{IsTestRun})```
Note the FilesPerJob is designed for splitting long-running files into number of jobs, which should yield an integer if divided by 1.0.
FilesPerJob is defaulted to 0, which disables both the fitter and the file splitting. Any non-zero value will enable the fitter, including 1.0.

If ```IsTestRun``` is not 0, the output path will be ```wprime/outputs/``` instead of the EOS paths, and the progress printing interval will be 1 event instead of 1000.

### Step 5: Submitting the job
Before batch submitting the job, you need first compile the code once with something like: ```root Validation.cc+(3,1,0,0,1)```,
where the parameters correspond to the ones set in the main analysis code.
Otherwise the jobs will all try to compile the code and lead to conflicts.
This step can also serve as a test run.

After it compiled, please check ```MakeSubmission.py```
if you are going to run fitter, then confirm ```FileSplit = 10``` or any other desired integer that can be divided by 1.0 to get a definite number.
Otherwise keep ```FileSplit = 0```
And run ```python3 MakeSubmission.py``` again to update the submission scripts

Then one can cd to ```wprime/Submits/``` and submit the jobs with ```condor_submit xxx.sub```


### Under construction below here

### Step 6: Plotting from the tree
https://github.com/sifuluo/DrawTools is the package that takes care of the output tree and draw plots.

In https://github.com/sifuluo/DrawTools/blob/master/Dataset.cc the cross-section, number of events before skimming, luminosity of each year.

It can give the normalization factor by ```DatasetLib.GetNormFactor(string DatasetName, Index_of_Year)```, where the two parameters stay consistent with previous steps.

```RatioPlot.cc``` Provides a macro to draw ratio plots with CMS frame.

An example to use it is ```test.cc```

---

# Further information

Everything happening in the framework during the analysis is in the Utilities folder. 

## DataFormat.cc

DataFormat.cc holds the structs into which all the information during preprocessing is dumped. Comments exists to describe each members of the structs.

## BranchReader.cc

BranchReader.cc is the direct interface for the input nanoAOD files.
It reads the branches and feed the contents to NanoAODReader.cc for preprocessing.

## NanoAODReader.cc

NanoAODReader.cc based on the four parameters in the Configs used to initialize the derived Analyzer in analysis macro,
determines the specific nanoAOD file to read from and owns the TChain or TTree to be read by BranchReader.cc

Then it takes the raw information from the nanoAOD files through BranchReader.cc.

It preprocesses the information, dumps them into physics objects defined in DataFormat.cc and works out a RegionID for event selections later.

## Analyzer.cc

Analyzer.cc takes the preprocessed information from NanoAODReader and process them with other modules such as the PUReweight and the ones you defined in your analysis macros in the derived analyzer class.

Analyzer also determines event selection based on the RegionID NanoAODReader.cc offers.

Later the kinematic fitter will be incorporated into Analyzer as well.
