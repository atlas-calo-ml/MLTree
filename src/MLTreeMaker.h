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

class TileTBID;
class ICaloSurfaceHelper;

namespace Trk {
  class IExtrapolator;
  class Surface;
  class TrackParametersIdHelper;
}

namespace InDet {
    class IInDetTrackSelectionTool;
}

class MLTreeMaker: public ::AthHistogramAlgorithm { 

  public: 
    MLTreeMaker( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~MLTreeMaker(); 

    virtual StatusCode  initialize();
    virtual StatusCode  execute();
    virtual StatusCode  finalize();

private: 
  bool m_doEventTree;
  bool m_doClusterTree;
  bool m_doClusterCells;
  bool m_doCalibHits;
  std::vector<std::string> m_CalibrationHitContainerKeys;
  bool m_doClusterMoments;
  bool m_doUncalibratedClusters;
  // bool m_isMC;
  bool m_doTracking;
  bool m_doJets;
  bool m_doEventCleaning;
  bool m_doPileup;
  bool m_doShapeEM;
  bool m_doShapeLC;
  bool m_doEventTruth;
  bool m_doTruthParticles;
  bool m_keepOnlyStableTruthParticles;
  std::string m_prefix;
  std::string m_eventInfoContainerName;
  std::string m_truthContainerName;
  std::string m_vxContainerName;
  std::string m_trackContainerName;
  std::string m_caloClusterContainerName;
  std::vector<std::string> m_jetContainerNames;
  ToolHandle<Trk::IExtrapolator> m_extrapolator;
  ToolHandle<Trk::IParticleCaloExtensionTool> m_theTrackExtrapolatorTool;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;
  Trk::TrackParametersIdHelper* m_trackParametersIdHelper;
  ToolHandle<ICaloSurfaceHelper> m_surfaceHelper;
  const TileTBID* m_tileTBID; 

  // Retrieve tree
  TTree* m_eventTree;
  TTree* m_clusterTree;

  // Cluster and cell selections
  float m_clusterE_min;
  float m_clusterE_max;
  float m_clusterEtaAbs_max;
  float m_cellE_thres;

  //// Add to eventTree

  // Event info
  int      m_runNumber;
  Long64_t m_eventNumber;
  int      m_lumiBlock;
  uint32_t m_coreFlags;
  uint32_t m_timeStamp;
  uint32_t m_timeStampNSOffset;
  bool     m_TileError;
  bool     m_LArError;
  bool     m_SCTError;
  uint32_t m_TileFlags;
  uint32_t m_LArFlags;
  uint32_t m_SCTFlags;
  int      m_mcEventNumber;
  int      m_mcChannelNumber;
  float    m_mcEventWeight;
  float    m_weight_pileup;
  float    m_correct_mu;
  int      m_rand_run_nr;
  int      m_rand_lumiblock_nr;
  int      m_bcid;
  float    m_prescale_DataWeight;
  // pileup
  int      m_npv;
  float    m_actualMu;
  float    m_averageMu;
  // shapeEM
  double   m_rhoEM;
  double   m_rhoLC;
  // truth
  int      m_pdgId1;
  int      m_pdgId2;
  int      m_pdfId1;
  int      m_pdfId2;
  float    m_x1;
  float    m_x2;
  // float m_scale;
  // float    m_q;
  // float m_pdf1;
  // float m_pdf2;
  float    m_xf1;
  float    m_xf2;

  // Truth particles
  int m_nTruthPart;
  std::vector<int>   m_pdgId;
  std::vector<int>   m_status;
  std::vector<int>   m_barcode;
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

  // Clusters and cells 
  long m_clusterCount;
  int m_nCluster;
  std::vector<int> m_cluster_nCells;
  std::vector<float> m_clusterE;
  std::vector<float> m_clusterPt;
  std::vector<float> m_clusterEta;
  std::vector<float> m_clusterPhi;

  std::vector<float> m_cluster_cell_centerCellEta;
  std::vector<float> m_cluster_cell_centerCellPhi;
  std::vector<int>   m_cluster_cell_centerCellLayer;
  std::vector<std::vector<float> > m_jet_pt;
  std::vector<std::vector<float> > m_jet_eta;
  std::vector<std::vector<float> > m_jet_phi;
  std::vector<std::vector<float> > m_jet_E;
  std::vector<std::vector<int> > m_jet_flavor;
  //// Add to clusterTree
    
  int   m_fCluster_nCells;
  float m_fClusterTruthE;
  float m_fClusterTruthPt;
  float m_fClusterTruthEta;
  float m_fClusterTruthPhi;
  int   m_fClusterIndex;
  float m_fClusterE;
  float m_fClusterECalib;
  float m_fClusterPt;
  float m_fClusterEta;
  float m_fClusterPhi;

  float m_fCluster_ENG_CALIB_TOT;
  float m_fCluster_ENG_CALIB_OUT_T;
  float m_fCluster_ENG_CALIB_DEAD_TOT;

  float m_fCluster_EM_PROBABILITY;
  float m_fCluster_HAD_WEIGHT;
  float m_fCluster_OOC_WEIGHT;
  float m_fCluster_DM_WEIGHT;
  float m_fCluster_CENTER_MAG;
  float m_fCluster_FIRST_ENG_DENS;
  float m_fCluster_CENTER_LAMBDA;
  float m_fCluster_ISOLATION;
  float m_fCluster_ENERGY_DigiHSTruth;

  std::vector<size_t> m_cluster_cell_ID;
  std::vector<float> m_cluster_cell_E;
  std::vector<float> m_cluster_cell_E_EM;
  std::vector<float> m_cluster_cell_E_nonEM;
  std::vector<float> m_cluster_cell_E_Invisible;
  std::vector<float> m_cluster_cell_E_Escaped;  

}; 

#endif
