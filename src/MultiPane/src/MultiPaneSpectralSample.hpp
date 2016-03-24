#ifndef MULTIPANESPECTRALSAMPLE_H
#define MULTIPANESPECTRALSAMPLE_H

#include <memory>
#include <vector>

#include "SpectralSample.hpp"

namespace SpectralAveraging {

  class CSpectralSampleData;
  class CSpectralProperties;
  //class CSample;
}

namespace MultiPane {

  class CMultiPaneSampleData;

  class CMultiPaneSpectralSample : public SpectralAveraging::CSpectralSample {
  public:
    CMultiPaneSpectralSample( std::shared_ptr< SpectralAveraging::CSpectralSampleData > t_SampleData,
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_SourceData );

    double getLayerAbsorbedEnergy( double const minLambda, double const maxLambda, size_t const Index );
    double getLayerAbsorptance( double const minLambda, double const maxLambda, size_t const Index );
    
  private:
    void reset();
    void calculateProperties();
    void integrateAndAppendAbsorptances( std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Absorptances );

    std::vector< std::shared_ptr< SpectralAveraging::CSpectralProperties > > m_AbsorbedLayersSource;
  
  };

}

#endif