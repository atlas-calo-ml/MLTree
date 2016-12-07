#
# @file     MLTreeMaker.h
# @author   Joakim Olsson <joakim.olsson@cern.ch>
# @brief    Athena package to save a tree that includes clusters, cells, tracks and truth information for projects using ML and Computer Vision
# @date     October 2016
#

theApp.EvtMax = -1
import AthenaPoolCnvSvc.ReadAthenaPool

# MC15c pi+ test
svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e3501_s2832_r8014/ESD.08446309._000227.pool.root.1" ]
# MC15a pi+ test
svcMgr.EventSelector.InputCollections = [ "" ]
# MC15a pi0 test
svcMgr.EventSelector.InputCollections = [ "" ]

# Setup MLTreeMaker algorithm
from AthenaCommon import CfgMgr
algSeq = CfgMgr.AthSequencer("AthAlgSeq")
algSeq += CfgMgr.MLTreeMaker(name = "MLTreeMaker",
                             TrackContainer = "InDetTrackParticles",
                             CaloClusterContainer = "CaloCalTopoClusters",
                             Prefix = "CALO",
                             EventCleaning = True,
                             Pileup = True,
                             OutputLevel = DEBUG)
algSeq.MLTreeMaker.RootStreamName = "OutputStream"

# Setup stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'DecisionSvc'):
    svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["OutputStream DATAFILE='MLderivation.pool.root' OPT='RECREATE'"]

# Setup up geometry needed for track extrapolation
include("RecExCond/AllDet_detDescr.py")
from AthenaCommon.CfgGetter import getService
getService("AtlasTrackingGeometrySvc")

# Configure object key store to recognize calo cells
from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
oks.addStreamESD('CaloCellContainer', ['AllCalo'] )
