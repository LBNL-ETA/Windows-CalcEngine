#include <gtest/gtest.h>
#include <memory>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"

#include "csvHandlers.hpp"
#include "matrixTesting.hpp"

// Some function names can be identical across different unit tests. This is the purpose of putting
// it under anonymous namespace
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
            Helper::writeVectorToCSV(matrix.getMatrix(), expectedCsvFile);
        }

        const auto correctResults = Helper::readVectorFromCSV(expectedCsvFile);
        Helper::compareMatrices(correctResults, matrix.getMatrix());
    }

    // Helper function to set up and test a configuration
    void runVenetianLayerTest(double Tmat,
                              double Rfmat,
                              double Rbmat,
                              double slatWidth,
                              double slatSpacing,
                              double slatTiltAngle,
                              double curvatureRadius,
                              size_t numOfSlatSegments,
                              const std::string & expectedCsvFrontTransmittanceFile,
                              const std::string & expectedCsvFrontReflectanceFile,
                              const std::string & expectedCsvBackTransmittanceFile,
                              const std::string & expectedCsvBackReflectanceFile,
                              const bool updateExpectedResults = false)
    {
        // Create material with specified properties
        auto material = SingleLayerOptics::Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // Create BSDF hemisphere
        auto bsdf =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        // Create Venetian layer with the configuration parameters
        auto shade = SingleLayerOptics::CBSDFLayerMaker::getVenetianLayer(
          material,
          bsdf,
          slatWidth,
          slatSpacing,
          slatTiltAngle,
          curvatureRadius,
          numOfSlatSegments,
          SingleLayerOptics::DistributionMethod::DirectionalDiffuse);

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

    double calculateCurvature(const double t_Rise, const double t_SlatWidth)
    {
        double curvature = 0;

        if(t_Rise > 0)
        {
            double aRise = t_Rise;
            if(t_Rise > t_SlatWidth / 2)
            {
                aRise = t_SlatWidth / 2;
            }
            curvature = (aRise * aRise + t_SlatWidth * t_SlatWidth / 4) / (2 * aRise);
        }
        return curvature;
    }
}   // namespace

// clang-format off
TEST(TestVenetianDirectionalMatrix, Configuration1_T0_R0_1_Slat0_Rise0)
 {
    SCOPED_TRACE("Testing Venetian layer with T=0, R=0.1, Slat=0, Rise=0 configuration.");
    runVenetianLayerTest(
        0.0, 0.1, 0.1,        // Material properties
        0.016, 0.012, 0, 0,    // Geometry properties
        1,                     // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0_R=0.1_Slat=0_nSegments=1_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0_R=0.1_Slat=0_nSegments=1_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0_R=0.1_Slat=0_nSegments=1_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0_R=0.1_Slat=0_nSegments=1_Rise=0.csv", // Expected results
        true
    );
}

TEST(TestVenetianDirectionalMatrix, Configuration1_T0_R0_1_Slat0_nSegments5_Rise0)
 {
    SCOPED_TRACE("Testing Venetian layer with T=0, R=0.1, Slat=0, nSegments=5, Rise=0 configuration.");
    runVenetianLayerTest(
        0.0, 0.1, 0.1,         // Material properties
        0.016, 0.012, 0, 0,    // Geometry properties
        5,                     // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0_R=0.1_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0_R=0.1_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0_R=0.1_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0_R=0.1_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        true
    );
}

TEST(TestVenetianDirectionalMatrix, Configuration1_T0_R0_1_Slat45_nSegments1_Rise0)
 {
    SCOPED_TRACE("Testing Venetian layer with T=0, R=0.1, Slat=45, nSegments=1, Rise=0 configuration.");
    runVenetianLayerTest(
        0.0, 0.1, 0.1,        // Material properties
        0.016, 0.012, 45, 0,  // Geometry properties
        1,                    // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0_R=0.1_Slat=45_nSegments=1_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0_R=0.1_Slat=45_nSegments=1_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0_R=0.1_Slat=45_nSegments=1_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0_R=0.1_Slat=45_nSegments=1_Rise=0.csv", // Expected results
        true
    );
}

TEST(TestVenetianDirectionalMatrix, Configuration2_T0_R0_15_Slat45_nSegments1_Rise0)
{
    SCOPED_TRACE("Testing Venetian layer with T=0, R=0.15, Slat=45, nSegments=1, Rise=0 configuration.");
    runVenetianLayerTest(
        0.0, 0.15, 0.15,       // Material properties
        0.016, 0.012, 45, 0,   // Geometry properties
        1,                     // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0_R=0.15_Slat=45_nSegments=1_Rise=0.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0_R=0.15_Slat=45_nSegments=1_Rise=0.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0_R=0.15_Slat=45_nSegments=1_Rise=0.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0_R=0.15_Slat=45_nSegments=1_Rise=0.csv",
        true
    );
}

TEST(TestVenetianDirectionalMatrix, Configuration3_T0_R0_2_Slat30_nSegments5_Rise0)
{
    SCOPED_TRACE("Testing Venetian layer with T=0, R=0.2, Slat=30, nSegments5, Rise=0 configuration.");
    runVenetianLayerTest(
        0.0, 0.2, 0.2,         // Material properties
        0.018, 0.014, 30, 0,   // Geometry properties
        5,                     // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0_R=0.2_Slat=30_nSegments=5_Rise=0.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0_R=0.2_Slat=30_nSegments=5_Rise=0.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0_R=0.2_Slat=30_nSegments=5_Rise=0.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0_R=0.2_Slat=30_nSegments=5_Rise=0.csv",
        true
    );
}

TEST(TestVenetianDirectionalMatrix, Configuration1_T0_R0_1_Slat0_nSegments5_Rise3)
 {
    SCOPED_TRACE("Testing Venetian layer with T=0, R=0.1, Slat=0, nSegments=5, Rise=3 configuration.");
    runVenetianLayerTest(
        0.0, 0.1, 0.1,                                         // Material properties
        0.016, 0.012, 0, calculateCurvature(0.003, 0.016),     // Geometry properties
        5,                                                     // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0_R=0.1_Slat=0_nSegments=5_Rise=3.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0_R=0.1_Slat=0_nSegments=5_Rise=3.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0_R=0.1_Slat=0_nSegments=5_Rise=3.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0_R=0.1_Slat=0_nSegments=5_Rise=3.csv", // Expected results
        true
    );
}

TEST(TestVenetianDirectionalMatrix, Configuration2_T0_R0_15_Slat45_nSegments5_Rise5)
{
    SCOPED_TRACE("Testing Venetian layer with T=0, R=0.15, Slat=45, nSegments=5, Rise=5 configuration.");
    runVenetianLayerTest(
        0.0, 0.15, 0.15,                                        // Material properties
        0.016, 0.012, 45, calculateCurvature(0.005, 0.016),     // Geometry properties
        5,                                                      // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0_R=0.15_Slat=45_nSegments=5_Rise=5.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0_R=0.15_Slat=45_nSegments=5_Rise=5.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0_R=0.15_Slat=45_nSegments=5_Rise=5.csv",
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0_R=0.15_Slat=45_nSegments=5_Rise=5.csv",
        true
    );
}

TEST(TestVenetianDirectionalMatrix, Configuration1_T0_1_R0_1_Slat0_nSegments5_Rise0)
 {
    SCOPED_TRACE("Testing Venetian layer with T=0.1, R=0.1, Slat=0, nSegments=5, Rise=0 configuration.");
    runVenetianLayerTest(
        0.1, 0.1, 0.1,         // Material properties
        0.016, 0.012, 0, 0,    // Geometry properties
        5,                     // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0.1_R=0.1_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0.1_R=0.1_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0.1_R=0.1_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0.1_R=0.1_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        true
    );
}

TEST(TestVenetianDirectionalMatrix, Configuration1_T0_1_R0_7_Slat0_nSegments5_Rise0)
 {
    SCOPED_TRACE("Testing Venetian layer with T=0.1, R=0.1, Slat=0, nSegments=5, Rise=0 configuration.");
    runVenetianLayerTest(
        0.1, 0.7, 0.7,         // Material properties
        0.016, 0.012, 0, 0,    // Geometry properties
        5,                     // Number of slat segments
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTf_T=0.1_R=0.7_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRf_T=0.1_R=0.7_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixTb_T=0.1_R=0.7_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        TEST_DATA_DIR "/data/TestVenetianDirectionalMatrixRb_T=0.1_R=0.7_Slat=0_nSegments=5_Rise=0.csv", // Expected results
        true
    );
}
//clang-format on

