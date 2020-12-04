#! /usr/bin/env python

# @file     submit_grid.py
# @author   Joakim Olsson <joakim.olsson@cern.ch>
# @brief    Launch grid jobs with the MLTree package
# @date     October 2016

import os
import subprocess as sp

try:
  __version__ = sp.check_output(["git","describe","--tags"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()
except:
  print("git not available to extract current tag")
  __version__ = "test"

import argparse
parser = argparse.ArgumentParser(add_help=True, description="Launch grid jobs", epilog="version: {0:s}".format(__version__))
parser.add_argument("--user", required=True, type=str, dest="user", metavar="<user>", help="Username")
parser.add_argument("--tag", required=False, type=str, dest="tag", default = __version__, metavar="<tag>", help="Output file tag")
parser.add_argument("--datasets", type=str, dest="datasets", required=False, default="datasets.json", metavar = "<datasets.json>", help="JSON file specifying the input and output datasets.")
parser.add_argument("--nFilesPerJob", required=False, type=int, dest="nFilesPerJob", default=1, help="Number of files per job")
# If sub-jobs exceed the walltime limit, they will get killed. When you want to submit long running jobs (e.g., customized G4 simulation), submit them to sites where longer walltime limit is available by specifying the expected execution time (in second) to the --maxCpuCount option.
parser.add_argument("--maxCpuCount", required=False, type=int, dest="maxCpuCount", default=172800, help="Max CPU time (default: 48 hrs)")
parser.add_argument("--dry-run", dest="dryrun", action="store_true", help="Don't submit any jobs")
args = parser.parse_args()

import json
datasets = json.load(file(args.datasets))
inDSs = datasets.get("inDSs", {})
outDSs = datasets.get("outDSs", {})

doBuild = True
doBuildAll = True

setup = "MLTree/MLTreeMaker.py"
#config = "--nFilesPerJob "+str(args.nFilesPerJob)+" --maxCpuCount "+str(args.maxCpuCount)
#config = "--nFilesPerJob "+str(args.nFilesPerJob)+" --allowTaskDuplication"
config = "--nFilesPerJob "+str(args.nFilesPerJob)

comFirst = "pathena {} --outDS {} --inDS {} {}"
comLater = "pathena {} --outDS {} --inDS {} --libDS LAST {}"

# Submit jobs to the grid with pathena
# https://twiki.cern.ch/twiki/bin/view/PanDA/PandaAthena
for i, inDS, outDS in zip(xrange(len(inDSs)), inDSs, outDSs):
  outDS = "user."+args.user+"."+outDS+"_"+args.tag
  if (i==0 and doBuild) or doBuildAll:
    command = comFirst.format(setup, outDS, inDS, config)
  else:
    command = comLater.format(setup, outDS, inDS, config)
  sp.call("echo "+command, shell=True)
  if not args.dryrun:
    sp.call(command, shell=True)
