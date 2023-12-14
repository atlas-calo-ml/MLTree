# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CellGeometryTreeMakerCfg(flags,name="CellGeometryTreeMaker",**kwargs):

    from MainCfg import GeneralServicesCfg
    result = GeneralServicesCfg(flags)
 
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(flags))
 
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))
    result.merge(CaloNoiseCondAlgCfg(flags,"electronicNoise"))

    CellGeometryTreeMaker = CompFactory.CellGeometryTreeMaker(**kwargs)
    CellGeometryTreeMaker.TwoGaussianNoise = flags.Calo.TopoCluster.doTwoGaussianNoise

    result.addEventAlgo(CellGeometryTreeMaker,sequenceName="AthAlgSeq")

    return result