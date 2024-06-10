#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestVenetianDirectionalShadeFlat45_5_Multiwavelength : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Layer;

protected:
    virtual void SetUp()
    {
        // Solar range material
        const auto Tsol = 0.1;
        const auto Rfsol = 0.7;
        const auto Rbsol = 0.7;
        // Visible range
        const auto Tvis = 0.2;
        const auto Rfvis = 0.6;
        const auto Rbvis = 0.6;

        const auto aMaterial =
          Material::dualBandMaterial(Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        // make cell geometry
        double slatWidth = 0.016;     // m
        double slatSpacing = 0.012;   // m
        double slatTiltAngle = 45;
        double curvatureRadius = 0;
        const size_t numOfSlatSegments = 5;

        // Method
        DistributionMethod aDistribution = DistributionMethod::DirectionalDiffuse;

        // create BSDF
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Layer = CBSDFLayerMaker::getVenetianLayer(aMaterial,
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
    std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestVenetianDirectionalShadeFlat45_5_Multiwavelength, TestVenetianMultiWavelength)
{
    SCOPED_TRACE("Begin Test: Venetian layer (multi range) - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    std::vector<BSDFIntegrator> aResults = aLayer->getWavelengthResults();

    const auto wavelengths{aLayer->getBandWavelengths()};
    const std::vector<double> correctWavelengths{0.3, 0.38, 0.780001, 2.5};

    EXPECT_EQ(wavelengths.size(), correctWavelengths.size());
    for(size_t i = 0u; i < correctWavelengths.size(); ++i)
    {
        EXPECT_NEAR(wavelengths[i], correctWavelengths[i], 1e-6);
    }

    size_t correctSize = correctWavelengths.size();

    EXPECT_EQ(correctSize, aResults.size());

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 1
    ///////////////////////////////////////////////////////////////////////

    auto aT = aResults[0].getMatrix(Side::Front, PropertySimple::T);

    std::vector<double> correctResults{
      0.765126, 0.82303,  3.858763, 5.115548, 3.858763, 0.82303,   0.018057, 0.016208,  0.018057,
      0.764539, 5.213146, 8.4662,   9.65615,  8.4662,   5.213146,  0.764539, 0.015238,  0.008733,
      0.006421, 0.008733, 0.015238, 0.764539, 9.187354, 10.653438, 8.403912, 10.653438, 9.187354,
      0.764539, 0.007329, 0.003926, 0.0073,   0.003926, 0.007329,  0.727829, 0.087112,  0.076552,
      0.087112, 0.727829, 0.022256, 0.025271, 0.022256};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < aT.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    auto aRf = aResults[0].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.062481, 0.062481, 0.050534, 0.044039, 0.050534, 0.062481, 0.051705,
                      0.045641, 0.051705, 0.062481, 0.041453, 0.023803, 0.017463, 0.023803,
                      0.041453, 0.062481, 0.043127, 0.025427, 0.018836, 0.025427, 0.043127,
                      0.062481, 0.019953, 0.00258,  0.004763, 0.00258,  0.019953, 0.062481,
                      0.02144,  0.00606,  0.012158, 0.00606,  0.02144,  0.062481, 0.015473,
                      0.016219, 0.015473, 0.062481, 0.028455, 0.023923, 0.028455};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < aRf.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 2
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[1].getMatrix(Side::Front, PropertySimple::T);

    correctResults = {0.773301, 0.831205, 3.866507,  5.12286,  3.866507,  0.831205, 0.02364,
                      0.021315, 0.02364,  0.772715,  5.220243, 8.471139,  9.66001,  8.471139,
                      5.220243, 0.772715, 0.019807,  0.011125, 0.008304,  0.011125, 0.019807,
                      0.772715, 9.191658, 10.656879, 8.411225, 10.656879, 9.191658, 0.772715,
                      0.009411, 0.005181, 0.009987,  0.005181, 0.009411,  0.736005, 0.112693,
                      0.117241, 0.112693, 0.736005,  0.038759, 0.062333,  0.038759};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < aT.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[1].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.052808, 0.052808, 0.042238, 0.036573, 0.042238, 0.052808, 0.043426,
                      0.038201, 0.043426, 0.052808, 0.034369, 0.019507, 0.014249, 0.019507,
                      0.034369, 0.052808, 0.036062, 0.021126, 0.015614, 0.021126, 0.036062,
                      0.052808, 0.016309, 0.00468,  0.008954, 0.00468,  0.016309, 0.052808,
                      0.017788, 0.006875, 0.013618, 0.006875, 0.017788, 0.052808, 0.010391,
                      0.010503, 0.010391, 0.052808, 0.018264, 0.01511,  0.018264};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < aRf.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 3
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[2].getMatrix(Side::Front, PropertySimple::T);

    correctResults = {0.765126, 0.82303,  3.858763,  5.115548, 3.858763,  0.82303,  0.018057,
                      0.016208, 0.018057, 0.764539,  5.213146, 8.4662,    9.65615,  8.4662,
                      5.213146, 0.764539, 0.015238,  0.008733, 0.006421,  0.008733, 0.015238,
                      0.764539, 9.187354, 10.653438, 8.403912, 10.653438, 9.187354, 0.764539,
                      0.007329, 0.003926, 0.0073,    0.003926, 0.007329,  0.727829, 0.087112,
                      0.076552, 0.087112, 0.727829,  0.022256, 0.025271,  0.022256};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < aT.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[2].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.062481, 0.062481, 0.050534, 0.044039, 0.050534, 0.062481, 0.051705,
                      0.045641, 0.051705, 0.062481, 0.041453, 0.023803, 0.017463, 0.023803,
                      0.041453, 0.062481, 0.043127, 0.025427, 0.018836, 0.025427, 0.043127,
                      0.062481, 0.019953, 0.00258,  0.004763, 0.00258,  0.019953, 0.062481,
                      0.02144,  0.00606,  0.012158, 0.00606,  0.02144,  0.062481, 0.015473,
                      0.016219, 0.015473, 0.062481, 0.028455, 0.023923, 0.028455};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < aRf.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 4
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[3].getMatrix(Side::Front, PropertySimple::T);

    correctResults = {0.765126, 0.82303,  3.858763,  5.115548, 3.858763,  0.82303,  0.018057,
                      0.016208, 0.018057, 0.764539,  5.213146, 8.4662,    9.65615,  8.4662,
                      5.213146, 0.764539, 0.015238,  0.008733, 0.006421,  0.008733, 0.015238,
                      0.764539, 9.187354, 10.653438, 8.403912, 10.653438, 9.187354, 0.764539,
                      0.007329, 0.003926, 0.0073,    0.003926, 0.007329,  0.727829, 0.087112,
                      0.076552, 0.087112, 0.727829,  0.022256, 0.025271,  0.022256};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < aT.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[3].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.062481, 0.062481, 0.050534, 0.044039, 0.050534, 0.062481, 0.051705,
                      0.045641, 0.051705, 0.062481, 0.041453, 0.023803, 0.017463, 0.023803,
                      0.041453, 0.062481, 0.043127, 0.025427, 0.018836, 0.025427, 0.043127,
                      0.062481, 0.019953, 0.00258,  0.004763, 0.00258,  0.019953, 0.062481,
                      0.02144,  0.00606,  0.012158, 0.00606,  0.02144,  0.062481, 0.015473,
                      0.016219, 0.015473, 0.062481, 0.028455, 0.023923, 0.028455};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < aRf.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }
}
