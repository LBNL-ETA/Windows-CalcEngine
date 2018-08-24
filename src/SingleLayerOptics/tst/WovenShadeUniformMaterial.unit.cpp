#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestWovenShadeUniformMaterial : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    virtual void SetUp()
    {
        // create material
		const auto Tmat = 0.2;
		const auto Rfmat = 0.75;
		const auto Rbmat = 0.75;
		const auto minLambda = 0.3;
		const auto maxLambda = 2.5;
        const auto aMaterial = SingleLayerOptics::Material::singleBandMaterial(Tmat, Tmat, Rfmat,
        	Rbmat, minLambda, maxLambda);

        // make cell geometry
		const auto diameter = 6.35;   // mm
		const auto spacing = 19.05;   // mm

        // create BSDF
        const auto aBSDF = CBSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Shade = CBSDFLayerMaker::getWovenLayer(aMaterial, aBSDF, diameter, spacing);
    }

public:
    std::shared_ptr<CBSDFLayer> GetShade()
    {
        return m_Shade;
    };
};

TEST_F(TestWovenShadeUniformMaterial, TestSolarProperties)
{
    SCOPED_TRACE("Begin Test: Woven shade uniform material.");

    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    std::shared_ptr<CBSDFIntegrator> aResults = aShade->getResults();

    const double tauDiff = aResults->DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.421567, tauDiff, 1e-6);

    const double RfDiff = aResults->DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.542281, RfDiff, 1e-6);

    const double RbDiff = aResults->DiffDiff(Side::Back, PropertySimple::R);
    EXPECT_NEAR(0.542281, RbDiff, 1e-6);

    auto aT = aResults->getMatrix(Side::Front, PropertySimple::T);

    const size_t size = aT.size();

    // Test diagonal
    std::vector<double> correctResults{
      5.816364532,   6.106748411,   6.105997413,   6.090352332,   6.105997413,   6.106748411,
      6.105997413,   6.090352332,   6.105997413,   5.133330004,   5.158338022,   5.034637516,
      4.822945422,   5.034637516,   5.158338022,   5.133330004,   5.158338022,   5.034637516,
      4.822945422,   5.034637516,   5.158338022,   3.796214925,   3.984144227,   3.39743563,
      1.367832912,   3.39743563,    3.984144227,   3.796214925,   3.984144227,   3.39743563,
      1.367832912,   3.39743563,    3.984144227,   0.06366197724, 0.06366197724, 0.06366197724,
      0.06366197724, 0.06366197724, 0.06366197724, 0.06366197724, 0.06366197724};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Test first row
    correctResults = {
      5.816364532,   0.03609580668, 0.03609923243, 0.03617059911, 0.03609923243, 0.03609580668,
      0.03609923243, 0.03617059911, 0.03609923243, 0.03870744384, 0.03858434635, 0.03919323995,
      0.04023525639, 0.03919323995, 0.03858434635, 0.03870744384, 0.03858434635, 0.03919323995,
      0.04023525639, 0.03919323995, 0.03858434635, 0.04528915348, 0.04436410513, 0.04725207318,
      0.05724242926, 0.04725207318, 0.04436410513, 0.04528915348, 0.04436410513, 0.04725207318,
      0.05724242926, 0.04725207318, 0.04436410513, 0.06366197724, 0.06366197724, 0.06366197724,
      0.06366197724, 0.06366197724, 0.06366197724, 0.06366197724, 0.06366197724};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(0, i), 1e-5);
    }

    // Test first row for reflectance matrix
    auto aRf = aResults->getMatrix(Side::Front, PropertySimple::R);

    correctResults = {
      0.1326291192, 0.135359275,  0.1353721216, 0.1356397466, 0.1353721216, 0.135359275,
      0.1353721216, 0.1356397466, 0.1353721216, 0.1451529144, 0.1446912988, 0.1469746498,
      0.1508822115, 0.1469746498, 0.1446912988, 0.1451529144, 0.1446912988, 0.1469746498,
      0.1508822115, 0.1469746498, 0.1446912988, 0.1698343255, 0.1663653942, 0.1771952744,
      0.2146591097, 0.1771952744, 0.1663653942, 0.1698343255, 0.1663653942, 0.1771952744,
      0.2146591097, 0.1771952744, 0.1663653942, 0.2387324146, 0.2387324146, 0.2387324146,
      0.2387324146, 0.2387324146, 0.2387324146, 0.2387324146, 0.2387324146};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(0, i), 1e-5);
    }

    // Test first row for reflectance matrix
    auto aRb = aResults->getMatrix(Side::Back, PropertySimple::R);

    correctResults = {
      0.1326291192, 0.135359275,  0.1353721216, 0.1356397466, 0.1353721216, 0.135359275,
      0.1353721216, 0.1356397466, 0.1353721216, 0.1451529144, 0.1446912988, 0.1469746498,
      0.1508822115, 0.1469746498, 0.1446912988, 0.1451529144, 0.1446912988, 0.1469746498,
      0.1508822115, 0.1469746498, 0.1446912988, 0.1698343255, 0.1663653942, 0.1771952744,
      0.2146591097, 0.1771952744, 0.1663653942, 0.1698343255, 0.1663653942, 0.1771952744,
      0.2146591097, 0.1771952744, 0.1663653942, 0.2387324146, 0.2387324146, 0.2387324146,
      0.2387324146, 0.2387324146, 0.2387324146, 0.2387324146, 0.2387324146};

    EXPECT_EQ(correctResults.size(), aRb.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRb(0, i), 1e-5);
    }
}
