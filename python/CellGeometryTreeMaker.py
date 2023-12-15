# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CellGeometryTreeMakerCfg(flags,name="CellGeometryTreeMaker",**kwargs):

    result = ComponentAccumulator()

    #Need both the geometry and the noise for the CellGeometryTreeMaker
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(flags))
 
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))
    result.merge(CaloNoiseCondAlgCfg(flags,"electronicNoise"))

    CellGeometryTreeMaker = CompFactory.CellGeometryTreeMaker(**kwargs)
    CellGeometryTreeMaker.TwoGaussianNoise = flags.Calo.TopoCluster.doTwoGaussianNoise

    #Need to specify sequence name, otherwise the tool will not be added to the correct sequence and some 
    #containers such as CaloCellContainer will not be available
    result.addEventAlgo(CellGeometryTreeMaker,sequenceName="AthAlgSeq")

    return result