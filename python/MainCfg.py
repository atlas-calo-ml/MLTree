# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def __MLTree():
  from MLTree.MLTreeConfigFlags import createMLTreeConfigFlags
  return createMLTreeConfigFlags()

def GeneralServicesCfg(cfgFlags):

  from AthenaConfiguration.MainServicesConfig import MainServicesCfg
  cfg = MainServicesCfg(cfgFlags)

  StoreGateSvc=CompFactory.StoreGateSvc
  cfg.addService(StoreGateSvc("DetectorStore"))

  histSvc = CompFactory.THistSvc(Output = ["OutputStream DATAFILE='"+ cfgFlags.MLTree.NtupleName+"', OPT='RECREATE'"])
  cfg.addService(histSvc)

  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  cfg.merge(PoolReadCfg(cfgFlags))

  return cfg

def MainCfg(cfgFlags):
    
    cfg = GeneralServicesCfg(cfgFlags)

    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    cfg.merge(CaloTopoClusterCfg(cfgFlags))

    #Given we rebuild topoclusters above, we must also rerun pflow
    #because when the topoclusters update then the links from FlowElement
    #to topocluster can become invalid. Rerunning pflow using
    #the rebuilt topoclusters solves this.
    #Note that the below config does not rebuild all Global PFlow links
    #which are not used in MLTreeMaker, so are not relevant in this context.
    from eflowRec.PFRun3Config import PFFullCfg
    cfg.merge(PFFullCfg(cfgFlags))
     
    from eflowRec.PFRun3Remaps import ListRemaps
 
    list_remaps=ListRemaps()
    for mapping in list_remaps:
      cfg.merge(mapping)    
         
    #decorate the topoclusters with calib hit calculations
    from CaloCalibHitRec.CaloCalibHitDecoratorCfg import CaloCalibHitDecoratorCfg, CaloCalibHitDecoratorFullEnergyCfg 
    #The decoration algorithms add the leading numTruthParticles energy deposits, in that topocluster, to each topocluster
    numTruthParticles = 10
    cfg.merge(CaloCalibHitDecoratorCfg(cfgFlags,name="CaloCalibClusterDecoratorAlgorithm_Visible", 
                                       CaloClusterWriteDecorHandleKey_NLeadingTruthParticles = "CaloTopoClusters."+cfgFlags.Calo.TopoCluster.CalibrationHitDecorationName+"_Visible",
                                       NumTruthParticles = numTruthParticles))

    cfg.merge(CaloCalibHitDecoratorFullEnergyCfg(cfgFlags,name="CaloCalibClusterDecoratorAlgorithm_Full",
                                                 CaloClusterWriteDecorHandleKey_NLeadingTruthParticles = "CaloTopoClusters."+cfgFlags.Calo.TopoCluster.CalibrationHitDecorationName+"_Full",
                                                 NumTruthParticles = numTruthParticles))    

    return cfg