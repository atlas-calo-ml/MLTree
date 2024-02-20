################################################################################
# Package: MLTree
################################################################################

# Converted from cmt/requirements using documentation here for building a "component library"
# https://twiki.cern.ch/twiki/bin/view/AtlasComputing/SoftwareTutorialAdvancedCMake

# Declare the package name:
atlas_subdir( MLTree )

# Define long list of link libraries as a local variable
set(	_athLINKLIBS	AthenaBaseComps
			CaloIdentifier
			TrigCaloEvent
			RecoToolInterfaces
			xAODCaloEvent
			xAODEventInfo
			xAODTruth
			xAODEventShape
			xAODJet
			xAODMissingET
			xAODEgamma
			xAODMuon
      xAODPFlow     
			CaloEvent
      CaloConditions
			CaloSimEvent
			CaloUtilsLib
			TileIdentifier
			TileEvent
			TrkExInterfaces
			TrkParameters
			TrkTrack
			xAODTracking
			CaloTrackingGeometryLib
			TrkSurfaces
			GeoPrimitives
			TrkCaloExtension
			CaloDetDescrLib
			TrkParametersIdentificationHelpers
			InDetTrackSelectionToolLib)



# External dependencies:
# find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MLTree
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${_athLINKLIBS} )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Clean up
unset( _athLINKLIBS)
