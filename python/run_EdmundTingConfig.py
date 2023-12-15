# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

#You can run this code from the command line (after Athena setup) with a command:
#python runCA.py --filesInput=<inputFileName> MLTree.NtupleName=<outputNtupleFileName>"

if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags
    from MLTree.MainCfg import __MLTree
    cfgFlags.addFlagsCategory("MLTree",__MLTree)

    cfgFlags.Exec.MaxEvents=10
    cfgFlags.Input.isMC=True
    #cfgFlags.Input.Files=["/home/markhodgkinson.linux/ESD.28115683._000440.pool.root.1"]    
    #cfgFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]
    #cfgFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.recon.ESD.e6337_e5984_s3170_r12674/ESD.25732025._000034.pool.root.1"]
    #cfgFlags.Input.Files=["/data/hodgkinson/dataFiles/mc20_13TeV/ESDFiles/mc20_13TeV.426329.ParticleGun_single_piminus_E2to5.recon.ESD.e5661_s3170_r13300/ESD.28115719._000001.pool.root.1"]
    #cfgFlags.Input.Files=["/data/hodgkinson/dataFiles/mc20_13TeV/ESDFiles/mc20_13TeV.426327.ParticleGun_single_piminus_logE5to2000.recon.ESD.e5661_s3170_r13300/ESD.28115683._000460.pool.root.1"]
    cfgFlags.Input.Files=["/data/hodgkinson/dataFiles/mc21_13p6TeV/ESDFiles/mc21_13p6TeV.801166.Py8EG_A14NNPDF23LO_jj_JZ1.recon.ESD.e8453_s3986_r14060/ESD.31373526._003793.pool.root.1"]
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
                           TrackTruthMatching = True,
                           DetailedTracking = False,
                           Pileup = False,                        
                           Clusters = True,
                           ClusterCells = True,
                           ClusterCalibHits = True,
                           ClusterCalibHitsPerCell = False,
                           ClusterMoments = False,
                           UncalibratedClusters = True,
                           TruthParticles = False,
                           EventTruth = False,
                           OnlyStableTruthParticles = False,
                           G4TruthParticles = False,
                           Jets = True,
                           JetContainers = ["AntiKt4EMPFlowJets"],
                           RootStreamName = "OutputStream"))                         

    cfg.run()
