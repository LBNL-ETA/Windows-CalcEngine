#ifndef MULTIPANEMEASUREDDATA_H
#define MULTIPANEMEASUREDDATA_H

#include "MeasuredSampleData.hpp"

namespace FenestrationCommon {

  class CSeries;

}

namespace MultiPane {

  // Contain multiple spectral data samples. It also calculates multipane properties (Transmittance,
  // Front reflectance and back reflectance)
  class CMultiPaneSampleData : public SpectralAveraging::CSpectralSampleData {
  public:
    CMultiPaneSampleData();

    void addSample( const std::shared_ptr< CSpectralSampleData >& t_Sample );
    std::shared_ptr< FenestrationCommon::CSeries > getLayerAbsorptances( size_t const Index );
    std::shared_ptr< std::vector< double > > getWavelengths();
    size_t numberOfLayers() const;

  private:
    void calculateProperties();

    void interpolate( const std::vector< double >& t_Wavelengths );
    void calculateEquivalentProperties();

    std::vector< std::shared_ptr< CSpectralSampleData > > m_MeasuredSamples;
    std::vector< std::shared_ptr< FenestrationCommon::CSeries > > m_LayerAbsorptances;

  };

}

#endif