#include "Material.hpp"
#include "WCECommon.hpp"
#include "MaterialDescription.hpp"
#include "WCESpectralAveraging.hpp"

using FenestrationCommon::WavelengthRange;
using FenestrationCommon::CWavelengthRange;
using FenestrationCommon::CSeries;
using SpectralAveraging::CSpectralSample;

namespace SingleLayerOptics
{
    std::shared_ptr<CMaterial> Material::dualBandMaterial(const double Tfsol,
                                                          const double Tbsol,
                                                          const double Rfsol,
                                                          const double Rbsol,
                                                          const double Tfvis,
                                                          const double Tbvis,
                                                          const double Rfvis,
                                                          const double Rbvis)
    {
        auto aSolarRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tfsol, Tbsol, Rfsol, Rbsol, WavelengthRange::Solar);
        auto aVisibleRangeMaterial = std::make_shared<CMaterialSingleBand>(
          Tfvis, Tbvis, Rfvis, Rbvis, WavelengthRange::Visible);
        return std::make_shared<CMaterialDualBand>(aVisibleRangeMaterial, aSolarRangeMaterial);
    }

    std::shared_ptr<CMaterial> Material::dualBandMaterial(const double Tfsol,
                                                          const double Tbsol,
                                                          const double Rfsol,
                                                          const double Rbsol,
                                                          const double Tfvis,
                                                          const double Tbvis,
                                                          const double Rfvis,
                                                          const double Rbvis,
                                                          const double ratio)
    {
        auto aSolarRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tfsol, Tbsol, Rfsol, Rbsol, WavelengthRange::Solar);
        auto aVisibleRangeMaterial = std::make_shared<CMaterialSingleBand>(
          Tfvis, Tbvis, Rfvis, Rbvis, WavelengthRange::Visible);
        return std::make_shared<CMaterialDualBand>(
          aVisibleRangeMaterial, aSolarRangeMaterial, ratio);
    }

    std::shared_ptr<CMaterial> Material::dualBandMaterial(
      const double Tfsol,
      const double Tbsol,
      const double Rfsol,
      const double Rbsol,
      const double Tfvis,
      const double Tbvis,
      const double Rfvis,
      const double Rbvis,
      const std::shared_ptr<FenestrationCommon::CSeries> & solarRadiation)
    {
        auto aSolarRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tfsol, Tbsol, Rfsol, Rbsol, WavelengthRange::Solar);
        auto aVisibleRangeMaterial = std::make_shared<CMaterialSingleBand>(
          Tfvis, Tbvis, Rfvis, Rbvis, WavelengthRange::Visible);
        return std::make_shared<CMaterialDualBand>(
          aVisibleRangeMaterial, aSolarRangeMaterial, solarRadiation);
    }

    std::shared_ptr<CMaterial> Material::singleBandMaterial(const double Tf,
                                                            const double Tb,
                                                            const double Rf,
                                                            const double Rb,
                                                            const double minLambda,
                                                            const double maxLambda)
    {
        return std::make_shared<CMaterialSingleBand>(Tf, Tb, Rf, Rb, minLambda, maxLambda);
    }

    std::shared_ptr<CMaterial>
      Material::singleBandMaterial(const double Tf,
                                   const double Tb,
                                   const double Rf,
                                   const double Rb,
                                   const FenestrationCommon::WavelengthRange range)
    {
        return std::make_shared<CMaterialSingleBand>(Tf, Tb, Rf, Rb, range);
    }

    std::shared_ptr<CMaterial> Material::nBandMaterial(
      const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
      const double thickness,
      const FenestrationCommon::MaterialType materialType,
      const FenestrationCommon::WavelengthRange range,
      const FenestrationCommon::IntegrationType integrationType,
      const double normalizationCoefficient)
    {
        auto aSample = std::make_shared<CSpectralSample>(
          measurement, nullptr, integrationType, normalizationCoefficient);

        // Need to determine if sample is subset of allowed range in which case integration range
        // needs to be narrowed.
        auto wlRange = CWavelengthRange(range);
        auto minLambda = wlRange.minLambda();
        auto maxLambda = wlRange.maxLambda();

        const auto sampleWls = measurement->getWavelengths();
        const auto minSample = sampleWls[0];
        const auto maxSample = sampleWls[sampleWls.size() - 1];

        // Narrow down wavelengths in case sample is not measured in that range
        if(minLambda < minSample)
        {
            minLambda = minSample;
        }

        if(maxLambda > maxSample)
        {
            maxLambda = maxSample;
        }

        // Narrow down sample in case it is over limits of desired measurements
        aSample->cutExtraData(minLambda, maxLambda);

        return std::make_shared<CMaterialSample>(
          aSample, thickness, materialType, minLambda, maxLambda);
    }

    std::shared_ptr<CMaterial> Material::nBandMaterial(
      const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
      double thickness,
      const FenestrationCommon::MaterialType materialType,
      const double minLambda,
      const double maxLambda,
      const FenestrationCommon::IntegrationType integrationType,
      double normalizationCoefficient)
    {
        auto aSample = std::make_shared<CSpectralSample>(
          measurement, nullptr, integrationType, normalizationCoefficient);

        // Narrow down sample in case it is over limits of desired measurements
        aSample->cutExtraData(minLambda, maxLambda);

        return std::make_shared<CMaterialSample>(
          aSample, thickness, materialType, minLambda, maxLambda);
    }

    std::shared_ptr<CMaterial> Material::nBandMaterial(
      const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
      const std::shared_ptr<CSeries> & detectorData,
      const double thickness,
      const FenestrationCommon::MaterialType materialType,
      const double minLambda,
      const double maxLambda,
      const FenestrationCommon::IntegrationType integrationType,
      const double normalizationCoefficient)
    {
        auto aSample = std::make_shared<CSpectralSample>(
          measurement, nullptr, integrationType, normalizationCoefficient);
        aSample->setDetectorData(detectorData);

        // Narrow down sample in case it is over limits of desired measurements
        aSample->cutExtraData(minLambda, maxLambda);

        return std::make_shared<CMaterialSample>(
          aSample, thickness, materialType, minLambda, maxLambda);
    }

    std::shared_ptr<CMaterial> Material::nBandMaterial(
      const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
      const std::shared_ptr<FenestrationCommon::CSeries> & detectorData,
      const double thickness,
      const FenestrationCommon::MaterialType materialType,
      const FenestrationCommon::WavelengthRange t_Range,
      const FenestrationCommon::IntegrationType integrationType,
      const double normalizationCoefficient)
    {
        auto aSample = std::make_shared<CSpectralSample>(
          measurement, nullptr, integrationType, normalizationCoefficient);

        aSample->setDetectorData(detectorData);
        return std::make_shared<CMaterialSample>(aSample, thickness, materialType, t_Range);
    }

}   // namespace SingleLayerOptics