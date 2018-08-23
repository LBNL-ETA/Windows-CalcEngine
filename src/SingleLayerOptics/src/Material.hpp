#pragma once

#include <memory>
#include "WCECommon.hpp"

namespace SpectralAveraging
{
    class CSpectralSampleData;
}

namespace SingleLayerOptics
{
    class CMaterial;


    class Material
    {
    public:
        static std::shared_ptr<CMaterial> dualBandMaterial(double Tfsol,
                                                           double Tbsol,
                                                           double Rfsol,
                                                           double Rbsol,
                                                           double Tfvis,
                                                           double Tbvis,
                                                           double Rfvis,
                                                           double Rbvis);

        static std::shared_ptr<CMaterial> dualBandMaterial(double Tfsol,
                                                           double Tbsol,
                                                           double Rfsol,
                                                           double Rbsol,
                                                           double Tfvis,
                                                           double Tbvis,
                                                           double Rfvis,
                                                           double Rbvis,
                                                           double ratio);

        static std::shared_ptr<CMaterial>
          dualBandMaterial(double Tfsol,
                           double Tbsol,
                           double Rfsol,
                           double Rbsol,
                           double Tfvis,
                           double Tbvis,
                           double Rfvis,
                           double Rbvis,
                           const std::shared_ptr<FenestrationCommon::CSeries> & solarRadiation);

        static std::shared_ptr<CMaterial> singleBandMaterial(
          double Tf, double Tb, double Rf, double Rb, double minLambda, double maxLambda);

        static std::shared_ptr<CMaterial> singleBandMaterial(
          double Tf, double Tb, double Rf, double Rb, FenestrationCommon::WavelengthRange range);

        static std::shared_ptr<CMaterial>
          nBandMaterial(const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
                        double thickness,
                        FenestrationCommon::MaterialType materialType,
                        FenestrationCommon::WavelengthRange range,
                        FenestrationCommon::IntegrationType integrationType =
                          FenestrationCommon::IntegrationType::Trapezoidal,
                        double normalizationCoefficient = 1);

        static std::shared_ptr<CMaterial>
          nBandMaterial(const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
                        double thickness,
                        FenestrationCommon::MaterialType materialType,
                        double minLambda,
                        double maxLambda,
                        FenestrationCommon::IntegrationType integrationType = FenestrationCommon::IntegrationType::Trapezoidal,
                        double normalizationCoefficient = 1);

		static std::shared_ptr<CMaterial>
		nBandMaterial(const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
					  const std::shared_ptr<FenestrationCommon::CSeries> & detectorData,
					  double thickness,
					  FenestrationCommon::MaterialType materialType,
					  double minLambda,
					  double maxLambda,
					  FenestrationCommon::IntegrationType integrationType = FenestrationCommon::IntegrationType::Trapezoidal,
					  double normalizationCoefficient = 1);

		static std::shared_ptr<CMaterial>
		nBandMaterial(const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
					  const std::shared_ptr<FenestrationCommon::CSeries> & detectorData,
					  double thickness,
					  FenestrationCommon::MaterialType materialType,
					  FenestrationCommon::WavelengthRange t_Range,
					  FenestrationCommon::IntegrationType integrationType = FenestrationCommon::IntegrationType::Trapezoidal,
					  double normalizationCoefficient = 1);

    };

}   // namespace SingleLayerOptics