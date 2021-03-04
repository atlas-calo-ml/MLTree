#! /usr/bin/env python

# Number of events
theApp.EvtMax = 1 # just read the calo geometry from the first event
import AthenaPoolCnvSvc.ReadAthenaPool


svcMgr.EventSelector.InputCollections = ["/eos/user/a/angerami/mc16_13TeV.900147.PG_singleDelta_logE5to2000_flatM1p2to5.recon.ESD.e8312_e7400_s3170_r12383/ESD.24193523._000160.pool.root.1"]
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-R2-2016-01-00-01" # For MC16



from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
oks.addStreamESD("CaloCellContainer", ["AllCalo"] )

#add the calo noise tool
from AthenaCommon.AppMgr import ToolSvc
if not hasattr(ToolSvc, "CaloNoiseToolDefault"):
    from CaloTools.CaloNoiseFlags import jobproperties
    jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(13.793)#nominal high mu run 2 settings
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theCaloNoiseTool = CaloNoiseToolDefault()
    ToolSvc += theCaloNoiseTool
    ToolSvc.CaloNoiseToolDefault.OutputLevel=VERBOSE

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from MLTree.MLTreeConf import CellGeometryTreeMaker
topSequence += CellGeometryTreeMaker(name = "CellGeometryTreeMaker", DoNeighbours=True)
topSequence.CellGeometryTreeMaker.RootStreamName = "OutputStream"


# Setup stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, "DecisionSvc"):
    svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["OutputStream DATAFILE='cell_geo.root' OPT='RECREATE'"]

# Setup up geometry needed for track extrapolation
include("RecExCond/AllDet_detDescr.py")
#from AthenaCommon.CfgGetter import getService
#getService("AtlasTrackingGeometrySvc")
