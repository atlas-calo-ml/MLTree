# MLTree 

ATLAS Athena package to save calorimeter clusters as images, with normalized cell energies as pixel values. Six images are saved for each cluster, corresponding to the barrels layers of the EM (EMB1, EMB2, EMB3) and HAD calorimeters (TileBar0, TileBar2, TileBar3). The image size is 0.4x0.4 in eta-phi space. Images are generated from ESD (Event Summary Data).

For questions please contact: joakim.olsson[at]cern.ch

## Setup

<details>
<summary>Using release 20</summary>
<br>
<pre>mkdir MLTreeAthenaAnalysis; cd MLTreeAthenaAnalysis
git clone https://github.com/jmrolsson/MLTree.git 
setupATLAS
#asetup 20.7.7.9,AtlasProduction,here
asetup 20.1.0.3,AtlasProduction,here
lsetup panda
cmt find_packages && cmt compile</pre>
</details>

Using release 21

Follow instructions for sparse checkout; this is a clunky way to get the athena/Projects directory structure.
```
mkdir MLTreeAthenaAnalysis; cd MLTreeAthenaAnalysis
setupATLAS
lsetup git
git atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git
```

Due to recent changes on the master branch, the 21.3 branch must be used
```
cd athena
git checkout -b 21.3 upstream/21.3
cd ..
```

Clone this git repository and create a package filter so athena knows to compile it
```
git clone https://github.com/angerami/MLTree.git athena/MLTree
echo "+ MLTree" > package_filters.txt
echo "- .*" >> package_filters.txt
```

Now setup for an out-of-source build
```
mkdir build; cd build
asetup 21.3,latest,Athena
cmake -DATLAS_PACKAGE_FILTER_FILE=../package_filters.txt ../athena/Projects/WorkDir
make
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
