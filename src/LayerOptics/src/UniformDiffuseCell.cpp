#include <assert.h>

#include "UniformDiffuseCell.hpp"
#include "MaterialDescription.hpp"
#include "CellDescription.hpp"
#include "BeamDirection.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {
  
  CUniformDiffuseCell::CUniformDiffuseCell( shared_ptr< CMaterial > t_MaterialProperties, 
    shared_ptr< CCellDescription > t_Cell ) : CBaseCell( t_MaterialProperties, t_Cell ) {

  }

  double CUniformDiffuseCell::T_dir_dif( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    return getMaterialProperty( Property::T, t_Side, t_Direction );
  }

  double CUniformDiffuseCell::R_dir_dif( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    return ( ( 1 - T_dir_dir( t_Side, t_Direction ) ) * m_Material->getProperty( Property::R,  t_Side ) );
  }

  shared_ptr< vector< double > > CUniformDiffuseCell::T_dir_dif_band( const Side t_Side, 
    shared_ptr< const CBeamDirection > t_Direction ) {
    return getMaterialProperties( Property::T, t_Side, t_Direction );
  }

  shared_ptr< vector< double > > CUniformDiffuseCell::R_dir_dif_band( const Side t_Side, 
    shared_ptr< const CBeamDirection > t_Direction ) {
    return getMaterialProperties( Property::R, t_Side, t_Direction );
  }

  double CUniformDiffuseCell::getMaterialProperty( const Property t_Property, const Side t_Side, 
    shared_ptr< const CBeamDirection > t_Direction ) {
    return ( ( 1 - T_dir_dir( t_Side, t_Direction ) ) * m_Material->getProperty( t_Property, t_Side ) );
  }

  shared_ptr< vector< double > > CUniformDiffuseCell::getMaterialProperties( const Property t_Property, 
    const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    double materialCoverFraction = 1 - T_dir_dir( t_Side, t_Direction );
    shared_ptr< vector< double > > aMaterialProperty = m_Material->getBandProperties( t_Property, t_Side );
    shared_ptr< vector< double > > aProperty = make_shared< vector< double > >();
    for( size_t i = 0; i < aMaterialProperty->size(); ++i ) {
      aProperty->push_back( materialCoverFraction * ( *aMaterialProperty )[ i ] );
    }
    return aProperty;
  }

}