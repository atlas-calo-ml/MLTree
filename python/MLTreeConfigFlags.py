# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

import os

def createMLTreeConfigFlags():
  outfile = os.getenv('OUTFILE') if os.getenv('OUTFILE')!='' else "mltree.root"
  mlTreeConfigFlags=AthConfigFlags()
  mlTreeConfigFlags.addFlag("MLTree.NtupleName",outfile) #Define output ntuple name

  return mlTreeConfigFlags

