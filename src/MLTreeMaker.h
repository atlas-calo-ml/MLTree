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
    // bool m_isMC;
    bool m_doTracking;
    bool m_doEventCleaning;
    bool m_doPileup;
    bool m_doShapeEM;
    bool m_doShapeLC;
    bool m_doEventTruth;

    std::string m_prefix;
    std::string m_eventInfoContainerName;
    std::string m_truthContainerName;
    std::string m_vxContainerName;
    std::string m_trackContainerName;
    std::string m_caloClusterContainerName;

    ToolHandle<Trk::IExtrapolator> m_extrapolator;
    ToolHandle<Trk::IParticleCaloExtensionTool> m_theTrackExtrapolatorTool;
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
    int m_nCluster;
    std::vector<int> m_cluster_nCells;
    std::vector<float> m_clusterE;
    std::vector<float> m_clusterPt;
    std::vector<float> m_clusterEta;
    std::vector<float> m_clusterPhi;
    std::vector<float> m_cluster_sumCellE;
    std::vector<float> m_cluster_cell_dEta;
    std::vector<float> m_cluster_cell_dPhi;
    std::vector<float> m_cluster_cell_dR_min;
    std::vector<float> m_cluster_cell_dR_max;
    std::vector<float> m_cluster_cell_dEta_min;
    std::vector<float> m_cluster_cell_dEta_max;
    std::vector<float> m_cluster_cell_dPhi_min;
    std::vector<float> m_cluster_cell_dPhi_max;

    std::vector<float> m_cluster_cell_centerCellEta;
    std::vector<float> m_cluster_cell_centerCellPhi;
    std::vector<int>   m_cluster_cell_centerCellLayer;

    //// Add to clusterTree
    
    int   m_fCluster_nCells;
    float m_fClusterE;
    float m_fClusterPt;
    float m_fClusterEta;
    float m_fClusterPhi;
  float m_fCluster_emProb;
    float m_fCluster_sumCellE;
    float m_fCluster_cell_dR_min;
    float m_fCluster_cell_dR_max;
    float m_fCluster_cell_dEta_min;
    float m_fCluster_cell_dEta_max;
    float m_fCluster_cell_dPhi_min;
    float m_fCluster_cell_dPhi_max;

    float m_fCluster_cell_centerCellEta;
    float m_fCluster_cell_centerCellPhi;
    int   m_fCluster_cell_centerCellLayer;

    std::vector<float> m_cluster_cellE_norm;

    // Images: eta x phi = 0.4 x 0.4 
    float m_EMB1[128][4];
    float m_EMB2[16][16];
    float m_EMB3[8][16];
    float m_TileBar0[4][4];
    float m_TileBar1[4][4];
    float m_TileBar2[2][4];

    int m_duplicate_EMB1;
    int m_duplicate_EMB2;
    int m_duplicate_EMB3;
    int m_duplicate_TileBar0;
    int m_duplicate_TileBar1;
    int m_duplicate_TileBar2;

}; 

#endif
