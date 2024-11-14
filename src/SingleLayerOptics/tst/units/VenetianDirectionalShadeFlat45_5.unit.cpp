#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"

#include "csvHandlers.hpp"
#include "matrixTesting.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat45_5 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    void SetUp() override
    {
        // create material
        const auto Tmat = 0.2;
        const auto Rfmat = 0.6;
        const auto Rbmat = 0.6;
        const auto aMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0;
        const size_t numOfSlatSegments = 5;

        // Method
        DistributionMethod aDistribution = DistributionMethod::DirectionalDiffuse;

        // create BSDF
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Shade = CBSDFLayerMaker::getVenetianLayer(aMaterial,
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
    std::shared_ptr<CBSDFLayer> GetShade()
    {
        return m_Shade;
    };
};

TEST_F(TestVenetianDirectionalShadeFlat45_5, TestVenetian1)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Flat, 45 degrees slats) - single band properties.");

    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    BSDFIntegrator aResults = aShade->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.386680, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.3855669, RfDiff, 1e-6);

    const auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    const auto correctT{Helper::readMatrixFromCSV(
      TEST_DATA_DIR "/data/TestVenetianDirectionalShadeFlat45_5_aT.csv")};

    Helper::compareMatrices(correctT, aT.getMatrix(), 1e-6);

    // Front reflectance
    const auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    const auto correctR{Helper::readMatrixFromCSV(
      TEST_DATA_DIR "/data/TestVenetianDirectionalShadeFlat45_5_aRf.csv")};

    Helper::compareMatrices(correctR, aRf.getMatrix(), 1e-6);
}
