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
    EXPECT_NEAR(0.525207, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.384905, RfDiff, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    std::vector<double> correctT{
      0.836275, 0.894179, 3.926157, 5.179186, 3.926157, 0.894179, 0.074268, 0.066963, 0.074268,
      0.835689, 5.274907, 8.509182, 9.689743, 8.509182, 5.274907, 0.835689, 0.062228, 0.03495,
      0.026087, 0.03495,  0.062228, 0.835689, 9.224807, 10.68338, 8.467551, 10.68338, 9.224807,
      0.835689, 0.029567, 0.016278, 0.031378, 0.016278, 0.029567, 0.798979, 0.354038, 0.368325,
      0.354038, 0.798979, 0.121765, 0.195826, 0.121765};


    EXPECT_EQ(correctT.size(), aT.size());
    for(size_t i = 0; i < aT.size(); ++i)
    {
        EXPECT_NEAR(correctT[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    std::vector<double> correctR{
      0.165903, 0.165903, 0.132696, 0.114899, 0.132696, 0.165903, 0.136429, 0.120012, 0.136429,
      0.165903, 0.107974, 0.061285, 0.044767, 0.061285, 0.107974, 0.165903, 0.113293, 0.066369,
      0.049053, 0.066369, 0.113293, 0.165903, 0.051239, 0.014704, 0.02813,  0.014704, 0.051239,
      0.165903, 0.055884, 0.021599, 0.042784, 0.021599, 0.055884, 0.165903, 0.032645, 0.032998,
      0.032645, 0.165903, 0.057379, 0.047471, 0.057379};

    EXPECT_EQ(correctR.size(), aRf.size());
    for(size_t i = 0; i < aRf.size(); ++i)
    {
        EXPECT_NEAR(correctR[i], aRf(i, i), 1e-5);
    }
}
