#include <assert.h>

#include "MultiLayer.hpp"
#include "MultiLayerInterRef.hpp"
#include "EquivalentLayer.hpp"
#include "OpticalLayer.hpp"
#include "OpticalSurface.hpp"
#include "LayerSingleComponent.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace SingleLayerOptics;
using namespace FenestrationCommon;

namespace MultiLayerOptics {
  
  CMultiLayer::CMultiLayer( 
    const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
    const double t_Tb_dif_dif, const double t_Rb_dif_dif ) {

    shared_ptr< const CLayer > aLayer = make_shared< CLayer >( 
      t_Tf_dir_dir, t_Rf_dir_dir,t_Tb_dir_dir, t_Rb_dir_dir, 
      t_Tf_dir_dif, t_Rf_dir_dif, t_Tb_dir_dif, t_Rb_dir_dif,
      t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif );

    initialize( aLayer );
  }

  CMultiLayer::CMultiLayer( const shared_ptr< const CLayer >& t_Layer ) {
    initialize( t_Layer );
  }

  void CMultiLayer::addLayer( 
    const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
    const double t_Tb_dif_dif, const double t_Rb_dif_dif,
    const Side t_Side ) {

    shared_ptr< CLayer > aLayer = make_shared< CLayer >(
      t_Tf_dir_dir, t_Rf_dir_dir, t_Tb_dir_dir, t_Rb_dir_dir,
      t_Tf_dir_dif, t_Rf_dir_dif, t_Tb_dir_dif, t_Rb_dir_dif,
      t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif );
    
    addLayer( aLayer, t_Side );
  }

  void CMultiLayer::addLayer( const shared_ptr< CLayer >& t_Layer, const Side t_Side ) {
    m_Layer->addLayer( *t_Layer, t_Side );
    m_InterRef->addLayer( t_Layer, t_Side );
  }

  double CMultiLayer::getPropertySimple( const PropertySimple t_Property, const Side t_Side,
    const Scattering t_Scattering ) const {
    assert( m_Layer != nullptr );
    return m_Layer->getPropertySimple( t_Property, t_Side, t_Scattering );
  }

  double CMultiLayer::getAbsorptanceLayer( const size_t Index, Side t_Side,
    ScatteringSimple t_Scattering ) {
    assert( m_InterRef != nullptr );
    return m_InterRef->getAbsorptance( Index, t_Side, t_Scattering );
  }

  double CMultiLayer::getAbsorptance( Side t_Side, ScatteringSimple t_Scattering ) {
    double aAbs = 0;
    for( size_t i = 0; i < m_InterRef->size(); ++i ) {
      aAbs += m_InterRef->getAbsorptance( i + 1, t_Side, t_Scattering );
    }
    return aAbs;
  }

  void CMultiLayer::initialize( const shared_ptr< const CLayer >& t_Layer ) {
    m_Layer = make_shared< CEquivalentLayer >( t_Layer );
    m_InterRef = make_shared< CInterRef >( t_Layer );
  }

}