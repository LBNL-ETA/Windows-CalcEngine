#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat0_1 : public testing::Test
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
        const auto slatWidth = 0.010;     // m
        const auto slatSpacing = 0.010;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0;
        const size_t numOfSlatSegments = 1;

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
    std::shared_ptr<CBSDFLayer> GetShade() const
    {
        return m_Shade;
    };
};

TEST_F(TestVenetianDirectionalShadeFlat0_1, TestVenetian1)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Flat, 45 degrees slats) - solar properties.");

    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    BSDFIntegrator aResults = aShade->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.62815686543915794, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.26606760038504212, RfDiff, 1e-6);

    const double theta = 23;
    const double phi = 198;

    const double tauHem = aResults.DirHem(Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.65530003675657189, tauHem, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    const size_t size = aT.size();

    std::vector<double> correctT{
      3.93441,  4.230955, 6.502014, 7.438862,  6.502014,  4.230955,  1.946733, 0.996723,  1.946733,
      3.931406, 7.253509, 9.654657, 10.532058, 9.654657,  7.253509,  3.931406, 0.576396,  0.065378,
      0.048215, 0.065378, 0.576396, 3.931406,  10.186359, 11.244577, 9.561371, 11.244577, 10.186359,
      3.931406, 0.054978, 0.011951, 0.023431,  0.011951,  0.054978,  3.743404, 0.088039,  0.088039,
      0.088039, 3.743404, 0.088039, 0.088039,  0.088039,
    };

    EXPECT_EQ(correctT.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctT[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    std::vector<double> correctR{
      0.044019, 0.044019, 0.041696, 0.039372, 0.041696, 0.044019, 0.041696, 0.039372, 0.041696,
      0.044019, 0.03821,  0.026592, 0.020783, 0.026592, 0.03821,  0.044019, 0.03821,  0.020037,
      0.013944, 0.020037, 0.03821,  0.044019, 0.023171, 0.018524, 0.039372, 0.018524, 0.023171,
      0.044019, 0.01626,  0.00771,  0.013944, 0.00771,  0.01626,  0.044019, 0.178554, 0.143667,
      0.178554, 0.044019, 0.043409, 0.05395,  0.043409};

    EXPECT_EQ(correctR.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctR[i], aRf(i, i), 1e-5);
    }
}

TEST_F(TestVenetianDirectionalShadeFlat0_1, AtWavelength)
{
    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    constexpr size_t wavelengthIndex{0u};
    auto aResults{aShade->getResultsAtWavelength(wavelengthIndex)};

    const auto correct{0.39574919753135934};
    const auto result{aResults.DiffDiff(Side::Front, PropertySimple::T)};

    EXPECT_NEAR(correct, result, 1e-6);
}