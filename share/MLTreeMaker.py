#! /usr/bin/env python

# @file     MLTreeMaker.h
# @author   Joakim Olsson <joakim.olsson@cern.ch>
# @brief    Athena package to save cell images of clusters for ML training
# @date     October 2016

# Number of events
theApp.EvtMax = -1 # all events in dataset
#theApp.EvtMax = 2000 # testing
import AthenaPoolCnvSvc.ReadAthenaPool

# Input files for testing

## MC15a e vs p+- at 100 GeV (50k events for each dataset)
#svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.422008.ParticleGun_single_ele_Pt100.recon.ESD.e4459_s2726_r7143/ESD.06642056._000019.pool.root.1"]
#svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.422015.ParticleGun_single_pion_Pt100.recon.ESD.e4459_s2726_r7143/ESD.06642133._000031.pool.root.1"]

# MC16 pi- (testing dataset)
#svcMgr.EventSelector.InputCollections = ["/eos/user/m/mswiatlo/esd/mc16_13TeV.428002.ParticleGun_single_piminus_logE0p2to2000.recon.ESD.e7279_s3411_r11281/ESD.17269624._000146.pool.root.1"]
#svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/work/a/angerami/private/JetML/mc16_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.recon.ESD.e7279_s3411_r11281/ESD.17269610._001596.pool.root.1"]
#svcMgr.EventSelector.InputCollections = ["/eos/user/a/angerami/mc16_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.recon.ESD.e3569_s3170_r10788_tid15388779_00/ESD.15388779._001630.pool.root.1"]
svcMgr.EventSelector.InputCollections = ["/eos/user/a/angerami/mc16_13TeV.426328.ParticleGun_single_piplus_logE5to2000.recon.ESD.e5661_s3170_r9857/ESD.11980046._000944.pool.root.1"]
#svcMgr.EventSelector.InputCollections = ["/eos/user/a/angerami/mc16_13TeV/ESD.15388997._000028.pool.root.1"]
#svcMgr.EventSelector.InputCollections = ["/eos/user/m/mswiatlo/esd/mc16_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e7279_s3411_r11281/ESD.17269616._000058.pool.root.1"]
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-R2-2016-01-00-01" # For MC16



from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
oks.addStreamESD("CaloCellContainer", ["AllCalo"] )

#Make sure input file has the containers we need
#CaloCalibrationHitContainer
DMCalibrationHitContainerNames = []
CalibrationHitContainerNames = []

#Digi Truth
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doDigiTruth=False

#truth jets
rerunTruthJets=True

from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
containers=af.fileinfos['eventdata_items']
for c in containers:
    if c[0]=='CaloCalibrationHitContainer' : 
        if 'Dead' in c[1] : DMCalibrationHitContainerNames += [c[1]]
        else: CalibrationHitContainerNames += [c[1]]
    if c[1]=='AllCalo_DigiHSTruth' : digitizationFlags.doDigiTruth=True
    if c[1]=='AntiKt4TruthJets' : rerunTruthJets=False




#Re-run topoclusters
#LC will overwrite cell weights.
#Reconstruction will make a "snapshot" container (CaloTopoClusters) with original cell weights.
#Individual clusters cross linked through sisterCluster method
#Cross linking is not presistified in ESD, so clustering must be re-run


#calib hit moments
from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doCalibHitMoments=True
from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter
CaloClusterTopoGetter()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
TopoCalibMoments=topSequence.CaloTopoCluster.TopoCalibMoments
TopoCalibMoments.MomentsNames += ["ENG_CALIB_TOT","ENG_CALIB_OUT_T","ENG_CALIB_DEAD_TOT"]
#note that TopoCalibMoments tool will FAIL SILENTLY if CaloCalibrationHitContainer names it's set up to use are not found in ESD
#read container names directly from ESD to prevent name mis-matches resulting in this failure mode
TopoCalibMoments.DMCalibrationHitContainerNames = DMCalibrationHitContainerNames
TopoCalibMoments.CalibrationHitContainerNames = CalibrationHitContainerNames
print(TopoCalibMoments)

if rerunTruthJets:
    include( "McParticleAlgs/TruthParticleBuilder_jobOptions.py" )
    from JetRec.JetRecFlags import jetFlags
    #this line should not be needed, but current jet algorithm implementation runs all of the pseudojet builders
    #even if they are not needed by the jet finders (truth only)
    #building origin-corrected topoclusters tries to modify existing const container
    jetFlags.useTracks=False
    from JetRec.JetRecStandardToolManager import jtm
    jtm.addJetFinder("AntiKt4TruthJets",    "AntiKt", 0.4,    "truth", ptmin= 5000)
    from JetRec.JetAlgorithm import addJetRecoToAlgSequence
    addJetRecoToAlgSequence()

#add MLTreeMaker directly to top sequence to ensure its run *after* topoclustering
from MLTree.MLTreeConf import MLTreeMaker
topSequence += MLTreeMaker(name = "MLTreeMaker",
                           TrackContainer = "InDetTrackParticles",
                           CaloClusterContainer = "CaloCalTopoClusters",
                           Prefix = "CALO",
                           ClusterEmin = 0.0,
                           ClusterEmax = 2000.0,
                           ClusterEtaAbsmax = 0.7,
                           EventCleaning = False,
                           Tracking = True,
                           Pileup = True,
                           EventTree = True,
                           ClusterTree = True,
                           ClusterMoments = True,
                           UncalibratedClusters = True,
                           TruthParticles = True,
                           EventTruth = False,
                           OnlyStableTruthParticles = True,
                           Jets = True,
                           JetContainers = ["AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4TruthJets"],
                           OutputLevel = INFO)
topSequence.MLTreeMaker.TrackSelectionTool.CutLevel = "TightPrimary"
topSequence.MLTreeMaker.RootStreamName = "OutputStream"


# Setup stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, "DecisionSvc"):
    svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["OutputStream DATAFILE='mltree.pool.root' OPT='RECREATE'"]

# Setup up geometry needed for track extrapolation
include("RecExCond/AllDet_detDescr.py")
from AthenaCommon.CfgGetter import getService
getService("AtlasTrackingGeometrySvc")

# Configure object key store to recognize calo cells
