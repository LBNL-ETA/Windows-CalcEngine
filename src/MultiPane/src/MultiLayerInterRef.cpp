#include <assert.h>

#include "MultiLayerInterRef.hpp"
#include "OpticalLayer.hpp"
#include "OpticalSurface.hpp"
#include "FenestrationCommon.hpp"
#include "EquivalentLayer.hpp"
#include "MultiLayerInterRefSingleComponent.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace LayerOptics;

namespace MultiPane {

  CInterRef::CInterRef( shared_ptr< CLayer > t_Layer ) : m_StateCalculated( false ) {
    m_Layers.push_back( t_Layer );
    for( Scattering aScattering : EnumScattering() ) {
      m_Energy[ aScattering ] = make_shared< CSurfaceEnergy >();
    }
    for( Side aSide : EnumSide() ) {
      m_StackedLayers[ aSide ] = make_shared< CLayer_List >();
    }
    shared_ptr< CLayerSingleComponent > aLayer = t_Layer->getLayer( Scattering::DirectDirect );
    m_DirectComponent = make_shared< CInterRefSingleComponent >( aLayer );
    aLayer = t_Layer->getLayer( Scattering::DiffuseDiffuse );
    m_DiffuseComponent = make_shared< CInterRefSingleComponent >( aLayer );
  }

  void CInterRef::addLayer( shared_ptr< CLayer > t_Layer, const Side t_Side ) {
    switch( t_Side ) {
    case Side::Front:
      m_Layers.insert( m_Layers.begin(), t_Layer );
      break;
    case Side::Back:
      m_Layers.push_back( t_Layer );
      break;
    default:
      assert( "Impossible side selection when adding new layer." );
      break;
    }

    // addition for pure components (direct and diffuse)
    shared_ptr< CLayerSingleComponent > aLayer = t_Layer->getLayer( Scattering::DirectDirect );
    m_DirectComponent->addLayer( aLayer, t_Side );
    aLayer = t_Layer->getLayer( Scattering::DiffuseDiffuse );
    m_DiffuseComponent->addLayer( aLayer, t_Side );

    m_StateCalculated = false;
  }

  double CInterRef::getEnergyToSurface( const size_t Index, const Side t_SurfaceSide,
    const Side t_EnergySide, const Scattering t_Scattering ) {
    calculateEnergies();
    shared_ptr< CSurfaceEnergy > aEnergy = m_Energy.at( t_Scattering );
    return aEnergy->IEnergy( Index, t_SurfaceSide, t_EnergySide );
  }

  void CInterRef::calculateEnergies() {
    if( !m_StateCalculated ) {
      calculateForwardLayers();
      calculateBackwardLayers();

      m_Energy[ Scattering::DirectDirect ] = m_DirectComponent->getSurfaceEnergy();
      m_Energy[ Scattering::DiffuseDiffuse ] = m_DiffuseComponent->getSurfaceEnergy();
      m_Energy[ Scattering::DirectDiffuse ] = calcDirectToDiffuseComponent();

    }
  }

  void CInterRef::calculateForwardLayers() {
    shared_ptr< CLayer_List > aLayers = m_StackedLayers.at( Side::Front );
    
    // Insert exterior environment first
    shared_ptr< CScatteringSurface > aFront = make_shared< CScatteringSurface >( 1, 0, 0, 0, 1, 0 );
    shared_ptr< CScatteringSurface > aBack = make_shared< CScatteringSurface >( 1, 0, 0, 0, 1, 0 );
    shared_ptr< CLayer > exterior = make_shared< CLayer >( aFront, aBack );
    aLayers->push_back( exterior );

    shared_ptr< CLayer > aLayer = m_Layers[ 0 ];
    aLayers->push_back( aLayer );
    CEquivalentLayer aEqLayer = CEquivalentLayer( aLayer );
    for( size_t i = 1; i < m_Layers.size(); ++i ) {
      aEqLayer.addLayer( m_Layers[ i ] );
      aLayer = aEqLayer.getLayer();
      aLayers->push_back( aLayer );
    }

    aLayers->push_back( exterior );
  }

  void CInterRef::calculateBackwardLayers() {
    shared_ptr< CLayer_List > aLayers = m_StackedLayers.at( Side::Back );

    // Insert interior environment
    shared_ptr< CScatteringSurface > aFront = make_shared< CScatteringSurface >( 1, 0, 0, 0, 1, 0 );
    shared_ptr< CScatteringSurface > aBack = make_shared< CScatteringSurface >( 1, 0, 0, 0, 1, 0 );
    shared_ptr< CLayer > exterior = make_shared< CLayer >( aFront, aBack );
    aLayers->push_back( exterior );

    size_t size = m_Layers.size() - 1;
    // Last layer just in
    shared_ptr< CLayer > aLayer = m_Layers[ size ];
    aLayers->insert( aLayers->begin(), aLayer );
    CEquivalentLayer aEqLayer = CEquivalentLayer( aLayer );
    for( size_t i = size; i > 0; --i ) {
      aEqLayer.addLayer( m_Layers[ i - 1 ], Side::Front );
      aLayer = aEqLayer.getLayer();
      aLayers->insert( aLayers->begin(), aLayer );
    }
    aLayers->insert( aLayers->begin(), exterior );
  }

  shared_ptr< CSurfaceEnergy > CInterRef::calcDiffuseEnergy() {
    //Sum of previous two components. Total diffuse energy that gets off the surfaces.
    shared_ptr< CSurfaceEnergy > diffSum = make_shared< CSurfaceEnergy >();

    for( Side aEnergyFlow : EnumSide() ) {
      for( size_t i = 1; i <= m_Layers.size(); ++i ) { // Layer indexing goes from one
        for( Side aSide : EnumSide() ) {
          Side oppSide = oppositeSide( aSide );
          // Calculate diffuse energy from direct exterior/interior beam
          double beamEnergy = 0;

          shared_ptr< CLayer > curLayer = ( *m_StackedLayers.at( oppSide ) )[ i ];

          if( aSide != aEnergyFlow ) {
            beamEnergy = curLayer->getProperty( PropertySimple::T, oppSide, Scattering::DirectDiffuse );
          }

          // Energy that gets converted to diffuse from beam that comes from interreflections in 
          // the gap or interior/exterior environments
          double R = curLayer->getProperty( PropertySimple::R, aSide, Scattering::DirectDiffuse );
          double intEnergy = R * m_Energy.at( Scattering::DirectDirect )->IEnergy( i, aSide, aEnergyFlow );
          diffSum->addEnergy( aSide, aEnergyFlow, beamEnergy + intEnergy );
        }
      }
    }

    return diffSum;
  }

  shared_ptr< CSurfaceEnergy > CInterRef::calcDirectToDiffuseComponent() {
    // Gets total diffuse components that is getting off (leaving) every surface.
    // Keep in mind that diffuse componet here only comes from scattering direct beam.
    shared_ptr< CSurfaceEnergy > diffSum = calcDiffuseEnergy();

    // Now need to calculate interreflections of total diffuse components that are leaving
    // every surface and calculate total diffuse component that is incoming to every surface.
    shared_ptr< CSurfaceEnergy > aScatter = make_shared< CSurfaceEnergy >();
    
    // Calculate total energy scatterred from beam to diffuse
    for( Side aEnergyFlow : EnumSide() ) {
      // In this case numbering goes through gas environments (gaps, interior and exterior)
      // becase we want to keep interreflectance calculations together
      for( size_t i = 0; i <= m_Layers.size(); ++i ) {
        shared_ptr< CLayer > fwdLayer = ( *m_StackedLayers.at( Side::Front ) )[ i ];
        shared_ptr< CLayer > bkwLayer = ( *m_StackedLayers.at( Side::Back ) )[ i + 1 ];
        double Ib = 0;
        if( i != 0 ) {
          Ib = diffSum->IEnergy( i, Side::Back, aEnergyFlow );
        }
        double If = 0;
        if( i != m_Layers.size() ) {
          If = diffSum->IEnergy( i + 1, Side::Front, aEnergyFlow );
        }
        double Rf_bkw = bkwLayer->getProperty( PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse );
        double Rb_fwd = fwdLayer->getProperty( PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse );
        double interRef = 1 / ( 1 - Rf_bkw * Rb_fwd );
        double Rb_bkw = bkwLayer->getProperty( PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse );
        double Ib_tot = ( Ib * Rf_bkw + If ) * interRef;
        double If_tot = ( Ib + Rb_fwd * If ) * interRef;
        if( i != 0 ) {
          aScatter->addEnergy( Side::Back, aEnergyFlow, Ib_tot );
        }
        if( i != m_Layers.size() ) {
          aScatter->addEnergy( Side::Front, aEnergyFlow, If_tot );
        }
      }
    }

    return aScatter;
  }

}