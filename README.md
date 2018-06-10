# MLTree 

ATLAS Athena package to save calorimeter clusters as images, with normalized cell energies as pixel values. Six images are saved for each cluster, corresponding to the barrels layers of the EM (EMB1, EMB2, EMB3) and HAD calorimeters (TileBar0, TileBar2, TileBar3). The image size is 0.4x0.4 in eta-phi space. Images are generated from ESD (Event Summary Data).

For questions please contact: joakim.olsson[at]cern.ch

## Setup

```
mkdir MLTreeAthenaAnalysis; cd MLTreeAthenaAnalysis
git clone https://github.com/jmrolsson/MLTree.git 
setupATLAS
#asetup 20.7.7.9,AtlasProduction,here
asetup 20.1.0.3,AtlasProduction,here
lsetup panda
cmt find_packages && cmt compile 
```

## Test run

This requires that input test files exist, which is specified in [MLTreeMaker.py](share/MLTreeMaker.py)

```
mkdir run; cd run
athena MLTree/MLTreeMaker.py
```

## Running on the grid

A script for launching grid jobs with different input files is available [here](python/launch_jobs.py): 

```
python python/launch_job.py --user <user> 
```
