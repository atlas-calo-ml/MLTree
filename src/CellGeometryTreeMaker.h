#ifndef MLTREE_CELLGEOMETRYTREEMAKER_H
#define MLTREE_CELLGEOMETRYTREEMAKER_H

#include <string>
#include <vector>
#include <CaloEvent/CaloCellContainer.h>
#include <GaudiKernel/ToolHandle.h>
#include <AthenaBaseComps/AthHistogramAlgorithm.h>

class ICalorimeterNoiseTool;

class CellGeometryTreeMaker: public ::AthHistogramAlgorithm { 

public: 
  CellGeometryTreeMaker( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CellGeometryTreeMaker(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

private: 
  typedef CaloCellContainer::size_type size_type;
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

  TTree* m_cellGeometryTree;
  std::vector<size_t> m_b_cell_geo_ID;
  std::vector<unsigned short> m_b_cell_geo_sampling;
  std::vector<float> m_b_cell_geo_eta;
  std::vector<float> m_b_cell_geo_phi;
  std::vector<float> m_b_cell_geo_rPerp;
  std::vector<float> m_b_cell_geo_deta;
  std::vector<float> m_b_cell_geo_dphi;
  std::vector<float> m_b_cell_geo_volume;
  std::vector<float> m_b_cell_geo_sigma;

  std::string m_cellContainerKey;


}; 

#endif