#include <assert.h>

#include "EquivalentLayer.hpp"
#include "OpticalSurface.hpp"
#include "OpticalLayer.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;

namespace MultiPane {

  CEquivalentLayer::CEquivalentLayer( const double Tf_dir_dir, const double Rf_dir_dir, const double Tb_dir_dir, const double Rb_dir_dir,
    const double Tf_dir_dif, const double Rf_dir_dif, const double Tb_dir_dif, const double Rb_dir_dif,
    const double Tf_dif_dif, const double Rf_dif_dif, const double Tb_dif_dif, const double Rb_dif_dif ) {
    shared_ptr< CScatteringSurface > aFrontSurface = 
      make_shared< CScatteringSurface >( Tf_dir_dir, Rf_dir_dir, Tf_dir_dif, Rf_dir_dif, Tf_dif_dif, Rf_dif_dif );
    shared_ptr< CScatteringSurface > 
      aBackSurface = make_shared< CScatteringSurface >( Tb_dir_dir, Rb_dir_dir, Tb_dir_dif, Rb_dir_dif, Tb_dif_dif, Rb_dif_dif );

    m_Layer = make_shared< CLayer >( aFrontSurface, aBackSurface );
  }

  CEquivalentLayer::CEquivalentLayer( shared_ptr< CLayer > t_Layer ) {
    if( t_Layer == nullptr ) {
      throw runtime_error("Layer must be provided.");
    }
    m_Layer = t_Layer;
  }

  void CEquivalentLayer::addLayer( const double Tf_dir_dir, const double Rf_dir_dir, const double Tb_dir_dir, const double Rb_dir_dir,
    const double Tf_dir_dif, const double Rf_dir_dif, const double Tb_dir_dif, const double Rb_dir_dif,
    const double Tf_dif_dif, const double Rf_dif_dif, const double Tb_dif_dif, const double Rb_dif_dif, 
    const Side t_Side ) {
    shared_ptr< CScatteringSurface > aFrontSurface = 
      make_shared< CScatteringSurface >( Tf_dir_dir, Rf_dir_dir, Tf_dir_dif, Rf_dir_dif, Tf_dif_dif, Rf_dif_dif );
    shared_ptr< CScatteringSurface > 
      aBackSurface = make_shared< CScatteringSurface >( Tb_dir_dir, Rb_dir_dir, Tb_dir_dif, Rb_dir_dif, Tb_dif_dif, Rb_dif_dif );
    shared_ptr< CLayer > aLayer = make_shared< CLayer >( aFrontSurface, aBackSurface );
    addLayer( aLayer, t_Side );
  }

  void CEquivalentLayer::addLayer( shared_ptr< CLayer > t_Layer, const Side t_Side ) {
    switch( t_Side ) {
    case Side::Front:
      calcEquivalentProperties( t_Layer, m_Layer );
      break;
    case Side::Back:
      calcEquivalentProperties( m_Layer, t_Layer );
      break;
    default:
      assert("Impossible side selection.");
      break;
    }
  }

  double CEquivalentLayer::getPropertySimple( const PropertySimple t_Property, const Side t_Side, 
    const Scattering t_Scattering ) const {
    shared_ptr< CScatteringSurface > aSurface = m_Layer->getSurface( t_Side );
    return aSurface->getPropertySimple( t_Property, t_Scattering );
  }

  void CEquivalentLayer::calcEquivalentProperties( shared_ptr< CLayer > t_First, shared_ptr< CLayer > t_Second ) {
    // Direct to diffuse componet calculation
    shared_ptr< const CScatteringSurface > f1 = t_First->getSurface( Side::Front );
    shared_ptr< const CScatteringSurface > b1 = t_First->getSurface( Side::Back );
    shared_ptr< const CScatteringSurface > f2 = t_Second->getSurface( Side::Front );
    shared_ptr< const CScatteringSurface > b2 = t_Second->getSurface( Side::Back );
    shared_ptr< SimpleResults > frontSide = calcDirectDiffuseTransAndRefl( f1, b1, f2, b2 );
    shared_ptr< SimpleResults > backSide = calcDirectDiffuseTransAndRefl( b2, f2, b1, f1 );
  }

  double CEquivalentLayer::getInterreflectance( 
    shared_ptr< const CScatteringSurface > t_First, 
    shared_ptr< const CScatteringSurface > t_Second,
    const Scattering t_Scattering ) {
    return 1 - t_First->getPropertySimple( PropertySimple::R, t_Scattering ) * 
      t_Second->getPropertySimple( PropertySimple::R, t_Scattering );
  }

  shared_ptr< SimpleResults > CEquivalentLayer::calcDirectDiffuseTransAndRefl( 
    shared_ptr< const CScatteringSurface > f1,
    shared_ptr< const CScatteringSurface > b1,
    shared_ptr< const CScatteringSurface > f2,
    shared_ptr< const CScatteringSurface > b2 ) {
    double dirInterrefl = getInterreflectance( b1, f2, Scattering::DirectDirect );
    double difInterrefl = getInterreflectance( b1, f2, Scattering::DiffuseDiffuse );
    shared_ptr< SimpleResults > aResult = make_shared< SimpleResults >();

    double Incoming_f2_dir = f1->getPropertySimple( PropertySimple::T, Scattering::DirectDirect ) / dirInterrefl;
    double Incoming_b1_dir = Incoming_f2_dir * f2->getPropertySimple( PropertySimple::R, Scattering::DirectDirect );

    return aResult;
  }

}