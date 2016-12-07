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

MLTreeMaker::MLTreeMaker( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthHistogramAlgorithm( name, pSvcLocator ),
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
  m_tileTBID(0)
{
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

  // Setup TTree and its branches
  CHECK( book(TTree("MLtree", "MLtree")) );
  m_tree = tree("MLtree");

  // Event info 
  m_tree->Branch("runNumber",             &m_runNumber,      "runNumber/I");
  m_tree->Branch("eventNumber",           &m_eventNumber,    "eventNumber/LI");
  m_tree->Branch("lumiBlock",             &m_lumiBlock,      "lumiBlock/I");
  m_tree->Branch("coreFlags",             &m_coreFlags,      "coreFlags/i");
  // if (m_isMC ) {
    m_tree->Branch("mcEventNumber",       &m_mcEventNumber,  "mcEventNumber/I");
    m_tree->Branch("mcChannelNumber",     &m_mcChannelNumber,"mcChannelNumber/I");
    m_tree->Branch("mcEventWeight",       &m_mcEventWeight,  "mcEventWeight/F");
  // } else {
  //   m_tree->Branch("bcid",                &m_bcid,           "bcid/I");
  //   m_tree->Branch("prescale_DataWeight", &m_prescale_DataWeight,  "prescale_DataWeight/F");
  // }
  if (m_doEventCleaning ) {
    m_tree->Branch("timeStamp",           &m_timeStamp,         "timeStamp/i");
    m_tree->Branch("timeStampNSOffset",   &m_timeStampNSOffset, "timeStampNSOffset/i");
    m_tree->Branch("TileError",           &m_TileError,         "TileError/O");
    m_tree->Branch("SCTError",            &m_SCTError,          "SCTError/O");
    m_tree->Branch("LArError",            &m_LArError,          "LArError/O");
    m_tree->Branch("TileFlags",           &m_TileFlags,         "TileFlags/i");
    m_tree->Branch("SCTFlags",            &m_SCTFlags,          "SCTFlags/i");
    m_tree->Branch("LArFlags",            &m_LArFlags,          "LArFlags/i");
  }
  if (m_doPileup ) {
    m_tree->Branch("NPV",                 &m_npv,            "NPV/I");
    m_tree->Branch("actualInteractionsPerCrossing",  &m_actualMu,  "actualInteractionsPerCrossing/F");
    m_tree->Branch("averageInteractionsPerCrossing", &m_averageMu, "averageInteractionsPerCrossing/F");
    m_tree->Branch("weight_pileup",       &m_weight_pileup,  "weight_pileup/F");
    // if (m_isMC){
      m_tree->Branch("correct_mu"       , &m_correct_mu       ,"correct_mu/F"       );          
      m_tree->Branch("rand_run_nr"      , &m_rand_run_nr      ,"rand_run_nr/I"      );         
      m_tree->Branch("rand_lumiblock_nr", &m_rand_lumiblock_nr,"rand_lumiblock_nr/I");  
    // }
  }
  if (m_doShapeEM ) {
    m_tree->Branch("rhoEM",               &m_rhoEM,            "rhoEM/D");
  }
  if (m_doShapeLC ) {
    m_tree->Branch("rhoLC",               &m_rhoLC,            "rhoLC/D");
  }
  if (m_doEventTruth /*&& m_isMC */ ) {
    m_tree->Branch("pdgId1",              &m_pdgId1,        "pdgId1/I" );
    m_tree->Branch("pdgId2",              &m_pdgId2,        "pdgId2/I" );
    m_tree->Branch("pdfId1",              &m_pdfId1,        "pdfId1/I" );
    m_tree->Branch("pdfId2",              &m_pdfId2,        "pdfId2/I" );
    m_tree->Branch("x1",                  &m_x1,            "x1/F"  );
    m_tree->Branch("x2",                  &m_x2,            "x2/F"  );
    // m_tree->Branch("scale",               &m_scale,         "scale/F");
    // m_tree->Branch("q",                   &m_q,             "q/F");
    // m_tree->Branch("pdf1",                &m_pdf1,          "pdf1/F");
    // m_tree->Branch("pdf2",                &m_pdf2,          "pdf2/F");
    m_tree->Branch("xf1",                 &m_xf1,           "xf1/F");
    m_tree->Branch("xf2",                 &m_xf2,           "xf2/F");
  } 

  // Truth particles
  m_tree->Branch("pdgId",               &m_pdgId);
  m_tree->Branch("status",              &m_status);
  m_tree->Branch("barcode",             &m_barcode);
  m_tree->Branch("truthPartPt",         &m_truthPartPt);
  m_tree->Branch("truthPartMass",       &m_truthPartMass);
  m_tree->Branch("truthPartEta",        &m_truthPartEta);
  m_tree->Branch("truthPartPhi",        &m_truthPartPhi);

  // Track variables
  m_tree->Branch("trkPt",               &m_trkPt);
  m_tree->Branch("trkP",                &m_trkP);
  m_tree->Branch("trkMass",             &m_trkMass);
  m_tree->Branch("trkEta",              &m_trkEta);
  m_tree->Branch("trkPhi",              &m_trkPhi);

  // Track extrapolation
  // Presampler
  m_tree->Branch("trkEta_PreSamplerB",  &m_trkEta_PreSamplerB);
  m_tree->Branch("trkPhi_PreSamplerB",  &m_trkPhi_PreSamplerB);
  m_tree->Branch("trkEta_PreSamplerE",  &m_trkEta_PreSamplerE);
  m_tree->Branch("trkPhi_PreSamplerE",  &m_trkPhi_PreSamplerE);
  // LAr EM Barrel layers
  m_tree->Branch("trkEta_EMB1",         &m_trkEta_EMB1); 
  m_tree->Branch("trkPhi_EMB1",         &m_trkPhi_EMB1); 
  m_tree->Branch("trkEta_EMB2",         &m_trkEta_EMB2); 
  m_tree->Branch("trkPhi_EMB2",         &m_trkPhi_EMB2); 
  m_tree->Branch("trkEta_EMB3",         &m_trkEta_EMB3); 
  m_tree->Branch("trkPhi_EMB3",         &m_trkPhi_EMB3); 
  // LAr EM Endcap layers
  m_tree->Branch("trkEta_EME1",         &m_trkEta_EME1); 
  m_tree->Branch("trkPhi_EME1",         &m_trkPhi_EME1); 
  m_tree->Branch("trkEta_EME2",         &m_trkEta_EME2); 
  m_tree->Branch("trkPhi_EME2",         &m_trkPhi_EME2); 
  m_tree->Branch("trkEta_EME3",         &m_trkEta_EME3); 
  m_tree->Branch("trkPhi_EME3",         &m_trkPhi_EME3); 
  // Hadronic Endcap layers
  m_tree->Branch("trkEta_HEC0",         &m_trkEta_HEC0); 
  m_tree->Branch("trkPhi_HEC0",         &m_trkPhi_HEC0); 
  m_tree->Branch("trkEta_HEC1",         &m_trkEta_HEC1); 
  m_tree->Branch("trkPhi_HEC1",         &m_trkPhi_HEC1); 
  m_tree->Branch("trkEta_HEC2",         &m_trkEta_HEC2); 
  m_tree->Branch("trkPhi_HEC2",         &m_trkPhi_HEC2); 
  m_tree->Branch("trkEta_HEC3",         &m_trkEta_HEC3); 
  m_tree->Branch("trkPhi_HEC3",         &m_trkPhi_HEC3); 
  // Tile Barrel layers
  m_tree->Branch("trkEta_TileBar0",     &m_trkEta_TileBar0); 
  m_tree->Branch("trkPhi_TileBar0",     &m_trkPhi_TileBar0); 
  m_tree->Branch("trkEta_TileBar1",     &m_trkEta_TileBar1); 
  m_tree->Branch("trkPhi_TileBar1",     &m_trkPhi_TileBar1); 
  m_tree->Branch("trkEta_TileBar2",     &m_trkEta_TileBar2); 
  m_tree->Branch("trkPhi_TileBar2",     &m_trkPhi_TileBar2); 
  // Tile Gap layers
  m_tree->Branch("trkEta_TileGap1",     &m_trkEta_TileGap1); 
  m_tree->Branch("trkPhi_TileGap1",     &m_trkPhi_TileGap1); 
  m_tree->Branch("trkEta_TileGap2",     &m_trkEta_TileGap2); 
  m_tree->Branch("trkPhi_TileGap2",     &m_trkPhi_TileGap2); 
  m_tree->Branch("trkEta_TileGap3",     &m_trkEta_TileGap3); 
  m_tree->Branch("trkPhi_TileGap3",     &m_trkPhi_TileGap3); 
  // Tile Extended Barrel layers
  m_tree->Branch("trkEta_TileExt0",     &m_trkEta_TileExt0);
  m_tree->Branch("trkPhi_TileExt0",     &m_trkPhi_TileExt0);
  m_tree->Branch("trkEta_TileExt1",     &m_trkEta_TileExt1);
  m_tree->Branch("trkPhi_TileExt1",     &m_trkPhi_TileExt1);
  m_tree->Branch("trkEta_TileExt2",     &m_trkEta_TileExt2);
  m_tree->Branch("trkPhi_TileExt2",     &m_trkPhi_TileExt2);

  // Clusters 
  m_tree->Branch("clusE",               &m_clusE);
  m_tree->Branch("clusPt",              &m_clusPt);
  m_tree->Branch("clusEta",             &m_clusEta);
  m_tree->Branch("clusPhi",             &m_clusPhi);

  // Cells
  m_tree->Branch("cellE",               &m_cellE);
  m_tree->Branch("cellEta",             &m_cellEta);
  m_tree->Branch("cellPhi",             &m_cellPhi);
  m_tree->Branch("cellR",               &m_cellR);

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
  m_truthPartMass.clear();
  m_truthPartEta.clear();
  m_truthPartPhi.clear();

  m_trkPt.clear();
  m_trkP.clear();
  m_trkMass.clear();
  m_trkEta.clear();
  m_trkPhi.clear();

  m_trkEta_PreSamplerB.clear();
  m_trkPhi_PreSamplerB.clear();
  m_trkEta_PreSamplerE.clear();
  m_trkPhi_PreSamplerE.clear();

  m_trkEta_EMB1.clear(); 
  m_trkPhi_EMB1.clear(); 
  m_trkEta_EMB2.clear(); 
  m_trkPhi_EMB2.clear(); 
  m_trkEta_EMB3.clear(); 
  m_trkPhi_EMB3.clear(); 

  m_trkEta_EME1.clear(); 
  m_trkPhi_EME1.clear(); 
  m_trkEta_EME2.clear(); 
  m_trkPhi_EME2.clear(); 
  m_trkEta_EME3.clear(); 
  m_trkPhi_EME3.clear(); 

  m_trkEta_HEC0.clear(); 
  m_trkPhi_HEC0.clear(); 
  m_trkEta_HEC1.clear(); 
  m_trkPhi_HEC1.clear(); 
  m_trkEta_HEC2.clear(); 
  m_trkPhi_HEC2.clear(); 
  m_trkEta_HEC3.clear(); 
  m_trkPhi_HEC3.clear(); 

  m_trkEta_TileBar0.clear(); 
  m_trkPhi_TileBar0.clear(); 
  m_trkEta_TileBar1.clear(); 
  m_trkPhi_TileBar1.clear(); 
  m_trkEta_TileBar2.clear(); 
  m_trkPhi_TileBar2.clear(); 

  m_trkEta_TileGap1.clear(); 
  m_trkPhi_TileGap1.clear(); 
  m_trkEta_TileGap2.clear(); 
  m_trkPhi_TileGap2.clear(); 
  m_trkEta_TileGap3.clear(); 
  m_trkPhi_TileGap3.clear(); 

  m_trkEta_TileExt0.clear();
  m_trkPhi_TileExt0.clear();
  m_trkEta_TileExt1.clear();
  m_trkPhi_TileExt1.clear();
  m_trkEta_TileExt2.clear();
  m_trkPhi_TileExt2.clear();

  m_clusE.clear();
  m_clusPt.clear();
  m_clusEta.clear();
  m_clusPhi.clear();

  m_cellE.clear();
  m_cellEta.clear();
  m_cellPhi.clear();
  m_cellR.clear();

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

  // Truth particles
  const xAOD::TruthParticleContainer* truthContainer = 0;
  CHECK(evtStore()->retrieve(truthContainer, m_truthContainerName));
  for (const auto& truth: *truthContainer) {

    m_pdgId.push_back(truth->pdgId());
    m_status.push_back(truth->status());
    m_barcode.push_back(truth->barcode());
    m_truthPartPt.push_back(truth->pt()/1e3);
    m_truthPartMass.push_back(truth->m()/1e3);
    m_truthPartEta.push_back(truth->eta());
    m_truthPartPhi.push_back(truth->phi());
  }

  // Tracks
  const xAOD::TrackParticleContainer* trackContainer = 0;
  CHECK(evtStore()->retrieve(trackContainer, m_trackContainerName));

  for (const auto& track : *trackContainer) {

    m_trkPt.push_back(track->pt()/1e3);
    m_trkP.push_back(TMath::Abs(1./track->qOverP())/1e3);
    m_trkMass.push_back(track->m()/1e3);
    m_trkEta.push_back(track->eta());
    m_trkPhi.push_back(track->phi());

    // A map to store the track parameters associated with the different layers of the calorimeter system
    std::map<CaloCell_ID::CaloSample, const Trk::TrackParameters*> parametersMap;

    // Get the CaloExtension object
    const Trk::CaloExtension* extension = 0;

    if (m_theTrackExtrapolatorTool->caloExtension(*track, extension)) {

      // Extract the CurvilinearParameters per each layer-track intersection
      const std::vector<const Trk::CurvilinearParameters*>& clParametersVector = extension->caloLayerIntersections();

      for (auto clParameter : clParametersVector) {

        unsigned int parametersIdentifier = clParameter->cIdentifier();
        CaloCell_ID::CaloSample intLayer;

        if (!m_trackParametersIdHelper->isValid(parametersIdentifier)) {
          std::cout << "invalid Track Identifier"<< std::endl;
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

    } else {
      msg(MSG::WARNING) << "TrackExtension failed for track with pt and eta " << track->pt() << " and " << track->eta() << endreq;
    }

    if (!(m_theTrackExtrapolatorTool->caloExtension(*track, extension))) continue; //No valid parameters for any of the layers of interest

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
    float trkEta_PreSamplerB_tmp = -999999999;
    float trkPhi_PreSamplerB_tmp = -999999999;
    float trkEta_PreSamplerE_tmp = -999999999;
    float trkPhi_PreSamplerE_tmp = -999999999;
    // LAr EM Barrel layers
    float trkEta_EMB1_tmp = -999999999;
    float trkPhi_EMB1_tmp = -999999999;
    float trkEta_EMB2_tmp = -999999999;
    float trkPhi_EMB2_tmp = -999999999;
    float trkEta_EMB3_tmp = -999999999;
    float trkPhi_EMB3_tmp = -999999999;
    // LAr EM Endcap layers
    float trkEta_EME1_tmp = -999999999;
    float trkPhi_EME1_tmp = -999999999;
    float trkEta_EME2_tmp = -999999999;
    float trkPhi_EME2_tmp = -999999999;
    float trkEta_EME3_tmp = -999999999;
    float trkPhi_EME3_tmp = -999999999;
    // Hadronic Endcap layers
    float trkEta_HEC0_tmp = -999999999;
    float trkPhi_HEC0_tmp = -999999999;
    float trkEta_HEC1_tmp = -999999999;
    float trkPhi_HEC1_tmp = -999999999;
    float trkEta_HEC2_tmp = -999999999;
    float trkPhi_HEC2_tmp = -999999999;
    float trkEta_HEC3_tmp = -999999999;
    float trkPhi_HEC3_tmp = -999999999;
    // Tile Barrel layers
    float trkEta_TileBar0_tmp = -999999999;
    float trkPhi_TileBar0_tmp = -999999999;
    float trkEta_TileBar1_tmp = -999999999;
    float trkPhi_TileBar1_tmp = -999999999;
    float trkEta_TileBar2_tmp = -999999999;
    float trkPhi_TileBar2_tmp = -999999999;
    // Tile Gap layers
    float trkEta_TileGap1_tmp = -999999999;
    float trkPhi_TileGap1_tmp = -999999999;
    float trkEta_TileGap2_tmp = -999999999;
    float trkPhi_TileGap2_tmp = -999999999;
    float trkEta_TileGap3_tmp = -999999999;
    float trkPhi_TileGap3_tmp = -999999999;
    // Tile Extended Barrel layers
    float trkEta_TileExt0_tmp = -999999999;
    float trkPhi_TileExt0_tmp = -999999999;
    float trkEta_TileExt1_tmp = -999999999;
    float trkPhi_TileExt1_tmp = -999999999;
    float trkEta_TileExt2_tmp = -999999999;
    float trkPhi_TileExt2_tmp = -999999999;

    if (parametersMap[CaloCell_ID::CaloSample::PreSamplerB]) trkEta_PreSamplerB_tmp = parametersMap[CaloCell_ID::CaloSample::PreSamplerB]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::PreSamplerB]) trkPhi_PreSamplerB_tmp = parametersMap[CaloCell_ID::CaloSample::PreSamplerB]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::PreSamplerE]) trkEta_PreSamplerE_tmp = parametersMap[CaloCell_ID::CaloSample::PreSamplerE]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::PreSamplerE]) trkPhi_PreSamplerE_tmp = parametersMap[CaloCell_ID::CaloSample::PreSamplerE]->momentum().phi();

    if (parametersMap[CaloCell_ID::CaloSample::EMB1]) trkEta_EMB1_tmp = parametersMap[CaloCell_ID::CaloSample::EMB1]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::EMB1]) trkPhi_EMB1_tmp = parametersMap[CaloCell_ID::CaloSample::EMB1]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::EMB2]) trkEta_EMB2_tmp = parametersMap[CaloCell_ID::CaloSample::EMB2]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::EMB2]) trkPhi_EMB2_tmp = parametersMap[CaloCell_ID::CaloSample::EMB2]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::EMB3]) trkEta_EMB3_tmp = parametersMap[CaloCell_ID::CaloSample::EMB3]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::EMB3]) trkPhi_EMB3_tmp = parametersMap[CaloCell_ID::CaloSample::EMB3]->momentum().phi();

    if (parametersMap[CaloCell_ID::CaloSample::EME1]) trkEta_EME1_tmp = parametersMap[CaloCell_ID::CaloSample::EME1]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::EME1]) trkPhi_EME1_tmp = parametersMap[CaloCell_ID::CaloSample::EME1]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::EME2]) trkEta_EME2_tmp = parametersMap[CaloCell_ID::CaloSample::EME2]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::EME2]) trkPhi_EME2_tmp = parametersMap[CaloCell_ID::CaloSample::EME2]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::EME3]) trkEta_EME3_tmp = parametersMap[CaloCell_ID::CaloSample::EME3]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::EME3]) trkPhi_EME3_tmp = parametersMap[CaloCell_ID::CaloSample::EME3]->momentum().phi();

    if (parametersMap[CaloCell_ID::CaloSample::HEC0]) trkEta_HEC0_tmp = parametersMap[CaloCell_ID::CaloSample::HEC0]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::HEC0]) trkPhi_HEC0_tmp = parametersMap[CaloCell_ID::CaloSample::HEC0]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::HEC1]) trkEta_HEC1_tmp = parametersMap[CaloCell_ID::CaloSample::HEC1]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::HEC1]) trkPhi_HEC1_tmp = parametersMap[CaloCell_ID::CaloSample::HEC1]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::HEC2]) trkEta_HEC2_tmp = parametersMap[CaloCell_ID::CaloSample::HEC2]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::HEC2]) trkPhi_HEC2_tmp = parametersMap[CaloCell_ID::CaloSample::HEC2]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::HEC3]) trkEta_HEC3_tmp = parametersMap[CaloCell_ID::CaloSample::HEC3]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::HEC3]) trkPhi_HEC3_tmp = parametersMap[CaloCell_ID::CaloSample::HEC3]->momentum().phi();

    if (parametersMap[CaloCell_ID::CaloSample::TileBar0]) trkEta_TileBar0_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar0]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileBar0]) trkPhi_TileBar0_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar0]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::TileBar1]) trkEta_TileBar1_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar1]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileBar1]) trkPhi_TileBar1_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar1]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::TileBar2]) trkEta_TileBar2_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar2]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileBar2]) trkPhi_TileBar2_tmp = parametersMap[CaloCell_ID::CaloSample::TileBar2]->momentum().phi();

    if (parametersMap[CaloCell_ID::CaloSample::TileGap1]) trkEta_TileGap1_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap1]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileGap1]) trkPhi_TileGap1_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap1]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::TileGap2]) trkEta_TileGap2_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap2]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileGap2]) trkPhi_TileGap2_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap2]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::TileGap3]) trkEta_TileGap3_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap3]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileGap3]) trkPhi_TileGap3_tmp = parametersMap[CaloCell_ID::CaloSample::TileGap3]->momentum().phi();

    if (parametersMap[CaloCell_ID::CaloSample::TileExt0]) trkEta_TileExt0_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt0]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileExt0]) trkPhi_TileExt0_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt0]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::TileExt1]) trkEta_TileExt1_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt1]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileExt1]) trkPhi_TileExt1_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt1]->momentum().phi();
    if (parametersMap[CaloCell_ID::CaloSample::TileExt2]) trkEta_TileExt2_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt2]->momentum().eta();
    if (parametersMap[CaloCell_ID::CaloSample::TileExt2]) trkPhi_TileExt2_tmp = parametersMap[CaloCell_ID::CaloSample::TileExt2]->momentum().phi();

    m_trkEta_PreSamplerB.push_back(trkEta_PreSamplerB_tmp);
    m_trkPhi_PreSamplerB.push_back(trkPhi_PreSamplerB_tmp);
    m_trkEta_PreSamplerE.push_back(trkEta_PreSamplerE_tmp);
    m_trkPhi_PreSamplerE.push_back(trkPhi_PreSamplerE_tmp);

    m_trkEta_EMB1.push_back(trkEta_EMB1_tmp); 
    m_trkPhi_EMB1.push_back(trkPhi_EMB1_tmp); 
    m_trkEta_EMB2.push_back(trkEta_EMB2_tmp); 
    m_trkPhi_EMB2.push_back(trkPhi_EMB2_tmp); 
    m_trkEta_EMB3.push_back(trkEta_EMB3_tmp); 
    m_trkPhi_EMB3.push_back(trkPhi_EMB3_tmp); 

    m_trkEta_EME1.push_back(trkEta_EME1_tmp); 
    m_trkPhi_EME1.push_back(trkPhi_EME1_tmp); 
    m_trkEta_EME2.push_back(trkEta_EME2_tmp); 
    m_trkPhi_EME2.push_back(trkPhi_EME2_tmp); 
    m_trkEta_EME3.push_back(trkEta_EME3_tmp); 
    m_trkPhi_EME3.push_back(trkPhi_EME3_tmp); 

    m_trkEta_HEC0.push_back(trkEta_HEC0_tmp); 
    m_trkPhi_HEC0.push_back(trkPhi_HEC0_tmp); 
    m_trkEta_HEC1.push_back(trkEta_HEC1_tmp); 
    m_trkPhi_HEC1.push_back(trkPhi_HEC1_tmp); 
    m_trkEta_HEC2.push_back(trkEta_HEC2_tmp); 
    m_trkPhi_HEC2.push_back(trkPhi_HEC2_tmp); 
    m_trkEta_HEC3.push_back(trkEta_HEC3_tmp); 
    m_trkPhi_HEC3.push_back(trkPhi_HEC3_tmp); 

    m_trkEta_TileBar0.push_back(trkEta_TileBar0_tmp); 
    m_trkPhi_TileBar0.push_back(trkPhi_TileBar0_tmp); 
    m_trkEta_TileBar1.push_back(trkEta_TileBar1_tmp); 
    m_trkPhi_TileBar1.push_back(trkPhi_TileBar1_tmp); 
    m_trkEta_TileBar2.push_back(trkEta_TileBar2_tmp); 
    m_trkPhi_TileBar2.push_back(trkPhi_TileBar2_tmp); 

    m_trkEta_TileGap1.push_back(trkEta_TileGap1_tmp); 
    m_trkPhi_TileGap1.push_back(trkPhi_TileGap1_tmp); 
    m_trkEta_TileGap2.push_back(trkEta_TileGap2_tmp); 
    m_trkPhi_TileGap2.push_back(trkPhi_TileGap2_tmp); 
    m_trkEta_TileGap3.push_back(trkEta_TileGap3_tmp); 
    m_trkPhi_TileGap3.push_back(trkPhi_TileGap3_tmp); 

    m_trkEta_TileExt0.push_back(trkEta_TileExt0_tmp);
    m_trkPhi_TileExt0.push_back(trkPhi_TileExt0_tmp);
    m_trkEta_TileExt1.push_back(trkEta_TileExt1_tmp);
    m_trkPhi_TileExt1.push_back(trkPhi_TileExt1_tmp);
    m_trkEta_TileExt2.push_back(trkEta_TileExt2_tmp);
    m_trkPhi_TileExt2.push_back(trkPhi_TileExt2_tmp);

  }

  // Calo clusters
  const xAOD::CaloClusterContainer* clusterContainer = 0; 
  CHECK(evtStore()->retrieve(clusterContainer, m_caloClusterContainerName));

  for (const auto& cluster : *clusterContainer) {

    m_clusE.push_back(cluster->e()/1e3);
    m_clusPt.push_back(cluster->pt()/1e3);
    m_clusEta.push_back(cluster->eta());
    m_clusPhi.push_back(cluster->phi());

  }

  // Calo cells
  const CaloCellContainer *caloCellContainer = 0;
  CHECK(evtStore()->retrieve(caloCellContainer, "AllCalo"));

  for (const auto& cell : *caloCellContainer) {

    // Calorimeter/CaloDetDescr/CaloDetDescr/CaloDetDescrElement.h
    if (!cell->caloDDE()) continue;

    m_cellE.push_back(cell->e()/1e3);
    m_cellPt.push_back(cell->pt()/1e3);
    m_cellEta.push_back(cell->caloDDE()->eta());
    m_cellPhi.push_back(cell->caloDDE()->phi());
    m_cellR.push_back(cell->caloDDE()->r());

  }

  m_tree->Fill();

  return StatusCode::SUCCESS;
}

StatusCode MLTreeMaker::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}
