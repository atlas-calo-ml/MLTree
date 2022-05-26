from AthenaConfiguration.ComponentFactory import CompFactory

if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Exec.SkipEvents=9
    cfgFlags.Exec.MaxEvents=1
    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files= ["/data/hodgkinson/dataFiles/mc20_13TeV/mc20_13TeV.426327.ParticleGun_single_piminus_logE5to2000.recon.ESD.e5661_s3781_r13300/ESD.27658295._000043.pool.root.1"]
    cfgFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(cfgFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))

    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    cfg.merge(CaloTopoClusterCfg(cfgFlags))
     
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    Trk__ParticleCaloExtensionToolFactory=CompFactory.Trk.ParticleCaloExtensionTool
#    cfg.popToolsAndMerge(AtlasExtrapolatorCfg(cfgFlags))
    pcExtensionTool = Trk__ParticleCaloExtensionToolFactory(Extrapolator = cfg.popToolsAndMerge(AtlasExtrapolatorCfg(cfgFlags)))
    
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
                           TheTrackExtrapolatorTool=pcExtensionTool)

    cfg.addEventAlgo(MLTreeMaker)

    cfg.run()
