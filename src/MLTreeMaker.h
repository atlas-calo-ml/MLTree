/**
 * @file    MLTreeMaker.h
 * @author  Joakim Olsson <joakim.olsson@cern.ch>
 * @brief   Athena package to save cell images of clusters for ML training 
 * @date    October 2016
 */

#ifndef MLTREE_MLTREEMAKER_H
#define MLTREE_MLTREEMAKER_H

#include <string>

#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventShape/EventShape.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

class TileTBID;
class ICaloSurfaceHelper;

namespace Trk
{
  class IExtrapolator;
  class Surface;
  class TrackParametersIdHelper;
}

namespace InDet
{
  class IInDetTrackSelectionTool;
}

class MLTreeMaker : public ::AthHistogramAlgorithm
{

public:
  MLTreeMaker(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~MLTreeMaker();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  Gaudi::Property<bool> m_doClusters{this, "Clusters", true, "Write clusters to tree"};
  Gaudi::Property<bool> m_doClusterCells{this, "ClusterCells", true, "Write cluster cells to tree"};
  Gaudi::Property<bool> m_doCalibHits{this, "ClusterCalibHits", true, "Write calibration hits data, for clusters and cells in clusters, to tree"};
  Gaudi::Property<bool> m_doCalibHitsPerCell{this, "ClusterCalibHitsPerCell", true, "Write calibration hits data, for cells in clusters, to tree"};
  Gaudi::Property<bool> m_doClusterMoments{this, "ClusterMoments", true, "Write cluster moments to tree"};
  Gaudi::Property<bool> m_doUncalibratedClusters{this, "UncalibratedClusters", true, "Use uncalibrated EM scale clusters to write cluster data to tree"};

  Gaudi::Property<bool> m_doTracking{this, "Tracking", false, "Write tracks to tree"};
  Gaudi::Property<bool> m_doJets{this, "Jets", false, "Write jets to tree"};
  Gaudi::Property<bool> m_doEventCleaning{this, "EventCleaning", false, "Write event cleaning data to tree"};
  Gaudi::Property<bool> m_doPileup{this, "Pileup", false, "Write pileup data to tree"};
  Gaudi::Property<bool> m_doShapeEM{this, "ShapeEM", false, "Write EM shape data to tree"};
  Gaudi::Property<bool> m_doShapeLC{this, "ShapeLC", false, "Write LC shape data to tree"};
  Gaudi::Property<bool> m_doEventTruth{this, "EventTruth", false, "Write event truth data to tree"};
  Gaudi::Property<bool> m_doTruthParticles{this, "TruthParticles", false, "Write truth particle related data to tree"};

  Gaudi::Property<bool> m_keepOnlyStableTruthParticles{this, "OnlyStableTruthParticles", true, "Write out truth particle data only for stable truth particles"};
  Gaudi::Property<bool> m_keepG4TruthParticles{this, "G4TruthParticles", false, "Write out truth particle data for G4 truth particles"};

  /** ReadHandle to retrieve xAOD::FlowElementContainer (charged) */
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_chargedFlowElementReadHandleKey{this, "ChargedFlowElementContainer", "JetETMissChargedParticleFlowObjects", "ReadHandleKey for the charged FlowElement container"};

  /** ReadHandleKey to retrieve xAOD::TruthParticleContainer */
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleReadHandleKey{this, "TruthParticleContainer", "TruthParticles", "ReadHandleKey for the truth particle container"};

  /** ReadHandleKey for Primary Vertices */
  SG::ReadHandleKey<xAOD::VertexContainer> m_vxReadHandleKey{this, "VxContainer", "PrimaryVertices", "ReadHandleKey for Primary Vertices"};

  /** ReadHandleKey for ID tracks */
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleReadHandleKey{this, "TrackContainer", "InDetTrackParticles", "ReadHandleKey for ID Tracks"};

  /** ReadHandleKey for CaloCluster */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClusterReadHandleKey{this, "CaloClusterContainer", "CaloCalTopoClusters", "ReadHandleKey for CaloClusters"};

  /** ReadDecorHandleKey for the CaloCluster calibration hit decorations */
  SG::ReadDecorHandleKey<xAOD::CaloClusterContainer> m_caloClusterCalibHitsDecorHandleKey{this, "CaloClusterCalibHitsDecor", "CaloCalTopoClusters.calclus_NLeadingTruthParticleBarcodeEnergyPairs", "ReadDecorHandleKey for the CaloCluster calibration hit decorations"};

  /** ReadhandleKey for the CaloCellContainer */
  SG::ReadHandleKey<CaloCellContainer> m_caloCellReadHandleKey{this, "CaloCellContainer", "AllCalo", "ReadHandleKey for CaloCells"};

  /** ReadHandleKey for EventInfo */
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoReadHandleKey{this, "EventContainer", "EventInfo", "ReadHandleKey for EventInfo"};

  /** ReadHandleKeys for EventShapes */
  SG::ReadHandleKey<xAOD::EventShape> m_lcTopoEventShapeReadHandleKey{this, "LCTopoEventShape", "", "ReadHandleKey for LCTopoEventShape"};
  SG::ReadHandleKey<xAOD::EventShape> m_emTopoEventShapeReadHandleKey{this, "EMTopoEventShape", "", "ReadHandleKey for EMTopoEventShape"};

  /** ReadHandleKey for TruthEvents */
  SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventReadHandleKey{this, "TruthEventContainer", "TruthEvents", "ReadHandleKey for TruthEvents"};

  /** ReadHandleKeyArray for JetContainers */
  SG::ReadHandleKeyArray<xAOD::JetContainer> m_jetReadHandleKeyArray{this, "JetContainers", {}, "ReadHandleKeyArray for JetContainers"};

  /** ReadHandleKeyArray for CalibrationHitContainers */
  SG::ReadHandleKeyArray<CaloCalibrationHitContainer> m_CalibrationHitContainerKeys{this, "CalibrationHitContainerNames", 
                                                                                          {"LArCalibrationHitActive",
                                                                                           "LArCalibrationHitInactive",
                                                                                           "TileCalibHitActiveCell",
                                                                                           "TileCalibHitInactiveCell"},
                                                                                           "ReadHandleKeyArray for CalibrationHitContainers"};

  std::unique_ptr<Trk::TrackParametersIdHelper> m_trackParametersIdHelper;
  ToolHandle<Trk::IParticleCaloExtensionTool> m_theTrackExtrapolatorTool{this, "TheTrackExtrapolatorTool", "Trk::ParticleCaloExtensionTool", "Tool to extrapolate tracks to the calorimeter"};
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool{this, "TrackSelectionTool", "InDet::InDetTrackSelectionTool/TrackSelectionTool", "Tool to select tracks"};

  // Cluster and cell selections (energies are in MeV)
  Gaudi::Property<float> m_clusterE_min{this, "ClusterEmin", 0.0, "Minimum cluster energy to write to TTree"};
  Gaudi::Property<float> m_clusterE_max{this, "ClusterEmax", 1e4, "Maximum cluster energy to write to TTree"};
  Gaudi::Property<float> m_clusterEtaAbs_max{this, "ClusterEtaAbsmax", 2.5, "Maximum cluster |eta| to write to TTree"};
  Gaudi::Property<float> m_cellE_thres{this, "CellEthres", 0.005, "Minimum clustered cell energy to write to TTree"};

  //Tree and branch data structures
  TTree *m_eventTree;

  // Event info
  int m_runNumber;
  Long64_t m_eventNumber;
  int m_lumiBlock;
  uint32_t m_coreFlags;
  uint32_t m_timeStamp;
  uint32_t m_timeStampNSOffset;
  bool m_TileError;
  bool m_LArError;
  bool m_SCTError;
  uint32_t m_TileFlags;
  uint32_t m_LArFlags;
  uint32_t m_SCTFlags;
  int m_mcEventNumber;
  int m_mcChannelNumber;
  float m_mcEventWeight;
  float m_weight_pileup;
  float m_correct_mu;
  int m_rand_run_nr;
  int m_rand_lumiblock_nr;
  int m_bcid;
  float m_prescale_DataWeight;
  // pileup
  int m_npv;
  float m_actualMu;
  float m_averageMu;
  // shapeEM
  double m_rhoEM;
  double m_rhoLC;
  // truth
  int m_pdgId1;
  int m_pdgId2;
  int m_pdfId1;
  int m_pdfId2;
  float m_x1;
  float m_x2;  
  float m_xf1;
  float m_xf2;

  // Truth particles
  int m_nTruthPart;
  int m_G4PreCalo_n_EM;
  float m_G4PreCalo_E_EM;
  int m_G4PreCalo_n_Had;
  float m_G4PreCalo_E_Had;
  float m_truthVertexX;
  float m_truthVertexY;
  float m_truthVertexZ;
  std::vector<int> m_truthPartPdgId;
  std::vector<int> m_truthPartStatus;
  std::vector<int> m_truthPartBarcode;
  std::vector<float> m_truthPartPt;
  std::vector<float> m_truthPartE;
  std::vector<float> m_truthPartMass;
  std::vector<float> m_truthPartEta;
  std::vector<float> m_truthPartPhi;

  // Track variables
  int m_nTrack;
  std::vector<float> m_trackPt;
  std::vector<float> m_trackP;
  std::vector<float> m_trackMass;
  std::vector<float> m_trackEta;
  std::vector<float> m_trackPhi;
  //index of matched truth particle in m_truthPart* vectors
  std::vector<int> m_trackTruthParticleIndex;
  //sum of visible calibration hit energy of this tracks
  //truth particle found in all topoclusters
  std::vector<float> m_trackVisibleCalHitCaloEnergy;
  //same for full calibration hit energy
  std::vector<float> m_trackFullCalHitCaloEnergy;
  //If this track was used in particle flow, then this is the
  //amount of energy removed from matched CaloCluster
  std::vector<float> m_trackSubtractedCaloEnergy;

  // Track quality variables
  uint8_t m_numberOfPixelHits;
  uint8_t m_numberOfSCTHits;
  uint8_t m_numberOfPixelDeadSensors;
  uint8_t m_numberOfSCTDeadSensors;
  uint8_t m_numberOfPixelSharedHits;
  uint8_t m_numberOfSCTSharedHits;
  uint8_t m_numberOfPixelHoles;
  uint8_t m_numberOfSCTHoles;
  uint8_t m_numberOfInnermostPixelLayerHits;
  uint8_t m_numberOfNextToInnermostPixelLayerHits;
  uint8_t m_expectInnermostPixelLayerHit;
  uint8_t m_expectNextToInnermostPixelLayerHit;
  uint8_t m_numberOfTRTHits;
  uint8_t m_numberOfTRTOutliers;

  std::vector<int> m_trackNumberOfPixelHits;
  std::vector<int> m_trackNumberOfSCTHits;
  std::vector<int> m_trackNumberOfPixelDeadSensors;
  std::vector<int> m_trackNumberOfSCTDeadSensors;
  std::vector<int> m_trackNumberOfPixelSharedHits;
  std::vector<int> m_trackNumberOfSCTSharedHits;
  std::vector<int> m_trackNumberOfPixelHoles;
  std::vector<int> m_trackNumberOfSCTHoles;
  std::vector<int> m_trackNumberOfInnermostPixelLayerHits;
  std::vector<int> m_trackNumberOfNextToInnermostPixelLayerHits;
  std::vector<int> m_trackExpectInnermostPixelLayerHit;
  std::vector<int> m_trackExpectNextToInnermostPixelLayerHit;
  std::vector<int> m_trackNumberOfTRTHits;
  std::vector<int> m_trackNumberOfTRTOutliers;
  std::vector<float> m_trackChiSquared;
  std::vector<int> m_trackNumberDOF;
  std::vector<float> m_trackD0;
  std::vector<float> m_trackZ0;

  // Track extrapolation
  // Presampler
  std::vector<float> m_trackEta_PreSamplerB;
  std::vector<float> m_trackPhi_PreSamplerB;
  std::vector<float> m_trackEta_PreSamplerE;
  std::vector<float> m_trackPhi_PreSamplerE;
  // LAr EM Barrel layers
  std::vector<float> m_trackEta_EMB1;
  std::vector<float> m_trackPhi_EMB1;
  std::vector<float> m_trackEta_EMB2;
  std::vector<float> m_trackPhi_EMB2;
  std::vector<float> m_trackEta_EMB3;
  std::vector<float> m_trackPhi_EMB3;
  // LAr EM Endcap layers
  std::vector<float> m_trackEta_EME1;
  std::vector<float> m_trackPhi_EME1;
  std::vector<float> m_trackEta_EME2;
  std::vector<float> m_trackPhi_EME2;
  std::vector<float> m_trackEta_EME3;
  std::vector<float> m_trackPhi_EME3;
  // Hadronic Endcap layers
  std::vector<float> m_trackEta_HEC0;
  std::vector<float> m_trackPhi_HEC0;
  std::vector<float> m_trackEta_HEC1;
  std::vector<float> m_trackPhi_HEC1;
  std::vector<float> m_trackEta_HEC2;
  std::vector<float> m_trackPhi_HEC2;
  std::vector<float> m_trackEta_HEC3;
  std::vector<float> m_trackPhi_HEC3;
  // Tile Barrel layers
  std::vector<float> m_trackEta_TileBar0;
  std::vector<float> m_trackPhi_TileBar0;
  std::vector<float> m_trackEta_TileBar1;
  std::vector<float> m_trackPhi_TileBar1;
  std::vector<float> m_trackEta_TileBar2;
  std::vector<float> m_trackPhi_TileBar2;
  // Tile Gap layers
  std::vector<float> m_trackEta_TileGap1;
  std::vector<float> m_trackPhi_TileGap1;
  std::vector<float> m_trackEta_TileGap2;
  std::vector<float> m_trackPhi_TileGap2;
  std::vector<float> m_trackEta_TileGap3;
  std::vector<float> m_trackPhi_TileGap3;
  // Tile Extended Barrel layers
  std::vector<float> m_trackEta_TileExt0;
  std::vector<float> m_trackPhi_TileExt0;
  std::vector<float> m_trackEta_TileExt1;
  std::vector<float> m_trackPhi_TileExt1;
  std::vector<float> m_trackEta_TileExt2;
  std::vector<float> m_trackPhi_TileExt2;

  //outer index is for jet container
  //inner index is for jets index w/in that container
  std::vector<std::vector<float>> m_jet_pt;
  std::vector<std::vector<float>> m_jet_eta;
  std::vector<std::vector<float>> m_jet_phi;
  std::vector<std::vector<float>> m_jet_E;
  std::vector<std::vector<int>> m_jet_flavor;

  // Clusters and cells
  int m_nCluster;
  std::vector<int> m_cluster_nCells;
  std::vector<float> m_cluster_E;
  std::vector<float> m_cluster_E_LCCalib;
  std::vector<float> m_cluster_Pt;
  std::vector<float> m_cluster_Eta;
  std::vector<float> m_cluster_Phi;

  std::vector<float> m_cluster_ENG_CALIB_TOT;
  std::vector<float> m_cluster_ENG_CALIB_OUT_T;
  std::vector<float> m_cluster_ENG_CALIB_DEAD_TOT;
  std::vector<float> m_cluster_EM_PROBABILITY;
  std::vector<float> m_cluster_HAD_WEIGHT;
  std::vector<float> m_cluster_OOC_WEIGHT;
  std::vector<float> m_cluster_DM_WEIGHT;
  std::vector<float> m_cluster_CENTER_MAG;
  std::vector<float> m_cluster_FIRST_ENG_DENS;
  std::vector<float> m_cluster_CENTER_LAMBDA;
  std::vector<float> m_cluster_ISOLATION;
  std::vector<float> m_cluster_ENERGY_DigiHSTruth;

  //cells in clusters
  std::vector<std::vector<size_t>> m_cluster_cell_ID;
  std::vector<std::vector<float>> m_cluster_cell_E;
  std::vector<std::vector<float>> m_cluster_cell_hitsE_EM;
  std::vector<std::vector<float>> m_cluster_cell_hitsE_nonEM;
  std::vector<std::vector<float>> m_cluster_cell_hitsE_Invisible;
  std::vector<std::vector<float>> m_cluster_cell_hitsE_Escaped;
  std::vector<std::vector<int>> m_cluster_fullHitsTruthIndex;
  std::vector<std::vector<float>> m_cluster_fullHitsTruthE;
  std::vector<std::vector<int>> m_cluster_visibleHitsTruthIndex;
  std::vector<std::vector<float>> m_cluster_visibleHitsTruthE;

  //all cells
  Gaudi::Property<bool> m_doAllCells{this, "AllCells", false, "Whether to store all cells in the event"};
  int m_nCells;
  std::vector<float> m_cell_E;
  std::vector<size_t> m_cell_ID;
  std::vector<float> m_cell_Time;
  std::vector<float> m_cell_Quality;

  static constexpr int m_G4BarcodeOffset = 200000;

  //Idealized barrel-endcap geometry parameters for flagging complicated early showers
  //Cylinder defined by these parameters is "inside" the calorimeter
  static constexpr float m_CaloBarrelRadius = 1450.;
  static constexpr float m_CaloBarrelEndCapTransitionZ = 3000.;
};

#endif
