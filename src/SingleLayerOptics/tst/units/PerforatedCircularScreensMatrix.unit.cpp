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
                                const double diameter,
                                const std::string & expectedCsvFrontTransmittanceFile,
                                const std::string & expectedCsvFrontReflectanceFile,
                                const std::string & expectedCsvBackTransmittanceFile,
                                const std::string & expectedCsvBackReflectanceFile,
                                const bool updateExpectedResults = false)
    {
        auto material = SingleLayerOptics::Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        auto bsdf =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        auto shade = SingleLayerOptics::CBSDFLayerMaker::getCircularPerforatedLayer(
          material, bsdf, xSpacing, ySpacing, thickness, diameter / 2.0);

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
TEST(TestPerforatedCircularScreensMatrix, Configuration1)
 {
    SCOPED_TRACE("Testing Perforated Circular layer with T=0, R=0.1, thickness=5, xSpacing=19.05, ySpacing=19.05, Diameter=6.35 configuration.");
    runPerforatedLayerTest(
        0.0, 0.1, 0.1,        // Material properties
        0.005, 0.01905, 0.01905, 0.00635,    // Geometry properties
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedCircularMatrixTf_T=0_R=0.1_xSpacing=19.05_ySpacing=19.05_Diameter=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedCircularMatrixRf_T=0_R=0.1_xSpacing=19.05_ySpacing=19.05_Diameter=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedCircularMatrixTb_T=0_R=0.1_xSpacing=19.05_ySpacing=19.05_Diameter=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedCircularMatrixRb_T=0_R=0.1_xSpacing=19.05_ySpacing=19.05_Diameter=6.35.csv",
        false
    );
}

TEST(TestPerforatedCircularScreensMatrix, Configuration2)
 {
    SCOPED_TRACE("Testing Perforated Circular layer with T=0.2, R=0.1, thickness=5, xSpacing=19.05, ySpacing=19.05, Diameter=6.35 configuration.");
    runPerforatedLayerTest(
        0.2, 0.1, 0.1,        // Material properties
        0.005, 0.01905, 0.01905, 0.00635,    // Geometry properties
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedCircularMatrixTf_T=0.2_R=0.1_xSpacing=19.05_ySpacing=19.05_Diameter=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedCircularMatrixRf_T=0.2_R=0.1_xSpacing=19.05_ySpacing=19.05_Diameter=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedCircularMatrixTb_T=0.2_R=0.1_xSpacing=19.05_ySpacing=19.05_Diameter=6.35.csv",
        TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestPerforatedCircularMatrixRb_T=0.2_R=0.1_xSpacing=19.05_ySpacing=19.05_Diameter=6.35.csv",
        false
    );
}
//clang-format on

// Add more configurations as needed

