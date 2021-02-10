#include "CellGeometryTreeMaker.h"
#include <CaloInterface/ICalorimeterNoiseTool.h>
#include <CaloEvent/CaloCellContainer.h>
#include <cmath>


CellGeometryTreeMaker::CellGeometryTreeMaker( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthHistogramAlgorithm( name, pSvcLocator ),
  m_noiseTool("CaloNoiseTool")
{
  declareProperty("CaloCellContainer", m_cellContainerKey="AllCalo");
}

CellGeometryTreeMaker::~CellGeometryTreeMaker() {}

StatusCode CellGeometryTreeMaker::initialize() 
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( book(TTree("CellGeo","CellGeo")));
  m_cellGeometryTree = tree("CellGeo");

  m_cellGeometryTree->Branch("cell_geo_ID",        &m_b_cell_geo_ID);
  m_cellGeometryTree->Branch("cell_geo_sampling",  &m_b_cell_geo_sampling);
  m_cellGeometryTree->Branch("cell_geo_eta",       &m_b_cell_geo_eta);
  m_cellGeometryTree->Branch("cell_geo_phi",       &m_b_cell_geo_phi);
  m_cellGeometryTree->Branch("cell_geo_rPerp",     &m_b_cell_geo_rPerp);
  m_cellGeometryTree->Branch("cell_geo_deta",      &m_b_cell_geo_deta);
  m_cellGeometryTree->Branch("cell_geo_dphi",      &m_b_cell_geo_dphi);
  m_cellGeometryTree->Branch("cell_geo_volume",    &m_b_cell_geo_volume);
  m_cellGeometryTree->Branch("cell_geo_sigma",     &m_b_cell_geo_sigma);


  ATH_CHECK(m_noiseTool.retrieve());
  ATH_MSG_INFO("Noise tool retrieved");

  return StatusCode::SUCCESS;
}

StatusCode CellGeometryTreeMaker::execute() 
{
  const CaloCellContainer* CellContainer = 0;
  CHECK(evtStore()->retrieve(CellContainer,m_cellContainerKey));
  //
  auto nCells=CellContainer->size();
  ATH_MSG_INFO("Number of cells " << nCells);

  if(m_cellGeometryTree->GetEntries() > 0) return StatusCode::SUCCESS;

  m_b_cell_geo_ID.clear();
  m_b_cell_geo_sampling.clear();
  m_b_cell_geo_eta.clear();
  m_b_cell_geo_phi.clear();
  m_b_cell_geo_rPerp.clear();
  m_b_cell_geo_deta.clear();
  m_b_cell_geo_dphi.clear();
  m_b_cell_geo_volume.clear();
  m_b_cell_geo_sigma.clear();

  m_b_cell_geo_ID.reserve(nCells);
  m_b_cell_geo_sampling.reserve(nCells);
  m_b_cell_geo_eta.reserve(nCells);
  m_b_cell_geo_phi.reserve(nCells);
  m_b_cell_geo_rPerp.reserve(nCells);
  m_b_cell_geo_deta.reserve(nCells);
  m_b_cell_geo_dphi.reserve(nCells);
  m_b_cell_geo_sigma.reserve(nCells);
  m_b_cell_geo_volume.reserve(nCells);

  for(const auto cellItr : *CellContainer)
  {
    auto theDDE=cellItr->caloDDE();
    m_b_cell_geo_ID.push_back(cellItr->ID().get_identifier32().get_compact());
    m_b_cell_geo_sampling.push_back(theDDE->getSampling());
    m_b_cell_geo_eta.push_back(theDDE->eta());
    m_b_cell_geo_phi.push_back(theDDE->phi());
    double cx=theDDE->x();
    double cy=theDDE->y();
    m_b_cell_geo_rPerp.push_back(std::sqrt(cx*cx+cy*cy));
    m_b_cell_geo_deta.push_back(theDDE->deta());
    m_b_cell_geo_dphi.push_back(theDDE->dphi());
    m_b_cell_geo_volume.push_back(theDDE->volume());
    m_b_cell_geo_sigma.push_back(m_noiseTool->getNoise(theDDE,ICalorimeterNoiseTool::TOTALNOISE));
  }
  m_cellGeometryTree->Fill();
  return StatusCode::SUCCESS;


  //

}

StatusCode CellGeometryTreeMaker::finalize() 
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

