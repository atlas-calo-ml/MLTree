#include "CellGeometryTreeMaker.h"
#include <CaloEvent/CaloCellContainer.h>
#include <CaloDetDescr/CaloDetDescrManager.h>
#include <cmath>


CellGeometryTreeMaker::CellGeometryTreeMaker( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthHistogramAlgorithm( name, pSvcLocator ),  
  m_doNeighbours(true)
{
  declareProperty("DoNeighbours",m_doNeighbours);
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
  if(m_doNeighbours) 
  {
    m_neighbourNames={"prevInPhi","nextInPhi","prevInEta","nextInEta",
		      "prevInSamp","nextInSamp","prevSubDet","nextSubDet","prevSuperCalo","nextSuperCalo"};
    m_neighbourTypes={LArNeighbours::prevInPhi,LArNeighbours::nextInPhi,LArNeighbours::prevInEta,LArNeighbours::nextInEta,
		      LArNeighbours::prevInSamp, LArNeighbours::nextInSamp, LArNeighbours::prevSubDet, LArNeighbours::nextSubDet, LArNeighbours::prevSuperCalo, LArNeighbours::nextSuperCalo};
    m_b_cell_geo_neighbourhood.reserve(m_neighbourNames.size());
    for(auto nname : m_neighbourNames)
    {
      m_b_cell_geo_neighbourhood.push_back(std::vector<int>());
      m_cellGeometryTree->Branch(std::string("cell_geo_"+nname).c_str(),&(m_b_cell_geo_neighbourhood.back()));
    }
  }
  ATH_CHECK( m_caloNoiseKey.initialize() );
  ATH_MSG_INFO("Noise conditions initialized");  

  /* Retrieve calorimeter detector manager */
  ATH_CHECK(m_caloMgrKey.initialize());

  ATH_CHECK(m_cellContainerKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode CellGeometryTreeMaker::execute() 
{

  SG::ReadHandle<CaloCellContainer> cellContainerHandle(m_cellContainerKey);
  if (!cellContainerHandle.isValid()) {
    ATH_MSG_ERROR("Could not retrieve CaloCellContainer with key " << m_cellContainerKey);
    return StatusCode::FAILURE;
  }

  //
  auto nCells=cellContainerHandle->size();

  if(m_cellGeometryTree->GetEntries() > 0) return StatusCode::SUCCESS;
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
  auto calo_dd_man  = *caloMgrHandle; 
  auto calo_id   = calo_dd_man->getCaloCell_ID();

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

  for(std::vector<int>& nn : m_b_cell_geo_neighbourhood) nn.assign(nCells,-1);
  std::vector<std::vector<IdentifierHash> > hashNeighbourhood;
  hashNeighbourhood.reserve(nCells);
  
  //the neighbour information
  //std::unordered_map<IdentifierHash,unsigned int> cell_map;
  std::unordered_map<unsigned int,unsigned int> cellHashMap;
  cellHashMap.reserve(nCells);

  SG::ReadCondHandle<CaloNoise> caloNoise (m_caloNoiseKey);  
  
  for(unsigned int iCell=0; iCell < nCells; iCell++)
  {
    auto pCell=cellContainerHandle->at(iCell);
    auto theDDE= pCell->caloDDE();
    m_b_cell_geo_ID.push_back(pCell->ID().get_identifier32().get_compact());
    m_b_cell_geo_sampling.push_back(theDDE->getSampling());
    m_b_cell_geo_eta.push_back(theDDE->eta());
    m_b_cell_geo_phi.push_back(theDDE->phi());
    double cx=theDDE->x();
    double cy=theDDE->y();
    m_b_cell_geo_rPerp.push_back(std::sqrt(cx*cx+cy*cy));
    m_b_cell_geo_deta.push_back(theDDE->deta());
    m_b_cell_geo_dphi.push_back(theDDE->dphi());
    m_b_cell_geo_volume.push_back(theDDE->volume());

    float sigma = m_twoGaussianNoise ? caloNoise->getEffectiveSigma(pCell->ID(),pCell->gain(),pCell->energy()) : 
    caloNoise->getNoise(pCell->ID(), pCell->gain());
    m_b_cell_geo_sigma.push_back(sigma);

    if(m_doNeighbours) cellHashMap[theDDE->calo_hash().value()]=iCell;

  }
  //one more loop over the cells to map hashes to indexes in the output tree vectors
  if(m_doNeighbours)
  {
    std::vector<IdentifierHash> v_NN;
    v_NN.reserve(22);
    for(unsigned int jCell=0; jCell<nCells; jCell++)
    {
      unsigned int cellHash=cellContainerHandle->at(jCell)->caloDDE()->calo_hash().value();
      for(unsigned int nType=0; nType<m_b_cell_geo_neighbourhood.size(); nType++)
      {
	calo_id->get_neighbours(cellHash,m_neighbourTypes[nType],v_NN);
	if(v_NN.size()==1) m_b_cell_geo_neighbourhood[nType][jCell]=cellHashMap[v_NN[0].value()];
	v_NN.clear();
      }
    }
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

