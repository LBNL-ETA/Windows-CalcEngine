#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestRectangularPerforatedShade1 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    virtual void SetUp()
    {
        // create material
        const auto Tmat = 0.0;
        const auto Rfmat = 0.7;
        const auto Rbmat = 0.7;
        const auto aMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        const auto x = 19.05;         // mm
        const auto y = 19.05;         // mm
        const auto thickness = 0.6;   // mm
        const auto xHole = 3.175;     // mm
        const auto yHole = 6.35;      // mm

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        m_Shade = CBSDFLayerMaker::getRectangularPerforatedLayer(
          aMaterial, aBSDF, x, y, thickness, xHole, yHole);
    }

public:
    std::shared_ptr<CBSDFLayer> GetShade()
    {
        return m_Shade;
    };
};

TEST_F(TestRectangularPerforatedShade1, TestSolarProperties)
{
    SCOPED_TRACE("Begin Test: Rectangular perforated cell - Solar properties.");

    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    BSDFIntegrator aResults = aShade->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.041876313, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.670686365, RfDiff, 1e-6);

    const auto theta{0};
    const auto phi{0};

    const auto tauDirHem{aResults.DirHem(Side::Front, PropertySurface::T, theta, phi)};
    EXPECT_NEAR(0.055556, tauDirHem, 1e-6);

    const double RfDirHem{aResults.DirHem(Side::Front, PropertySurface::R, theta, phi)};
    EXPECT_NEAR(0.661111, RfDirHem, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySurface::T);

    // Test only diagonal of transmittance matrix
    const size_t size = aT.size();

    std::vector<double> correctResults{
      0.722625, 0.731048, 0.728881, 0.754961, 0.728881, 0.731048, 0.728881, 0.754961, 0.728881,
      0.622917, 0.614362, 0.632505, 0.672486, 0.632505, 0.614362, 0.622917, 0.614362, 0.632505,
      0.672486, 0.632505, 0.614362, 0.534246, 0.523031, 0.557403, 0.628150, 0.557403, 0.523031,
      0.534246, 0.523031, 0.557403, 0.628150, 0.557403, 0.523031, 0.146104, 0.219651, 0.416249,
      0.219651, 0.146104, 0.219651, 0.416249, 0.219651};

    std::vector<double> calculatedResults;
    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aT(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], 1e-5);
    }

    auto aRf = aResults.getMatrix(Side::Front, PropertySurface::R);

    correctResults = {0.210438, 0.211198, 0.211233, 0.210818, 0.211233, 0.211198, 0.211233,
                      0.210818, 0.211233, 0.212138, 0.212284, 0.211973, 0.211288, 0.211973,
                      0.212284, 0.212138, 0.212284, 0.211973, 0.211288, 0.211973, 0.212284,
                      0.213658, 0.213850, 0.213261, 0.212048, 0.213261, 0.213850, 0.213658,
                      0.213850, 0.213261, 0.212048, 0.213261, 0.213850, 0.220182, 0.218856,
                      0.215310, 0.218856, 0.220182, 0.218856, 0.215310, 0.218856};

    calculatedResults.clear();
    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aRf(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], 1e-5);
    }
}

TEST_F(TestRectangularPerforatedShade1, AtWavelength)
{
    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    constexpr size_t wavelengthIndex{0u};
    auto aResults{aShade->getResultsAtWavelength(wavelengthIndex)};

    const auto correct{0.0418763};
    const auto result{aResults.DiffDiff(Side::Front, PropertySurface::T)};

    EXPECT_NEAR(correct, result, 1e-6);
}
