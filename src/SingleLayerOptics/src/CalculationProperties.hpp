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

        FenestrationCommon::IntegrationType m_IntegrationType{
          FenestrationCommon::IntegrationType::Trapezoidal};
        double m_NormalizationCoefficient{1.0};

        [[nodiscard]] FenestrationCommon::CSeries scaledSolarRadiation() const;
        [[nodiscard]] bool isDetectorDataValid() const;
    };
}   // namespace SingleLayerOptics