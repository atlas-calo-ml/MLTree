################################################################################
# Package: MLTree
################################################################################

# Converted from cmt/requirements using documentation here for building a "component library"
# https://twiki.cern.ch/twiki/bin/view/AtlasComputing/SoftwareTutorialAdvancedCMake

# Declare the package name:
atlas_subdir( MLTree )

atlas_depends_on_subdirs(	PRIVATE
				Control/AthenaBaseComps
				Calorimeter/CaloIdentifier
				Trigger/TrigEvent/TrigCaloEvent
				Reconstruction/RecoTools/RecoToolInterfaces
				Event/xAOD/xAODCaloEvent
				Event/xAOD/xAODEventInfo
				Event/xAOD/xAODTruth
				Event/xAOD/xAODEventShape
				Event/xAOD/xAODJet
				Calorimeter/CaloEvent
				Calorimeter/CaloUtils
				TileCalorimeter/TileIdentifier
				TileCalorimeter/TileEvent
				Tracking/TrkExtrapolation/TrkExInterfaces
				Tracking/TrkEvent/TrkParameters
				Tracking/TrkEvent/TrkTrack
				Event/xAOD/xAODTracking
				Calorimeter/CaloTrackingGeometry
				Tracking/TrkDetDescr/TrkSurfaces
				DetectorDescription/GeoPrimitives
				Tracking/TrkEvent/TrkCaloExtension
				Calorimeter/CaloDetDescr
				Tracking/TrkEvent/TrkParametersIdentificationHelpers)

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
			CaloEvent
			CaloUtils
			TileIdentifier
			TileEvent
			TrkExInterfaces
			TrkParameters
			TrkTrack
			xAODTracking
			CaloTrackingGeometry
			TrkSurfaces
			GeoPrimitives
			TrkCaloExtension
			CaloDetDescr
			TrkParametersIdentificationHelpers)



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