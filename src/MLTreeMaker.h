/**
 * @file    MLTreeMaker.h
 * @author  Joakim Olsson <joakim.olsson@cern.ch>
# @brief    Athena package to save a tree that includes clusters, cells, tracks and truth information for projects using ML and Computer Vision
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
    // bool m_isMC;
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
    TTree* m_tree;

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
    std::vector<int>   m_pdgId;
    std::vector<int>   m_status;
    std::vector<int>   m_barcode;
    std::vector<float> m_truthPartPt;
    std::vector<float> m_truthPartMass;
    std::vector<float> m_truthPartEta;
    std::vector<float> m_truthPartPhi;

    // Track variables
    std::vector<float> m_trkPt;
    std::vector<float> m_trkP;
    std::vector<float> m_trkMass;
    std::vector<float> m_trkEta;
    std::vector<float> m_trkPhi;

    // Track extrapolation
    // Presampler
    std::vector<float> m_trkEta_PreSamplerB;
    std::vector<float> m_trkPhi_PreSamplerB;
    std::vector<float> m_trkEta_PreSamplerE;
    std::vector<float> m_trkPhi_PreSamplerE;
    // LAr EM Barrel layers
    std::vector<float> m_trkEta_EMB1; 
    std::vector<float> m_trkPhi_EMB1; 
    std::vector<float> m_trkEta_EMB2; 
    std::vector<float> m_trkPhi_EMB2; 
    std::vector<float> m_trkEta_EMB3; 
    std::vector<float> m_trkPhi_EMB3; 
    // LAr EM Endcap layers
    std::vector<float> m_trkEta_EME1; 
    std::vector<float> m_trkPhi_EME1; 
    std::vector<float> m_trkEta_EME2; 
    std::vector<float> m_trkPhi_EME2; 
    std::vector<float> m_trkEta_EME3; 
    std::vector<float> m_trkPhi_EME3; 
    // Hadronic Endcap layers
    std::vector<float> m_trkEta_HEC0; 
    std::vector<float> m_trkPhi_HEC0; 
    std::vector<float> m_trkEta_HEC1; 
    std::vector<float> m_trkPhi_HEC1; 
    std::vector<float> m_trkEta_HEC2; 
    std::vector<float> m_trkPhi_HEC2; 
    std::vector<float> m_trkEta_HEC3; 
    std::vector<float> m_trkPhi_HEC3; 
    // Tile Barrel layers
    std::vector<float> m_trkEta_TileBar0; 
    std::vector<float> m_trkPhi_TileBar0; 
    std::vector<float> m_trkEta_TileBar1; 
    std::vector<float> m_trkPhi_TileBar1; 
    std::vector<float> m_trkEta_TileBar2; 
    std::vector<float> m_trkPhi_TileBar2; 
    // Tile Gap layers
    std::vector<float> m_trkEta_TileGap1; 
    std::vector<float> m_trkPhi_TileGap1; 
    std::vector<float> m_trkEta_TileGap2; 
    std::vector<float> m_trkPhi_TileGap2; 
    std::vector<float> m_trkEta_TileGap3; 
    std::vector<float> m_trkPhi_TileGap3; 
    // Tile Extended Barrel layers
    std::vector<float> m_trkEta_TileExt0;
    std::vector<float> m_trkPhi_TileExt0;
    std::vector<float> m_trkEta_TileExt1;
    std::vector<float> m_trkPhi_TileExt1;
    std::vector<float> m_trkEta_TileExt2;
    std::vector<float> m_trkPhi_TileExt2;

    // Clusters 
    std::vector<float> m_clusE;
    std::vector<float> m_clusPt;
    std::vector<float> m_clusEta;
    std::vector<float> m_clusPhi;
    std::vector<std::vector<float> > m_clus_;

    // Cells
    std::vector<float> m_cellE;
    std::vector<float> m_cellPt;
    std::vector<float> m_cellEta;
    std::vector<float> m_cellPhi;
    std::vector<float> m_cellR;

}; 

#endif
