#include "MultiPanePhotovoltaic.hpp"

#include <cassert>

namespace MultiLayerOptics
{
    ///////////////////////////////////////////////////////////////////////////////////////
    // CMultiPanePhotovoltaic
    ///////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<CMultiPanePhotovoltaic> CMultiPanePhotovoltaic::create(
      const std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> & layers,
      const FenestrationCommon::CSeries & t_SolarRadiation,
      const FenestrationCommon::CSeries & t_DetectorData)
    {
        return std::unique_ptr<CMultiPanePhotovoltaic>(
          new CMultiPanePhotovoltaic(layers, t_SolarRadiation, t_DetectorData));
    }

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

            auto frontJscPrime = aLayer->jscPrime(FenestrationCommon::Side::Front);
            auto backJscPrime = aLayer->jscPrime(FenestrationCommon::Side::Back);

            auto IPlus = aAngularProperties.iplus(Index - 1);
            auto IMinus = aAngularProperties.iminus(Index - 1);

            auto frontJsc = frontJscPrime * IMinus;
            auto backJsc = backJscPrime * IPlus;

            auto testHeat = aAngularProperties.AbsBySide(Index, FenestrationCommon::Side::Front);

            FenestrationCommon::CSeries AbsHeat =
                aAngularProperties.AbsBySide(Index, FenestrationCommon::Side::Front);
            //    * aLayer->W(FenestrationCommon::Side::Front)
            //  + aAngularProperties.AbsBySide(Index, FenestrationCommon::Side::Back)
            //      * aLayer->W(FenestrationCommon::Side::Back);

            const auto aMult = AbsHeat * m_SolarRadiation;

            const auto iIntegrated = aMult.integrate(t_IntegrationType, normalizationCoefficient);

            const double totalProperty = iIntegrated->sum(minLambda, maxLambda);
            const double totalSolar =
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

    double
      CMultiPanePhotovoltaic::AbsElectricity(size_t Index,
                                             double t_Angle,
                                             double minLambda,
                                             double maxLambda,
                                             FenestrationCommon::IntegrationType t_IntegrationType,
                                             double normalizationCoefficient)
    {
        return Abs(
                 Index, t_Angle, minLambda, maxLambda, t_IntegrationType, normalizationCoefficient)
               - AbsHeat(Index,
                         t_Angle,
                         minLambda,
                         maxLambda,
                         t_IntegrationType,
                         normalizationCoefficient);
    }
}   // namespace MultiLayerOptics