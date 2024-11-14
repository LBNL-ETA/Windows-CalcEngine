#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"

#include "csvHandlers.hpp"
#include "matrixTesting.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

namespace
{
    void processWavelength(int index, BSDFIntegrator & aResults, const std::string & test_data_dir)
    {
        // Front transmittance
        const auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);
        const std::string file_path_t =
          test_data_dir + "/data/TestVenetianDirectionalShadeFlat45_5_Multiwavelength_aT_wl"
          + std::to_string(index + 1) + ".csv";

        const auto correctT = Helper::readMatrixFromCSV(file_path_t);
        Helper::compareMatrices(correctT, aT.getMatrix(), 1e-6);

        // Front reflectance
        const auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);
        const std::string file_path_r =
          test_data_dir + "/data/TestVenetianDirectionalShadeFlat45_5_Multiwavelength_aRf_wl"
          + std::to_string(index + 1) + ".csv";

        const auto correctR = Helper::readMatrixFromCSV(file_path_r);
        Helper::compareMatrices(correctR, aRf.getMatrix(), 1e-6);
    }
}   // namespace

class TestVenetianDirectionalShadeFlat45_5_Multiwavelength : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Layer;

protected:
    void SetUp() override
    {
        // Solar range material
        const auto Tsol = 0.1;
        const auto Rfsol = 0.7;
        const auto Rbsol = 0.7;
        // Visible range
        const auto Tvis = 0.2;
        const auto Rfvis = 0.6;
        const auto Rbvis = 0.6;

        const auto aMaterial =
          Material::dualBandMaterial(Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        // make cell geometry
        double slatWidth = 0.016;     // m
        double slatSpacing = 0.012;   // m
        double slatTiltAngle = 45;
        double curvatureRadius = 0;
        const size_t numOfSlatSegments = 5;

        // Method
        DistributionMethod aDistribution = DistributionMethod::DirectionalDiffuse;

        // create BSDF
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Layer = CBSDFLayerMaker::getVenetianLayer(aMaterial,
                                                    aBSDF,
                                                    slatWidth,
                                                    slatSpacing,
                                                    slatTiltAngle,
                                                    curvatureRadius,
                                                    numOfSlatSegments,
                                                    aDistribution,
                                                    true);
    }

public:
    [[nodiscard]] std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestVenetianDirectionalShadeFlat45_5_Multiwavelength, TestVenetianMultiWavelength)
{
    SCOPED_TRACE("Begin Test: Venetian layer (multi range) - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    std::vector<BSDFIntegrator> aResults = aLayer->getWavelengthResults();

    const auto wavelengths{aLayer->getBandWavelengths()};
    const std::vector<double> correctWavelengths{0.3, 0.38, 0.780001, 2.5};

    EXPECT_EQ(wavelengths.size(), correctWavelengths.size());
    for(size_t i = 0u; i < correctWavelengths.size(); ++i)
    {
        EXPECT_NEAR(wavelengths[i], correctWavelengths[i], 1e-6);
    }

    size_t correctSize = correctWavelengths.size();

    EXPECT_EQ(correctSize, aResults.size());

    processWavelength(0, aResults[0], TEST_DATA_DIR);
    processWavelength(1, aResults[1], TEST_DATA_DIR);
    processWavelength(2, aResults[2], TEST_DATA_DIR);
    processWavelength(3, aResults[3], TEST_DATA_DIR);
}
