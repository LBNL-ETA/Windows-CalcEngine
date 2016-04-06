#include <assert.h>
#include <algorithm>
#include <stdexcept>
#include <math.h>

#include "MultiPaneSpecular.hpp"
#include "AbsorptancesMultiPane.hpp"
#include "SpecularCell.hpp"
#include "EquivalentLayer.hpp"
#include "Series.hpp"
#include "BeamDirection.hpp"
#include "FenestrationCommon.hpp"
#include "Hemispherical2DIntegrator.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace LayerOptics;

namespace MultiPane {

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CEquivalentLayerAngle
  ////////////////////////////////////////////////////////////////////////////////////////////
  CEquivalentLayerAngle::CEquivalentLayerAngle( shared_ptr< CEquivalentLayer > t_Layer, 
    shared_ptr< CAbsorptancesMultiPane > t_Abs, const double t_Angle ) :
    m_Layer( t_Layer ), m_Abs( t_Abs ), m_Angle( t_Angle ) {
    
  };

  double CEquivalentLayerAngle::angle() const {
    return m_Angle;
  };

  shared_ptr< CEquivalentLayer > CEquivalentLayerAngle::layer() const {
    return m_Layer;
  };

  shared_ptr< CSeries > CEquivalentLayerAngle::getProperties( const Side t_Side, const Property t_Property ) {
    shared_ptr< CSeries > aProperty = nullptr;
    switch( t_Side ) {
    case Side::Front:
      switch (t_Property) {
      case Property::T:
        aProperty = m_Layer->T();
        break;
      case Property::R:
        aProperty = m_Layer->Rf();
        break;
      case Property::Abs:
        aProperty = m_Layer->AbsF();
        break;
      default:
        assert("Incorrect property selection.");
        break;
      }
      break;
    case Side::Back:
      switch (t_Property) {
      case Property::T:
        aProperty = m_Layer->T();
        break;
      case Property::R:
        aProperty = m_Layer->Rb();
        break;
      case Property::Abs:
        aProperty = m_Layer->AbsB();
        break;
      default:
        assert("Incorrect property selection.");
        break;
      }
      break;
    default:
      throw runtime_error("Incorrect selection of side.");
      break;
    }
    assert( aProperty != nullptr );
    return aProperty;
  };

  shared_ptr< CSeries > CEquivalentLayerAngle::Abs( size_t const Index ) {
    return m_Abs->Abs( Index );
  };

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CMultiPaneSpecular
  ////////////////////////////////////////////////////////////////////////////////////////////
  CMultiPaneSpecular::CMultiPaneSpecular( shared_ptr< vector< double > > t_CommonWavelength,
    shared_ptr< CSeries > t_SolarRadiation,
    shared_ptr< CSpecularCell > t_Layer ) : m_CommonWavelengths( t_CommonWavelength ), m_SolarRadiation( t_SolarRadiation ) {
    m_SolarRadiation = m_SolarRadiation->interpolate( m_CommonWavelengths );
    addLayer( t_Layer );
  };

  void CMultiPaneSpecular::addLayer( shared_ptr< CSpecularCell > t_Layer ) {
    m_Layers.push_back( t_Layer );
  };

  double CMultiPaneSpecular::getProperty( const Side t_Side, const Property t_Property, const double t_Angle,
    const double minLambda, const double maxLambda, const IntegrationType t_IntegrationType ) {

    shared_ptr< CEquivalentLayerAngle > aAngularProperties = getAngular( t_Angle );

    shared_ptr< CSeries > aProperties = aAngularProperties->getProperties( t_Side, t_Property );

    aProperties = aProperties->mMult( m_SolarRadiation )->integrate( t_IntegrationType );

    double totalProperty = aProperties->sum( minLambda, maxLambda );
    double totalSolar = m_SolarRadiation->integrate( t_IntegrationType )->sum( minLambda, maxLambda );

    assert( totalSolar > 0 );

    return totalProperty / totalSolar;
  };

  double CMultiPaneSpecular::getHemisphericalProperty( const Side t_Side, const Property t_Property, 
    shared_ptr< const vector< double > > t_Angles, 
    const double minLambda, const double maxLambda, const IntegrationType t_IntegrationType ) {
    size_t size = t_Angles->size();
    shared_ptr< CSeries > aAngularProperties = make_shared< CSeries >();
    for( size_t i = 0; i < size; ++i ) {
      double angle = ( *t_Angles )[ i ];
      double aProperty = getProperty( t_Side, t_Property, angle, minLambda, maxLambda, t_IntegrationType );
      aAngularProperties->addProperty( angle, aProperty );
    }
    CHemispherical2DIntegrator aIntegrator = CHemispherical2DIntegrator( aAngularProperties, t_IntegrationType );
    return aIntegrator.value();
  };

  double CMultiPaneSpecular::Abs( size_t const Index, const double t_Angle, 
    const double minLambda, const double maxLambda, const IntegrationType t_IntegrationType ) {
    shared_ptr< CEquivalentLayerAngle > aAngularProperties = getAngular( t_Angle );
    shared_ptr< CSeries > aProperties = aAngularProperties->Abs( Index - 1 );

    aProperties = aProperties->mMult( m_SolarRadiation )->integrate( t_IntegrationType );

    double totalProperty = aProperties->sum( minLambda, maxLambda );
    double totalSolar = m_SolarRadiation->integrate( t_IntegrationType )->sum( minLambda, maxLambda );

    assert( totalSolar > 0 );

    return totalProperty / totalSolar;
  };

  double CMultiPaneSpecular::AbsHemispherical( size_t const Index, shared_ptr< const vector< double > > t_Angles, 
    const double minLambda, const double maxLambda, const IntegrationType t_IntegrationType ) {
    size_t size = t_Angles->size();
    shared_ptr< CSeries > aAngularProperties = make_shared< CSeries >();
    for( size_t i = 0; i < size; ++i ) {
      double angle = ( *t_Angles )[ i ];
      double aAbs = Abs( Index, angle, minLambda, maxLambda, t_IntegrationType );
      aAngularProperties->addProperty( angle, aAbs );
    }

    CHemispherical2DIntegrator aIntegrator = CHemispherical2DIntegrator( aAngularProperties, t_IntegrationType );
    return aIntegrator.value();
  };

  shared_ptr< CEquivalentLayerAngle > CMultiPaneSpecular::getAngular( const double t_Angle ) {
    shared_ptr< CEquivalentLayerAngle > aAngularProperties = nullptr;

    vector< shared_ptr< CEquivalentLayerAngle > >::iterator it;
    it = find_if( m_EquivalentAngle.begin(), m_EquivalentAngle.end(), 
      [ &t_Angle ]( shared_ptr< CEquivalentLayerAngle > obj ) { return fabs( obj->angle() - t_Angle ) < 1e-6; } );
    
    if( it != m_EquivalentAngle.end() ) {
      aAngularProperties = ( *it );
    } else {
      aAngularProperties = createNewAngular( t_Angle );
    }

    return aAngularProperties;
  };

  shared_ptr< CEquivalentLayerAngle > CMultiPaneSpecular::createNewAngular( const double t_Angle ) {
    // Create direction for specular. It is irrelevant what is Phi angle and it is chosen to be zero in this case
    shared_ptr< CBeamDirection > aDirection = make_shared< CBeamDirection >( t_Angle, 0 );
    shared_ptr< CEquivalentLayer > aEqLayer = nullptr;
    shared_ptr< CAbsorptancesMultiPane > aAbs = nullptr;
    for( size_t i = 0; i < m_Layers.size(); ++i ) {
      shared_ptr< vector< double > > wl = m_Layers[ i ]->getBandWavelengths();
      shared_ptr< vector< double > > Tv = m_Layers[ i ]->T_dir_dir_band( Side::Front, aDirection );
      shared_ptr< vector< double > > Rfv = m_Layers[ i ]->R_dir_dir_band( Side::Front, aDirection );
      shared_ptr< vector< double > > Rbv = m_Layers[ i ]->R_dir_dir_band( Side::Back, aDirection );
      shared_ptr< CSeries > T = make_shared< CSeries >();
      shared_ptr< CSeries > Rf = make_shared< CSeries >();
      shared_ptr< CSeries > Rb = make_shared< CSeries >();
      for( size_t j = 0; j < wl->size(); ++j ) {
        T->addProperty( ( *wl )[ j ], ( *Tv )[ j ] );
        Rf->addProperty( ( *wl )[ j ], ( *Rfv )[ j ] );
        Rb->addProperty( ( *wl )[ j ], ( *Rbv )[ j ] );
      }
      T = T->interpolate( m_CommonWavelengths );
      Rf = Rf->interpolate( m_CommonWavelengths );
      Rb = Rb->interpolate( m_CommonWavelengths );
      if( i == 0 ) {
        aEqLayer = make_shared< CEquivalentLayer >( T, Rf, Rb );
        aAbs = make_shared< CAbsorptancesMultiPane >( T, Rf, Rb );
      } else {
        assert( aEqLayer != nullptr );
        assert( aAbs != nullptr );
        aEqLayer->addLayer( T, Rf, Rb );
        aAbs->addLayer( T, Rf, Rb );
      }
    }
    assert( aEqLayer != nullptr );
    assert( aAbs != nullptr );

    shared_ptr< CEquivalentLayerAngle > newLayer = make_shared< CEquivalentLayerAngle >( aEqLayer, aAbs, t_Angle );

    m_EquivalentAngle.push_back( newLayer );

    return newLayer;

  };


}