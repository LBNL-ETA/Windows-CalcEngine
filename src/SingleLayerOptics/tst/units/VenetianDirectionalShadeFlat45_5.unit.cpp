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
    EXPECT_NEAR(0.50151276286608992, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.36325907133573787, RfDiff, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    const size_t size = aT.size();

    std::vector<double> correctT{
      0.836275, 0.894179, 3.926157, 5.179186, 3.926157, 0.894179, 0.062283, 0.051332, 0.062283,
      0.835689, 5.274907, 8.509182, 9.689743, 8.509182, 5.274907, 0.835689, 0.046494, 0.022677,
      0.016396, 0.022677, 0.046494, 0.835689, 9.224807, 10.68338, 8.467551, 10.68338, 9.224807,
      0.835689, 0.018792, 0.009667, 0.018727, 0.009667, 0.018792, 0.798979, 0.156426, 0.095757,
      0.156426, 0.798979, 0.080085, 0.146459, 0.080085};


    EXPECT_EQ(correctT.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctT[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    std::vector<double> correctR{
      0.165903, 0.165903, 0.132696, 0.114899, 0.132696, 0.165903, 0.134906, 0.117926, 0.134906,
      0.165903, 0.107974, 0.061285, 0.044767, 0.061285, 0.107974, 0.165903, 0.111117, 0.064266,
      0.047276, 0.064266, 0.111117, 0.165903, 0.051239, 0.014704, 0.02813,  0.014704, 0.051239,
      0.165903, 0.05396,  0.016014, 0.030913, 0.016014, 0.05396,  0.165903, 0.104318, 0.154355,
      0.104318, 0.165903, 0.048289, 0.050568, 0.048289};

    EXPECT_EQ(correctR.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctR[i], aRf(i, i), 1e-5);
    }
}
