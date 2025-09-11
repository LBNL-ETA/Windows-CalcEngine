#include "CalculationProperties.hpp"

namespace SingleLayerOptics
{

    CalculationProperties::CalculationProperties(
      const FenestrationCommon::CSeries & solarRadiation,
      std::optional<std::vector<double>> commonWavelengths,
      std::optional<FenestrationCommon::CSeries> detectorData,
      FenestrationCommon::IntegrationType integrationType,
      double normalizationCoefficient) :
        SolarRadiation(solarRadiation),
        CommonWavelengths(std::move(commonWavelengths)),
        DetectorData(std::move(detectorData)),
        m_IntegrationType(integrationType),
        m_NormalizationCoefficient(normalizationCoefficient)
    {}

    FenestrationCommon::CSeries CalculationProperties::scaledSolarRadiation() const
    {
        FenestrationCommon::CSeries result{SolarRadiation};
        if(CommonWavelengths.has_value())
        {
            result = result.interpolate(CommonWavelengths.value());
        }

        if(isDetectorDataValid())
        {
            result = result * DetectorData.value().interpolate(result.getXArray());
        }
        return result;
    }
    bool CalculationProperties::isDetectorDataValid() const
    {
        return DetectorData.has_value() && DetectorData.value().size() != 0;
    }
}   // namespace SingleLayerOptics