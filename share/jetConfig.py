from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, ComponentAccumulator
from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4EMTopo, AntiKt4Truth
from JetRecConfig.StandardLargeRJets import AntiKt10LCTopo
from JetRecConfig.JetRecConfig import getJetDefAlgs

from JetRecConfig.StandardJetConstits import stdConstitDic

from JetCalibTools.JetCalibToolsConfig import pflowcontexts, topocontexts

def reOrderAlgs(algs):
    """In runIII the scheduler automatically orders algs, so the JetRecConfig helpers do not try to enforce the correct ordering.
    This is not the case in runII config for which this jobO is intended --> This function makes sure some jet-related algs are well ordered.
    """
    evtDensityAlgs = [ (i,alg) for (i,alg) in enumerate(algs) if alg.getType() == 'EventDensityAthAlg' ]
    pjAlgs = [ (i,alg) for (i,alg) in enumerate(algs) if alg.getType() == 'PseudoJetAlgorithm' ]
    pairsToswap = []
    for i,edalg in evtDensityAlgs:
        edInput = edalg.EventDensityTool.InputContainer
        for j,pjalg in pjAlgs:
            if j<i: continue 
            if edInput == str(pjalg.OutputContainer):
                pairsToswap.append( (i,j) )
    for (i,j) in pairsToswap:
        algs[i], algs[j] = algs[j], algs[i]
    return algs

##
# Temporary hack : JetConstituentModSequence for EMPFlow seems to be scheduled
# somewhere else in the standard reco chain with a different alg name as the alg created by JetRecConfig helpers.
# The trick below will make so the new helpers do NOT schedule a JetConstituentModSequence and thus avoid conflicts.
stdConstitDic.EMPFlow._locked = False
stdConstitDic.EMPFlow.inputname = stdConstitDic.EMPFlow.containername


# the Standard list of jets to run :
jetdefs = [AntiKt4Truth]

# we'll remember the EventDensity collections we create.
evtDensities = []

#--------------------------------------------------------------
# Create the jet algs from the jet definitions
#--------------------------------------------------------------

from AthenaConfiguration.OldFlags2NewFlags import getNewConfigFlags
ConfigFlags = getNewConfigFlags()

for jd in jetdefs:
    algs, jetdef_i = getJetDefAlgs(ConfigFlags, jd, True)
    algs = reOrderAlgs( [a for a in algs if a is not None])
    for a in algs:
        topSequence += conf2toConfigurable(a)
        if "EventDensityAthAlg" in a.getType():
            evtDensities.append( str(a.EventDensityTool.OutputContainer) )


