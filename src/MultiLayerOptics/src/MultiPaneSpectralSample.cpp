#include <iterator>
#include <stdexcept>

#include "MultiPaneSpectralSample.hpp"
#include "MultiPaneSampleData.hpp"
#include "WCESpectralAveraging.hpp"
#include "WCECommon.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

namespace MultiLayerOptics
{
    CMultiPaneSpectralSample::CMultiPaneSpectralSample(
      const std::shared_ptr<CSpectralSampleData> & t_SampleData, const CSeries & t_SourceData) :
        CSpectralSample(t_SampleData, t_SourceData),
        m_AbsorbedLayersSource{{Side::Front, std::vector<CSeries>()},
                               {Side::Back, std::vector<CSeries>()}}
    {}

    double CMultiPaneSpectralSample::getLayerAbsorbedEnergy(double const minLambda,
                                                            double const maxLambda,
                                                            size_t const Index,
                                                            Side side)
    {
        double aEnergy = 0;
        calculateState(IntegrationType::Trapezoidal, 1);
        if(Index > m_AbsorbedLayersSource.size())
        {
            throw std::runtime_error("Index for glazing layer absorptance is out of range.");
        }
        aEnergy = m_AbsorbedLayersSource.at(side)[Index - 1].sum(minLambda, maxLambda);
        return aEnergy;
    }

    double CMultiPaneSpectralSample::getLayerAbsorptance(double const minLambda,
                                                         double const maxLambda,
                                                         size_t const Index,
                                                         Side side)
    {
        calculateState(IntegrationType::Trapezoidal, 1);
        double absorbedEnergy = getLayerAbsorbedEnergy(minLambda, maxLambda, Index, side);
        double incomingEnergy = m_IncomingSource.sum(minLambda, maxLambda);
        return absorbedEnergy / incomingEnergy;
    }

    void
      CMultiPaneSpectralSample::calculateProperties(FenestrationCommon::IntegrationType integrator,
                                                    double normalizationCoefficient)
    {
        if(!m_StateCalculated)
        {
            for(const auto side : EnumSide())
            {
                CSpectralSample::calculateProperties(integrator, normalizationCoefficient);
                if(std::dynamic_pointer_cast<CMultiPaneSampleData>(m_SampleData) != NULL)
                {
                    std::shared_ptr<CMultiPaneSampleData> aSample =
                      std::dynamic_pointer_cast<CMultiPaneSampleData>(m_SampleData);

                    const size_t numOfLayers = aSample->numberOfLayers();
                    for(size_t i = 0; i < numOfLayers; ++i)
                    {
                        auto layerAbsorbed = aSample->getLayerAbsorptances(i + 1, side);
                        integrateAndAppendAbsorptances(
                          layerAbsorbed, side, integrator, normalizationCoefficient);
                    }
                }
                else
                {
                    auto layerAbsorbed = m_SampleData->properties(Property::Abs, side);
                    integrateAndAppendAbsorptances(layerAbsorbed, side, integrator, normalizationCoefficient);
                }
            }

            m_StateCalculated = true;
        }
    }

    void CMultiPaneSpectralSample::integrateAndAppendAbsorptances(const CSeries & t_Absorptances,
                                                                  Side side,
                                                                  FenestrationCommon::IntegrationType integrator,
                                                                  double normalizationCoefficient)
    {
        CSeries aAbs = t_Absorptances;
        if(m_WavelengthSet != WavelengthSet::Data)
        {
            aAbs = aAbs.interpolate(m_Wavelengths);
        }
        aAbs = aAbs * m_IncomingSource;
        aAbs = aAbs.integrate(integrator, normalizationCoefficient);
        m_AbsorbedLayersSource.at(side).push_back(aAbs);
    }

    void CMultiPaneSpectralSample::reset()
    {
        CSpectralSample::reset();
        for(const auto side : EnumSide())
        {
            m_AbsorbedLayersSource.at(side).clear();
        }
    }

}   // namespace MultiLayerOptics
