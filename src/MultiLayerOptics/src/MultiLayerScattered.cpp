#include <assert.h>

#include "MultiLayerScattered.hpp"
#include "MultiLayerInterRef.hpp"
#include "EquivalentScatteringLayer.hpp"
#include "ScatteringLayer.hpp"
#include "OpticalSurface.hpp"
#include "LayerSingleComponent.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace SingleLayerOptics;
using namespace FenestrationCommon;

namespace MultiLayerOptics {
  
  CMultiLayerScattered::CMultiLayerScattered( 
    const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
    const double t_Tb_dif_dif, const double t_Rb_dif_dif ) {

    shared_ptr< const CScatteringLayer > aLayer = make_shared< CScatteringLayer >( 
      t_Tf_dir_dir, t_Rf_dir_dir,t_Tb_dir_dir, t_Rb_dir_dir, 
      t_Tf_dir_dif, t_Rf_dir_dif, t_Tb_dir_dif, t_Rb_dir_dif,
      t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif );

    initialize( aLayer );
  }

  CMultiLayerScattered::CMultiLayerScattered( const shared_ptr< const CScatteringLayer >& t_Layer ) {
    initialize( t_Layer );
  }

  void CMultiLayerScattered::addLayer( 
    const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
    const double t_Tb_dif_dif, const double t_Rb_dif_dif,
    const Side t_Side ) {

    shared_ptr< CScatteringLayer > aLayer = make_shared< CScatteringLayer >(
      t_Tf_dir_dir, t_Rf_dir_dir, t_Tb_dir_dir, t_Rb_dir_dir,
      t_Tf_dir_dif, t_Rf_dir_dif, t_Tb_dir_dif, t_Rb_dir_dif,
      t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif );
    
    addLayer( aLayer, t_Side );
  }

  void CMultiLayerScattered::addLayer( const shared_ptr< CScatteringLayer >& t_Layer, const Side t_Side ) {
    m_Layer->addLayer( *t_Layer, t_Side );
    m_InterRef->addLayer( t_Layer, t_Side );
  }

  double CMultiLayerScattered::getPropertySimple( const PropertySimple t_Property, const Side t_Side,
    const Scattering t_Scattering ) const {
    assert( m_Layer != nullptr );
    return m_Layer->getPropertySimple( t_Property, t_Side, t_Scattering );
  }

  double CMultiLayerScattered::getAbsorptanceLayer( const size_t Index, Side t_Side,
    ScatteringSimple t_Scattering ) {
    assert( m_InterRef != nullptr );
    return m_InterRef->getAbsorptance( Index, t_Side, t_Scattering );
  }

  double CMultiLayerScattered::getAbsorptance( Side t_Side, ScatteringSimple t_Scattering ) {
    double aAbs = 0;
    for( size_t i = 0; i < m_InterRef->size(); ++i ) {
      aAbs += m_InterRef->getAbsorptance( i + 1, t_Side, t_Scattering );
    }
    return aAbs;
  }

  void CMultiLayerScattered::initialize( const shared_ptr< const CScatteringLayer >& t_Layer ) {
    m_Layer = make_shared< CEquivalentScatteringLayer >( t_Layer );
    m_InterRef = make_shared< CInterRef >( t_Layer );
  }

}