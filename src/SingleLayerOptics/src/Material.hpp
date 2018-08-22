#pragma once

#include <memory>

namespace FenestrationCommon
{
    enum class WavelengthRange;
    enum class MaterialType;
    class CSeries;
}   // namespace FenestrationCommon

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

        static std::shared_ptr<CMaterial> nBandMaterial(
          const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
          double thickness,
          FenestrationCommon::MaterialType materialType,
          FenestrationCommon::WavelengthRange range,
          const std::shared_ptr<FenestrationCommon::CSeries> & detectorData = nullptr);

        static std::shared_ptr<CMaterial> nBandMaterial(
          const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
          double thickness,
          FenestrationCommon::MaterialType materialType,
          double minLambda,
          double maxLambda,
          const std::shared_ptr<FenestrationCommon::CSeries> & detectorData = nullptr);
    };

}   // namespace SingleLayerOptics