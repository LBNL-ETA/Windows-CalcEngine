#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestCircularPerforatedShade1 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    virtual void SetUp()
    {
        // create material
        const auto Tmat = 0.2;
        const auto Rfmat = 0.75;
        const auto Rbmat = 0.66;
        const auto aMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        const auto x = 0.0225;          // m
        const auto y = 0.0381;          // m
        const auto thickness = 0.005;   // m
        const auto radius = 0.00835;    // m

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Shade =
          CBSDFLayerMaker::getCircularPerforatedLayer(aMaterial, aBSDF, x, y, thickness, radius);
    }

public:
    std::shared_ptr<CBSDFLayer> GetShade()
    {
        return m_Shade;
    };
};

TEST_F(TestCircularPerforatedShade1, TestSolarProperties)
{
    constexpr auto Tolerance = 1e-6;
    SCOPED_TRACE("Begin Test: Circular perforated cell - Solar properties.");

    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    BSDFIntegrator aResults = aShade->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.320845, tauDiff, Tolerance);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.636708, RfDiff, Tolerance);

    const double RbDiff = aResults.DiffDiff(Side::Back, PropertySimple::R);
    EXPECT_NEAR(0.560303, RbDiff, Tolerance);

    auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    const auto size = aT.size();

    // Test diagonal
    std::vector<double> correctResults{
      3.370933, 3.282731, 3.282731, 3.282731, 3.282731, 3.282731, 3.282731, 3.282731, 3.282731,
      2.649459, 2.649459, 2.649459, 2.649459, 2.649459, 2.649459, 2.649459, 2.649459, 2.649459,
      2.649459, 2.649459, 2.649459, 2.006498, 2.006498, 2.006498, 2.006498, 2.006498, 2.006498,
      2.006498, 2.006498, 2.006498, 2.006498, 2.006498, 2.006498, 0.063662, 0.063662, 0.063662,
      0.063662, 0.063662, 0.063662, 0.063662, 0.063662};

    std::vector<double> calculatedResults;
    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aT(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], Tolerance);
    }

    // Test first row
    correctResults = {3.370933, 0.048978, 0.048978, 0.048978, 0.048978, 0.048978, 0.048978,
                      0.048978, 0.048978, 0.050934, 0.050934, 0.050934, 0.050934, 0.050934,
                      0.050934, 0.050934, 0.050934, 0.050934, 0.050934, 0.050934, 0.050934,
                      0.054099, 0.054099, 0.054099, 0.054099, 0.054099, 0.054099, 0.054099,
                      0.054099, 0.054099, 0.054099, 0.054099, 0.054099, 0.063662, 0.063662,
                      0.063662, 0.063662, 0.063662, 0.063662, 0.063662, 0.063662};

    calculatedResults.clear();
    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aT(0, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], Tolerance);
    }

    // Test first row for reflectance matrix
    auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.177733, 0.183667, 0.183667, 0.183667, 0.183667, 0.183667, 0.183667,
                      0.183667, 0.183667, 0.191002, 0.191002, 0.191002, 0.191002, 0.191002,
                      0.191002, 0.191002, 0.191002, 0.191002, 0.191002, 0.191002, 0.191002,
                      0.202870, 0.202870, 0.202870, 0.202870, 0.202870, 0.202870, 0.202870,
                      0.202870, 0.202870, 0.202870, 0.202870, 0.202870, 0.238732, 0.238732,
                      0.238732, 0.238732, 0.238732, 0.238732, 0.238732, 0.238732};

    calculatedResults.clear();
    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aRf(0, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], Tolerance);
    }

    // Test first row for reflectance matrix
    auto aRb = aResults.getMatrix(Side::Back, PropertySimple::R);

    correctResults = {0.156405, 0.161627, 0.161627, 0.161627, 0.161627, 0.161627, 0.161627,
                      0.161627, 0.161627, 0.168082, 0.168082, 0.168082, 0.168082, 0.168082,
                      0.168082, 0.168082, 0.168082, 0.168082, 0.168082, 0.168082, 0.168082,
                      0.178526, 0.178526, 0.178526, 0.178526, 0.178526, 0.178526, 0.178526,
                      0.178526, 0.178526, 0.178526, 0.178526, 0.178526, 0.210085, 0.210085,
                      0.210085, 0.210085, 0.210085, 0.210085, 0.210085, 0.210085};

    calculatedResults.clear();
    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aRb(0, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], Tolerance);
    }
}

TEST_F(TestCircularPerforatedShade1, AtWavelength)
{

    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    constexpr size_t wavelengthIndex{0u};
    auto aResults{aShade->getResultsAtWavelength(wavelengthIndex)};

    const auto correct{0.320845};
    const auto result{aResults.DiffDiff(Side::Front, PropertySimple::T)};

    EXPECT_NEAR(correct, result, 1e-6);
}