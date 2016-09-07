#ifndef MULTIPANESPECTRALSAMPLE_H
#define MULTIPANESPECTRALSAMPLE_H

#include <memory>
#include <vector>

#include "SpectralSample.hpp"

namespace FenestrationCommon {

  class CSeries;

}

namespace SpectralAveraging {

  class CSpectralSampleData;  
  //class CSample;
}

namespace MultiPane {

  class CMultiPaneSampleData;

  class CMultiPaneSpectralSample : public SpectralAveraging::CSpectralSample {
  public:
    CMultiPaneSpectralSample( const std::shared_ptr< SpectralAveraging::CSpectralSampleData >& t_SampleData,
      const std::shared_ptr< FenestrationCommon::CSeries >& t_SourceData );

    double getLayerAbsorbedEnergy( double const minLambda, double const maxLambda, size_t const Index );
    double getLayerAbsorptance( double const minLambda, double const maxLambda, size_t const Index );
    
  private:
    void reset();
    void calculateProperties();
    void integrateAndAppendAbsorptances( const std::shared_ptr< FenestrationCommon::CSeries >& t_Absorptances );

    std::vector< std::shared_ptr< FenestrationCommon::CSeries > > m_AbsorbedLayersSource;
  
  };

}

#endif