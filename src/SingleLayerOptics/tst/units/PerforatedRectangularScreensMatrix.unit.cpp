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
                       FenestrationCommon::PropertySimple property,
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
    void runPerforatedLayerTest(const double Tmat,
                                const double Rfmat,
                                const double Rbmat,
                                const double thickness,
                                const double xSpacing,
                                const double ySpacing,
                                const double xHole,
                                const double yHole,
                                const std::string & expectedCsvFrontTransmittanceFile,
                                const std::string & expectedCsvFrontReflectanceFile,
                                const std::string & expectedCsvBackTransmittanceFile,
                                const std::string & expectedCsvBackReflectanceFile,
                                const bool updateExpectedResults = false)
    {
        auto material = SingleLayerOptics::Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        auto bsdf =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        auto shade = SingleLayerOptics::CBSDFLayerMaker::getRectangularPerforatedLayer(
          material, bsdf, xSpacing, ySpacing, thickness, xHole, yHole);

        auto results = shade->getResults();

        processMatrix(results,
                      FenestrationCommon::Side::Front,
                      FenestrationCommon::PropertySimple::T,
                      expectedCsvFrontTransmittanceFile,
                      updateExpectedResults);

        processMatrix(results,
                      FenestrationCommon::Side::Front,
                      FenestrationCommon::PropertySimple::R,
                      expectedCsvFrontReflectanceFile,
                      updateExpectedResults);

        processMatrix(results,
                      FenestrationCommon::Side::Back,
                      FenestrationCommon::PropertySimple::T,
                      expectedCsvBackTransmittanceFile,
                      updateExpectedResults);

        processMatrix(results,
                      FenestrationCommon::Side::Back,
                      FenestrationCommon::PropertySimple::R,
                      expectedCsvBackReflectanceFile,
                      updateExpectedResults);
    }
}   // namespace

// clang-format off
TEST(TestPerforatedRectangularScreensMatrix, Configuration1)
 {
    SCOPED_TRACE("Testing Perforated Rectangular layer with T=0, R=0.1, thickness=5, xSpacing=19.05, ySpacing=19.05, xHole=6.35, yHole=6.35 configuration.");
    runPerforatedLayerTest(
        0.0, 0.1, 0.1,        // Material properties
        0.005, 0.02105, 0.01905, 0.00635, 0.00635,    // Geometry properties
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedRectangularMatrixTf_T=0_R=0.1_xSpacing=21.05_ySpacing=19.05_xHole=6.35_yHole=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedRectangularMatrixRf_T=0_R=0.1_xSpacing=21.05_ySpacing=19.05_xHole=6.35_yHole=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedRectangularMatrixTb_T=0_R=0.1_xSpacing=21.05_ySpacing=19.05_xHole=6.35_yHole=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedRectangularMatrixRb_T=0_R=0.1_xSpacing=21.05_ySpacing=19.05_xHole=6.35_yHole=6.35.csv",
        false
    );
}

TEST(TestPerforatedRectangularScreensMatrix, Configuration2)
 {
    SCOPED_TRACE("Testing Perforated Rectangular layer with T=0.2, R=0.1, thickness=5, xSpacing=19.05, ySpacing=19.05, xHole=6.35, yHole=6.35 configuration.");
    runPerforatedLayerTest(
        0.2, 0.1, 0.1,        // Material properties
        0.005, 0.02105, 0.01905, 0.00635, 0.00635,    // Geometry properties
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedRectangularMatrixTf_T=0.2_R=0.1_xSpacing=21.05_ySpacing=19.05_xHole=6.35_yHole=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedRectangularMatrixRf_T=0.2_R=0.1_xSpacing=21.05_ySpacing=19.05_xHole=6.35_yHole=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedRectangularMatrixTb_T=0.2_R=0.1_xSpacing=21.05_ySpacing=19.05_xHole=6.35_yHole=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedRectangularMatrixRb_T=0.2_R=0.1_xSpacing=21.05_ySpacing=19.05_xHole=6.35_yHole=6.35.csv",
        false
    );
}
//clang-format on

// Add more configurations as needed

