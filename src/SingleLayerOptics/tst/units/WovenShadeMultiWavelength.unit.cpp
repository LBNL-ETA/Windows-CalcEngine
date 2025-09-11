#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"

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
        const std::string file_path_t = test_data_dir + "/data/TestWovenShadeMultiWavelength/aTf"
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
        const std::string file_path_r = test_data_dir + "/data/TestWovenShadeMultiWavelength/aRf"
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

class TestWovenShadeMultiWavelength : public testing::Test
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
        const auto diameter = 6.35;   // mm
        const auto spacing = 19.05;   // mm

        // create BSDF
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Layer = CBSDFLayerMaker::getWovenLayer(aMaterial, aBSDF, diameter, spacing);
    }

public:
    std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestWovenShadeMultiWavelength, TestWovenMultiWavelength)
{
    SCOPED_TRACE("Begin Test: Perforated layer (multi range) - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    auto aResults = aLayer->getWavelengthResults();

    const auto wavelengths{aLayer->getBandWavelengths()};
    const std::vector correctWavelengths{0.3, 0.379999, 0.380001, 0.779999, 0.780001, 2.5};

    EXPECT_EQ(wavelengths.size(), correctWavelengths.size());

    for(size_t i = 0U; i < correctWavelengths.size(); ++i)
    {
        EXPECT_NEAR(wavelengths[i], correctWavelengths[i], 1e-6);
    }

    size_t correctSize = correctWavelengths.size();

    EXPECT_EQ(correctSize, aResults.size());

    processWavelength(0, aResults[0], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(1, aResults[1], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(2, aResults[2], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(3, aResults[3], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(4, aResults[4], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
    processWavelength(5, aResults[5], TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
}

TEST_F(TestWovenShadeMultiWavelength, AtWavelength)
{
    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    constexpr size_t wavelengthIndex{3u};
    auto aResults = aLayer->getResultsAtWavelength(wavelengthIndex);

    processWavelength(3, aResults, TEST_DATA_DIR_SINGLE_LAYER_OPTICS);
}