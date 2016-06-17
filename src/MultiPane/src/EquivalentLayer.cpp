#include <assert.h>

#include "EquivalentLayer.hpp"
#include "EquivalentLayerSingleComponent.hpp"
#include "OpticalSurface.hpp"
#include "OpticalLayer.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;

namespace MultiPane {

  CEquivalentLayer::CEquivalentLayer( const double Tf_dir_dir, const double Rf_dir_dir, 
    const double Tb_dir_dir, const double Rb_dir_dir,
    const double Tf_dir_dif, const double Rf_dir_dif, 
    const double Tb_dir_dif, const double Rb_dir_dif,
    const double Tf_dif_dif, const double Rf_dif_dif, 
    const double Tb_dif_dif, const double Rb_dif_dif ) {
    
    shared_ptr< CScatteringSurface > aFrontSurface = 
      make_shared< CScatteringSurface >( Tf_dir_dir, Rf_dir_dir, Tf_dir_dif, 
        Rf_dir_dif, Tf_dif_dif, Rf_dif_dif );
    shared_ptr< CScatteringSurface > 
      aBackSurface = make_shared< CScatteringSurface >( Tb_dir_dir, Rb_dir_dir, Tb_dir_dif, 
        Rb_dir_dif, Tb_dif_dif, Rb_dif_dif );

    m_Layer = make_shared< CLayer >( aFrontSurface, aBackSurface );

    m_DiffuseLayer = make_shared< CEquivalentLayerSingleComponent >( Tf_dif_dif, Rf_dif_dif, 
      Tb_dif_dif, Rb_dif_dif );

    m_BeamLayer = make_shared< CEquivalentLayerSingleComponent >( Tf_dir_dir, Rf_dir_dir,
      Tb_dir_dir, Rb_dir_dir );
  }

  CEquivalentLayer::CEquivalentLayer( shared_ptr< const CLayer > t_Layer ) {
    if( t_Layer == nullptr ) {
      throw runtime_error("Layer must be provided.");
    }
    m_Layer = make_shared< CLayer >( t_Layer );

    double Tf = t_Layer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DirectDirect );
    double Rf = t_Layer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DirectDirect );
    double Tb = t_Layer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DirectDirect );
    double Rb = t_Layer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DirectDirect );
    
    m_BeamLayer = make_shared< CEquivalentLayerSingleComponent >( Tf, Rf, Tb, Rb );

    Tf = t_Layer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DiffuseDiffuse );
    Rf = t_Layer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse );
    Tb = t_Layer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DiffuseDiffuse );
    Rb = t_Layer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse );

    m_DiffuseLayer = make_shared< CEquivalentLayerSingleComponent >( Tf, Rf, Tb, Rb );
  }

  void CEquivalentLayer::addLayer( const double Tf_dir_dir, const double Rf_dir_dir, 
    const double Tb_dir_dir, const double Rb_dir_dir,
    const double Tf_dir_dif, const double Rf_dir_dif, 
    const double Tb_dir_dif, const double Rb_dir_dif,
    const double Tf_dif_dif, const double Rf_dif_dif, 
    const double Tb_dif_dif, const double Rb_dif_dif, 
    const Side t_Side ) {
    
    shared_ptr< CScatteringSurface > aFrontSurface = 
      make_shared< CScatteringSurface >( Tf_dir_dir, Rf_dir_dir, 
                                         Tf_dir_dif, Rf_dir_dif, 
                                         Tf_dif_dif, Rf_dif_dif );
    
    shared_ptr< CScatteringSurface > aBackSurface = 
      make_shared< CScatteringSurface >( Tb_dir_dir, Rb_dir_dir, 
                                         Tb_dir_dif, Rb_dir_dif, 
                                         Tb_dif_dif, Rb_dif_dif );
    
    shared_ptr< CLayer > aLayer = make_shared< CLayer >( aFrontSurface, aBackSurface );
    addLayer( aLayer, t_Side );
  }

  void CEquivalentLayer::addLayer( shared_ptr< const CLayer > t_Layer, const Side t_Side ) {
    addLayerComponents( t_Layer, t_Side );
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

  shared_ptr< CLayer > CEquivalentLayer::getLayer() const {
    return m_Layer;
  }

  void CEquivalentLayer::calcEquivalentProperties( shared_ptr< const CLayer > t_First, 
    shared_ptr< const CLayer > t_Second ) {
    // Direct to diffuse componet calculation
    shared_ptr< const CScatteringSurface > f1 = t_First->getSurface( Side::Front );
    shared_ptr< const CScatteringSurface > b1 = t_First->getSurface( Side::Back );
    shared_ptr< const CScatteringSurface > f2 = t_Second->getSurface( Side::Front );
    shared_ptr< const CScatteringSurface > b2 = t_Second->getSurface( Side::Back );
    shared_ptr< SimpleResults > frontSide = calcDirectDiffuseTransAndRefl( f1, b1, f2, b2 );
    shared_ptr< SimpleResults > backSide = calcDirectDiffuseTransAndRefl( b2, f2, b1, f1 );

    double Tf_dir_dif = frontSide->T;
    double Rf_dir_dif = frontSide->R;
    double Tb_dir_dif = backSide->T;
    double Rb_dir_dif = backSide->R;

    // Direct component
    double Tf_dir_dir = m_BeamLayer->getProperty( Property::T, Side::Front );
    double Rf_dir_dir = m_BeamLayer->getProperty( Property::R, Side::Front );
    double Tb_dir_dir = m_BeamLayer->getProperty( Property::T, Side::Back );
    double Rb_dir_dir = m_BeamLayer->getProperty( Property::R, Side::Back );

    // Diffuse component
    double Tf_dif_dif = m_DiffuseLayer->getProperty( Property::T, Side::Front );
    double Rf_dif_dif = m_DiffuseLayer->getProperty( Property::R, Side::Front );
    double Tb_dif_dif = m_DiffuseLayer->getProperty( Property::T, Side::Back );
    double Rb_dif_dif = m_DiffuseLayer->getProperty( Property::R, Side::Back );

    shared_ptr< CScatteringSurface > aFrontSurface =
      make_shared< CScatteringSurface >( Tf_dir_dir, Rf_dir_dir, Tf_dir_dif,
        Rf_dir_dif, Tf_dif_dif, Rf_dif_dif );
    shared_ptr< CScatteringSurface >
      aBackSurface = make_shared< CScatteringSurface >( Tb_dir_dir, Rb_dir_dir, Tb_dir_dif,
        Rb_dir_dif, Tb_dif_dif, Rb_dif_dif );

    m_Layer = make_shared< CLayer >( aFrontSurface, aBackSurface );
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
    
    shared_ptr< SimpleResults > aResult = make_shared< SimpleResults >();

    // Diffuse from direct beam component on the outside
    // Direct to direct interreflectance component
    double dirInterrefl = getInterreflectance( b1, f2, Scattering::DirectDirect );


    double If1_dif_ray = f1->getPropertySimple( PropertySimple::R, Scattering::DirectDiffuse );
    double Ib1_dif_ray = f1->getPropertySimple( PropertySimple::T, Scattering::DirectDiffuse );

    // Diffuse on surface from gap beam interreflections
    // First calculate direct beam that is incoming to surfaces b1 and f2
    double Incoming_f2_dir = f1->getPropertySimple( PropertySimple::T,
      Scattering::DirectDirect ) / dirInterrefl;
    double Incoming_b1_dir = Incoming_f2_dir * f2->getPropertySimple( PropertySimple::R,
      Scattering::DirectDirect );

    // Each component is calculated by simple multiplication of incoming beam with direct to diffuse property
    double If1_dif_inbm = Incoming_b1_dir * b1->getPropertySimple( PropertySimple::T, Scattering::DirectDiffuse );
    double Ib1_dif_inbm = Incoming_b1_dir * b1->getPropertySimple( PropertySimple::R, Scattering::DirectDiffuse );

    double If2_dif_inbm = Incoming_f2_dir * f2->getPropertySimple( PropertySimple::R, Scattering::DirectDiffuse );
    double Ib2_dif_inbm = Incoming_f2_dir * f2->getPropertySimple( PropertySimple::T, Scattering::DirectDiffuse );

    // Diffuse on surfaces from gap diffuse interreflections
    // First calculate diffuse components that are leaving surfaces in the gap
    double I_b1_dif = Ib1_dif_ray + Ib1_dif_inbm;
    double I_f2_dif = If2_dif_inbm;

    // Diffuse interreflectance component
    double difInterrefl = getInterreflectance( b1, f2, Scattering::DiffuseDiffuse );

    double I_fwd = ( I_b1_dif + I_f2_dif * 
      b1->getPropertySimple( PropertySimple::R, Scattering::DiffuseDiffuse ) ) / difInterrefl;
    double I_bck = ( I_b1_dif * f2->getPropertySimple( PropertySimple::R, Scattering::DiffuseDiffuse )
      + I_f2_dif ) / difInterrefl;

    double If1_dif_dif = I_bck * b1->getPropertySimple( PropertySimple::T, Scattering::DiffuseDiffuse );
    double Ib2_dif_dif = I_fwd * f2->getPropertySimple( PropertySimple::T, Scattering::DiffuseDiffuse );

    aResult->T = Ib2_dif_inbm + Ib2_dif_dif;
    aResult->R = If1_dif_ray + If1_dif_inbm + If1_dif_dif;

    return aResult;
  }

  void CEquivalentLayer::addLayerComponents( shared_ptr< const CLayer > t_Layer, const Side t_Side ) {
    double Tf = t_Layer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DirectDirect );
    double Rf = t_Layer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DirectDirect );
    double Tb = t_Layer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DirectDirect );
    double Rb = t_Layer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DirectDirect );
    m_BeamLayer->addLayer( Tf, Rf, Tb, Rb, t_Side );

    Tf = t_Layer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DiffuseDiffuse );
    Rf = t_Layer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse );
    Tb = t_Layer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DiffuseDiffuse );
    Rb = t_Layer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse );
    m_DiffuseLayer->addLayer( Tf, Rf, Tb, Rb, t_Side );
  }

}