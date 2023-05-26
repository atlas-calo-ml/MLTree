#! /usr/bin/env python

# @file     MLTreeMaker.h
# @author   Joakim Olsson <joakim.olsson@cern.ch>
# @brief    Athena package to save cell images of clusters for ML training
# @date     October 2016

# Number of events
theApp.EvtMax = 10 # all events in dataset
#theApp.EvtMax = 2000 # testing
import AthenaPoolCnvSvc.ReadAthenaPool

# Input files for testing

# MC16 pi- (testing dataset)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput=["/eos/user/a/angerami/mc16_13TeV.426328.ParticleGun_single_piplus_logE5to2000.recon.ESD.e5661_s3170_r9857/ESD.11980046._000944.pool.root.1"]
#athenaCommonFlags.FilesInput=["/data/hodgkinson/dataFiles/mc16_13TeV/ESDFiles/mc16_13TeV.426328.ParticleGun_single_piplus_logE5to2000.recon.ESD.e5661_s3170_r9857/ESD.11980046._001600.pool.root.1"]
athenaCommonFlags.FilesInput=["/data/hodgkinson/dataFiles/mc20_13TeV/ESDFiles/mc20_13TeV.426327.ParticleGun_single_piminus_logE5to2000.recon.ESD.e5661_s3781_r13300/ESD.27658295._000172.pool.root.1"]
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

#LC will overwrite cell weights.
#Reconstruction will make a "snapshot" container (CaloTopoClusters) with original cell weights.
#Individual clusters cross linked through sisterCluster method
#Cross linking is not presistified in ESD, so clustering must be re-run

#setup calorimeter conditions
include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
from LArConditionsCommon import LArAlignable

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
    include("MLTree/jetConfig.py")

#add MLTreeMaker directly to top sequence to ensure its run *after* topoclustering
from MLTree.MLTreeConf import MLTreeMaker

#Setup track extrapolation tool
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = AtlasExtrapolator())
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += pcExtensionTool

topSequence += MLTreeMaker(name = "MLTreeMaker",
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
                           OnlyStableTruthParticles = False,
                           G4TruthParticles = False,
                           Jets = True,
                           JetContainers = ["AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4TruthJets"],
                           OutputLevel = INFO,
                           TheTrackExtrapolatorTool=pcExtensionTool)
topSequence.MLTreeMaker.TrackSelectionTool.CutLevel = "TightPrimary"
topSequence.MLTreeMaker.RootStreamName = "OutputStream"

if topSequence.MLTreeMaker.ClusterCalibHits:
    topSequence.MLTreeMaker.CalibrationHitContainerNames = CalibrationHitContainerNames

#Disable thinning of nonexistent shallow copy pflow containers
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)

#if cluster cells requested, write out calo cell geometry tree too
if topSequence.MLTreeMaker.ClusterCells:
    #Setup conditions algorithm to access noise
    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg(noisetype="electronicNoise")
    from MLTree.MLTreeConf import CellGeometryTreeMaker
    topSequence += CellGeometryTreeMaker(name = "CellGeometryTreeMaker")
    topSequence.CellGeometryTreeMaker.RootStreamName = "OutputStream"

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

# Configure object key store to recognize calo cells
