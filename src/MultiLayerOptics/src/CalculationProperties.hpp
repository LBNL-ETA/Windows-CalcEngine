#pragma once

#include <optional>
#include <vector>

#include <WCECommon.hpp>

namespace MultiLayerOptics
{
    struct CalculationProperties
    {
        explicit CalculationProperties(
          const FenestrationCommon::CSeries & solarRadiation,
          std::optional<std::vector<double>> commonWavelengths = std::nullopt,
          std::optional<FenestrationCommon::CSeries> detectorData = std::nullopt);;

        FenestrationCommon::CSeries SolarRadiation;
        std::optional<std::vector<double>> CommonWavelengths;
        std::optional<FenestrationCommon::CSeries> DetectorData;

        [[nodiscard]] FenestrationCommon::CSeries scaledSolarRadiation() const;
    };

    class MultiPaneCalcluationsSetter
    {
        virtual void setCalculationProperties(const CalculationProperties & calcProperties) = 0;
    };
}   // namespace MultiLayerOptics