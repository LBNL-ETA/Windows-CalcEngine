#include <stdexcept>
#include <assert.h>

#include "SpectralSample.hpp"
#include "MeasuredSampleData.hpp"
#include "Series.hpp"
#include "IntegratorStrategy.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SpectralAveraging {

  //////////////////////////////////////////////////////////////////////////////////////
  ////  CSample
  //////////////////////////////////////////////////////////////////////////////////////

  CSample::CSample( shared_ptr< CSeries > t_SourceData ) : m_SourceData( t_SourceData ),
    m_WavelengthSet( WavelengthSet::Data ), m_IntegrationType( IntegrationType::Trapezoidal ),
    m_StateCalculated( false ) {
    m_DetectorData = nullptr;
    m_Wavelengths = nullptr;
    reset();
  }

  shared_ptr< CSeries > CSample::getSourceData() {
    calculateState(); // must interpolate data to same wavelengths
    return m_SourceData; 
  }

  void CSample::setDetectorData( shared_ptr< CSeries > t_DetectorData ) {
    m_DetectorData = t_DetectorData;
    reset();
  }

  void CSample::assignDetectorAndWavelengths( shared_ptr< CSample > t_Sample ) {
    m_DetectorData = t_Sample->m_DetectorData;
    m_Wavelengths = t_Sample->m_Wavelengths;
    m_WavelengthSet = t_Sample->m_WavelengthSet;
  }

  void CSample::setWavelengths( WavelengthSet t_WavelengthSet, shared_ptr< vector< double > > t_Wavelenghts ) {
    m_WavelengthSet = t_WavelengthSet;
    switch ( t_WavelengthSet ) {
    case WavelengthSet::Custom:
      if( t_Wavelenghts == nullptr ) {
        throw runtime_error("Need to provide custom wavelength set.");
      }
      m_Wavelengths = t_Wavelenghts;
      break;
    case WavelengthSet::Source:
      if( m_SourceData == nullptr ) {
        throw runtime_error("Cannot extract wavelenghts from source. Source is empty.");
      }
      m_Wavelengths = m_SourceData->getXArray();
      break;
    case WavelengthSet::Data:
      m_Wavelengths = getWavelengthsFromSample();
      break;
    default:
      throw runtime_error("Incorrect definition of wavelength set source.");
      break;
    }
    reset();
  }

  double CSample::getEnergy( const double minLambda, const double maxLambda, const Property t_Property, const Side t_Side ) {
    calculateState();
    double Energy = 0;
    switch ( t_Property ) {
    case Property::T:
      Energy = m_TransmittedSource->sum( minLambda, maxLambda );
      break;
    case Property::R:
      switch ( t_Side ) {
      case Side::Front:
        Energy = m_ReflectedFrontSource->sum( minLambda, maxLambda );
        break;
      case Side::Back:
        Energy = m_ReflectedBackSource->sum( minLambda, maxLambda );
        break;
      default:
        assert("Incorrect selection of sample side.");
        break;
      }
      break;
    case Property::Abs:
      switch ( t_Side ) {
      case Side::Front:
        Energy = m_AbsorbedFrontSource->sum( minLambda, maxLambda );
        break;
      case Side::Back:
        Energy = m_AbsorbedBackSource->sum( minLambda, maxLambda );
        break;
      default:
        assert("Incorrect selection of sample side.");
        break;
      }
      break;
    default:
      assert("Incorrect selection of sample property.");
      break;
    }

    return Energy;
  }

  double CSample::getProperty( const double minLambda, const double maxLambda, const Property t_Property, const Side t_Side ) {
    calculateState();
    double Prop = 0;
    double incomingEnergy = m_IncomingSource->sum( minLambda, maxLambda );
    double propertyEnergy = 0;
    switch ( t_Property ) {
    case Property::T:
      propertyEnergy = m_TransmittedSource->sum( minLambda, maxLambda );
      break;
    case Property::R:
      switch ( t_Side ) {
      case Side::Front:
        propertyEnergy = m_ReflectedFrontSource->sum( minLambda, maxLambda );
        break;
      case Side::Back:
        propertyEnergy = m_ReflectedBackSource->sum( minLambda, maxLambda );
        break;
      default:
        assert("Incorrect selection of sample side.");
        break;
      }
      break;
    case Property::Abs:
      switch ( t_Side ) {
      case Side::Front:
        propertyEnergy = m_AbsorbedFrontSource->sum( minLambda, maxLambda );
        break;
      case Side::Back:
        propertyEnergy = m_AbsorbedBackSource->sum( minLambda, maxLambda );
        break;
      default:
        assert("Incorrect selection of sample side.");
        break;
      }
      break;
    default:
      throw runtime_error("Incorrect selection of sample property.");
      break;
    }

    Prop = propertyEnergy / incomingEnergy;

    return Prop;
  }

  shared_ptr< CSeries > CSample::getEnergyProperties( const Property t_Property, const Side t_Side ) {
    calculateState();

    shared_ptr< CSeries > aProperty = nullptr;
    switch ( t_Property ) {
    case Property::T:
      aProperty = m_TransmittedSource;
      break;
    case Property::R:
      switch ( t_Side ) {
      case Side::Front:
        aProperty = m_ReflectedFrontSource;
        break;
      case Side::Back:
        aProperty = m_ReflectedBackSource;
        break;
      default:
        assert("Incorrect selection of sample side.");
        break;
      }
      break;
    case Property::Abs:
      switch ( t_Side ) {
      case Side::Front:
        aProperty = m_AbsorbedFrontSource;
        break;
      case Side::Back:
        aProperty = m_AbsorbedBackSource;
        break;
      default:
        assert("Incorrect selection of sample side.");
        break;
      }
      break;
    default:
      throw runtime_error("Incorrect selection of sample property.");
      break;
    }

    return aProperty;
  }

  size_t CSample::getBandSize() const {
    return m_Wavelengths->size();
  }

  void CSample::reset() {
    m_StateCalculated = false;
    m_IncomingSource = nullptr;
    m_TransmittedSource = nullptr;
    m_ReflectedFrontSource = nullptr;
    m_ReflectedBackSource = nullptr;
    m_AbsorbedFrontSource = nullptr;
    m_AbsorbedBackSource = nullptr;
  }

  void CSample::calculateState() {
    if( !m_StateCalculated ) {
      if( m_WavelengthSet != WavelengthSet::Custom ) {
        setWavelengths( m_WavelengthSet );
        assert( m_Wavelengths != nullptr );
      }
      if( m_Wavelengths == nullptr ) {
        throw runtime_error("Wavelength range is not set. Properties cannot be calculated without given wavelenght set.");
      }

      if( m_SourceData == nullptr ) {
        throw runtime_error("Source data are not set.");
      }
      m_IncomingSource = m_SourceData->interpolate( m_Wavelengths );

      if( m_DetectorData != nullptr ) {
        shared_ptr< CSeries > interpolatedDetector = m_DetectorData->interpolate( m_Wavelengths );
        m_IncomingSource = m_IncomingSource->mMult( interpolatedDetector );
      }

      calculateProperties();

      m_IncomingSource = m_IncomingSource->integrate( m_IntegrationType );
      m_TransmittedSource = m_TransmittedSource->integrate( m_IntegrationType );
      m_ReflectedFrontSource = m_ReflectedFrontSource->integrate( m_IntegrationType );
      m_ReflectedBackSource = m_ReflectedBackSource->integrate( m_IntegrationType );
      m_AbsorbedFrontSource = m_AbsorbedFrontSource->integrate( m_IntegrationType );
      m_AbsorbedBackSource = m_AbsorbedBackSource->integrate( m_IntegrationType );

      m_StateCalculated = true;
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////
  ////  CSpectralSample
  //////////////////////////////////////////////////////////////////////////////////////

  CSpectralSample::CSpectralSample( shared_ptr< CSpectralSampleData > t_SampleData, shared_ptr< CSeries > t_SourceData ) : 
    CSample( t_SourceData ), m_SampleData( t_SampleData) {
    if( t_SampleData == nullptr ) {
      throw runtime_error("Sample must have measured data.");
    }
    setWavelengths( m_WavelengthSet );
    shared_ptr< CSeries > m_Transmittance = nullptr;
    shared_ptr< CSeries > m_RefFront = nullptr;
    shared_ptr< CSeries > m_RefBack = nullptr;
    shared_ptr< CSeries > m_Abs = nullptr;
  }

  shared_ptr< CSpectralSampleData > CSpectralSample::getMeasuredData() {
    calculateState(); // Interpolation is needed before returning the data
    return m_SampleData; 
  }

  std::shared_ptr< std::vector< double > > CSpectralSample::getWavelengthsFromSample() {
    return m_SampleData->getWavelengths();
  }

  shared_ptr< CSeries > CSpectralSample::getWavelengthsProperty( const Property t_Property, const Side t_Side ) {
    calculateState();
    shared_ptr< CSeries > aProperty = nullptr;
    switch( t_Property ) {
      case Property::T:
        aProperty = m_Transmittance;
        break;
      case Property::R:
        switch ( t_Side ) {
        case Side::Front:
          aProperty = m_RefFront;
          break;
        case Side::Back:
          aProperty = m_RefBack;
          break;
        default:
          assert("Incorrect selection of Side.");
          break;
        }
        break;
      case Property::Abs:
        switch ( t_Side ) {
        case Side::Front:
          aProperty = m_AbsFront;
          break;
        case Side::Back:
          aProperty = m_AbsBack;
          break;
        default:
          assert("Incorrect selection of Side.");
          break;
        }
        break;
      default:
        assert("Non existent property requested.");
        break;
    }

    return aProperty;
  }

  void CSpectralSample::calculateProperties() {

    // No need to do interpolation if wavelength set is already from the data.
    if( m_WavelengthSet == WavelengthSet::Data ) {
      m_Transmittance = m_SampleData->properties( SampleData::T );
      m_RefFront = m_SampleData->properties( SampleData::Rf );
      m_RefBack = m_SampleData->properties( SampleData::Rb );
      m_AbsFront = m_SampleData->properties( SampleData::AbsF );
      m_AbsBack = m_SampleData->properties( SampleData::AbsB );
    } else {
      m_Transmittance = m_SampleData->properties( SampleData::T )->interpolate( m_Wavelengths );
      m_RefFront = m_SampleData->properties( SampleData::Rf )->interpolate( m_Wavelengths );
      m_RefBack = m_SampleData->properties( SampleData::Rb )->interpolate( m_Wavelengths );
      m_AbsFront = m_SampleData->properties( SampleData::AbsF )->interpolate( m_Wavelengths );
      m_AbsBack = m_SampleData->properties( SampleData::AbsB )->interpolate( m_Wavelengths );
    }

    assert( m_IncomingSource != nullptr );

    // Calculation of energy balances
    m_TransmittedSource = m_Transmittance->mMult( m_IncomingSource );
    m_ReflectedFrontSource = m_RefFront->mMult( m_IncomingSource );
    m_ReflectedBackSource = m_RefBack->mMult( m_IncomingSource );
    m_AbsorbedFrontSource = m_AbsFront->mMult( m_IncomingSource );
    m_AbsorbedBackSource = m_AbsBack->mMult( m_IncomingSource );
  }

}
