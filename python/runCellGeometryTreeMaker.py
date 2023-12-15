# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags
    from MLTree.MainCfg import __MLTree
    cfgFlags.addFlagsCategory("MLTree",__MLTree)

    cfgFlags.Exec.MaxEvents=-1
    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.recon.ESD.e6337_e5984_s3170_r12674/ESD.25732025._000034.pool.root.1"]
    cfgFlags.Concurrency.NumThreads=1
    cfgFlags.MLTree.NtupleName="CellGeo"
    cfgFlags.fillFromArgs()
    cfgFlags.lock()

    from MainCfg import GeneralServicesCfg
    cfg = GeneralServicesCfg(cfgFlags)

    from MLTree.CellGeometryTreeMaker import CellGeometryTreeMakerCfg
    cfg.merge(CellGeometryTreeMakerCfg(cfgFlags))

    cfg.run()