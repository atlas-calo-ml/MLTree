#ifndef MLTREE_CELLGEOMETRYTREEMAKER_H
#define MLTREE_CELLGEOMETRYTREEMAKER_H

#include <string>
#include <vector>
#include <CaloEvent/CaloCellContainer.h>
#include <GaudiKernel/ToolHandle.h>
#include <AthenaBaseComps/AthHistogramAlgorithm.h>
#include <CaloIdentifier/LArNeighbours.h>
#include "CaloConditions/CaloNoise.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

class CellGeometryTreeMaker: public ::AthHistogramAlgorithm { 

public: 
  CellGeometryTreeMaker( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CellGeometryTreeMaker(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

private: 
  typedef CaloCellContainer::size_type size_type;

  /** Conditions Handles to access calorimeter noise */
  SG::ReadCondHandleKey<CaloNoise> m_caloElectronicNoiseKey{this, "CaloNoise","electronicNoise","CaloNoise object to read."};  
  SG::ReadCondHandleKey<CaloNoise> m_caloTotalNoiseKey{this, "CaloNoise","totalNoise","CaloNoise object to read."};  

  /** Toggle what type of noise to get */
  Gaudi::Property<bool> m_twoGaussianNoise{this,"TwoGaussianNoise",true,"Whether to use 2-gaussian noise description for TileCal"};

  /** ReadCondHandleKey for CaloDetDescrManager */
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
     , "CaloDetDescrManager"
     , "CaloDetDescrManager"
     , "SG Key for CaloDetDescrManager in the Condition Store" };

  /** ReadHandleKey for the CaloCellContainer */

  SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey{this, "CaloCellContainer", "AllCalo", "SG key for the CaloCellContainer"};

  bool m_doNeighbours;

  TTree* m_cellGeometryTree;
  std::vector<size_t> m_b_cell_geo_ID;
  std::vector<unsigned short> m_b_cell_geo_sampling;
  std::vector<float> m_b_cell_geo_eta;
  std::vector<float> m_b_cell_geo_phi;
  std::vector<float> m_b_cell_geo_rPerp;
  std::vector<float> m_b_cell_geo_deta;
  std::vector<float> m_b_cell_geo_dphi;
  std::vector<float> m_b_cell_geo_volume;
  std::vector<float> m_b_cell_geo_electronicNoise;
  std::vector<float> m_b_cell_geo_totalNoise;
  std::vector<std::vector<int> > m_b_cell_geo_neighbourhood;

  std::vector<std::string> m_neighbourNames;
  std::vector<LArNeighbours::neighbourOption> m_neighbourTypes;

}; 

#endif
