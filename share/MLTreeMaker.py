#
# @file     MLTreeMaker.h
# @author   Joakim Olsson <joakim.olsson@cern.ch>
# @brief    Athena package to save cell images of clusters for ML training
# @date     October 2016
#

theApp.EvtMax = -1
# theApp.EvtMax = 100
import AthenaPoolCnvSvc.ReadAthenaPool

## MC15c pi+ test
# svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e3501_s2832_r8014/ESD.08446309._000227.pool.root.1"]
## MC15a pi+ test
# svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e3501_s2141_s2132_r6569/ESD.05281497._001502.pool.root.1"]
## MC15a pi0 test
svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.recon.ESD.e3496_s2139_s2132_r6569/ESD.05281500._001571.pool.root.1",
                                         "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.recon.ESD.e3496_s2139_s2132_r6569/ESD.05281500._003596.pool.root.1"]

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-R2-2015-02-01-00' # For MC15a single pion samples

# from AthenaCommon.GlobalFlags import globalflags
# globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-R2-2015-02-01-00')

# Suggestion from Peter Loch to turn off local cluster calibration
from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib.set_Value_and_Lock(False)
from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter

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
