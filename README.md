# MLTree 
Athena package to save a tree that includes clusters, cells, tracks and truth information for projects using ML and Computer Vision.

More functionality and documentation to be added asap!

For questions please contact: joakim.olsson[at]cern.ch

## Setup

```
mkdir MLTreeAthenaAnalysis; cd MLTreeAthenaAnalysis
git clone https://github.com/jmrolsson/MLTree.git 
setupATLAS
asetup 20.7.7.4,AtlasDerivation,here
lsetup panda
cmt find_packages && cmt compile 
```

## Test run

```
mkdir run; cd run
athena MLTree/MLTreeMaker.py
```
