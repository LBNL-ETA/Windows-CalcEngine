#include <assert.h>

#include "EquivalentLayerSingleComponent.hpp"
#include "LayerSingleComponent.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;

namespace MultiPane {

  CEquivalentLayerSingleComponent::CEquivalentLayerSingleComponent( const double t_Tf, const double t_Rf, 
    const double t_Tb, const double t_Rb ) {
    m_EquivalentLayer = make_shared< CLayerSingleComponent >( t_Tf, t_Rf, t_Tb, t_Rb );
  }

  CEquivalentLayerSingleComponent::CEquivalentLayerSingleComponent( 
    shared_ptr< const CLayerSingleComponent > t_Layer ) {
    
    const double Tf = t_Layer->getProperty( Property::T, Side::Front );
    const double Rf = t_Layer->getProperty( Property::R, Side::Front );
    const double Tb = t_Layer->getProperty( Property::T, Side::Back );
    const double Rb = t_Layer->getProperty( Property::R, Side::Back );

    m_EquivalentLayer = make_shared< CLayerSingleComponent >( Tf, Rf, Tb, Rb );
  }

  void CEquivalentLayerSingleComponent::addLayer( const double t_Tf, const double t_Rf, const double t_Tb, 
    const double t_Rb, const Side t_Side ) {
    shared_ptr< CLayerSingleComponent > firstLayer = nullptr;
    shared_ptr< CLayerSingleComponent > secondLayer = nullptr;
    switch( t_Side ) {
    case Side::Front:
      firstLayer = make_shared< CLayerSingleComponent >( t_Tf, t_Rf, t_Tb, t_Rb );
      secondLayer = m_EquivalentLayer;
      break;
    case Side::Back:
      firstLayer = m_EquivalentLayer;
      secondLayer = make_shared< CLayerSingleComponent >( t_Tf, t_Rf, t_Tb, t_Rb );
      break;
    default:
      assert("Error in selection of side in double layer calculations.");
      break;
    }

    double Tf = T( firstLayer, secondLayer, Side::Front );
    double Tb = T( firstLayer, secondLayer, Side::Back );
    double Rf = R( firstLayer, secondLayer, Side::Front );
    double Rb = R( firstLayer, secondLayer, Side::Back );
    m_EquivalentLayer = make_shared< CLayerSingleComponent >( Tf, Rf, Tb, Rb );
  }

  void CEquivalentLayerSingleComponent::addLayer( shared_ptr< const CLayerSingleComponent > t_Layer, 
    const Side t_Side ) {
    const double Tf = t_Layer->getProperty( Property::T, Side::Front );
    const double Rf = t_Layer->getProperty( Property::R, Side::Front );
    const double Tb = t_Layer->getProperty( Property::T, Side::Back );
    const double Rb = t_Layer->getProperty( Property::R, Side::Back );
    addLayer( Tf, Rf, Tb, Rb, t_Side );
  }

  double CEquivalentLayerSingleComponent::getProperty( const Property t_Property, const Side t_Side ) const {
    return m_EquivalentLayer->getProperty( t_Property, t_Side );
  }

  shared_ptr< CLayerSingleComponent > CEquivalentLayerSingleComponent::getLayer() const {
    return m_EquivalentLayer; 
  }

  double CEquivalentLayerSingleComponent::interreflectance( shared_ptr< const CLayerSingleComponent > t_Layer1, 
    shared_ptr< const CLayerSingleComponent > t_Layer2 ) const {
    return 1 / ( 1 - t_Layer1->getProperty( Property::R, Side::Back ) * t_Layer2->getProperty( Property::R, Side::Front ) );
  }

  double CEquivalentLayerSingleComponent::T( shared_ptr< const CLayerSingleComponent > t_Layer1, 
    shared_ptr< const CLayerSingleComponent > t_Layer2, Side t_Side ) const {
    return t_Layer1->getProperty( Property::T, t_Side ) * t_Layer2->getProperty( Property::T, t_Side ) * 
      interreflectance( t_Layer1, t_Layer2 );
  }

  double CEquivalentLayerSingleComponent::R( shared_ptr< const CLayerSingleComponent > t_Layer1, 
    shared_ptr< const CLayerSingleComponent > t_Layer2, Side t_Side ) const {
    shared_ptr< const CLayerSingleComponent > firstLayer = nullptr;
    shared_ptr< const CLayerSingleComponent > secondLayer = nullptr;
    switch( t_Side ) {
    case Side::Front:
      firstLayer = t_Layer1;
      secondLayer = t_Layer2;
      break;
    case Side::Back:
      firstLayer = t_Layer2;
      secondLayer = t_Layer1;
      break;
    default:
      assert("Impossible selection of side in double layer calculations.");
      break;
    }
    Side opposite = oppositeSide( t_Side );
    return firstLayer->getProperty( Property::R, t_Side ) + firstLayer->getProperty( Property::T, t_Side ) * 
      firstLayer->getProperty( Property::T, opposite ) * 
      secondLayer->getProperty( Property::R, t_Side ) * interreflectance( t_Layer1, t_Layer2 );
  }

}
