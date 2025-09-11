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
    constexpr auto writeResults{false};
    void processWavelength(int index, BSDFIntegrator & aResults, const std::string & test_data_dir)
    {
        // Front transmittance
        const auto aT = aResults.getMatrix(Side::Front, PropertySurface::T);
        const std::string file_path_t =
          test_data_dir + "/data/TestVenetianUniformShadeFlat45_5_Multiwavelength/aTf"
          + std::to_string(index + 1) + ".csv";

        if(writeResults)
        {
            Helper::writeMatrixToCSV(aT.getMatrix(), file_path_t);
        }
        else
        {
            const auto correctT = Helper::readMatrixFromCSV(file_path_t);
            Helper::compareMatrices(correctT, aT.getMatrix(), 1e-6);
        }

        // Front reflectance
        const auto aRf = aResults.getMatrix(Side::Front, PropertySurface::R);
        const std::string file_path_r =
          test_data_dir + "/data/TestVenetianUniformShadeFlat45_5_Multiwavelength/aRf"
          + std::to_string(index + 1) + ".csv";

        if(writeResults)
        {
            Helper::writeMatrixToCSV(aRf.getMatrix(), file_path_r);
        }
        else
        {
            const auto correctR = Helper::readMatrixFromCSV(file_path_r);
            Helper::compareMatrices(correctR, aRf.getMatrix(), 1e-6);
        }
    }
}   // namespace

class TestVenetianUniformShadeFlat45_5_Multiwavelength : public testing::Test
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
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0;
        const size_t numOfSlatSegments = 5;

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
                                                    DistributionMethod::UniformDiffuse,
                                                    true);
    }

public:
    std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestVenetianUniformShadeFlat45_5_Multiwavelength, TestVenetianMultiWavelength)
{
    SCOPED_TRACE("Begin Test: Venetian layer (multi range) - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    auto aResults = aLayer->getWavelengthResults();

    size_t correctSize = 6;

    EXPECT_EQ(correctSize, aResults.size());

    processWavelength(0, aResults[0], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(1, aResults[1], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(2, aResults[2], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(3, aResults[3], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(4, aResults[4], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(5, aResults[5], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
}
