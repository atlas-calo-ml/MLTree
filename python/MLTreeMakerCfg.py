# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def MLTreeMakerCfg(flags, name = "MLTreeMaker", **kwargs):

    result = ComponentAccumulator()

    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
    pcExtensionTool = result.popToolsAndMerge(ParticleCaloExtensionToolCfg(flags))

    from InDetConfig.InDetTrackSelectionToolConfig import PFTrackSelectionToolCfg
    MLTreeMaker = CompFactory.MLTreeMaker(**kwargs)
    MLTreeMaker.TheTrackExtrapolatorTool = pcExtensionTool
    MLTreeMaker.TrackSelectionTool=result.popToolsAndMerge(PFTrackSelectionToolCfg(flags))

    for c in flags.Input.Collections:
        print ("c is ", c)

    #Need to specify sequence name, otherwise the tool will not be added to the correct sequence and some 
    #containers such as EventInfo will not be available
    result.addEventAlgo(MLTreeMaker,sequenceName="AthAlgSeq")

    return result