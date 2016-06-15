#include "MultiLayer.hpp"
#include "MultiLayerInterRefSingleComponent.hpp"
#include "EquivalentLayer.hpp"
#include "OpticalLayer.hpp"
#include "OpticalSurface.hpp"
#include "LayerSingleComponent.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;

namespace MultiPane {
  
  CMultiLayer::CMultiLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
    const double t_Tb_dif_dif, const double t_Rb_dif_dif ) {

    shared_ptr< const CLayer > aLayer = getLayer( 
      t_Tf_dir_dir, t_Rf_dir_dir,t_Tb_dir_dir, t_Rb_dir_dir, 
      t_Tf_dir_dif, t_Rf_dir_dif, t_Tb_dir_dif, t_Rb_dir_dif,
      t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif );

    createInterreflectances( aLayer );
  }

  CMultiLayer::CMultiLayer( shared_ptr< const CLayer > t_Layer ) {
    createInterreflectances( t_Layer );
  }

  void CMultiLayer::addLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
    const double t_Tb_dif_dif, const double t_Rb_dif_dif,
    const Side t_Side ) {

    shared_ptr< const CLayer > aLayer = getLayer(
      t_Tf_dir_dir, t_Rf_dir_dir, t_Tb_dir_dir, t_Rb_dir_dir,
      t_Tf_dir_dif, t_Rf_dir_dif, t_Tb_dir_dif, t_Rb_dir_dif,
      t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif );
    
    addLayer( aLayer, t_Side );
  }

  void CMultiLayer::addLayer( shared_ptr< const CLayer > t_Layer, const Side t_Side ) {
    shared_ptr< CLayerSingleComponent > aDirectScatter = t_Layer->getLayer( Scattering::DirectDirect );
    shared_ptr< CLayerSingleComponent > aDiffuseScatter = t_Layer->getLayer( Scattering::DiffuseDiffuse );

    m_Interref[ ScatteringSimple::Direct ]->addLayer( aDirectScatter, t_Side );
    m_Interref[ ScatteringSimple::Diffuse ]->addLayer( aDiffuseScatter, t_Side );

    // TODO: Need to create forward(Front) and backward(Back) layers
    m_EquivalentLayer[ Side::Front ]->addLayer( t_Layer, t_Side );
    m_EquivalentLayer[ Side::Back ]->addLayer( t_Layer, t_Side );
  }

  shared_ptr< CLayer > CMultiLayer::getLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir,
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif,
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif,
    const double t_Tb_dif_dif, const double t_Rb_dif_dif ) {
    
    shared_ptr< CScatteringSurface > aFrontSurface =
      make_shared< CScatteringSurface >( t_Tf_dir_dir, t_Rf_dir_dir, t_Tf_dir_dif, t_Rf_dir_dif,
        t_Tf_dif_dif, t_Rf_dif_dif );

    shared_ptr< CScatteringSurface > aBackSurface =
      make_shared< CScatteringSurface >( t_Tb_dir_dir, t_Rb_dir_dir, t_Tb_dir_dif, t_Rb_dir_dif,
        t_Tb_dif_dif, t_Rb_dif_dif );

    return make_shared< CLayer >( aFrontSurface, aBackSurface );
  }

  void CMultiLayer::createInterreflectances( shared_ptr< const CLayer > t_Layer ) {

    shared_ptr< CLayerSingleComponent > aDirectScatter = t_Layer->getLayer( Scattering::DirectDirect );
    shared_ptr< CLayerSingleComponent > aDiffuseScatter = t_Layer->getLayer( Scattering::DiffuseDiffuse );

    m_Interref[ ScatteringSimple::Direct ] = make_shared< CInterRefSingleComponent >( aDirectScatter );
    m_Interref[ ScatteringSimple::Diffuse ] = make_shared< CInterRefSingleComponent >( aDiffuseScatter );

    // TODO: Forward and backward layer must be created instead of this
    m_EquivalentLayer[ Side::Front ] = make_shared< CEquivalentLayer >( t_Layer );
    m_EquivalentLayer[ Side::Back ] = make_shared< CEquivalentLayer >( t_Layer );
  }

}