#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat0_3 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    virtual void SetUp()
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

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.50867495442994748, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.22828908995601985, RfDiff, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    std::vector<double> correctT{
      13.007242, 14.019711, 8.879333, 6.75793,   8.879333, 14.019711, 8.879333,  6.75793,  8.879333,
      12.996986, 5.47603,   0.098804, 0.103304,  0.098804, 5.47603,   12.996986, 5.47603,  0.098804,
      0.103304,  0.098804,  5.47603,  12.996986, 0.101199, 0.102473,  0.101186,  0.102473, 0.101199,
      12.996986, 0.101199,  0.102473, 0.101186,  0.102473, 0.101199,  12.355109, 0.127734, 0.13675,
      0.127734,  12.355109, 0.127734, 0.13675,   0.127734};

    EXPECT_EQ(correctT.size(), aT.size());
    for(size_t i = 0; i < aT.size(); ++i)
    {
        EXPECT_NEAR(correctT[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    std::vector<double> correctR{
      0,        0,        0.013333, 0.026667, 0.013333, 0,        0.013333, 0.026667, 0.013333,
      0,        0.033334, 0.098802, 0.102213, 0.098802, 0.033334, 0,        0.033334, 0.098802,
      0.102213, 0.098802, 0.033334, 0,        0.100776, 0.097083, 0.093999, 0.097083, 0.100776,
      0,        0.100776, 0.097083, 0.093999, 0.097083, 0.100776, 0,        0.094803, 0.095402,
      0.094803, 0,        0.094803, 0.095402, 0.094803};

    EXPECT_EQ(correctR.size(), aRf.size());
    for(size_t i = 0; i < aRf.size(); ++i)
    {
        EXPECT_NEAR(correctR[i], aRf(i, i), 1e-6);
    }
}
