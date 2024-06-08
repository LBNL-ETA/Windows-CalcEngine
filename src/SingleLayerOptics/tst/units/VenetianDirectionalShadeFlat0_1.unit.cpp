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
    EXPECT_NEAR(0.55329584855741987, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.34092861726678308, RfDiff, 1e-6);

    const double theta = 23;
    const double phi = 198;

    const double tauHem = aResults.DirHem(Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.54432441274896237, tauHem, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    const size_t size = aT.size();

    std::vector<double> correctT{
      3.853753, 4.150298, 6.425614, 7.36672,   6.425614,  4.150298, 1.870334, 0.924581, 1.870334,
      3.850749, 7.183496, 9.615871, 10.504625, 9.615871,  7.183496, 3.850749, 0.506383, 0.020037,
      0.013944, 0.020037, 0.506383, 3.850749,  10.154553, 11.25115, 9.577312, 11.25115, 10.154553,
      3.850749, 0.01626,  0.00771,  0.013944,  0.00771,   0.01626,  3.662747, 0.178554, 0.143667,
      0.178554, 3.662747, 0.043409, 0.05395,   0.043409};

    EXPECT_EQ(correctT.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctT[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    std::vector<double> correctR{
      0.124677, 0.124677, 0.118095, 0.111514, 0.118095, 0.124677, 0.118095, 0.111514, 0.118095,
      0.124677, 0.108223, 0.065378, 0.048215, 0.065378, 0.108223, 0.124677, 0.108223, 0.065378,
      0.048215, 0.065378, 0.108223, 0.124677, 0.054978, 0.011951, 0.023431, 0.011951, 0.054978,
      0.124677, 0.054978, 0.011951, 0.023431, 0.011951, 0.054978, 0.124677, 0.088039, 0.088039,
      0.088039, 0.124677, 0.088039, 0.088039, 0.088039};

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

    const auto correct{0.37192019576813157};
    const auto result{aResults.DiffDiff(Side::Front, PropertySimple::T)};

    EXPECT_NEAR(correct, result, 1e-6);
}