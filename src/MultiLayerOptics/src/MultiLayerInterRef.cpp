#include <assert.h>
#include <stdexcept>

#include "MultiLayerInterRef.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"
#include "EquivalentScatteringLayer.hpp"
#include "MultiLayerInterRefSingleComponent.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace SingleLayerOptics;

namespace MultiLayerOptics {

  CInterRef::CInterRef( const shared_ptr< const CScatteringLayer >& t_Layer ) : m_StateCalculated( false ) {
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

    for( Side aSide : EnumSide() ) {
      m_Abs[ make_pair( aSide, ScatteringSimple::Diffuse ) ] = make_shared< vector< double > >();
      m_Abs[ make_pair( aSide, ScatteringSimple::Direct ) ] = make_shared< vector< double > >();
    }
  }

  void CInterRef::addLayer( const shared_ptr< const CScatteringLayer >& t_Layer, const Side t_Side ) {
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

  double CInterRef::getAbsorptance( const size_t Index, Side t_Side, ScatteringSimple t_Scattering ) {
    calculateEnergies();
    shared_ptr< vector< double > > aVector = m_Abs.at( make_pair( t_Side, t_Scattering ) );
    size_t vecSize = aVector->size();
    if( vecSize < Index ) {
      throw range_error("Requested layer index is out of range.");
    }
    return ( *aVector )[ Index - 1 ];
  }

  double CInterRef::getEnergyToSurface( const size_t Index, const Side t_SurfaceSide,
    const EnergyFlow t_EnergyFlow, const Scattering t_Scattering ) {
    calculateEnergies();
    shared_ptr< CSurfaceEnergy > aEnergy = m_Energy.at( t_Scattering );
    return aEnergy->IEnergy( Index, t_SurfaceSide, t_EnergyFlow );
  }

  size_t CInterRef::size() const {
    return m_Layers.size();
  }

  void CInterRef::calculateEnergies() {
    if( !m_StateCalculated ) {
      calculateForwardLayers();
      calculateBackwardLayers();

      m_Energy[ Scattering::DirectDirect ] = m_DirectComponent->getSurfaceEnergy();
      m_Energy[ Scattering::DiffuseDiffuse ] = m_DiffuseComponent->getSurfaceEnergy();
      m_Energy[ Scattering::DirectDiffuse ] = calcDirectToDiffuseComponent();

      calculateAbsroptances();

    }
  }

  void CInterRef::calculateForwardLayers() {
    shared_ptr< CLayer_List > aLayers = m_StackedLayers.at( Side::Front );
    
    // Insert exterior environment first
    shared_ptr< CScatteringSurface > aFront = make_shared< CScatteringSurface >( 1, 0, 0, 0, 1, 0 );
    shared_ptr< CScatteringSurface > aBack = make_shared< CScatteringSurface >( 1, 0, 0, 0, 1, 0 );
    shared_ptr< CScatteringLayer > exterior = make_shared< CScatteringLayer >( aFront, aBack );
    aLayers->push_back( exterior );

    shared_ptr< const CScatteringLayer > aLayer = m_Layers[ 0 ];
    aLayers->push_back( aLayer );
    CEquivalentScatteringLayer aEqLayer = CEquivalentScatteringLayer( aLayer );
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
    shared_ptr< const CScatteringLayer > exterior = make_shared< CScatteringLayer >( aFront, aBack );
    aLayers->push_back( exterior );

    size_t size = m_Layers.size() - 1;
    // Last layer just in
    shared_ptr< const CScatteringLayer > aLayer = m_Layers[ size ];
    aLayers->insert( aLayers->begin(), aLayer );
    CEquivalentScatteringLayer aEqLayer = CEquivalentScatteringLayer( aLayer );
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

    for( EnergyFlow aEnergyFlow : EnumEnergyFlow() ) {
      for( size_t i = 1; i <= m_Layers.size(); ++i ) { // Layer indexing goes from one
        for( Side aSide : EnumSide() ) {
          Side oppSide = oppositeSide( aSide );
          // Calculate diffuse energy from direct exterior/interior beam
          double beamEnergy = 0;

          shared_ptr< const CScatteringLayer > curLayer = ( *m_StackedLayers.at( oppSide ) )[ i ];

          if( ( aSide == Side::Front && aEnergyFlow == EnergyFlow::Backward ) || 
              ( aSide == Side::Back && aEnergyFlow == EnergyFlow::Forward ) ) {
            beamEnergy = curLayer->getPropertySimple( PropertySimple::T, oppSide, Scattering::DirectDiffuse );
          }

          // Energy that gets converted to diffuse from beam that comes from interreflections in 
          // the gap or interior/exterior environments
          double R = curLayer->getPropertySimple( PropertySimple::R, aSide, Scattering::DirectDiffuse );
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
    for( EnergyFlow aEnergyFlow : EnumEnergyFlow() ) {
      // In this case numbering goes through gas environments (gaps, interior and exterior)
      // becase we want to keep interreflectance calculations together
      for( size_t i = 0; i <= m_Layers.size(); ++i ) {
        shared_ptr< const CScatteringLayer > fwdLayer = ( *m_StackedLayers.at( Side::Front ) )[ i ];
        shared_ptr< const CScatteringLayer > bkwLayer = ( *m_StackedLayers.at( Side::Back ) )[ i + 1 ];
        double Ib = 0;
        if( i != 0 ) {
          Ib = diffSum->IEnergy( i, Side::Back, aEnergyFlow );
        }
        double If = 0;
        if( i != m_Layers.size() ) {
          If = diffSum->IEnergy( i + 1, Side::Front, aEnergyFlow );
        }
        double Rf_bkw = bkwLayer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse );
        double Rb_fwd = fwdLayer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse );
        double interRef = 1 / ( 1 - Rf_bkw * Rb_fwd );
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

  void CInterRef::calculateAbsroptances() {
    for( size_t i = 0; i < m_Layers.size(); ++i ) {
      for( EnergyFlow aEnergyFlow : EnumEnergyFlow() ) {
        double EnergyDirect = 0;
        double EnergyDiffuse = 0;
        for( Side aSide : EnumSide() ) {
          double Adir = m_Layers[ i ]->getAbsorptance( aSide, ScatteringSimple::Direct );
          EnergyDirect += Adir * m_Energy[ Scattering::DirectDirect ]->IEnergy( i + 1, aSide, aEnergyFlow );
          double Adif = m_Layers[ i ]->getAbsorptance( aSide, ScatteringSimple::Diffuse );
          EnergyDirect += Adif * m_Energy[ Scattering::DirectDiffuse ]->IEnergy( i + 1, aSide, aEnergyFlow );
          EnergyDiffuse += Adif * m_Energy[ Scattering::DiffuseDiffuse ]->IEnergy( i + 1, aSide, aEnergyFlow );
        }
        // Note that front and back absorptances are actually reffereing to forward and backward
        // energy flows. That is why we need this conversion.
        Side flowSide = getSideFromFlow( aEnergyFlow );
        m_Abs.at( make_pair( flowSide, ScatteringSimple::Direct ) )->push_back( EnergyDirect );
        m_Abs.at( make_pair( flowSide, ScatteringSimple::Diffuse ) )->push_back( EnergyDiffuse );
      }
    }
  }

}