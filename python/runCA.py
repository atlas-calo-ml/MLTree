#You can run this code from the command line (after Athena setup) with a command:
#python runCA.py --filesInput=<inputFileName> MLTree.NtupleName=<outputNtupleFileName>"

if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags
    from MLTree.MainCfg import __MLTree
    #from AthenaCommon.Constants import DEBUG
 
    cfgFlags.addFlagsCategory("MLTree",__MLTree)

    import os
    file_list = os.getenv('FILELIST')
    #file_list = '/eos/user/e/edreyer/MLTreeAthenaAnalysis/samples/train.txt'

    with open(file_list, "r") as f:
        flist = [line.strip() for line in f if not line.startswith("#")]

    cfgFlags.Exec.MaxEvents=-1
    #cfgFlags.Exec.OutputLevel=DEBUG
    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files=flist
    cfgFlags.Concurrency.NumThreads=1
    cfgFlags.fillFromArgs()
    cfgFlags.lock()


    from MLTree.MainCfg import MainCfg
    cfg = MainCfg(cfgFlags)

    from MLTree.MLTreeMakerCfg import MLTreeMakerCfg
    cfg.merge(MLTreeMakerCfg(cfgFlags,
                          TrackContainer = "InDetTrackParticles",
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
                           OnlyStableTruthParticles = True,
                           G4TruthParticles = False,
                           Jets = True,
                           Pflow = True,
                           JetContainers = ["AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4TruthJets","AntiKt4EMPFlowJets"],
                           RootStreamName = "OutputStream"))                         

    cfg.run()
