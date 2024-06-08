#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat45_5 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    virtual void SetUp()
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
    EXPECT_NEAR(0.558780, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.305478, RfDiff, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    const size_t size = aT.size();

    std::vector<double> correctT{
      0.9098,   0.967704, 3.971351, 5.21146,  3.971351, 0.967704,  0.134906, 0.117926,  0.134906,
      0.909213, 5.302693, 8.514661, 9.690895, 8.514661, 5.302693,  0.909213, 0.111117,  0.064266,
      0.047276, 0.064266, 0.111117, 0.909213, 9.227419, 10.659209, 8.413055, 10.659209, 9.227419,
      0.909213, 0.05396,  0.016014, 0.030913, 0.016014, 0.05396,   0.872504, 0.104318,  0.154355,
      0.104318, 0.872504, 0.108403, 0.15656,  0.108403};


    EXPECT_EQ(correctT.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctT[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    std::vector<double> correctR{
      0.092379, 0.092379, 0.087502, 0.082626, 0.087502, 0.092379, 0.065485, 0.052836, 0.065485,
      0.092379, 0.080188, 0.055806, 0.043615, 0.055806, 0.080188, 0.092379, 0.048089, 0.021387,
      0.014095, 0.021387, 0.048089, 0.092379, 0.048627, 0.038875, 0.082626, 0.038875, 0.048627,
      0.092379, 0.016819, 0.005889, 0.009875, 0.005889, 0.016819, 0.092379, 0.156426, 0.095757,
      0.156426, 0.092379, 0.028485, 0.033976, 0.028485};

    EXPECT_EQ(correctR.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctR[i], aRf(i, i), 1e-5);
    }
}
