#You can run this code from the command line (after Athena setup) with a command:
#python runDiJets.py --filesInput=<inputFileName> MLTree.NtupleName=<outputNtupleFileName>"

#This configuration is designed to run on di-jet samples for the HSF May 2023 Workshop

if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags
    from MLTree.MainCfg import __MLTree
    cfgFlags.addFlagsCategory("MLTree",__MLTree)

    cfgFlags.Exec.MaxEvents=-1
    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.recon.ESD.e6337_e5984_s3170_r12674/ESD.25732025._000034.pool.root.1"]
    cfgFlags.Concurrency.NumThreads=1
    cfgFlags.fillFromArgs()
    cfgFlags.lock()


    from MLTree.MainCfg import MainCfg
    cfg = MainCfg(cfgFlags)

    from MLTree.MLTreeMakerCfg import MLTreeMakerCfg
    cfg.merge(MLTreeMakerCfg(cfgFlags,
                          TrackContainer = "InDetTrackParticles",
                           CaloClusterContainer = "CaloCalTopoClusters",
                           ClusterEmin = 0.0,
                           ClusterEmax = 2000.0,
                           ClusterEtaAbsmax = 3.0,
                           EventCleaning = False,
                           Tracking = True,
                           Pileup = False,
                           AllCells = True,
                           Clusters = True,
                           ClusterCells = True,
                           ClusterCalibHits = True,
                           ClusterCalibHitsPerCell = False,
                           ClusterMoments = True,
                           UncalibratedClusters = True,
                           TruthParticles = True,
                           EventTruth = False,
                           G4TruthParticles = False,
                           Jets = True,
                           JetContainers = ["AntiKt4EMPFlowJets","AntiKt4TruthJets"],
                           RootStreamName = "OutputStream"))                         

    cfg.run()
