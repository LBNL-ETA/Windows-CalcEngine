#include "CalculationProperties.hpp"

namespace MultiLayerOptics{

    CalculationProperties::CalculationProperties(
      const FenestrationCommon::CSeries & solarRadiation,
      std::optional<std::vector<double>> commonWavelengths,
      std::optional<FenestrationCommon::CSeries> detectorData) :
        SolarRadiation(solarRadiation),
        CommonWavelengths(std::move(commonWavelengths)),
        DetectorData(std::move(detectorData)){}

    FenestrationCommon::CSeries CalculationProperties::scaledSolarRadiation() const
    {
        FenestrationCommon::CSeries result{SolarRadiation};
        if(CommonWavelengths.has_value())
        {
            result = result.interpolate(CommonWavelengths.value());
        }

        if(DetectorData.has_value())
        {
            result = result * DetectorData.value().interpolate(result.getXArray());
        }
        return result;
    }
}