from AthenaConfiguration.ComponentFactory import CompFactory

def __MLTree():
  from MLTree.MLTreeConfigFlags import createMLTreeConfigFlags
  return createMLTreeConfigFlags()

if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags
        
    from MLTree.MLTreeConfigFlags import createMLTreeConfigFlags
    cfgFlags.addFlagsCategory("MLTree",__MLTree)
    #cfgFlags.MLTree.NtupleName="mark"    

    cfgFlags.Exec.MaxEvents=-1
    cfgFlags.Input.isMC=True
    #cfgFlags.Concurrency.NumThreads=4
    #cfgFlags.Concurrency.NumProcs=1
    #cfgFlags.Input.Files= ["/data/hodgkinson/dataFiles/mc20_13TeV/ESDFiles/mc20_13TeV.426327.ParticleGun_single_piminus_logE5to2000.recon.ESD.e5661_s3781_r13300/ESD.27658295._000043.pool.root.1"]    
    #cfgFlags.Input.Files=["/data/hodgkinson/dataFiles/mc20_13TeV/ESDFiles/mc20_13TeV.426327.ParticleGun_single_piminus_logE5to2000.recon.ESD.e5661_s3170_r13300/ESD.28115683._000210.pool.root.1"]
    cfgFlags.fillFromArgs()
    cfgFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(cfgFlags)

    StoreGateSvc=CompFactory.StoreGateSvc
    cfg.addService(StoreGateSvc("DetectorStore"))

    histSvc = CompFactory.THistSvc(Output = ["OutputStream DATAFILE='"+ cfgFlags.MLTree.NtupleName+"', OPT='RECREATE'"])
    cfg.addService(histSvc)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))

    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    cfg.merge(CaloTopoClusterCfg(cfgFlags))
     
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    Trk__ParticleCaloExtensionToolFactory=CompFactory.Trk.ParticleCaloExtensionTool
    pcExtensionTool = Trk__ParticleCaloExtensionToolFactory(Extrapolator = cfg.popToolsAndMerge(AtlasExtrapolatorCfg(cfgFlags)))
    
    from InDetConfig.InDetTrackSelectionToolConfig import PFTrackSelectionToolCfg
    from AthenaCommon.Constants import INFO
    MLTreeMaker = CompFactory.MLTreeMaker(TrackContainer = "InDetTrackParticles",
                           CaloClusterContainer = "CaloCalTopoClusters",
                           Prefix = "CALO",
                           ClusterEmin = 0.0,
                           ClusterEmax = 2000.0,
                           ClusterEtaAbsmax = 3.0,
                           EventCleaning = False,
                           Tracking = True,
                           Pileup = False,
                           Clusters = True,
                           ClusterCells = True,
                           ClusterCalibHits = True,
                           ClusterCalibHitsPerCell = False,
                           ClusterMoments = True,
                           UncalibratedClusters = True,
                           TruthParticles = True,
                           EventTruth = False,
                           OnlyStableTruthParticles = False,
                           G4TruthParticles = False,
                           Jets = True,
                           JetContainers = ["AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4TruthJets"],
                           OutputLevel = INFO,
                           TheTrackExtrapolatorTool=pcExtensionTool,
                           TrackSelectionTool=cfg.popToolsAndMerge(PFTrackSelectionToolCfg(cfgFlags)))

    cfg.addEventAlgo(MLTreeMaker)
    cfg.getEventAlgo("MLTreeMaker").RootStreamName = "OutputStream"
    cfg.getEventAlgo("MLTreeMaker").TrackSelectionTool.CutLevel = "TightPrimary"

    cfg.run()
