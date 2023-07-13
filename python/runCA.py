#You can run this code from the command line (after Athena setup) with a command:
#python runCA.py --filesInput=<inputFileName> MLTree.NtupleName=<outputNtupleFileName>"

if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags
    from MLTree.MainCfg import __MLTree
    #from AthenaCommon.Constants import DEBUG
 
    cfgFlags.addFlagsCategory("MLTree",__MLTree)

    flist = [
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000012.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000018.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000019.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000022.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000035.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000045.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000055.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000056.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000077.pool.root.1",
          "/storage/agrp/dreyet/MLTreeAthenaAnalysis/samples/mc20_13TeV.364702.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2WithSW.recon.ESD.e7142_e5984_s4027_r14266/test/ESD.31872166._000079.pool.root.1",
    ]

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
                           JetContainers = ["AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4TruthJets"],
                           RootStreamName = "OutputStream"))                         

    cfg.run()
