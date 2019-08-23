#include "MultiPanePhotovoltaic.hpp"

#include <cassert>

namespace MultiLayerOptics
{
    ///////////////////////////////////////////////////////////////////////////////////////
    // CMultiPanePhotovoltaic
    ///////////////////////////////////////////////////////////////////////////////////////

    CMultiPanePhotovoltaic::CMultiPanePhotovoltaic(
      const std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> & layers,
      const FenestrationCommon::CSeries & t_SolarRadiation,
      const FenestrationCommon::CSeries & t_DetectorData) :
        CMultiPaneSpecular(layers, t_SolarRadiation, t_DetectorData)
    {}

    CMultiPanePhotovoltaic::CMultiPanePhotovoltaic(
      std::vector<double> const & t_CommonWavelength,
      const FenestrationCommon::CSeries & t_SolarRadiation,
      const std::shared_ptr<SingleLayerOptics::SpecularLayer> & t_Layer) :
        CMultiPaneSpecular(t_CommonWavelength, t_SolarRadiation, t_Layer)
    {}

    double
      CMultiPanePhotovoltaic::AbsHeat(const size_t Index,
                                      const double t_Angle,
                                      const double minLambda,
                                      const double maxLambda,
                                      const FenestrationCommon::IntegrationType t_IntegrationType,
                                      const double normalizationCoefficient)
    {
        if(dynamic_cast<SingleLayerOptics::PhotovoltaicLayer *>(m_Layers[Index - 1].get())
           != nullptr)
        {
            CEquivalentLayerSingleComponentMWAngle aAngularProperties = getAngular(t_Angle);
            auto aLayer =
              dynamic_cast<SingleLayerOptics::PhotovoltaicLayer *>(m_Layers[Index - 1].get());

            FenestrationCommon::CSeries AbsHeat =
              aAngularProperties.AbsBySide(Index - 1, FenestrationCommon::Side::Front)
                * aLayer->PCE(FenestrationCommon::Side::Front)
              + aAngularProperties.AbsBySide(Index - 1, FenestrationCommon::Side::Back)
                  * aLayer->PCE(FenestrationCommon::Side::Back);

            auto aMult = AbsHeat * m_SolarRadiation;

            auto iIntegrated = aMult.integrate(t_IntegrationType, normalizationCoefficient);

            double totalProperty = iIntegrated->sum(minLambda, maxLambda);
            double totalSolar =
              m_SolarRadiation.integrate(t_IntegrationType, normalizationCoefficient)
                ->sum(minLambda, maxLambda);

            assert(totalSolar > 0);
            return totalProperty / totalSolar;
        }
        else
        {
            return Abs(
              Index, t_Angle, minLambda, maxLambda, t_IntegrationType, normalizationCoefficient);
        }
    }
}   // namespace MultiLayerOptics