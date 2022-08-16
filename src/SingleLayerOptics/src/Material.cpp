#include <stdexcept>

#include "Material.hpp"
#include "WCECommon.hpp"
#include "MaterialDescription.hpp"
#include "WCESpectralAveraging.hpp"

using FenestrationCommon::WavelengthRange;
using FenestrationCommon::CWavelengthRange;
using FenestrationCommon::CSeries;
using SpectralAveraging::CSpectralSample;
using SpectralAveraging::CPhotovoltaicSample;

namespace SingleLayerOptics
{
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
          std::make_shared<CMaterialSingleBand>(Tfsol, Tbsol, Rfsol, Rbsol);
        auto aVisibleRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tfvis, Tbvis, Rfvis, Rbvis);
        auto material =
          std::make_shared<CMaterialDualBand>(aVisibleRangeMaterial, aSolarRangeMaterial);
        material->createRangesFromRatio(ratio);
        return material;
    }

    std::shared_ptr<CMaterial> Material::dualBandMaterial(const double Tfsol,
                                                          const double Tbsol,
                                                          const double Rfsol,
                                                          const double Rbsol,
                                                          const double Tfvis,
                                                          const double Tbvis,
                                                          const double Rfvis,
                                                          const double Rbvis,
                                                          const CSeries & solarRadiation)
    {
        auto aSolarRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tfsol, Tbsol, Rfsol, Rbsol);
        auto aVisibleRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tfvis, Tbvis, Rfvis, Rbvis);
        auto material =
          std::make_shared<CMaterialDualBand>(aVisibleRangeMaterial, aSolarRangeMaterial);
        material->createRangesFromSolarRadiation(solarRadiation);
        return material;
    }

    std::shared_ptr<CMaterial>
      Material::dualBandBSDFMaterial(std::vector<std::vector<double>> const & Tfsol,
                                     std::vector<std::vector<double>> const & Tbsol,
                                     std::vector<std::vector<double>> const & Rfsol,
                                     std::vector<std::vector<double>> const & Rbsol,
                                     std::vector<std::vector<double>> const & Tfvis,
                                     std::vector<std::vector<double>> const & Tbvis,
                                     std::vector<std::vector<double>> const & Rfvis,
                                     std::vector<std::vector<double>> const & Rbvis,
                                     BSDFHemisphere const & hemisphere,
                                     double ratio)
    {
        auto aSolarRangeMaterial =
          std::make_shared<CMaterialSingleBandBSDF>(Tfsol, Tbsol, Rfsol, Rbsol, hemisphere);
        auto aVisibleRangeMaterial =
          std::make_shared<CMaterialSingleBandBSDF>(Tfvis, Tbvis, Rfvis, Rbvis, hemisphere);
        auto material =
          std::make_shared<CMaterialDualBandBSDF>(aVisibleRangeMaterial, aSolarRangeMaterial);
        material->createRangesFromRatio(ratio);
        return material;
    }

    std::shared_ptr<CMaterial>
      Material::dualBandBSDFMaterial(std::vector<std::vector<double>> const & Tfsol,
                                     std::vector<std::vector<double>> const & Tbsol,
                                     std::vector<std::vector<double>> const & Rfsol,
                                     std::vector<std::vector<double>> const & Rbsol,
                                     std::vector<std::vector<double>> const & Tfvis,
                                     std::vector<std::vector<double>> const & Tbvis,
                                     std::vector<std::vector<double>> const & Rfvis,
                                     std::vector<std::vector<double>> const & Rbvis,
                                     BSDFHemisphere const & hemisphere,
                                     const FenestrationCommon::CSeries & solarRadiation)
    {
        auto aSolarRangeMaterial =
          std::make_shared<CMaterialSingleBandBSDF>(Tfsol, Tbsol, Rfsol, Rbsol, hemisphere);
        auto aVisibleRangeMaterial =
          std::make_shared<CMaterialSingleBandBSDF>(Tfvis, Tbvis, Rfvis, Rbvis, hemisphere);
        auto material =
          std::make_shared<CMaterialDualBandBSDF>(aVisibleRangeMaterial, aSolarRangeMaterial);
        material->createRangesFromSolarRadiation(solarRadiation);

        return material;
    }

    std::shared_ptr<CMaterial> Material::singleBandMaterial(const double Tf,
                                                            const double Tb,
                                                            const double Rf,
                                                            const double Rb)
    {
        return std::make_shared<CMaterialSingleBand>(Tf, Tb, Rf, Rb);
    }

    std::shared_ptr<CMaterial>
      Material::singleBandBSDFMaterial(const std::vector<std::vector<double>> & Tf,
                                       const std::vector<std::vector<double>> & Tb,
                                       const std::vector<std::vector<double>> & Rf,
                                       const std::vector<std::vector<double>> & Rb,
                                       const BSDFHemisphere & hemisphere)
    {
        return std::make_shared<CMaterialSingleBandBSDF>(Tf, Tb, Rf, Rb, hemisphere);
    }

    std::shared_ptr<CMaterial> Material::nBandMaterial(
      const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
      double thickness,
      FenestrationCommon::MaterialType materialType,
      FenestrationCommon::IntegrationType integrationType,
      double normalizationCoefficient)
    {
        auto aSample = std::make_shared<CSpectralSample>(
          measurement, CSeries(), integrationType, normalizationCoefficient);

        if(aSample->getWavelengthsFromSample().empty())
        {
            throw std::runtime_error("Given measured sample does not have measurements withing "
                                     "requested range. Calculation is not possible.");
        }

        return std::make_shared<CMaterialSample>(aSample, thickness, materialType);
    }

    std::shared_ptr<CMaterial> Material::nBandMaterial(
      const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
      const FenestrationCommon::CSeries & detectorData,
      const double thickness,
      const FenestrationCommon::MaterialType materialType,
      const FenestrationCommon::IntegrationType integrationType,
      const double normalizationCoefficient)
    {
        auto aSample = std::make_shared<CSpectralSample>(
          measurement, CSeries(), integrationType, normalizationCoefficient);
        aSample->setDetectorData(detectorData);

        if(aSample->getWavelengthsFromSample().empty())
        {
            throw std::runtime_error("Given measured sample does not have measurements withing "
                                     "requested range. Calculation is not possible.");
        }

        return std::make_shared<CMaterialSample>(aSample, thickness, materialType);
    }

    std::shared_ptr<CMaterialPhotovoltaicSample> Material::nBandPhotovoltaicMaterial(
      const std::shared_ptr<SpectralAveraging::PhotovoltaicSampleData> & measurement,
      double thickness,
      FenestrationCommon::MaterialType materialType,
      FenestrationCommon::IntegrationType integrationType,
      double normalizationCoefficient)
    {
        auto aSample = std::make_shared<SpectralAveraging::CPhotovoltaicSample>(
          measurement, CSeries(), integrationType, normalizationCoefficient);

        if(aSample->getWavelengthsFromSample().empty())
        {
            throw std::runtime_error("Given measured sample does not have measurements withing "
                                     "requested range. Calculation is not possible.");
        }

        return std::make_shared<CMaterialPhotovoltaicSample>(aSample, thickness, materialType);
    }

}   // namespace SingleLayerOptics
