#include <vector>
#include <algorithm>
#include <assert.h>
#include <math.h>

#include "AngularSpectralSample.hpp"
#include "MeasuredSampleData.hpp"
#include "Series.hpp"
#include "SpectralSample.hpp"
#include "AngularProperties.hpp"
#include "SpecularLayer.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SpectralAveraging {

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //// CAngularSpectralProperties
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  CAngularSpectralProperties::CAngularSpectralProperties( shared_ptr< CSpectralSample > t_SpectralSample, 
    const double t_Angle, const SpecularMaterialType t_Type, const double t_Thickness ) : 
    m_Angle( t_Angle ), m_Thickness( t_Thickness ) {

    m_AngularData = make_shared< CSpectralSampleData >();
    calculateAngularProperties( t_SpectralSample, t_Type );
  };

  double CAngularSpectralProperties::angle() const {
    return m_Angle;
  };

  shared_ptr< CSpectralSampleData > CAngularSpectralProperties::properties() const {
    return m_AngularData;
  };

  void CAngularSpectralProperties::calculateAngularProperties( shared_ptr< CSpectralSample > t_SpectralSample, 
    const SpecularMaterialType t_Type ) {

    assert( t_SpectralSample != nullptr );

    shared_ptr< CSpectralSampleData > aMeasuredData = t_SpectralSample->getMeasuredData();

    if( m_Angle != 0 ) {
      shared_ptr< CSeries > aSourceData = t_SpectralSample->getSourceData();

      shared_ptr< vector< double > > aWavelengths = aMeasuredData->getWavelengths();
      shared_ptr< CSeries > aT = aMeasuredData->properties( SampleData::T );
      assert( aT->size() == aWavelengths->size() );

      shared_ptr< CSeries > aRf = aMeasuredData->properties( SampleData::Rf );
      assert( aRf->size() == aWavelengths->size() );

      shared_ptr< CSeries > aRb = aMeasuredData->properties( SampleData::Rb );
      assert( aRb->size() == aWavelengths->size() );

      double lowLambda = 0.3;
      double highLambda = 2.5;

      // TODO: Only one side is measured and it is considered that front properties are equal to back properties
      double aTSolNorm = t_SpectralSample->getProperty( lowLambda, highLambda, Property::T, Side::Front );

      for( size_t i = 0; i < aWavelengths->size(); ++i ) {
        double ww = ( *aWavelengths )[ i ] * 1e-6;
        double T = ( *aT )[ i ]->value();
        double Rf = ( *aRf )[ i ]->value();
        double Rb = ( *aRb )[ i ]->value();

        SpecularLayerSurfaceType aSurfaceType = coatingType( t_Type );

        CAngularPropertiesFactory aFrontFactory = CAngularPropertiesFactory( T, Rf, m_Thickness, aTSolNorm );
        CAngularPropertiesFactory aBackFactory = CAngularPropertiesFactory( T, Rb, m_Thickness, aTSolNorm );

        shared_ptr< CAngularProperties > aFrontProperties = aFrontFactory.getAngularProperties( aSurfaceType );
        shared_ptr< CAngularProperties > aBackProperties = aBackFactory.getAngularProperties( aSurfaceType );

        double Tangle = aFrontProperties->transmittance( m_Angle, ww );
        double Rfangle = aFrontProperties->reflectance( m_Angle, ww );
        double Rbangle = aBackProperties->reflectance( m_Angle, ww );

        m_AngularData->addRecord( ww * 1e6, Tangle, Rfangle, Rbangle );
      } 
    } else {
      m_AngularData = aMeasuredData;
    }
    
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //// CSpectralSampleAngle
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  CSpectralSampleAngle::CSpectralSampleAngle( shared_ptr< CSpectralSample > t_Sample, const double t_Angle ) :
    m_Sample( t_Sample ), m_Angle( t_Angle ) {
    
  };

  double CSpectralSampleAngle::angle() const {
    return m_Angle;  
  };
  
  shared_ptr< CSpectralSample > CSpectralSampleAngle::sample() const {
    return m_Sample; 
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //// CAngularSpectralSample
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  CAngularSpectralSample::CAngularSpectralSample( shared_ptr< CSpectralSample > t_SpectralSample, 
    const double t_Thickness, const FenestrationCommon::SpecularMaterialType t_Type ) :
    m_SpectralSampleZero( t_SpectralSample ), m_Thickness( t_Thickness ), m_Type( t_Type ) {
    
  };

  double CAngularSpectralSample::getProperty( double const minLambda, double const maxLambda, 
    const Property t_Property, const Side t_Side, const double t_Angle ) {
    shared_ptr< CSpectralSample > aSample = findSpectralSample( t_Angle );
    
    return aSample->getProperty( minLambda, maxLambda, t_Property, t_Side );
  };

  shared_ptr< vector< double > > CAngularSpectralSample::getWavelengthsProperty( 
    double const minLambda, double const maxLambda, 
    const Property t_Property, const Side t_Side, const double t_Angle ) {

    shared_ptr< CSpectralSample > aSample = findSpectralSample( t_Angle );

    shared_ptr< CSeries > aProperties = aSample->getWavelengthsProperty( t_Property, t_Side );

    shared_ptr< vector< double > > aValues = make_shared< vector< double > >();

    for( shared_ptr< CSeriesPoint > aProperty : *aProperties ) {
      if( aProperty->x() >= minLambda && aProperty->x() <= maxLambda ) {
        aValues->push_back( aProperty->value() );
      }
    }
    
    return aValues;
  };

  shared_ptr< vector< double > > CAngularSpectralSample::getBandWavelengths() const {
    return m_SpectralSampleZero->getWavelengthsFromSample();
  };

  shared_ptr< CSpectralSample > CAngularSpectralSample::findSpectralSample( const double t_Angle ) {
    shared_ptr< CSpectralSample > aSample = nullptr;

    vector < shared_ptr< CSpectralSampleAngle > >::iterator it;
  
    it = find_if( m_SpectralProperties.begin(), m_SpectralProperties.end(), 
      [ &t_Angle ]( shared_ptr< CSpectralSampleAngle > obj ) { return fabs( obj->angle() - t_Angle ) < 1e-6; } );

    if( it != m_SpectralProperties.end() ) {
      aSample = ( *it )->sample();
    } else {
      shared_ptr< CAngularSpectralProperties > aAngularData = 
        make_shared< CAngularSpectralProperties >( m_SpectralSampleZero, t_Angle, m_Type, m_Thickness );

      aSample = make_shared< CSpectralSample >( aAngularData->properties(), m_SpectralSampleZero->getSourceData() );
      aSample->assignDetectorAndWavelengths( m_SpectralSampleZero );
      shared_ptr< CSpectralSampleAngle > aSpectralSampleAngle = 
        make_shared< CSpectralSampleAngle >( aSample, t_Angle );
      m_SpectralProperties.push_back( aSpectralSampleAngle );
    }

    return aSample;
  };

}