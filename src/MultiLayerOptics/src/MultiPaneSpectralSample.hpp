#ifndef MultiPaneSpectralSample_H
#define MultiPaneSpectralSample_H

#include <memory>
#include <vector>

#include "WCESpectralAveraging.hpp"

namespace FenestrationCommon
{
    class CSeries;
}

namespace SpectralAveraging
{
    class CSpectralSampleData;
    // class CSample;
}   // namespace SpectralAveraging

namespace MultiLayerOptics
{
    class CMultiPaneSampleData;

    class CMultiPaneSpectralSample : public SpectralAveraging::CSpectralSample
    {
    public:
        CMultiPaneSpectralSample(
          const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & t_SampleData,
          const FenestrationCommon::CSeries & t_SourceData);

        double getLayerAbsorbedEnergy(double const minLambda,
                                      double const maxLambda,
                                      size_t const Index,
                                      FenestrationCommon::Side side);
        double
          getLayerAbsorptance(double const minLambda, double const maxLambda, size_t const Index, FenestrationCommon::Side side);

    private:
        void reset();
        void calculateProperties(FenestrationCommon::IntegrationType integrator,
                                 double m_NormalizationCoefficient);
        void integrateAndAppendAbsorptances(const FenestrationCommon::CSeries & t_Absorptances, FenestrationCommon::Side side,FenestrationCommon::IntegrationType integrator,
                                            double m_NormalizationCoefficient);

        std::map<FenestrationCommon::Side, std::vector<FenestrationCommon::CSeries>> m_AbsorbedLayersSource;
    };

}   // namespace MultiLayerOptics

#endif
