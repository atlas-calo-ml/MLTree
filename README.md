# MLTree 
Athena package to save cell images of clusters for ML training.

For questions please contact: joakim.olsson[at]cern.ch

## Setup

```
mkdir MLTreeAthenaAnalysis; cd MLTreeAthenaAnalysis
git clone https://github.com/jmrolsson/MLTree.git 
setupATLAS
asetup 20.7.7.9,AtlasProduction,here
lsetup panda
cmt find_packages && cmt compile 
```

## Test run

```
mkdir run; cd run
athena MLTree/MLTreeMaker.py
```
