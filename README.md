# WPrime
This is an analysis framework to read from the nanoAOD files, process the contents into a tree for plotting.

## Running the package

## Step 1: Listing files to run over
This package runs on Andrew's skimmed samples.
To create a file list. Running the python scipt ```python GetLocalFileNames.py```
After line 21, the samples going to be run are listed with their folder name.
Right now the default configuration is for running tWprime analysis with signal samples loaded from line 41 to line 48.
Note the syntax to append a dataset is ```datasets.append(["name"])```. with the parameter being a list of string.


## Step 2: Creating folders and submission scripts
```python MakeSubmission.py``` takes care of this. And the products are intended to run on lxplus condor system.
You will need to configure the ```SampleTypes``` to the dataset you want to process.
It is a list of strings of the folder names same as in the Step 1.
The submission scripts are located in ```Submits/```
This script also creates folders in the EOS, that is configured at line 93 (***Subject to change***).


## Step 3: Configure the Submit.sh
In ```Submits/Submit.sh``` the next to last line is the macro to be run that needs to be configured.
And the line before that should be changed to your working folder.

## Step 4: Main analysis code
In the base path of this package is where your analysis macro should be placed.
The ```Validation.cc``` and ```PileupDist.cc``` are examples of how the framework is used.

In the first parts, you need to define a derived class of Analyzer,
eg. ```class ThisAnalysis : public Analyzer {  }```.
In this class, you need to define the variables and analysis functions.

In addition to that, you need to define ```void BookBranches()``` where the branches going to be saved in the output tree and/or histograms to be saved are declared.

Then in ```void FillBranchContent() {} ``` is the main part of analysis that eventually assigns value to the tree branches and fills the histograms.

After the class definition, the code main function acts as an interface between the input arguments and the Analyzer.
This function has to have the same name as the macro's filename.
The input parameters are the indices of sample year, sample type, trigger index, and file index.

The correspondence relation between indices and actual value is enumerated in ```Utilities/Constants.cc```.

In the main function, a ```Configs``` instance need to be initialized with the four parameters. Then you can initialize your derived ```Analyzer``` object with the ```Configs```.

```Analyzer.SetOutput("folder")``` needs to be specified and the folder should stay consistent with the one set in Step 2.

A loop over events follows and there you need two lines in it:

```if (Analyzer.ReadEvent(iEvent)) continue;``` 

```Analyzer.FillBranchContent();```

After the loop simply save the file and close it:

```Analyzer.SaveOutput(); Analyzer.CloseOutput();```

In the previous statements, ```Analyzer. ``` should be replaced by the name of your derived analyzer instance. And depending on whether it is a pointer or object, an arrow or a dot is to be used.

Detailed configurations will be explained next.

### Step 5: Plotting from the tree
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