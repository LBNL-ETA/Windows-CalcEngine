#pragma once

#include <memory>
#include "WCECommon.hpp"

namespace SpectralAveraging
{
    class CSpectralSampleData;
    class PhotovoltaicSampleData;
}   // namespace SpectralAveraging

namespace SingleLayerOptics
{
    class CMaterial;
    class CMaterialPhotovoltaicSample;
    class BSDFHemisphere;

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
                                                           double Rbvis,
                                                           double ratio = ConstantsData::NIRRatio);

        static std::shared_ptr<CMaterial>
          dualBandMaterial(double Tfsol,
                           double Tbsol,
                           double Rfsol,
                           double Rbsol,
                           double Tfvis,
                           double Tbvis,
                           double Rfvis,
                           double Rbvis,
                           const FenestrationCommon::CSeries & solarRadiation);

        static std::shared_ptr<CMaterial>
          dualBandBSDFMaterial(std::vector<std::vector<double>> const & Tfsol,
                               std::vector<std::vector<double>> const & Tbsol,
                               std::vector<std::vector<double>> const & Rfsol,
                               std::vector<std::vector<double>> const & Rbsol,
                               std::vector<std::vector<double>> const & Tfvis,
                               std::vector<std::vector<double>> const & Tbvis,
                               std::vector<std::vector<double>> const & Rfvis,
                               std::vector<std::vector<double>> const & Rbvis,
                               BSDFHemisphere const & hemisphere,
                               double ratio = ConstantsData::NIRRatio);

        static std::shared_ptr<CMaterial>
          dualBandBSDFMaterial(std::vector<std::vector<double>> const & Tfsol,
                               std::vector<std::vector<double>> const & Tbsol,
                               std::vector<std::vector<double>> const & Rfsol,
                               std::vector<std::vector<double>> const & Rbsol,
                               std::vector<std::vector<double>> const & Tfvis,
                               std::vector<std::vector<double>> const & Tbvis,
                               std::vector<std::vector<double>> const & Rfvis,
                               std::vector<std::vector<double>> const & Rbvis,
                               BSDFHemisphere const & hemisphere,
                               const FenestrationCommon::CSeries & solarRadiation);

        static std::shared_ptr<CMaterial>
          singleBandMaterial(double Tf, double Tb, double Rf, double Rb);

        static std::shared_ptr<CMaterial>
          singleBandBSDFMaterial(const std::vector<std::vector<double>> & Tf,
                                 const std::vector<std::vector<double>> & Tb,
                                 const std::vector<std::vector<double>> & Rf,
                                 const std::vector<std::vector<double>> & Rb,
                                 const BSDFHemisphere & hemisphere);


        static std::shared_ptr<CMaterial>
          nBandMaterial(const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
                        double thickness,
                        FenestrationCommon::MaterialType materialType);

        static std::shared_ptr<CMaterial>
          nBandMaterial(const std::shared_ptr<SpectralAveraging::CSpectralSampleData> & measurement,
                        const FenestrationCommon::CSeries & detectorData,
                        const double thickness,
                        const FenestrationCommon::MaterialType materialType);

        static std::shared_ptr<CMaterialPhotovoltaicSample> nBandPhotovoltaicMaterial(
          const std::shared_ptr<SpectralAveraging::PhotovoltaicSampleData> & measurement,
          double thickness,
          FenestrationCommon::MaterialType materialType);
    };

}   // namespace SingleLayerOptics
