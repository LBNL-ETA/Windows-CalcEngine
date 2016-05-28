#include <assert.h>

#include "MultiLayerInterreflectances.hpp"
#include "LayerSingleComponent.hpp"
#include "EquivalentLayerSingleComponent.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace LayerOptics;

namespace MultiPane {

  CInterreflectances::CInterreflectances( const double t_Tf, const double t_Rf, const double t_Tb, 
    const double t_Rb ) : m_StateCalculated( false ) {
    m_IEnergy[ std::make_pair( Side::Front, Side::Front ) ] = make_shared< vector< double > >();
    m_IEnergy[ std::make_pair( Side::Front, Side::Back ) ] = make_shared< vector< double > >();
    m_IEnergy[ std::make_pair( Side::Back, Side::Front ) ] = make_shared< vector< double > >();
    m_IEnergy[ std::make_pair( Side::Back, Side::Back ) ] = make_shared< vector< double > >();

    addLayer( t_Tf, t_Rf, t_Tb, t_Rb );
  }

  void CInterreflectances::addLayer( const double t_Tf, const double t_Rf, const double t_Tb, const double t_Rb, 
    const Side t_Side ) {
    shared_ptr< CLayerSingleComponent > aLayer = make_shared< CLayerSingleComponent >( t_Tf, t_Rf, t_Tb, t_Rb );
    switch( t_Side ) {
    case Side::Front:
      m_Layers.insert( m_Layers.begin(), aLayer );
      break;
    case Side::Back:
      m_Layers.push_back( aLayer );
      break;
    default:
      assert("Impossible side selection when adding new layer.");
      break;
    }
    m_StateCalculated = false;
  }

  double CInterreflectances::getEnergyToSurface( const size_t Index, const Side t_Side, const Side t_EnergySide ) {
    calculateEnergies();
    return ( *IEnergy( t_Side, t_EnergySide ) )[ Index - 1 ];
  }

  double CInterreflectances::getLayerAbsorptance( const size_t Index, const Side t_Side ) {
    double frontAbs = m_Layers[ Index - 1 ]->getProperty( Property::Abs, Side::Front ) * getEnergyToSurface( Index, Side::Front, t_Side );
    double backAbs = m_Layers[ Index - 1 ]->getProperty( Property::Abs, Side::Back ) * getEnergyToSurface( Index, Side::Back, t_Side );
    return frontAbs + backAbs;
  }

  void CInterreflectances::calculateEnergies() {
    if( !m_StateCalculated ) {
      calculateForwardLayers();
      calculateBackwardLayers();

      // Front side calculations
      for( size_t i = 0; i <= m_Layers.size(); ++i ) {
        shared_ptr< CLayerSingleComponent > aForwardLayer = m_ForwardLayers[ i ];
        shared_ptr< CLayerSingleComponent > aBackwardLayer = m_BackwardLayers[ i ];
        
        double Tf = aForwardLayer->getProperty( Property::T, Side::Front );
        double Tb = aBackwardLayer->getProperty( Property::T, Side::Back );
        double Rf = aBackwardLayer->getProperty( Property::R, Side::Front );
        double Rb = aForwardLayer->getProperty( Property::R, Side::Back );
        double iReflectance = 1 / ( 1 - Rf * Rb );

        if( i != m_Layers.size() ) {
          IEnergy( Side::Front, Side::Front )->push_back( Tf * iReflectance );
          IEnergy( Side::Front, Side::Back )->push_back( Tb * Rb * iReflectance );
        }
        if( i != 0 ) {
          IEnergy( Side::Back, Side::Front )->push_back( Tf * Rf * iReflectance );
          IEnergy( Side::Back, Side::Back )->push_back( Tb * iReflectance );
        }
      }

      m_StateCalculated = true;
    }
  }

  void CInterreflectances::calculateForwardLayers() {
    // Insert exterior environment properties
    shared_ptr< CLayerSingleComponent > aLayer = make_shared< CLayerSingleComponent >( 1, 0, 1, 0 );
    m_ForwardLayers.push_back( aLayer );

    // First layer just in. No calculation is needed
    aLayer = m_Layers[ 0 ];
    m_ForwardLayers.push_back( aLayer );
    CEquivalentLayerSingleComponent aEqLayer = CEquivalentLayerSingleComponent( aLayer );
    for( size_t i = 1; i < m_Layers.size(); ++i ) {
      aEqLayer.addLayer( m_Layers[ i ] );
      aLayer = aEqLayer.getLayer();
      m_ForwardLayers.push_back( aLayer );
    }
  }

  void CInterreflectances::calculateBackwardLayers() {
    // Insert interior environment properties
    shared_ptr< CLayerSingleComponent > aLayer = make_shared< CLayerSingleComponent >( 1, 0, 1, 0 );
    m_BackwardLayers.push_back( aLayer );

    size_t size = m_Layers.size() - 1;
    // Last layer just in. No calculation is needed
    aLayer = m_Layers[ size ];
    m_BackwardLayers.insert( m_BackwardLayers.begin(), aLayer );
    CEquivalentLayerSingleComponent aEqLayer = CEquivalentLayerSingleComponent( aLayer );
    for( size_t i = size ; i > 0; --i ) {
      aEqLayer.addLayer( m_Layers[ i - 1 ], Side::Front );
      aLayer = aEqLayer.getLayer();
      m_BackwardLayers.insert( m_BackwardLayers.begin(), aLayer );
    }
  }

  shared_ptr< vector< double > > CInterreflectances::IEnergy( const Side t_Side, const Side t_EnergyFlow ) {
    return m_IEnergy[ std::make_pair( t_Side, t_EnergyFlow ) ];
  }

}