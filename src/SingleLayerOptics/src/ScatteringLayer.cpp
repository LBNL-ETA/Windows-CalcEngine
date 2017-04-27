#include <stdexcept>

#include "ScatteringLayer.hpp"
#include "LayerSingleComponent.hpp"
#include "OpticalSurface.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace SingleLayerOptics;
using namespace FenestrationCommon;

namespace SingleLayerOptics {

  CScatteringLayer::CScatteringLayer( const shared_ptr< CScatteringSurface >& t_Front, 
    const shared_ptr< CScatteringSurface >& t_Back ) {
    if( t_Front == nullptr ) {
      throw runtime_error("Front surface must be created.");
    }
    if( t_Back == nullptr ) {
      throw runtime_error("Back surface must be created.");
    }

    m_Surface[ Side::Front ] = t_Front;
    m_Surface[ Side::Back ] = t_Back;
  }

  CScatteringLayer::CScatteringLayer( const shared_ptr< const CScatteringLayer >& t_Layer ) {
    m_Surface[ Side::Front ] = t_Layer->getSurface( Side::Front );
    m_Surface[ Side::Back ] = t_Layer->getSurface( Side::Back );
  }

  CScatteringLayer::CScatteringLayer(
    const double Tf_dir_dir, const double Rf_dir_dir,
    const double Tb_dir_dir, const double Rb_dir_dir,
    const double Tf_dir_dif, const double Rf_dir_dif,
    const double Tb_dir_dif, const double Rb_dir_dif,
    const double Tf_dif_dif, const double Rf_dif_dif,
    const double Tb_dif_dif, const double Rb_dif_dif ) {
    m_Surface[ Side::Front ] = make_shared< CScatteringSurface >( Tf_dir_dir, Rf_dir_dir, 
                                                                  Tf_dir_dif, Rf_dir_dif, 
                                                                  Tf_dif_dif, Rf_dif_dif );
    m_Surface[ Side::Back ] = make_shared< CScatteringSurface >( Tb_dir_dir, Rb_dir_dir,
                                                                 Tb_dir_dif, Rb_dir_dif,
                                                                 Tb_dif_dif, Rb_dif_dif );
  }

  shared_ptr< CScatteringSurface > CScatteringLayer::getSurface( const Side t_Side ) const {
    return m_Surface.at( t_Side );
  }

  double CScatteringLayer::getPropertySimple( const PropertySimple t_Property, const Side t_Side, const Scattering t_Scattering ) const {
    shared_ptr< CScatteringSurface > aSurface = getSurface( t_Side );
    return aSurface->getPropertySimple( t_Property, t_Scattering );
  }

  void CScatteringLayer::setPropertySimple( const PropertySimple t_Property, const Side t_Side, 
    const Scattering t_Scattering, const double value ) const {
    shared_ptr< CScatteringSurface > aSurface = getSurface( t_Side );
    return aSurface->setPropertySimple( t_Property, t_Scattering, value );
  }

  double CScatteringLayer::getAbsorptance( const Side t_Side, const ScatteringSimple t_Scattering ) const {
    shared_ptr< CScatteringSurface > aSurface = getSurface( t_Side );
    return aSurface->getAbsorptance( t_Scattering );
  }

  shared_ptr< CLayerSingleComponent > CScatteringLayer::getLayer( const Scattering t_Scattering ) const {
    double Tf = getPropertySimple( PropertySimple::T, Side::Front, t_Scattering );
    double Rf = getPropertySimple( PropertySimple::R, Side::Front, t_Scattering );
    double Tb = getPropertySimple( PropertySimple::T, Side::Back, t_Scattering );
    double Rb = getPropertySimple( PropertySimple::R, Side::Back, t_Scattering );
    shared_ptr< CLayerSingleComponent > aLayer = make_shared< CLayerSingleComponent >( Tf, Rf, Tb, Rb );
    return aLayer;
  }

}