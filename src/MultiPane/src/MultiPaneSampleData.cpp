#include <vector>
#include <algorithm>
#include <iterator>
#include <assert.h>

#include "MultiPaneSampleData.hpp"
#include "SpectralProperties.hpp"
#include "EquivalentLayer.hpp"
#include "AbsorptancesMultiPane.hpp"
#include "CommonWavelengths.hpp"

using namespace std;
using namespace SpectralAveraging;

namespace MultiPane {

  CMultiPaneSampleData::CMultiPaneSampleData() : CSpectralSampleData() {
    
  }

  shared_ptr< vector< double > > CMultiPaneSampleData::getWavelengths() {
    CCommonWavelengths aWavelengths;

    vector< shared_ptr< CSpectralSampleData > >::iterator it;
    for( it = m_MeasuredSamples.begin(); it < m_MeasuredSamples.end(); ++it ) {
      aWavelengths.addWavelength( ( *it )->getWavelengths() );
    }

    // wavelengths will be combined into common one and no extrapolation will be done
    return aWavelengths.getCombinedWavelengths( Combine::Interpolate );
  }

  size_t CMultiPaneSampleData::numberOfLayers() const {
    return m_MeasuredSamples.size(); 
  };

  void CMultiPaneSampleData::addSample( shared_ptr< CSpectralSampleData > t_Sample ) {
    m_MeasuredSamples.push_back( t_Sample );
  };

  void CMultiPaneSampleData::calculateProperties() {
    if( !m_absCalculated ) {
      calculateEquivalentProperties();
      m_absCalculated = true;
    }
  };

  shared_ptr< CSpectralProperties > CMultiPaneSampleData::getLayerAbsorptances( size_t const Index ) {
    calculateProperties();
    if( ( Index - 1 ) > m_LayerAbsorptances.size() ) {
      throw runtime_error("Index out of range. ");
    }
    return m_LayerAbsorptances[ Index - 1 ];
  };

  // Interpolate current sample data to new wavelengths set
  void CMultiPaneSampleData::interpolate( shared_ptr< vector< double > > t_Wavelengths ) {
    vector< shared_ptr< CSpectralSampleData > >::iterator it;
    for( it = m_MeasuredSamples.begin(); it < m_MeasuredSamples.end(); ++it) {
      ( *it )->interpolate( t_Wavelengths );
    }

    CSpectralSampleData::interpolate( t_Wavelengths );
  };

  void CMultiPaneSampleData::calculateEquivalentProperties() {
    shared_ptr< vector< double > > wavelengths =  getWavelengths();
    interpolate( wavelengths );

    assert( m_MeasuredSamples.size() != 0 );

    shared_ptr< CSpectralProperties > T = m_MeasuredSamples[0]->properties( SampleData::T );
    shared_ptr< CSpectralProperties > Rf = m_MeasuredSamples[0]->properties( SampleData::Rf );
    shared_ptr< CSpectralProperties > Rb = m_MeasuredSamples[0]->properties( SampleData::Rb );
    CEquivalentLayer aEqivalentLayer( T, Rf, Rb );
    CAbsorptancesMultiPane aAbsorptances( T, Rf, Rb );

    vector< shared_ptr< CSpectralSampleData > >::iterator it;
    for( it = next( m_MeasuredSamples.begin() ); it < m_MeasuredSamples.end(); ++it) {
      aEqivalentLayer.addLayer( ( *it )->properties( SampleData::T ),
        ( *it )->properties( SampleData::Rf ), ( *it )->properties( SampleData::Rb ) );
      aAbsorptances.addLayer( ( *it )->properties( SampleData::T ),
        ( *it )->properties( SampleData::Rf ), ( *it )->properties( SampleData::Rb ) );
    }

    m_Transmittances = aEqivalentLayer.T();
    m_ReflectancesFront = aEqivalentLayer.Rf();
    m_ReflectancesBack = aEqivalentLayer.Rb();
    m_AbsorptancesFront = aEqivalentLayer.AbsF();
    m_AbsorptancesBack = aEqivalentLayer.AbsB();

    m_LayerAbsorptances.clear();
    size_t size = aAbsorptances.numOfLayers();
    for( size_t i = 0; i < size; ++i ) {
      m_LayerAbsorptances.push_back( aAbsorptances.Abs( i ) );
    }

  };

}