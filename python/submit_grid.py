#! /usr/bin/env python
import commands
import subprocess as sp

# Joakim Olsson <joakim.olsson@cern.ch>

nFilesPerJob = 20

# If sub-jobs exceed the walltime limit, they will get killed. When you want to submit long running jobs (e.g., customized G4 simulation), submit them to sites where longer walltime limit is available by specifying the expected execution time (in second) to the --maxCpuCount option.
maxCpuCount = 252000 # 70 hrs ##172800 # 48 hrs

tag = '20161216_4'
user = 'jolsson'

doBuild = True
doBuildAll = True

inDSs = ["mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.recon.ESD.e3496_s2139_s2132_r6569",
         "mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e3501_s2141_s2132_r6569"]

outDSs = ["mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.e3496_s2139_s2132_r6569_images",
          "mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.e3501_s2141_s2132_r6569_images"]

setup = 'MLTree/MLTreeMaker.py'
config = '--nFilesPerJob '+str(nFilesPerJob)+' --maxCpuCount '+str(maxCpuCount)

comFirst = 'pathena {} --outDS {} --inDS {} {}'
comLater = 'pathena {} --outDS {} --inDS {} --libDS LAST {}'

# Submit jobs to the grid with pathena
# https://twiki.cern.ch/twiki/bin/view/PanDA/PandaAthena
for i,inDS in enumerate(inDSs):
    outDS = 'user.'+user+'.'+outDSs[i]+'.'+tag
    # print
    # print 'Input dataset: '+inDS
    # print 'Output dataset: '+outDS
    if (i==0 and doBuild) or doBuildAll:
        command = comFirst.format(setup, outDS, inDS, config)
    else:
        command = comLater.format(setup, outDS, inDS, config)
    sp.call('echo '+command, shell=True)
    sp.call(command, shell=True)
