#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"

#include "csvHandlers.hpp"
#include "matrixTesting.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat0_3 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    void SetUp() override
    {
        // create material
        const auto Tmat = 0.1;
        const auto Rfmat = 0.7;
        const auto Rbmat = 0.7;
        const auto aMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.010;   // m
        const auto slatTiltAngle = 0;
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

TEST_F(TestVenetianDirectionalShadeFlat0_3, TestVenetian1)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Flat, 45 degrees slats) - solar properties.");

    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    auto aResults = aShade->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.498801, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.238163, RfDiff, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySurface::T);

    const auto correctT{
      Helper::readMatrixFromCSV(TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestVenetianDirectionalShadeFlat0_3_aT.csv")};

    Helper::compareMatrices(correctT, aT.getMatrix(), 1e-6);

    const auto aRf = aResults.getMatrix(Side::Front, PropertySurface::R);

    const auto correctR{
      Helper::readMatrixFromCSV(TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestVenetianDirectionalShadeFlat0_3_aRf.csv")};

    Helper::compareMatrices(correctR, aRf.getMatrix(), 1e-6);
}
