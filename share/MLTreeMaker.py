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

#svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.422008.ParticleGun_single_ele_Pt100.recon.ESD.e4459_s2726_r7143/ESD.06642056._000019.pool.root.1"]
svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.422015.ParticleGun_single_pion_Pt100.recon.ESD.e4459_s2726_r7143/ESD.06642133._000031.pool.root.1"]

## MC15a pi+ test
#svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e3501_s2141_s2132_r6569/ESD.05281497._000506.pool.root.1",
#                                         "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e3501_s2141_s2132_r6569/ESD.05281497._001502.pool.root.1",
#                                         "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e3501_s2141_s2132_r6569/ESD.05281497._002564.pool.root.1",
#                                         "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.recon.ESD.e3501_s2141_s2132_r6569/ESD.05281497._003262.pool.root.1"]

## MC15a pi0 test
# svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.recon.ESD.e3496_s2139_s2132_r6569/ESD.05281500._000231.pool.root.1",
#                                          "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.recon.ESD.e3496_s2139_s2132_r6569/ESD.05281500._001571.pool.root.1",
#                                          "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.recon.ESD.e3496_s2139_s2132_r6569/ESD.05281500._002530.pool.root.1",
#                                          "/afs/cern.ch/user/j/jolsson/work/datasets/mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.recon.ESD.e3496_s2139_s2132_r6569/ESD.05281500._003596.pool.root.1"]

from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.DetDescrVersion="ATLAS-R2-2015-02-01-00" # For MC15a single pion logE0p2to2000 samples
jobproperties.Global.DetDescrVersion="ATLAS-R2-2015-03-01-00" # For MC15a single pion/electron Pt100 samples

# from AthenaCommon.GlobalFlags import globalflags
# globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R2-2015-02-01-00")

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
                             ClusterEmin = 50.0,
                             ClusterEmax = 500.0,
                             EventCleaning = False,
                             Tracking = False,
                             Pileup = False,
                             EventTree = False,
                             ClusterTree = True,
                             OutputLevel = DEBUG)
algSeq.MLTreeMaker.RootStreamName = "OutputStream"

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
from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
oks.addStreamESD("CaloCellContainer", ["AllCalo"] )
