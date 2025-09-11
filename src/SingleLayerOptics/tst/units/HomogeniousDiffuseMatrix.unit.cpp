#include <gtest/gtest.h>
#include <memory>

#include <WCECommon.hpp>
#include <WCESingleLayerOptics.hpp>

#include "csvHandlers.hpp"
#include "matrixTesting.hpp"


namespace
{
    void processMatrix(SingleLayerOptics::BSDFIntegrator & results,
                       FenestrationCommon::Side side,
                       FenestrationCommon::PropertySurface property,
                       const std::string & expectedCsvFile,
                       bool updateExpectedResults)
    {
        const auto & matrix = results.getMatrix(side, property);

        if(updateExpectedResults)
        {
            Helper::writeMatrixToCSV(matrix.getMatrix(), expectedCsvFile);
        }

        const auto correctResults = Helper::readMatrixFromCSV(expectedCsvFile);
        Helper::compareMatrices(correctResults, matrix.getMatrix());
    }

    // Helper function to set up and test a configuration
    void runLayerTest(const double Tmat,
                      const double Rfmat,
                      const double Rbmat,
                      const std::string & expectedCsvFrontTransmittanceFile,
                      const std::string & expectedCsvFrontReflectanceFile,
                      const std::string & expectedCsvBackTransmittanceFile,
                      const std::string & expectedCsvBackReflectanceFile,
                      const bool updateExpectedResults = false)
    {
        auto material = SingleLayerOptics::Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        auto bsdf =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        auto shade = SingleLayerOptics::CBSDFLayerMaker::getHomogeneousDiffuseLayer(material, bsdf);

        auto results = shade->getResults();

        processMatrix(results,
                      FenestrationCommon::Side::Front,
                      FenestrationCommon::PropertySurface::T,
                      expectedCsvFrontTransmittanceFile,
                      updateExpectedResults);

        processMatrix(results,
                      FenestrationCommon::Side::Front,
                      FenestrationCommon::PropertySurface::R,
                      expectedCsvFrontReflectanceFile,
                      updateExpectedResults);

        processMatrix(results,
                      FenestrationCommon::Side::Back,
                      FenestrationCommon::PropertySurface::T,
                      expectedCsvBackTransmittanceFile,
                      updateExpectedResults);

        processMatrix(results,
                      FenestrationCommon::Side::Back,
                      FenestrationCommon::PropertySurface::R,
                      expectedCsvBackReflectanceFile,
                      updateExpectedResults);
    }
}   // namespace

// clang-format off
TEST(TestHomogeniousDiffuseMatrix, Configuration1)
 {
    SCOPED_TRACE("Testing Perforated Circular layer with T=0, R=0.1 configuration.");
    runLayerTest(
        0.0, 0.1, 0.1,        // Material properties
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestHomogeneousDiffuseMatrixTf_T=0_R=0.1.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestHomogeneousDiffuseMatrixRf_T=0_R=0.1.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestHomogeneousDiffuseMatrixTb_T=0_R=0.1.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestHomogeneousDiffuseMatrixRb_T=0_R=0.1.csv",
        false
    );
}

TEST(TestHomogeniousDiffuseMatrix, Configuration2)
{
    SCOPED_TRACE("Testing Perforated Circular layer with T=0.7, R=0.1 configuration.");
    runLayerTest(
        0.7, 0.1, 0.1,        // Material properties
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestHomogeneousDiffuseMatrixTf_T=0.7_R=0.1.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestHomogeneousDiffuseMatrixRf_T=0.7_R=0.1.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestHomogeneousDiffuseMatrixTb_T=0.7_R=0.1.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestHomogeneousDiffuseMatrixRb_T=0.7_R=0.1.csv",
        false
    );
}
//clang-format on

// Add more configurations as needed

