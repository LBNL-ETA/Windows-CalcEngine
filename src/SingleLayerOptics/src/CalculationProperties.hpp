#pragma once

#include <optional>
#include <vector>

#include "WCECommon.hpp"

namespace SingleLayerOptics
{
    struct CalculationProperties
    {
        CalculationProperties() = default;
        explicit CalculationProperties(
          const FenestrationCommon::CSeries & solarRadiation,
          std::optional<std::vector<double>> commonWavelengths = std::nullopt,
          std::optional<FenestrationCommon::CSeries> detectorData = std::nullopt,
          FenestrationCommon::IntegrationType integrationType =
            FenestrationCommon::IntegrationType::Trapezoidal,
          double normalizationCoefficient = 1.0);

        FenestrationCommon::CSeries SolarRadiation;
        std::optional<std::vector<double>> CommonWavelengths;
        std::optional<FenestrationCommon::CSeries> DetectorData;

        FenestrationCommon::IntegrationType m_IntegrationType;
        double m_NormalizationCoefficient;

        [[nodiscard]] FenestrationCommon::CSeries scaledSolarRadiation() const;
        bool isDetectorDataValid() const;
    };
}   // namespace MultiLayerOptics