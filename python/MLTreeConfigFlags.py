# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createMLTreeConfigFlags():
  mlTreeConfigFlags=AthConfigFlags()
  mlTreeConfigFlags.addFlag("MLTree.NtupleName","mltree.root") #Define output ntuple nam,e

  return mlTreeConfigFlags

