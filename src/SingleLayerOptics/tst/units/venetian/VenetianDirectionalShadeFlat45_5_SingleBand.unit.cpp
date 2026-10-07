#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"

#include "csvHandlers.hpp"
#include "matrixTesting.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat45_5_SingleBand : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    void SetUp() override
    {
        // create material
        const auto Tmat = 0.0;
        const auto Rfmat = 0.1;
        const auto Rbmat = 0.1;
        const auto aMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        const auto slatWidth = 0.05;     // m
        const auto slatSpacing = 0.07;   // m
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

TEST_F(TestVenetianDirectionalShadeFlat45_5_SingleBand, TestVenetian1)
{
    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    BSDFIntegrator aResults = aShade->getResults();

    const double tau = aResults.DirHem(Side::Front, PropertySurface::T, 0, 0);
    EXPECT_NEAR(0.501682, tau, 1e-6);

    const double Rf = aResults.DirHem(Side::Front, PropertySurface::R, 0, 0);
    EXPECT_NEAR(0.035756, Rf, 1e-6);

    const auto absFront = aResults.AbsDiffDiff(Side::Front);
    EXPECT_NEAR(0.539078, absFront, 1e-6);
}
