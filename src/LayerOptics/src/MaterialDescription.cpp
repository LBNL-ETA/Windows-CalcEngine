#include <assert.h>
#include <stdexcept>

#include "MaterialDescription.hpp"
#include "FenestrationCommon.hpp"
#include "WavelengthRange.hpp"
#include "SpectralSample.hpp"
#include "AngularSpectralSample.hpp"
#include "NIRRatio.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

namespace LayerOptics {

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Surface
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Surface::Surface( const double t_T, const double t_R ) {
    if( t_T + t_R > 1 ) {
      throw runtime_error("Sum of transmittance and reflectance is greater than one.");
    }
    m_Property[ Property::T ] = t_T;
    m_Property[ Property::R ] = t_R;
    m_Property[ Property::Abs ] = 1 - t_T - t_R;
  }

  double Surface::getProperty( const Property t_Property ) {
    return m_Property.at( t_Property );
  }

  ////////////////////////////////////////////////////////////////////////////////////
  ////   RMaterialProperties
  ////////////////////////////////////////////////////////////////////////////////////

  RMaterialProperties::RMaterialProperties( const double aTf, const double aTb, 
    const double aRf, const double aRb ) {

    m_Surface[ Side::Front ] = make_shared< Surface >( aTf, aRf );
    m_Surface[ Side::Back ] = make_shared< Surface >( aTb, aRb );

  }

  double RMaterialProperties::getProperty( const Property t_Property, const Side t_Side ) const {
    return m_Surface.at( t_Side )->getProperty( t_Property );
  }

  ////////////////////////////////////////////////////////////////////////////////////
  ////   CMaterialBand
  ////////////////////////////////////////////////////////////////////////////////////

  CMaterialBand::CMaterialBand( const double minLambda, const double maxLambda ) : 
    m_MinLambda( minLambda ), m_MaxLambda( maxLambda ), m_WavelengthsCalculated( false ) {
  
  }

  double CMaterialBand::getPropertyAtAngle( const Property t_Property, const Side t_Side, const double ) const {
    return getProperty( t_Property, t_Side ); // Default behavior is no angular dependence
  }

  shared_ptr< vector< double > > CMaterialBand::getBandPropertiesAtAngle( const Property t_Property, 
    const Side t_Side, const double  ) const {
    return getBandProperties( t_Property, t_Side );  // Default beahvior is no angular dependence
  }

  shared_ptr< vector< RMaterialProperties > > CMaterialBand::getBandProperties() {
    shared_ptr< vector< RMaterialProperties > > aProperties = make_shared< vector< RMaterialProperties > >();

    shared_ptr< vector< double > > Tf = getBandProperties( Property::T, Side::Front );
    shared_ptr< vector< double > > Tb = getBandProperties( Property::T, Side::Back );
    shared_ptr< vector< double > > Rf = getBandProperties( Property::R, Side::Front );
    shared_ptr< vector< double > > Rb = getBandProperties( Property::R, Side::Back );

    size_t size = getBandSize();
    for( size_t i = 0; i < size; ++i ) {
      RMaterialProperties aMaterial = 
        RMaterialProperties( ( *Tf )[ i ], ( *Tb )[ i ], ( *Rf )[ i ], ( *Rb )[ i ] );
      aProperties->push_back( aMaterial );
    }

    return aProperties;
  }

  shared_ptr< vector< double > > CMaterialBand::getBandWavelengths() {
    if( !m_WavelengthsCalculated )  {
      m_Wavelengths = calculateBandWavelengths();
    }
    return m_Wavelengths;
  }

  size_t CMaterialBand::getBandSize() {
   return getBandWavelengths()->size();
  }

  int CMaterialBand::getBandIndex( const double t_Wavelength ) {
    int aIndex = -1;
    size_t size = getBandSize();
    for( size_t i = 0; i < size; ++i ) {
      if( ( *m_Wavelengths )[ i ] < ( t_Wavelength + 1e-6 ) ) {
        ++aIndex;
      }
    }
    return aIndex;
  }

  double CMaterialBand::getMinLambda() const {
    return m_MinLambda;
  }

  double CMaterialBand::getMaxLambda() const {
    return m_MaxLambda;
  }

  ////////////////////////////////////////////////////////////////////////////////////
  ////   CMaterialSingleBand
  ////////////////////////////////////////////////////////////////////////////////////
  CMaterialSingleBand::CMaterialSingleBand( const double t_Tf, const double t_Tb, const double t_Rf, const double t_Rb,
    const double minLambda, const double maxLambda ) : CMaterialBand( minLambda, maxLambda ) {
	m_Property[ Side::Front ] = make_shared< Surface >( t_Tf, t_Rf );
	m_Property[ Side::Back ] = make_shared< Surface >( t_Tb, t_Rb );
  }

  double CMaterialSingleBand::getProperty( Property t_Property, Side t_Side ) const {
	return m_Property.at( t_Side )->getProperty( t_Property );
  }

  shared_ptr< vector< double > > CMaterialSingleBand::getBandProperties( 
    const Property t_Property, const Side t_Side ) const {
    shared_ptr< vector< double > > aResult = make_shared< vector< double > >();
    aResult->push_back( getProperty( t_Property, t_Side ) );
    return aResult;
  }

  shared_ptr< vector< double > > CMaterialSingleBand::calculateBandWavelengths() {
    shared_ptr< vector< double > > aWavelengths = make_shared< vector< double > >();
    aWavelengths->push_back( m_MinLambda );
    return aWavelengths;
  }

  ////////////////////////////////////////////////////////////////////////////////////
  ////   CMaterialDualBand
  ////////////////////////////////////////////////////////////////////////////////////

  CMaterialDualBand::CMaterialDualBand( shared_ptr< CMaterialBand > t_PartialRange,
    shared_ptr< CMaterialBand > t_SolarRange, const double t_Ratio ) : CMaterialBand( 0.3, 2.5 ),
    m_MaterialFullRange( t_SolarRange ) {
    checkIfMaterialWithingSolarRange( t_PartialRange );
    createUVRange();
    createNIRRange( t_PartialRange, t_SolarRange, t_Ratio );
  }

  CMaterialDualBand::CMaterialDualBand( shared_ptr< CMaterialBand > t_PartialRange,
    shared_ptr< CMaterialBand > t_SolarRange, 
    shared_ptr< CSeries > t_SolarRadiation ) : CMaterialBand( 0.3, 2.5 ),
    m_MaterialFullRange( t_SolarRange ) {
    checkIfMaterialWithingSolarRange( t_PartialRange );
    createUVRange();
    double lowLambda = t_PartialRange->getMinLambda();
    double highLambda = t_PartialRange->getMaxLambda();
    CNIRRatio nirRatio = CNIRRatio( t_SolarRadiation, lowLambda, highLambda );
    createNIRRange( t_PartialRange, t_SolarRange, nirRatio.ratio() );
  }

  double CMaterialDualBand::getProperty( Property t_Property, Side t_Side ) const {
    return m_MaterialFullRange->getProperty( t_Property, t_Side );
  }

  shared_ptr< vector< double > > CMaterialDualBand::getBandProperties( const Property t_Property, 
    const Side t_Side ) const {
    size_t aSize = m_Materials.size();
    shared_ptr< vector< double > > aResults = make_shared< vector< double > >();
    for( size_t i = 0; i < aSize; ++i ) {
      double value = m_Materials[ i ]->getProperty( t_Property, t_Side );
      aResults->push_back( value );
    }
    return aResults;
  }

  shared_ptr< vector< double > > CMaterialDualBand::calculateBandWavelengths() {
    shared_ptr< vector< double > > aWavelengths = make_shared< vector< double > >();
    size_t size = m_Materials.size();
    for( size_t i = 0; i < size; ++i ) {
      aWavelengths->push_back( m_Materials[ i ]->getMinLambda() );
    }

    return aWavelengths;
  }

  void CMaterialDualBand::checkIfMaterialWithingSolarRange( shared_ptr< CMaterialBand > t_Material ) const {
    double lowLambda = t_Material->getMinLambda();
    double highLambda = t_Material->getMaxLambda();
    if( lowLambda < 0.32 || highLambda < 0.32 || lowLambda > 2.5 || highLambda > 2.5 ) {
      throw runtime_error("Material properties out of range. Wavelength range must be between 0.32 and 2.5 microns.");
    }
  }

  void CMaterialDualBand::createUVRange() {
    double T = 0;
    double R = 0;
    double minLambda = 0.3;
    double maxLambda = 0.32;
    shared_ptr< CMaterialBand > aUVMaterial = make_shared< CMaterialSingleBand >( T, T, R, R, minLambda, maxLambda );
    m_Materials.push_back( aUVMaterial );
  }

  void CMaterialDualBand::createNIRRange( 
    shared_ptr< CMaterialBand > t_PartialRange,
    shared_ptr< CMaterialBand > t_SolarRange, const double t_Fraction ) {
    double Tf_nir = getModifiedProperty( t_PartialRange->getProperty( Property::T, Side::Front ), 
      t_SolarRange->getProperty( Property::T, Side::Front ), t_Fraction );
    double Tb_nir = getModifiedProperty( t_PartialRange->getProperty( Property::T, Side::Back ), 
      t_SolarRange->getProperty( Property::T, Side::Back ), t_Fraction );

    double Rf_nir = getModifiedProperty( t_PartialRange->getProperty( Property::R, Side::Front ), 
      t_SolarRange->getProperty( Property::R, Side::Front ), t_Fraction );
    double Rb_nir = getModifiedProperty( t_PartialRange->getProperty( Property::R, Side::Back ), 
      t_SolarRange->getProperty( Property::R, Side::Back ), t_Fraction );

    double minRangeLambda = t_PartialRange->getMinLambda();

    if( minRangeLambda > 0.32 ) {
      shared_ptr< CMaterialSingleBand > aMaterial = 
        make_shared< CMaterialSingleBand >( Tf_nir, Tb_nir, Rf_nir, Rb_nir, 0.32, minRangeLambda );
      m_Materials.push_back( aMaterial );
    }

    m_Materials.push_back( t_PartialRange );

    double maxRangeLambda = t_PartialRange->getMaxLambda();
    shared_ptr< CMaterialSingleBand > aMaterial = 
      make_shared< CMaterialSingleBand >( Tf_nir, Tb_nir, Rf_nir, Rb_nir, maxRangeLambda, 2.5 );
    m_Materials.push_back( aMaterial );
  }

  double CMaterialDualBand::getModifiedProperty( const double t_Range, 
    const double t_Solar, const double t_Fraction ) const {
    assert( t_Fraction != 1 );
    return ( t_Solar - t_Fraction * t_Range ) / ( 1 - t_Fraction );
  }

  ////////////////////////////////////////////////////////////////////////////////////
  ////   CMaterialSample
  ////////////////////////////////////////////////////////////////////////////////////

  CMaterialSample::CMaterialSample( shared_ptr< CSpectralSample > t_SpectralSample, 
    const double t_Thickness, const SpecularMaterialType t_Type, 
    const double minLambda, const double maxLambda ) : 
    CMaterialBand( minLambda, maxLambda ) {

    if( t_SpectralSample == nullptr ) {
      throw runtime_error("Cannot create specular material from non-existing sample.");
    }

    m_AngularSample = make_shared< CAngularSpectralSample >( t_SpectralSample, t_Thickness, t_Type );
    
  }

  double CMaterialSample::getPropertyAtAngle( const Property t_Property, 
    const Side t_Side, const double t_Angle ) const {
    assert( m_AngularSample );
    return m_AngularSample->getProperty( m_MinLambda, m_MaxLambda, t_Property, t_Side, t_Angle );
  }

  double CMaterialSample::getProperty( const Property t_Property, const Side t_Side ) const {
    return getPropertyAtAngle( t_Property, t_Side, 0 );
  }

  shared_ptr< vector< double > > CMaterialSample::getBandPropertiesAtAngle( const Property t_Property, 
    const Side t_Side, const double t_Angle ) const {
    assert( m_AngularSample );
    return m_AngularSample->getWavelengthsProperty( m_MinLambda, m_MaxLambda, t_Property, t_Side, t_Angle );
  }

  shared_ptr< vector< double > > CMaterialSample::getBandProperties( const Property t_Property, 
    const Side t_Side ) const {
    return getBandPropertiesAtAngle( t_Property, t_Side, 0 );
  }

  shared_ptr< vector< double > > CMaterialSample::calculateBandWavelengths() {
    return m_AngularSample->getBandWavelengths(); 
  }

}