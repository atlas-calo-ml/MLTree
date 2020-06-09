#include "MLTreeMaker.h"

// Tracks
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

// Extrapolation to the calo
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "CaloDetDescr/CaloDepthTool.h"

// Calo and cell information
#include "TileEvent/TileContainer.h"
#include "TileIdentifier/TileTBID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloTrackingGeometry/ICaloSurfaceHelper.h"
#include "TrkSurfaces/DiscSurface.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloUtils/CaloClusterSignalState.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

// Other xAOD incudes
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODEventShape/EventShape.h"

#include <string>
#include <vector>
#include <cmath>
#include <utility>
#include <limits>
#include <map>

MLTreeMaker::MLTreeMaker( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthHistogramAlgorithm( name, pSvcLocator ),
  m_doEventTree(false),
  m_doClusterTree(true),
  m_doClusterMoments(true),
  m_doUncalibratedClusters(true),
  m_doTracking(false),
  m_doEventCleaning(false),
  m_doPileup(false),
  m_doShapeEM(false),
  m_doShapeLC(false),
  m_doEventTruth(false),
  m_prefix(""),
  m_eventInfoContainerName("EventInfo"),
  m_truthContainerName("TruthParticles"),
  m_vxContainerName("PrimaryVertices"),
  m_trackContainerName("InDetTrackParticles"),
  m_caloClusterContainerName("CaloCalTopoClusters"),
  m_extrapolator("Trk::Extrapolator"),
  m_theTrackExtrapolatorTool("Trk::ParticleCaloExtensionTool"),
  m_trackParametersIdHelper(new Trk::TrackParametersIdHelper),
  m_surfaceHelper("CaloSurfaceHelper/CaloSurfaceHelper"),
  m_tileTBID(0),
  m_clusterE_min(90.0),
  m_clusterE_max(110.0),
  m_clusterEtaAbs_max(0.7),
  m_cellE_thres(0.005) // 5 MeV threshold
{
  declareProperty("ClusterEmin", m_clusterE_min);
  declareProperty("ClusterEmax", m_clusterE_max);
  declareProperty("ClusterEtaAbsmax", m_clusterEtaAbs_max);
  declareProperty("EventTree", m_doEventTree);
  declareProperty("ClusterTree", m_doClusterTree);
  declareProperty("ClusterMoments", m_doClusterMoments);
  declareProperty("UncalibratedClusters", m_doUncalibratedClusters);
  declareProperty("Tracking", m_doTracking);
  declareProperty("EventCleaning", m_doEventCleaning);
  declareProperty("Pileup", m_doPileup);
  declareProperty("ShapeEM", m_doShapeEM);
  declareProperty("ShapeLC", m_doShapeLC);
  declareProperty("EventTruth", m_doEventTruth);
  declareProperty("Prefix", m_prefix);
  declareProperty("EventContainer", m_eventInfoContainerName);
  declareProperty("TrackContainer", m_trackContainerName);
  declareProperty("CaloClusterContainer", m_caloClusterContainerName);
  declareProperty("Extrapolator", m_extrapolator);
  declareProperty("TheTrackExtrapolatorTool", m_theTrackExtrapolatorTool);
}

MLTreeMaker::~MLTreeMaker() {}

StatusCode MLTreeMaker::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (m_prefix=="") {
    ATH_MSG_WARNING("No decoration prefix name provided");
  }

  // const xAOD::EventInfo* eventInfo(nullptr);
  // CHECK( evtStore()->retrieve(eventInfo, m_eventInfoContainerName) );
  // m_isMC = ( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) );

  ATH_CHECK( m_extrapolator.retrieve() );
  ATH_CHECK( m_theTrackExtrapolatorTool.retrieve() );
  ATH_CHECK( m_surfaceHelper.retrieve() );
  // Get the test beam identifier for the MBTS
  ATH_CHECK( detStore()->retrieve(m_tileTBID) );

  if (m_doEventTree) {

    // Setup the event level TTree and its branches
    CHECK( book(TTree("EventTree", "EventTree")) );
    m_eventTree = tree("EventTree");

    // Event info 
    m_eventTree->Branch("runNumber",             &m_runNumber,      "runNumber/I");
    m_eventTree->Branch("eventNumber",           &m_eventNumber,    "eventNumber/LI");
    m_eventTree->Branch("lumiBlock",             &m_lumiBlock,      "lumiBlock/I");
    m_eventTree->Branch("coreFlags",             &m_coreFlags,      "coreFlags/i");
    // if (m_isMC ) {
    m_eventTree->Branch("mcEventNumber",       &m_mcEventNumber,  "mcEventNumber/I");
    m_eventTree->Branch("mcChannelNumber",     &m_mcChannelNumber,"mcChannelNumber/I");
    m_eventTree->Branch("mcEventWeight",       &m_mcEventWeight,  "mcEventWeight/F");
    // } else {
    //   m_eventTree->Branch("bcid",                &m_bcid,           "bcid/I");
    //   m_eventTree->Branch("prescale_DataWeight", &m_prescale_DataWeight,  "prescale_DataWeight/F");
    // }
    if (m_doEventCleaning ) {
      m_eventTree->Branch("timeStamp",           &m_timeStamp,         "timeStamp/i");
      m_eventTree->Branch("timeStampNSOffset",   &m_timeStampNSOffset, "timeStampNSOffset/i");
      m_eventTree->Branch("TileError",           &m_TileError,         "TileError/O");
      m_eventTree->Branch("SCTError",            &m_SCTError,          "SCTError/O");
      m_eventTree->Branch("LArError",            &m_LArError,          "LArError/O");
      m_eventTree->Branch("TileFlags",           &m_TileFlags,         "TileFlags/i");
      m_eventTree->Branch("SCTFlags",            &m_SCTFlags,          "SCTFlags/i");
      m_eventTree->Branch("LArFlags",            &m_LArFlags,          "LArFlags/i");
    }
    if (m_doPileup ) {
      m_eventTree->Branch("NPV",                 &m_npv,            "NPV/I");
      m_eventTree->Branch("actualInteractionsPerCrossing",  &m_actualMu,  "actualInteractionsPerCrossing/F");
      m_eventTree->Branch("averageInteractionsPerCrossing", &m_averageMu, "averageInteractionsPerCrossing/F");
      m_eventTree->Branch("weight_pileup",       &m_weight_pileup,  "weight_pileup/F");
      // if (m_isMC){
      m_eventTree->Branch("correct_mu"       , &m_correct_mu       ,"correct_mu/F"       );          
      m_eventTree->Branch("rand_run_nr"      , &m_rand_run_nr      ,"rand_run_nr/I"      );         
      m_eventTree->Branch("rand_lumiblock_nr", &m_rand_lumiblock_nr,"rand_lumiblock_nr/I");  
      // }
    }
    if (m_doShapeEM ) {
      m_eventTree->Branch("rhoEM",               &m_rhoEM,            "rhoEM/D");
    }
    if (m_doShapeLC ) {
      m_eventTree->Branch("rhoLC",               &m_rhoLC,            "rhoLC/D");
    }
    if (m_doEventTruth /*&& m_isMC */ ) {
      m_eventTree->Branch("pdgId1",              &m_pdgId1,        "pdgId1/I" );
      m_eventTree->Branch("pdgId2",              &m_pdgId2,        "pdgId2/I" );
      m_eventTree->Branch("pdfId1",              &m_pdfId1,        "pdfId1/I" );
      m_eventTree->Branch("pdfId2",              &m_pdfId2,        "pdfId2/I" );
      m_eventTree->Branch("x1",                  &m_x1,            "x1/F"  );
      m_eventTree->Branch("x2",                  &m_x2,            "x2/F"  );
      // m_eventTree->Branch("scale",               &m_scale,         "scale/F");
      // m_eventTree->Branch("q",                   &m_q,             "q/F");
      // m_eventTree->Branch("pdf1",                &m_pdf1,          "pdf1/F");
      // m_eventTree->Branch("pdf2",                &m_pdf2,          "pdf2/F");
      m_eventTree->Branch("xf1",                 &m_xf1,           "xf1/F");
      m_eventTree->Branch("xf2",                 &m_xf2,           "xf2/F");
    } 

    // Truth particles
    m_eventTree->Branch("nTruthPart",          &m_nTruthPart, "nTruthPart/I");
    m_eventTree->Branch("pdgId",               &m_pdgId);
    m_eventTree->Branch("status",              &m_status);
    m_eventTree->Branch("barcode",             &m_barcode);
    m_eventTree->Branch("truthPartPt",         &m_truthPartPt);
    m_eventTree->Branch("truthPartE",          &m_truthPartE);
    m_eventTree->Branch("truthPartMass",       &m_truthPartMass);
    m_eventTree->Branch("truthPartEta",        &m_truthPartEta);
    m_eventTree->Branch("truthPartPhi",        &m_truthPartPhi);

    // Track variables
    m_eventTree->Branch("nTrack",              &m_nTrack, "nTrack/I");
    m_eventTree->Branch("trackPt",             &m_trackPt);
    m_eventTree->Branch("trackP",              &m_trackP);
    m_eventTree->Branch("trackMass",           &m_trackMass);
    m_eventTree->Branch("trackEta",            &m_trackEta);
    m_eventTree->Branch("trackPhi",            &m_trackPhi);

    // Track extrapolation
    // Presampler
    m_eventTree->Branch("trackEta_PreSamplerB",  &m_trackEta_PreSamplerB);
    m_eventTree->Branch("trackPhi_PreSamplerB",  &m_trackPhi_PreSamplerB);
    m_eventTree->Branch("trackEta_PreSamplerE",  &m_trackEta_PreSamplerE);
    m_eventTree->Branch("trackPhi_PreSamplerE",  &m_trackPhi_PreSamplerE);
    // LAr EM Barrel layers
    m_eventTree->Branch("trackEta_EMB1",         &m_trackEta_EMB1); 
    m_eventTree->Branch("trackPhi_EMB1",         &m_trackPhi_EMB1); 
    m_eventTree->Branch("trackEta_EMB2",         &m_trackEta_EMB2); 
    m_eventTree->Branch("trackPhi_EMB2",         &m_trackPhi_EMB2); 
    m_eventTree->Branch("trackEta_EMB3",         &m_trackEta_EMB3); 
    m_eventTree->Branch("trackPhi_EMB3",         &m_trackPhi_EMB3); 
    // LAr EM Endcap layers
    m_eventTree->Branch("trackEta_EME1",         &m_trackEta_EME1); 
    m_eventTree->Branch("trackPhi_EME1",         &m_trackPhi_EME1); 
    m_eventTree->Branch("trackEta_EME2",         &m_trackEta_EME2); 
    m_eventTree->Branch("trackPhi_EME2",         &m_trackPhi_EME2); 
    m_eventTree->Branch("trackEta_EME3",         &m_trackEta_EME3); 
    m_eventTree->Branch("trackPhi_EME3",         &m_trackPhi_EME3); 
    // Hadronic Endcap layers
    m_eventTree->Branch("trackEta_HEC0",         &m_trackEta_HEC0); 
    m_eventTree->Branch("trackPhi_HEC0",         &m_trackPhi_HEC0); 
    m_eventTree->Branch("trackEta_HEC1",         &m_trackEta_HEC1); 
    m_eventTree->Branch("trackPhi_HEC1",         &m_trackPhi_HEC1); 
    m_eventTree->Branch("trackEta_HEC2",         &m_trackEta_HEC2); 
    m_eventTree->Branch("trackPhi_HEC2",         &m_trackPhi_HEC2); 
    m_eventTree->Branch("trackEta_HEC3",         &m_trackEta_HEC3); 
    m_eventTree->Branch("trackPhi_HEC3",         &m_trackPhi_HEC3); 
    // Tile Barrel layers
    m_eventTree->Branch("trackEta_TileBar0",     &m_trackEta_TileBar0); 
    m_eventTree->Branch("trackPhi_TileBar0",     &m_trackPhi_TileBar0); 
    m_eventTree->Branch("trackEta_TileBar1",     &m_trackEta_TileBar1); 
    m_eventTree->Branch("trackPhi_TileBar1",     &m_trackPhi_TileBar1); 
    m_eventTree->Branch("trackEta_TileBar2",     &m_trackEta_TileBar2); 
    m_eventTree->Branch("trackPhi_TileBar2",     &m_trackPhi_TileBar2); 
    // Tile Gap layers
    m_eventTree->Branch("trackEta_TileGap1",     &m_trackEta_TileGap1); 
    m_eventTree->Branch("trackPhi_TileGap1",     &m_trackPhi_TileGap1); 
    m_eventTree->Branch("trackEta_TileGap2",     &m_trackEta_TileGap2); 
    m_eventTree->Branch("trackPhi_TileGap2",     &m_trackPhi_TileGap2); 
    m_eventTree->Branch("trackEta_TileGap3",     &m_trackEta_TileGap3); 
    m_eventTree->Branch("trackPhi_TileGap3",     &m_trackPhi_TileGap3); 
    // Tile Extended Barrel layers
    m_eventTree->Branch("trackEta_TileExt0",     &m_trackEta_TileExt0);
    m_eventTree->Branch("trackPhi_TileExt0",     &m_trackPhi_TileExt0);
    m_eventTree->Branch("trackEta_TileExt1",     &m_trackEta_TileExt1);
    m_eventTree->Branch("trackPhi_TileExt1",     &m_trackPhi_TileExt1);
    m_eventTree->Branch("trackEta_TileExt2",     &m_trackEta_TileExt2);
    m_eventTree->Branch("trackPhi_TileExt2",     &m_trackPhi_TileExt2);

    // Clusters 
    m_eventTree->Branch("nCluster",               &m_nCluster, "nCluster/I");
    m_eventTree->Branch("clusterE",               &m_clusterE);
    m_eventTree->Branch("clusterPt",              &m_clusterPt);
    m_eventTree->Branch("clusterEta",             &m_clusterEta);
    m_eventTree->Branch("clusterPhi",             &m_clusterPhi);
    m_eventTree->Branch("cluster_nCells",         &m_cluster_nCells);
    m_eventTree->Branch("cluster_sumCellE",       &m_cluster_sumCellE);
    m_eventTree->Branch("cluster_cell_dEta",      &m_cluster_cell_dEta);
    m_eventTree->Branch("cluster_cell_dPhi",      &m_cluster_cell_dPhi);
    m_eventTree->Branch("cluster_cell_dR_min",    &m_cluster_cell_dR_min);
    m_eventTree->Branch("cluster_cell_dR_max",    &m_cluster_cell_dR_max);
    m_eventTree->Branch("cluster_cell_dEta_min",  &m_cluster_cell_dEta_min);
    m_eventTree->Branch("cluster_cell_dEta_max",  &m_cluster_cell_dEta_max);
    m_eventTree->Branch("cluster_cell_dPhi_min",  &m_cluster_cell_dPhi_min);
    m_eventTree->Branch("cluster_cell_dPhi_max",  &m_cluster_cell_dPhi_max);

    m_eventTree->Branch("m_cluster_cell_centerCellEta",    &m_cluster_cell_centerCellEta);
    m_eventTree->Branch("m_cluster_cell_centerCellPhi",    &m_cluster_cell_centerCellPhi);
    m_eventTree->Branch("m_cluster_cell_centerCellLayer",  &m_cluster_cell_centerCellLayer);
  }

  if (m_doClusterTree) {

    // Setup the cluster TTree and its branches
    CHECK( book(TTree("ClusterTree", "ClusterTree")) );
    m_clusterTree = tree("ClusterTree");

    m_clusterTree->Branch("runNumber",        &m_runNumber,         "runNumber/I");
    m_clusterTree->Branch("eventNumber",      &m_eventNumber,       "eventNumber/I");

    m_clusterTree->Branch("truthE",           &m_fClusterTruthE,    "truthE/F");
    m_clusterTree->Branch("truthPt",          &m_fClusterTruthPt,   "truthPt/F");
    m_clusterTree->Branch("truthEta",         &m_fClusterTruthEta,  "truthEta/F");
    m_clusterTree->Branch("truthPhi",         &m_fClusterTruthPhi,  "truthPhi/F");
    m_clusterTree->Branch("clusterIndex",     &m_fClusterIndex,     "clusterIndex/I");
    m_clusterTree->Branch("nCluster",         &m_nCluster,          "nCluster/I");
    m_clusterTree->Branch("clusterE",         &m_fClusterE,         "clusterE/F");
    m_clusterTree->Branch("clusterECalib",    &m_fClusterECalib,    "clusterECalib/F");
    m_clusterTree->Branch("clusterPt",        &m_fClusterPt,        "clusterPt/F");
    m_clusterTree->Branch("clusterEta",       &m_fClusterEta,       "clusterEta/F");
    m_clusterTree->Branch("clusterPhi",       &m_fClusterPhi,       "clusterPhi/F");
    m_clusterTree->Branch("cluster_nCells",   &m_fCluster_nCells,   "cluster_nCells/I");
    m_clusterTree->Branch("cluster_sumCellE", &m_fCluster_sumCellE, "cluster_sumCellE/F");

    if(m_doClusterMoments)
    {
      m_clusterTree->Branch("cluster_ENG_CALIB_TOT",     &m_fCluster_ENG_CALIB_TOT,      "cluster_ENG_CALIB_TOT/F");
      m_clusterTree->Branch("cluster_ENG_CALIB_OUT_T",   &m_fCluster_ENG_CALIB_OUT_T,    "cluster_ENG_CALIB_OUT_T/F");
      m_clusterTree->Branch("cluster_ENG_CALIB_DEAD_TOT",&m_fCluster_ENG_CALIB_DEAD_TOT, "cluster_ENG_CALIB_DEAD_TOT/F");
      m_clusterTree->Branch("cluster_EM_PROBABILITY",   &m_fCluster_EM_PROBABILITY,   "cluster_EM_PROBABILITY/F");
      m_clusterTree->Branch("cluster_HAD_WEIGHT", &m_fCluster_HAD_WEIGHT,  "cluster_HAD_WEIGHT/F");
      m_clusterTree->Branch("cluster_OOC_WEIGHT", &m_fCluster_OOC_WEIGHT,  "cluster_OOC_WEIGHT/F");
      m_clusterTree->Branch("cluster_DM_WEIGHT", &m_fCluster_DM_WEIGHT,  "cluster_DM_WEIGHT/F");
      m_clusterTree->Branch("cluster_CENTER_MAG", &m_fCluster_CENTER_MAG,  "cluster_CENTER_MAG/F");
      m_clusterTree->Branch("cluster_FIRST_ENG_DENS", &m_fCluster_FIRST_ENG_DENS,  "cluster_FIRST_ENG_DENS/F");
    }

    m_clusterTree->Branch("cluster_cell_dR_min",    &m_fCluster_cell_dR_min,   "cluster_cell_dR_min/F");
    m_clusterTree->Branch("cluster_cell_dR_max",    &m_fCluster_cell_dR_max,   "cluster_cell_dR_max/F");
    m_clusterTree->Branch("cluster_cell_dEta_min",  &m_fCluster_cell_dEta_min, "cluster_cell_dEta_min/F");
    m_clusterTree->Branch("cluster_cell_dEta_max",  &m_fCluster_cell_dEta_max, "cluster_cell_dEta_max/F");
    m_clusterTree->Branch("cluster_cell_dPhi_min",  &m_fCluster_cell_dPhi_min, "cluster_cell_dPhi_min/F");
    m_clusterTree->Branch("cluster_cell_dPhi_max",  &m_fCluster_cell_dPhi_max, "cluster_cell_dPhi_max/F");
    m_clusterTree->Branch("cluster_cell_centerCellEta",    &m_fCluster_cell_centerCellEta,   "cluster_cell_centerCellEta/F");
    m_clusterTree->Branch("cluster_cell_centerCellPhi",    &m_fCluster_cell_centerCellPhi,   "cluster_cell_centerCellPhi/F");
    m_clusterTree->Branch("cluster_cell_centerCellLayer",  &m_fCluster_cell_centerCellLayer, "cluster_cell_centerCellLayer/I");

    m_clusterTree->Branch("cluster_cellE_norm", &m_cluster_cellE_norm);

    // Images
    m_clusterTree->Branch("EMB1",           &m_EMB1[0],         "EMB1[128][4]/F");
    m_clusterTree->Branch("EMB2",           &m_EMB2[0],         "EMB2[16][16]/F");
    m_clusterTree->Branch("EMB3",           &m_EMB3[0],         "EMB3[8][16]/F");
    m_clusterTree->Branch("TileBar0",       &m_TileBar0[0],     "TileBar0[4][4]/F");
    m_clusterTree->Branch("TileBar1",       &m_TileBar1[0],     "TileBar1[4][4]/F");
    m_clusterTree->Branch("TileBar2",       &m_TileBar2[0],     "TileBar2[2][4]/F");

    // Check for duplicates
    m_clusterTree->Branch("duplicate_EMB1",       &m_duplicate_EMB1,     "duplicate_EMB1/I");
    m_clusterTree->Branch("duplicate_EMB2",       &m_duplicate_EMB2,     "duplicate_EMB2/I");
    m_clusterTree->Branch("duplicate_EMB3",       &m_duplicate_EMB3,     "duplicate_EMB3/I");
    m_clusterTree->Branch("duplicate_TileBar0",   &m_duplicate_TileBar0, "duplicate_TileBar0/I");
    m_clusterTree->Branch("duplicate_TileBar1",   &m_duplicate_TileBar1, "duplicate_TileBar1/I");
    m_clusterTree->Branch("duplicate_TileBar2",   &m_duplicate_TileBar2, "duplicate_TileBar2/I");
  }

  return StatusCode::SUCCESS;
}

StatusCode MLTreeMaker::execute() {  

  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // Clear all variables from previous event
  m_runNumber = m_eventNumber = m_mcEventNumber = m_mcChannelNumber = m_bcid = m_lumiBlock = 0;
  m_coreFlags = 0;
  // event cleaning
  m_LArError = false;
  m_TileError = false;
  m_SCTError = false;
  m_LArFlags = 0;
  m_TileFlags = 0;
  m_SCTFlags = 0;
  m_mcEventWeight = 1.;
  m_prescale_DataWeight = 1.;
  m_weight_pileup = 1.;
  m_timeStamp = -999;
  m_timeStampNSOffset = -999;
  // pileup
  m_npv = -999;
  m_actualMu = m_averageMu = -999;
  // shapeEM
  m_rhoEM = -999;
  // shapeLC
  m_rhoLC = -999;
  // truth
  m_pdgId1 = m_pdgId2 = m_pdfId1 = m_pdfId2 = -999;
  m_x1 = m_x2 = -999;
  m_xf1 = m_xf2 = -999;
  //m_scale = m_q = m_pdf1 = m_pdf2 = -999;

  m_pdgId.clear();
  m_status.clear();
  m_barcode.clear();
  m_truthPartPt.clear();
  m_truthPartE.clear();
  m_truthPartMass.clear();
  m_truthPartEta.clear();
  m_truthPartPhi.clear();

  m_trackPt.clear();
  m_trackP.clear();
  m_trackMass.clear();
  m_trackEta.clear();
  m_trackPhi.clear();

  m_trackEta_PreSamplerB.clear();
  m_trackPhi_PreSamplerB.clear();
  m_trackEta_PreSamplerE.clear();
  m_trackPhi_PreSamplerE.clear();

  m_trackEta_EMB1.clear(); 
  m_trackPhi_EMB1.clear(); 
  m_trackEta_EMB2.clear(); 
  m_trackPhi_EMB2.clear(); 
  m_trackEta_EMB3.clear(); 
  m_trackPhi_EMB3.clear(); 

  m_trackEta_EME1.clear(); 
  m_trackPhi_EME1.clear(); 
  m_trackEta_EME2.clear(); 
  m_trackPhi_EME2.clear(); 
  m_trackEta_EME3.clear(); 
  m_trackPhi_EME3.clear(); 

  m_trackEta_HEC0.clear(); 
  m_trackPhi_HEC0.clear(); 
  m_trackEta_HEC1.clear(); 
  m_trackPhi_HEC1.clear(); 
  m_trackEta_HEC2.clear(); 
  m_trackPhi_HEC2.clear(); 
  m_trackEta_HEC3.clear(); 
  m_trackPhi_HEC3.clear(); 

  m_trackEta_TileBar0.clear(); 
  m_trackPhi_TileBar0.clear(); 
  m_trackEta_TileBar1.clear(); 
  m_trackPhi_TileBar1.clear(); 
  m_trackEta_TileBar2.clear(); 
  m_trackPhi_TileBar2.clear(); 

  m_trackEta_TileGap1.clear(); 
  m_trackPhi_TileGap1.clear(); 
  m_trackEta_TileGap2.clear(); 
  m_trackPhi_TileGap2.clear(); 
  m_trackEta_TileGap3.clear(); 
  m_trackPhi_TileGap3.clear(); 

  m_trackEta_TileExt0.clear();
  m_trackPhi_TileExt0.clear();
  m_trackEta_TileExt1.clear();
  m_trackPhi_TileExt1.clear();
  m_trackEta_TileExt2.clear();
  m_trackPhi_TileExt2.clear();

  m_clusterE.clear();
  m_clusterPt.clear();
  m_clusterEta.clear();
  m_clusterPhi.clear();
  m_cluster_nCells.clear();
  m_cluster_sumCellE.clear();
  m_cluster_cell_dEta.clear();
  m_cluster_cell_dPhi.clear();
  m_cluster_cell_dR_min.clear();
  m_cluster_cell_dR_max.clear();
  m_cluster_cell_dEta_min.clear();
  m_cluster_cell_dEta_max.clear();
  m_cluster_cell_dPhi_min.clear();
  m_cluster_cell_dPhi_max.clear();

  m_cluster_cell_centerCellEta.clear();
  m_cluster_cell_centerCellPhi.clear();
  m_cluster_cell_centerCellLayer.clear();

  // General event information
  const xAOD::EventInfo* eventInfo(nullptr);
  CHECK( evtStore()->retrieve(eventInfo, m_eventInfoContainerName) );

  m_runNumber         = eventInfo->runNumber();
  m_eventNumber       = eventInfo->eventNumber();
  m_lumiBlock         = eventInfo->lumiBlock();
  m_coreFlags         = eventInfo->eventFlags(xAOD::EventInfo::Core);
  // if (m_isMC ) {
  m_mcEventNumber   = eventInfo->mcEventNumber();
  m_mcChannelNumber = eventInfo->mcChannelNumber();
  m_mcEventWeight   = eventInfo->mcEventWeight();
  // } else {
  //   m_bcid            = eventInfo->bcid();
  // }

  if (m_doEventCleaning ) {

    if (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error ) m_LArError = true;
    else m_LArError = false;
    m_LArFlags = eventInfo->eventFlags(xAOD::EventInfo::LAr);

    if (eventInfo->errorState(xAOD::EventInfo::Tile)==xAOD::EventInfo::Error ) m_TileError = true;
    else m_TileError = false;
    m_TileFlags = eventInfo->eventFlags(xAOD::EventInfo::Tile);

    if (eventInfo->errorState(xAOD::EventInfo::SCT)==xAOD::EventInfo::Error ) m_SCTError = true;
    else m_SCTError = false;
    m_SCTFlags = eventInfo->eventFlags(xAOD::EventInfo::SCT);

    m_timeStamp = eventInfo->timeStamp();
    m_timeStampNSOffset = eventInfo->timeStampNSOffset();
  }

  if (m_doPileup ) {

    const xAOD::VertexContainer* vertices = 0;
    CHECK( evtStore()->retrieve( vertices, m_vxContainerName));
    int m_npv = 0;
    unsigned int Ntracks = 2;
    for (const auto& vertex : *vertices)
    {
      if (vertex->vertexType() == xAOD::VxType::NoVtx) continue;
      if (vertex->nTrackParticles() < Ntracks ) continue;
      m_npv++;
    }
    m_actualMu  = eventInfo->actualInteractionsPerCrossing();
    m_averageMu = eventInfo->averageInteractionsPerCrossing();

    // if (m_isMC ) {
    static SG::AuxElement::ConstAccessor< float > weight_pileup ("PileupWeight");
    static SG::AuxElement::ConstAccessor< float >  correct_mu("corrected_averageInteractionsPerCrossing");
    static SG::AuxElement::ConstAccessor< unsigned int > rand_run_nr("RandomRunNumber");
    static SG::AuxElement::ConstAccessor< unsigned int > rand_lumiblock_nr("RandomLumiBlockNumber");

    if ( weight_pileup.isAvailable( *eventInfo ) )	 { m_weight_pileup = weight_pileup( *eventInfo ); }	    else { m_weight_pileup = 1.0; }
    if ( correct_mu.isAvailable( *eventInfo ) )	 { m_correct_mu = correct_mu( *eventInfo ); }		    else { m_correct_mu = -1.0; }
    if ( rand_run_nr.isAvailable( *eventInfo ) )	 { m_rand_run_nr = rand_run_nr( *eventInfo ); } 	    else { m_rand_run_nr = 900000; }
    if ( rand_lumiblock_nr.isAvailable( *eventInfo ) ) { m_rand_lumiblock_nr = rand_lumiblock_nr( *eventInfo ); } else { m_rand_lumiblock_nr = 0; }

    // } else {
    //   static SG::AuxElement::ConstAccessor< float > prsc_DataWeight ("prescale_DataWeight");
    //
    //   if (prsc_DataWeight.isAvailable( *eventInfo ) )	 {
    //     m_prescale_DataWeight = prsc_DataWeight( *eventInfo ); 
    //   }
    //   else { 
    //     m_prescale_DataWeight = 1.0; 
    //   }
    // }
  }

  if (m_doShapeLC ) {
    const xAOD::EventShape* evtShape(nullptr);
    CHECK(evtStore()->retrieve( evtShape, "Kt4LCTopoEventShape"));
    if ( !evtShape->getDensity( xAOD::EventShape::Density, m_rhoLC ) ) {
      Info("FillEvent()","Could not retrieve xAOD::EventShape::Density from xAOD::EventShape");
      m_rhoLC = -999;
    }
  }

  if (m_doShapeEM ) {
    const xAOD::EventShape* evtShape(nullptr);
    CHECK(evtStore()->retrieve( evtShape, "Kt4EMTopoEventShape"));
    if ( !evtShape->getDensity( xAOD::EventShape::Density, m_rhoEM ) ) {
      Info("FillEvent()","Could not retrieve xAOD::EventShape::Density from xAOD::EventShape");
      m_rhoEM = -999;
    }
  }

  if (m_doEventTruth /*&& m_isMC*/ ) {

    const xAOD::TruthEventContainer* truthEventContainer = 0;
    CHECK(evtStore()->retrieve( truthEventContainer, "TruthEvents"));
    if (truthEventContainer ) {
      const xAOD::TruthEvent* truthEventContainervent = truthEventContainer->at(0);
      truthEventContainervent->pdfInfoParameter(m_pdgId1,   xAOD::TruthEvent::PDGID1);
      truthEventContainervent->pdfInfoParameter(m_pdgId2,   xAOD::TruthEvent::PDGID2);
      truthEventContainervent->pdfInfoParameter(m_pdfId1,   xAOD::TruthEvent::PDFID1);
      truthEventContainervent->pdfInfoParameter(m_pdfId2,   xAOD::TruthEvent::PDFID2);
      truthEventContainervent->pdfInfoParameter(m_x1,       xAOD::TruthEvent::X1);
      truthEventContainervent->pdfInfoParameter(m_x2,       xAOD::TruthEvent::X2);
      // truthEventContainervent->pdfInfoParameter(m_scale,    xAOD::TruthEvent::SCALE);
      // truthEventContainervent->pdfInfoParameter(m_q,        xAOD::TruthEvent::Q);
      // truthEventContainervent->pdfInfoParameter(m_pdf1,     xAOD::TruthEvent::PDF1);
      // truthEventContainervent->pdfInfoParameter(m_pdf2,     xAOD::TruthEvent::PDF2);
      truthEventContainervent->pdfInfoParameter(m_xf1,      xAOD::TruthEvent::XF1);
      truthEventContainervent->pdfInfoParameter(m_xf2,      xAOD::TruthEvent::XF2);

      // // crashes because of q?`
      //   const xAOD::TruthEvent::PdfInfo info = truthEventContainervent->pdfInfo();
      //   if (info.valid() ) {
      //     m_pdgId1 = info.pdgId1;
      //     m_pdgId2 = info.pdgId2;
      //     m_pdfId1 = info.pdfId1;
      //     m_pdfId2 = info.pdfId2;
      //     m_x1     = info.x1;
      //     m_x2     = info.x2;
      //     //m_q      = info.Q;
      //     m_xf1    = info.xf1;
      //     m_xf2    = info.xf2;
      //   }
    }
  }

  if(m_doClusterTree)
  {
    const xAOD::TruthParticleContainer* truthContainer = 0;
    CHECK(evtStore()->retrieve(truthContainer, m_truthContainerName));
    if(truthContainer->size()==0)
    {
      m_fClusterTruthE=-999;
      m_fClusterTruthPt=-999;
      m_fClusterTruthEta=-999;
      m_fClusterTruthPhi=-999;
    }
    else
    {
      m_fClusterTruthE=(*truthContainer)[0]->e()*1e-3;
      m_fClusterTruthPt=(*truthContainer)[0]->pt()*1e-3;
      m_fClusterTruthEta=(*truthContainer)[0]->eta();
      m_fClusterTruthPhi=(*truthContainer)[0]->phi();
    }
  }
  if (m_doEventTree) {

    // Truth particles
    const xAOD::TruthParticleContainer* truthContainer = 0;
    CHECK(evtStore()->retrieve(truthContainer, m_truthContainerName));

    m_nTruthPart = 0;
    for (const auto& truth: *truthContainer) {

      m_pdgId.push_back(truth->pdgId());
      m_status.push_back(truth->status());
      m_barcode.push_back(truth->barcode());
      m_truthPartPt.push_back(truth->pt()/1e3);
      m_truthPartE.push_back(truth->e()/1e3);
      m_truthPartMass.push_back(truth->m()/1e3);
      m_truthPartEta.push_back(truth->eta());
      m_truthPartPhi.push_back(truth->phi());
      m_nTruthPart++;
    }

    if (m_doTracking) {

      // Tracks
      const xAOD::TrackParticleContainer* trackContainer = 0;
      CHECK(evtStore()->retrieve(trackContainer, m_trackContainerName));

      m_nTrack = 0;
      for (const auto& track : *trackContainer) {

        m_trackPt.push_back(track->pt()/1e3);
        m_trackP.push_back(TMath::Abs(1./track->qOverP())/1e3);
        m_trackMass.push_back(track->m()/1e3);
        m_trackEta.push_back(track->eta());
        m_trackPhi.push_back(track->phi());

        // A map to store the track parameters associated with the different layers of the calorimeter system
        std::map<CaloCell_ID::CaloSample, const Trk::TrackParameters*> parametersMap;

        // Get the CaloExtension object
	//For R22 replace with
	//std::unique_ptr<Trk::CaloExtension> extension=m_theTrackExtrapolatorTool->caloExtension(*track);
	//if(extension)

	const Trk::CaloExtension* extension = 0;
	if (m_theTrackExtrapolatorTool->caloExtension(*track, extension)) 
	{
          // Extract the CurvilinearParameters per each layer-track intersection
          const std::vector<const Trk::CurvilinearParameters*>& clParametersVector = extension->caloLayerIntersections();

          for (auto clParameter : clParametersVector) {

            unsigned int parametersIdentifier = clParameter->cIdentifier();
            CaloCell_ID::CaloSample intLayer;

            if (!m_trackParametersIdHelper->isValid(parametersIdentifier)) {
              std::cout << "Invalid Track Identifier"<< std::endl;
              intLayer = CaloCell_ID::CaloSample::Unknown;
            } else {
              intLayer = m_trackParametersIdHelper->caloSample(parametersIdentifier);
            }

            if (parametersMap[intLayer] == NULL) {
              parametersMap[intLayer] = clParameter->clone();
            } else if (m_trackParametersIdHelper->isEntryToVolume(clParameter->cIdentifier())) {
              delete parametersMap[intLayer];
              parametersMap[intLayer] = clParameter->clone();
            }
          }

        }
	else {
	  ATH_MSG_WARNING("TrackExtension failed for track with pt and eta " << track->pt() << " and " << track->eta());
	  continue;
        }

        //  ---------Calo Sample layer Variables---------
        //  PreSamplerB=0, EMB1, EMB2, EMB3, // LAr barrel
        //  PreSamplerE, EME1, EME2, EME3,   // LAr EM endcap
        //  HEC0, HEC1, HEC2, HEC3,          // Hadronic end cap cal.
        //  TileBar0, TileBar1, TileBar2,    // Tile barrel
        //  TileGap1, TileGap2, TileGap3,    // Tile gap (ITC & scint)
        //  TileExt0, TileExt1, TileExt2,    // Tile extended barrel
        //  FCAL0, FCAL1, FCAL2,             // Forward EM endcap (excluded)
        //  Unknown

        // Presampler
        float trackEta_PreSamplerB_tmp = -999999999;
        float trackPhi_PreSamplerB_tmp = -999999999;
        float trackEta_PreSamplerE_tmp = -999999999;
        float trackPhi_PreSamplerE_tmp = -999999999;
        // LAr EM Barrel layers
        float trackEta_EMB1_tmp = -999999999;
        float trackPhi_EMB1_tmp = -999999999;
        float trackEta_EMB2_tmp = -999999999;
        float trackPhi_EMB2_tmp = -999999999;
        float trackEta_EMB3_tmp = -999999999;
        float trackPhi_EMB3_tmp = -999999999;
        // LAr EM Endcap layers
        float trackEta_EME1_tmp = -999999999;
        float trackPhi_EME1_tmp = -999999999;
        float trackEta_EME2_tmp = -999999999;
        float trackPhi_EME2_tmp = -999999999;
        float trackEta_EME3_tmp = -999999999;
        float trackPhi_EME3_tmp = -999999999;
        // Hadronic Endcap layers
        float trackEta_HEC0_tmp = -999999999;
        float trackPhi_HEC0_tmp = -999999999;
        float trackEta_HEC1_tmp = -999999999;
        float trackPhi_HEC1_tmp = -999999999;
        float trackEta_HEC2_tmp = -999999999;
        float trackPhi_HEC2_tmp = -999999999;
        float trackEta_HEC3_tmp = -999999999;
        float trackPhi_HEC3_tmp = -999999999;
        // Tile Barrel layers
        float trackEta_TileBar0_tmp = -999999999;
        float trackPhi_TileBar0_tmp = -999999999;
        float trackEta_TileBar1_tmp = -999999999;
        float trackPhi_TileBar1_tmp = -999999999;
        float trackEta_TileBar2_tmp = -999999999;
        float trackPhi_TileBar2_tmp = -999999999;
        // Tile Gap layers
        float trackEta_TileGap1_tmp = -999999999;
        float trackPhi_TileGap1_tmp = -999999999;
        float trackEta_TileGap2_tmp = -999999999;
        float trackPhi_TileGap2_tmp = -999999999;
        float trackEta_TileGap3_tmp = -999999999;
        float trackPhi_TileGap3_tmp = -999999999;
        // Tile Extended Barrel layers
        float trackEta_TileExt0_tmp = -999999999;
        float trackPhi_TileExt0_tmp = -999999999;
        float trackEta_TileExt1_tmp = -999999999;
        float trackPhi_TileExt1_tmp = -999999999;
        float trackEta_TileExt2_tmp = -999999999;
        float trackPhi_TileExt2_tmp = -999999999;

        if (parametersMap[CaloCell_ID::CaloSample::PreSamplerB]) trackEta_PreSamplerB_tmp = parametersMap[CaloCell_ID::CaloSample::PreSamplerB]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::PreSamplerB]) trackPhi_PreSamplerB_tmp = parametersMap[CaloCell_ID::CaloSample::PreSamplerB]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::PreSamplerE]) trackEta_PreSamplerE_tmp = parametersMap[CaloCell_ID::CaloSample::PreSamplerE]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::PreSamplerE]) trackPhi_PreSamplerE_tmp = parametersMap[CaloCell_ID::CaloSample::PreSamplerE]->momentum().phi();

        if (parametersMap[CaloCell_ID::CaloSample::EMB1]) trackEta_EMB1_tmp = parametersMap[CaloCell_ID::CaloSample::EMB1]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::EMB1]) trackPhi_EMB1_tmp = parametersMap[CaloCell_ID::CaloSample::EMB1]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::EMB2]) trackEta_EMB2_tmp = parametersMap[CaloCell_ID::CaloSample::EMB2]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::EMB2]) trackPhi_EMB2_tmp = parametersMap[CaloCell_ID::CaloSample::EMB2]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::EMB3]) trackEta_EMB3_tmp = parametersMap[CaloCell_ID::CaloSample::EMB3]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::EMB3]) trackPhi_EMB3_tmp = parametersMap[CaloCell_ID::CaloSample::EMB3]->momentum().phi();

        if (parametersMap[CaloCell_ID::CaloSample::EME1]) trackEta_EME1_tmp = parametersMap[CaloCell_ID::CaloSample::EME1]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::EME1]) trackPhi_EME1_tmp = parametersMap[CaloCell_ID::CaloSample::EME1]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::EME2]) trackEta_EME2_tmp = parametersMap[CaloCell_ID::CaloSample::EME2]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::EME2]) trackPhi_EME2_tmp = parametersMap[CaloCell_ID::CaloSample::EME2]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::EME3]) trackEta_EME3_tmp = parametersMap[CaloCell_ID::CaloSample::EME3]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::EME3]) trackPhi_EME3_tmp = parametersMap[CaloCell_ID::CaloSample::EME3]->momentum().phi();

        if (parametersMap[CaloCell_ID::CaloSample::HEC0]) trackEta_HEC0_tmp = parametersMap[CaloCell_ID::CaloSample::HEC0]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::HEC0]) trackPhi_HEC0_tmp = parametersMap[CaloCell_ID::CaloSample::HEC0]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::HEC1]) trackEta_HEC1_tmp = parametersMap[CaloCell_ID::CaloSample::HEC1]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::HEC1]) trackPhi_HEC1_tmp = parametersMap[CaloCell_ID::CaloSample::HEC1]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::HEC2]) trackEta_HEC2_tmp = parametersMap[CaloCell_ID::CaloSample::HEC2]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::HEC2]) trackPhi_HEC2_tmp = parametersMap[CaloCell_ID::CaloSample::HEC2]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::HEC3]) trackEta_HEC3_tmp = parametersMap[CaloCell_ID::CaloSample::HEC3]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::HEC3]) trackPhi_HEC3_tmp = parametersMap[CaloCell_ID::CaloSample::HEC3]->momentum().phi();

        if (parametersMap[CaloCell_ID::CaloSample::TileBar0]) trackEta_TileBar0_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar0]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileBar0]) trackPhi_TileBar0_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar0]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::TileBar1]) trackEta_TileBar1_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar1]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileBar1]) trackPhi_TileBar1_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar1]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::TileBar2]) trackEta_TileBar2_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar2]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileBar2]) trackPhi_TileBar2_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar2]->momentum().phi();

        if (parametersMap[CaloCell_ID::CaloSample::TileGap1]) trackEta_TileGap1_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap1]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileGap1]) trackPhi_TileGap1_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap1]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::TileGap2]) trackEta_TileGap2_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap2]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileGap2]) trackPhi_TileGap2_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap2]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::TileGap3]) trackEta_TileGap3_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap3]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileGap3]) trackPhi_TileGap3_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap3]->momentum().phi();

        if (parametersMap[CaloCell_ID::CaloSample::TileExt0]) trackEta_TileExt0_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt0]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileExt0]) trackPhi_TileExt0_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt0]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::TileExt1]) trackEta_TileExt1_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt1]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileExt1]) trackPhi_TileExt1_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt1]->momentum().phi();
        if (parametersMap[CaloCell_ID::CaloSample::TileExt2]) trackEta_TileExt2_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt2]->momentum().eta();
        if (parametersMap[CaloCell_ID::CaloSample::TileExt2]) trackPhi_TileExt2_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt2]->momentum().phi();

        m_trackEta_PreSamplerB.push_back(trackEta_PreSamplerB_tmp);
        m_trackPhi_PreSamplerB.push_back(trackPhi_PreSamplerB_tmp);
        m_trackEta_PreSamplerE.push_back(trackEta_PreSamplerE_tmp);
        m_trackPhi_PreSamplerE.push_back(trackPhi_PreSamplerE_tmp);

        m_trackEta_EMB1.push_back(trackEta_EMB1_tmp); 
        m_trackPhi_EMB1.push_back(trackPhi_EMB1_tmp); 
        m_trackEta_EMB2.push_back(trackEta_EMB2_tmp); 
        m_trackPhi_EMB2.push_back(trackPhi_EMB2_tmp); 
        m_trackEta_EMB3.push_back(trackEta_EMB3_tmp); 
        m_trackPhi_EMB3.push_back(trackPhi_EMB3_tmp); 

        m_trackEta_EME1.push_back(trackEta_EME1_tmp); 
        m_trackPhi_EME1.push_back(trackPhi_EME1_tmp); 
        m_trackEta_EME2.push_back(trackEta_EME2_tmp); 
        m_trackPhi_EME2.push_back(trackPhi_EME2_tmp); 
        m_trackEta_EME3.push_back(trackEta_EME3_tmp); 
        m_trackPhi_EME3.push_back(trackPhi_EME3_tmp); 

        m_trackEta_HEC0.push_back(trackEta_HEC0_tmp); 
        m_trackPhi_HEC0.push_back(trackPhi_HEC0_tmp); 
        m_trackEta_HEC1.push_back(trackEta_HEC1_tmp); 
        m_trackPhi_HEC1.push_back(trackPhi_HEC1_tmp); 
        m_trackEta_HEC2.push_back(trackEta_HEC2_tmp); 
        m_trackPhi_HEC2.push_back(trackPhi_HEC2_tmp); 
        m_trackEta_HEC3.push_back(trackEta_HEC3_tmp); 
        m_trackPhi_HEC3.push_back(trackPhi_HEC3_tmp); 

        m_trackEta_TileBar0.push_back(trackEta_TileBar0_tmp); 
        m_trackPhi_TileBar0.push_back(trackPhi_TileBar0_tmp); 
        m_trackEta_TileBar1.push_back(trackEta_TileBar1_tmp); 
        m_trackPhi_TileBar1.push_back(trackPhi_TileBar1_tmp); 
        m_trackEta_TileBar2.push_back(trackEta_TileBar2_tmp); 
        m_trackPhi_TileBar2.push_back(trackPhi_TileBar2_tmp); 

        m_trackEta_TileGap1.push_back(trackEta_TileGap1_tmp); 
        m_trackPhi_TileGap1.push_back(trackPhi_TileGap1_tmp); 
        m_trackEta_TileGap2.push_back(trackEta_TileGap2_tmp); 
        m_trackPhi_TileGap2.push_back(trackPhi_TileGap2_tmp); 
        m_trackEta_TileGap3.push_back(trackEta_TileGap3_tmp); 
        m_trackPhi_TileGap3.push_back(trackPhi_TileGap3_tmp); 

        m_trackEta_TileExt0.push_back(trackEta_TileExt0_tmp);
        m_trackPhi_TileExt0.push_back(trackPhi_TileExt0_tmp);
        m_trackEta_TileExt1.push_back(trackEta_TileExt1_tmp);
        m_trackPhi_TileExt1.push_back(trackPhi_TileExt1_tmp);
        m_trackEta_TileExt2.push_back(trackEta_TileExt2_tmp);
        m_trackPhi_TileExt2.push_back(trackPhi_TileExt2_tmp);

        m_nTrack++;
      }
    }
  }

  // Calo clusters
  const xAOD::CaloClusterContainer* clusterContainer = 0; 
  CHECK(evtStore()->retrieve(clusterContainer, m_caloClusterContainerName));

  //first sort the clusters by energy
  //fill a (multi)map with key = energy and value = index of cluster in list
  std::multimap<float,unsigned int,std::greater<float> > clusterRanks;

  for (unsigned int iCluster=0; iCluster < clusterContainer->size(); iCluster++)
  {
    auto calibratedCluster=(*clusterContainer)[iCluster];
    auto cluster=calibratedCluster;
    if(m_doUncalibratedClusters) cluster=calibratedCluster->getSisterCluster();

    float clusterE = cluster->e()/1e3;
    float clusterEta = cluster->eta();
    if (clusterE < m_clusterE_min || 
        clusterE > m_clusterE_max || 
        fabs(clusterEta) > m_clusterEtaAbs_max) continue;

    clusterRanks.emplace_hint(clusterRanks.end(),clusterE,iCluster);
  }  
  m_nCluster = clusterRanks.size();

  //loop over clusters in order of their energies
  //clusters failing E or eta cut are not included in loop
  unsigned int jCluster=0;
  for(auto mpair : clusterRanks)
  {
    auto calibratedCluster=(*clusterContainer)[mpair.second];
    auto cluster=calibratedCluster;
    if(m_doUncalibratedClusters) cluster=calibratedCluster->getSisterCluster();
    float clusterE = cluster->e()/1e3;
    float clusterPt = cluster->pt()/1e3;
    float clusterEta = cluster->eta();
    float clusterPhi = cluster->phi();

    //ARA: adding code to retreive EM_PROBABILITY
    double cluster_ENG_CALIB_TOT = 0;
    double cluster_ENG_CALIB_OUT_T = 0;
    double cluster_ENG_CALIB_DEAD_TOT = 0;

    double cluster_EM_PROBABILITY = 0;
    double cluster_HAD_WEIGHT = 0;
    double cluster_OOC_WEIGHT = 0;
    double cluster_DM_WEIGHT = 0;
    double cluster_CENTER_MAG = 0;
    double cluster_FIRST_ENG_DENS = 0;

    if(m_doClusterMoments)
    {
      if( !cluster->retrieveMoment( xAOD::CaloCluster::ENG_CALIB_TOT, cluster_ENG_CALIB_TOT) ) cluster_ENG_CALIB_TOT=-1.;
      else cluster_ENG_CALIB_TOT*=1e-3;
      if( !cluster->retrieveMoment( xAOD::CaloCluster::ENG_CALIB_OUT_T, cluster_ENG_CALIB_OUT_T) ) cluster_ENG_CALIB_OUT_T=-1.;
      else cluster_ENG_CALIB_OUT_T*=1e-3;
      if( !cluster->retrieveMoment( xAOD::CaloCluster::ENG_CALIB_DEAD_TOT, cluster_ENG_CALIB_DEAD_TOT) ) cluster_ENG_CALIB_DEAD_TOT=-1.;
      else cluster_ENG_CALIB_DEAD_TOT*=1e-3;

      if( !cluster->retrieveMoment( xAOD::CaloCluster::CENTER_MAG, cluster_CENTER_MAG) ) cluster_CENTER_MAG=-1.;
      if( !cluster->retrieveMoment( xAOD::CaloCluster::FIRST_ENG_DENS, cluster_FIRST_ENG_DENS) ) cluster_FIRST_ENG_DENS=-1.;
      else cluster_FIRST_ENG_DENS*=1e-3;

      //for moments related to the calibration, use calibratedCluster or they will be undefined
      if( !calibratedCluster->retrieveMoment( xAOD::CaloCluster::EM_PROBABILITY, cluster_EM_PROBABILITY) ) cluster_EM_PROBABILITY = -1.;
      if( !calibratedCluster->retrieveMoment( xAOD::CaloCluster::HAD_WEIGHT, cluster_HAD_WEIGHT) ) cluster_HAD_WEIGHT=-1.;
      if( !calibratedCluster->retrieveMoment( xAOD::CaloCluster::OOC_WEIGHT, cluster_OOC_WEIGHT) ) cluster_OOC_WEIGHT=-1.;
      if( !calibratedCluster->retrieveMoment( xAOD::CaloCluster::DM_WEIGHT, cluster_DM_WEIGHT) ) cluster_DM_WEIGHT=-1.;
    }




    if (m_doEventTree) {
      m_clusterE.push_back(clusterE);
      m_clusterPt.push_back(clusterPt);
      m_clusterEta.push_back(clusterEta);
      m_clusterPhi.push_back(clusterPhi);
    }

    float dR_min = 1e8;
    float dR_max = -1;
    float dEta_min = 1e8;
    float dEta_max = 1e8;
    float dPhi_min = 1e8;
    float dPhi_max = 1e8;
    float centerCellEta = 1e8;
    float centerCellPhi = 1e8;
    CaloCell_ID::CaloSample centerCellLayer=CaloCell_ID::Unknown;

    int nCells = 0;
    float sumCellE = 0;
    float sumWeights=0;
    float sumCellE_unweighted=0;
    bool fillCellValidation = false;

    // Figure out which cell is at the center if the cluster and fill some validation plots
    CaloClusterCellLink::const_iterator it_cell = cluster->cell_begin();
    CaloClusterCellLink::const_iterator it_cell_end = cluster->cell_end();
    for(; it_cell != it_cell_end; it_cell++){
      const CaloCell* cell = (*it_cell);
      if (!cell->caloDDE()) continue;

      float dEta = cell->eta() - clusterEta;
      float dPhi = cell->phi() - clusterPhi;
      // if (fabs(dPhi) > TMath::Pi()) dPhi = 2*TMath::Pi()-fabs(dPhi);
      if (m_doEventTree) {
        m_cluster_cell_dEta.push_back(dEta);
        m_cluster_cell_dPhi.push_back(dPhi);
      }

      float dR_tmp = sqrt(pow(dEta,2) + pow(dPhi,2));
      if (dR_min > dR_tmp) {
        dR_min = dR_tmp;
        dEta_min = dEta;
        dPhi_min = dPhi;
        centerCellEta = cell->eta();
        centerCellPhi = cell->phi();
        centerCellLayer = cell->caloDDE()->getSampling();
      };
      if (dR_max < dR_tmp) {
        dR_max = dR_tmp;
        dEta_max = dEta;
        dPhi_max = dPhi;
      };

      fillCellValidation = true;
      sumCellE += cell->e()*(it_cell.weight())/1e3;
      sumCellE_unweighted += cell->e()*1e-3;
      sumWeights += it_cell.weight();
      nCells++;
    }

    if (fillCellValidation && m_doEventTree) {
      m_cluster_nCells.push_back(nCells);
      m_cluster_sumCellE.push_back(sumCellE);
      m_cluster_cell_dR_min.push_back(dR_min);
      m_cluster_cell_dR_max.push_back(dR_max);
      m_cluster_cell_dEta_min.push_back(dEta_min);
      m_cluster_cell_dEta_max.push_back(dEta_max);
      m_cluster_cell_dPhi_min.push_back(dPhi_min);
      m_cluster_cell_dPhi_max.push_back(dPhi_max);

      m_cluster_cell_centerCellEta.push_back(centerCellEta);
      m_cluster_cell_centerCellPhi.push_back(centerCellPhi);
      m_cluster_cell_centerCellLayer.push_back((int)centerCellLayer);
    }

    if (m_doClusterTree) {

      // Clear images
      memset(m_EMB1, 0, sizeof(m_EMB1[0][0]) * 128 * 4);
      memset(m_EMB2, 0, sizeof(m_EMB2[0][0]) * 16 * 16);
      memset(m_EMB3, 0, sizeof(m_EMB3[0][0]) * 8 * 16);
      memset(m_TileBar0, 0, sizeof(m_TileBar0[0][0]) * 4 * 4);
      memset(m_TileBar1, 0, sizeof(m_TileBar1[0][0]) * 4 * 4);
      memset(m_TileBar2, 0, sizeof(m_TileBar2[0][0]) * 2 * 4);

      m_duplicate_EMB1 = 0;
      m_duplicate_EMB2 = 0;
      m_duplicate_EMB3 = 0;
      m_duplicate_TileBar0 = 0;
      m_duplicate_TileBar1 = 0;
      m_duplicate_TileBar2 = 0;

      m_cluster_cellE_norm.clear();

      // Fill images 
      int iEta = 0;
      int iPhi = 0;
      float cellSizeEta[] = {0.0031, 0.0250, 0.0500, 0.1, 0.1, 0.2};
      float cellSizePhi[] = {0.0980, 0.0245, 0.0245, 0.1, 0.1, 0.1};
      it_cell = cluster->cell_begin();
      it_cell_end = cluster->cell_end();
      //std::cout << "------------------------" << std::endl;
      //std::cout << "centerCellEta " << centerCellEta << std::endl; 
      //std::cout << "centerCellPhi " << centerCellPhi << std::endl; 
      int cell_i = 0;
      float sumCellE_i = 0.;
      for(; it_cell != it_cell_end; it_cell++){
        const CaloCell* cell = (*it_cell);
        if (!cell->caloDDE()) continue;

        float dEta = cell->eta() - centerCellEta;
        float dPhi = cell->phi() - centerCellPhi;
        float cellE = cell->e()*(it_cell.weight())/1e3;
        float cellE_norm = cellE/clusterE;

        // noise rejection
        if (cellE < m_cellE_thres) continue;
        if (cellE_norm < 0) continue;

        m_cluster_cellE_norm.push_back(cellE_norm);
        //std::cout << "cell #: " << cell_i << std::endl; 
        //std::cout << "cellE_norm: " << cellE_norm << std::endl; 
        //std::cout << "cell->eta() " << cell->eta() << std::endl; 
        //std::cout << "cell->phi() " << cell->phi() << std::endl; 
        //std::cout << "dEta: " << dEta << std::endl; 
        //std::cout << "dPhi: " << dPhi << std::endl; 

        if (fabs(dEta) > 0.2 || fabs(dPhi) > 0.2) continue;

        cell_i++;
        sumCellE_i += cellE;

        // Ugly, but will do for now
        CaloCell_ID::CaloSample cellLayer = cell->caloDDE()->getSampling();
        if (cellLayer == CaloCell_ID::CaloSample::EMB1) {
          iEta = floor(dEta/cellSizeEta[0]+0.1); //+0.1 to avoid floating point errors
          iPhi = floor(dPhi/cellSizePhi[0]+0.1); 
          if (m_EMB1[iEta+64][iPhi+2] != 0) m_duplicate_EMB1++; // check for duplicates
          if (iEta < 128 && iPhi < 4) m_EMB1[iEta+64][iPhi+2] += cellE_norm;
          //std::cout << "cellLayer: " << (int)cellLayer << std::endl; 
          //std::cout << "iEta: " << iEta << std::endl; 
          //std::cout << "iPhi: " << iPhi << std::endl; 
        } else if (cellLayer == CaloCell_ID::CaloSample::EMB2) {
          iEta = floor(dEta/cellSizeEta[1]+0.1); 
          iPhi = floor(dPhi/cellSizePhi[1]+0.1); 
          if (m_EMB2[iEta+8][iPhi+8] != 0) m_duplicate_EMB2++; // check for duplicates
          if (iEta < 16 && iPhi < 16) m_EMB2[iEta+8][iPhi+8] += cellE_norm;
          //std::cout << "cellLayer: " << (int)cellLayer << std::endl; 
          //std::cout << "iEta: " << iEta << std::endl; 
          //std::cout << "iPhi: " << iPhi << std::endl; 
        } else if (cellLayer == CaloCell_ID::CaloSample::EMB3) {
          iEta = floor(dEta/cellSizeEta[2]+0.1); 
          iPhi = floor(dPhi/cellSizePhi[2]+0.1); 
          if (m_EMB2[iEta+4][iPhi+8] != 0) m_duplicate_EMB3++; // check for duplicates
          if (iEta < 8 && iPhi < 16) m_EMB3[iEta+4][iPhi+8] += cellE_norm;
          //std::cout << "cellLayer: " << (int)cellLayer << std::endl; 
          //std::cout << "iEta: " << iEta << std::endl; 
          //std::cout << "iPhi: " << iPhi << std::endl; 
        } else if (cellLayer == CaloCell_ID::CaloSample::TileBar0) {
          iEta = floor(dEta/cellSizeEta[3]+0.1); 
          iPhi = floor(dPhi/cellSizePhi[3]+0.1); 
          if (m_TileBar0[iEta+2][iPhi+2] != 0) m_duplicate_TileBar0++; // check for duplicates
          if (iEta < 4 && iPhi < 4) m_TileBar0[iEta+2][iPhi+2] += cellE_norm;
          // std::cout << "cellLayer: " << (int)cellLayer << std::endl; 
          // std::cout << "iEta: " << iEta << std::endl; 
          // std::cout << "iPhi: " << iPhi << std::endl; 
        } else if (cellLayer == CaloCell_ID::CaloSample::TileBar1) {
          iEta = floor(dEta/cellSizeEta[4]+0.1); 
          iPhi = floor(dPhi/cellSizePhi[4]+0.1); 
          if (m_TileBar1[iEta+2][iPhi+2] != 0) m_duplicate_TileBar1++; // check for duplicates
          if (iEta < 4 && iPhi < 4) m_TileBar1[iEta+2][iPhi+2] += cellE_norm;
          // std::cout << "cellLayer: " << (int)cellLayer << std::endl; 
          // std::cout << "iEta: " << iEta << std::endl; 
          // std::cout << "iPhi: " << iPhi << std::endl; 
        } else if (cellLayer == CaloCell_ID::CaloSample::TileBar2) {
          iEta = floor(dEta/cellSizeEta[5]+0.1); 
          iPhi = floor(dPhi/cellSizePhi[5]+0.1); 
          if (m_TileBar2[iEta+1][iPhi+2] != 0) m_duplicate_TileBar2++; // check for duplicates
          if (iEta < 2 && iPhi < 4) m_TileBar2[iEta+1][iPhi+2] += cellE_norm;
          // std::cout << "cellLayer: " << (int)cellLayer << std::endl; 
          // std::cout << "iEta: " << iEta << std::endl; 
          // std::cout << "iPhi: " << iPhi << std::endl; 
        }
      }

      //std::cout << "cell_i: " << cell_i << std::endl; 
      //std::cout << "sumCellE_i: " << sumCellE_i << std::endl; 
      //std::cout << "clusterE: " << clusterE << std::endl; 
      m_fClusterIndex = jCluster;
      jCluster++;


      m_fClusterE = clusterE;
      m_fClusterECalib = calibratedCluster->e()*1e-3;
      m_fClusterPt = clusterPt;
      m_fClusterEta = clusterEta;
      m_fClusterPhi = clusterPhi; 
      m_fCluster_nCells = cell_i;
      m_fCluster_sumCellE = sumCellE_i;


      if(m_doClusterMoments)
      {
	m_fCluster_ENG_CALIB_TOT=cluster_ENG_CALIB_TOT;
	m_fCluster_ENG_CALIB_OUT_T=cluster_ENG_CALIB_OUT_T;
	m_fCluster_ENG_CALIB_DEAD_TOT=cluster_ENG_CALIB_DEAD_TOT;
	
	m_fCluster_EM_PROBABILITY = cluster_EM_PROBABILITY;
	m_fCluster_HAD_WEIGHT=cluster_HAD_WEIGHT;
	m_fCluster_OOC_WEIGHT=cluster_OOC_WEIGHT;
	m_fCluster_DM_WEIGHT=cluster_DM_WEIGHT;
	m_fCluster_CENTER_MAG=cluster_CENTER_MAG;
	m_fCluster_FIRST_ENG_DENS=cluster_FIRST_ENG_DENS*1e-3;
      }

      m_fCluster_cell_dR_min = dR_min;
      m_fCluster_cell_dR_max = dR_max;
      m_fCluster_cell_dEta_min = dEta_min;
      m_fCluster_cell_dEta_max = dEta_max;
      m_fCluster_cell_dPhi_min = dPhi_min;
      m_fCluster_cell_dPhi_max = dPhi_max;

      m_fCluster_cell_centerCellEta = centerCellEta;
      m_fCluster_cell_centerCellPhi = centerCellPhi;
      m_fCluster_cell_centerCellLayer = (int)centerCellLayer;

      //if (m_duplicate_EMB1 == 0 && m_duplicate_EMB2 == 0 && m_duplicate_EMB3 == 0 &&
      //    m_duplicate_TileBar0 == 0 && m_duplicate_TileBar1 == 0 && m_duplicate_TileBar2 == 0) {

      m_clusterTree->Fill();
      //}
    }
  }

  if (m_doEventTree) m_eventTree->Fill();

  return StatusCode::SUCCESS;
}

StatusCode MLTreeMaker::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}
