# MLTree 

ATLAS Athena package to save calorimeter clusters as images, with normalized cell energies as pixel values. Six images are saved for each cluster, corresponding to the barrels layers of the EM (EMB1, EMB2, EMB3) and HAD calorimeters (TileBar0, TileBar2, TileBar3). The image size is 0.4x0.4 in eta-phi space. Images are generated from ESD (Event Summary Data).

For questions please contact: joakim.olsson[at]cern.ch in general and m.hodgkinson[at]sheffield.ac.uk for questions specific to the Release 22 branch.

## Setup

<details>
<summary> Using release 22</summary>
<br>
<pre>mkdir MLTreeAthenaAnalysis; cd MLTreeAthenaAnalysis
setupATLAS
lsetup git
git atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git
cd athena
git clone https://github.com/atlas-calo-ml/MLTree.git athena/MLTree
cd athena/MLTree
git checkout upstream/Release22
cd ../../
echo "+ MLTree" > package_filters.txt
echo "- .*" >> package_filters.txt
mkdir build; cd build
asetup Athena,22.0.69
cmake -DATLAS_PACKAGE_FILTER_FILE=../package_filters.txt ../athena/Projects/WorkDir
make
source ../build/x86*/setup.sh
mkdir ../run;cd ../run
#adjust input file name in below file prior to running
python ../athena/MLTree/run/runCA.py
#To run on the grid a command like this would work
lsetup panda
prun --exec="python runCA.py --filesInput=%IN MLTree.NtupleName=mltree.root" --inDS=mc20_13TeV.426332.ParticleGun_single_piplus_E0p4to2.recon.ESD.e5661_s3170_r13300 --outDS=user.mhodgkin.mc20_13TeV.426332.ParticleGun_single_piplus_E0p4to2.MLTree.e5661_s3170_r13300.V3 --useAthenaPackage --outputs="mltree.root" --nFilesPerJob=5
#Please choose your own in and outDS. 
#You must specify nFilesPerJob because the default choice results in the jobs running out of memory
#useAthenaPackage sends your local environment with the job
#outputs is the name of the file the job produces locally.
</pre>
</details>

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

<details>
<summary>Using release 21</summary>
<br>
<pre>
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
git clone https://github.com/atlas-calo-ml/MLTree.git athena/MLTree
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
</pre>
</details>
