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

    // Test only diagonal of transmittance matrix
    size_t size = aT.size();

    std::vector<double> correctResults{
      0.765126, 0.82303,  3.858763, 5.115548, 3.858763, 0.82303,   0.01198,  0.008255,  0.01198,
      0.764539, 5.213146, 8.4662,   9.65615,  8.4662,   5.213146,  0.764539, 0.007232,  0.002492,
      0.001496, 0.002492, 0.007232, 0.764539, 9.187354, 10.653438, 8.403912, 10.653438, 9.187354,
      0.764539, 0.001852, 0.000594, 0.000949, 0.000594, 0.001852,  0.727829, 0.045386,  0.033435,
      0.045386, 0.727829, 0.001944, 0.001943, 0.001944};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    auto aRf = aResults[0].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.062481, 0.062481, 0.050534, 0.044039, 0.050534, 0.062481, 0.050536,
                      0.044041, 0.050536, 0.062481, 0.041453, 0.023803, 0.017463, 0.023803,
                      0.041453, 0.062481, 0.041455, 0.023805, 0.017465, 0.023805, 0.041455,
                      0.062481, 0.019953, 0.00258,  0.004763, 0.00258,  0.019953, 0.062481,
                      0.019955, 0.002581, 0.004764, 0.002581, 0.019955, 0.062481, 0.005491,
                      0.00374,  0.005491, 0.062481, 0.01548,  0.016228, 0.01548};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 2
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[1].getMatrix(Side::Front, PropertySimple::T);

    correctResults = {0.773301, 0.831205, 3.866507,  5.12286,  3.866507,  0.831205, 0.019825,
                      0.016339, 0.019825, 0.772715,  5.220243, 8.471139,  9.66001,  8.471139,
                      5.220243, 0.772715, 0.014799,  0.007218, 0.005219,  0.007218, 0.014799,
                      0.772715, 9.191658, 10.656879, 8.411225, 10.656879, 9.191658, 0.772715,
                      0.005981, 0.003077, 0.005961,  0.003077, 0.005981,  0.736005, 0.049792,
                      0.03048,  0.049792, 0.736005,  0.025492, 0.046619,  0.025492};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[1].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.052808, 0.052808, 0.042238, 0.036573, 0.042238, 0.052808, 0.042942,
                      0.037537, 0.042942, 0.052808, 0.034369, 0.019507, 0.014249, 0.019507,
                      0.034369, 0.052808, 0.035369, 0.020456, 0.015048, 0.020456, 0.035369,
                      0.052808, 0.016309, 0.00468,  0.008954, 0.00468,  0.016309, 0.052808,
                      0.017176, 0.005097, 0.00984,  0.005097, 0.017176, 0.052808, 0.033205,
                      0.049132, 0.033205, 0.052808, 0.015371, 0.016096, 0.015371};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 3
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[2].getMatrix(Side::Front, PropertySimple::T);

    correctResults = {0.765126, 0.82303,  3.858763,  5.115548, 3.858763,  0.82303,  0.01198,
                      0.008255, 0.01198,  0.764539,  5.213146, 8.4662,    9.65615,  8.4662,
                      5.213146, 0.764539, 0.007232,  0.002492, 0.001496,  0.002492, 0.007232,
                      0.764539, 9.187354, 10.653438, 8.403912, 10.653438, 9.187354, 0.764539,
                      0.001852, 0.000594, 0.000949,  0.000594, 0.001852,  0.727829, 0.045386,
                      0.033435, 0.045386, 0.727829,  0.001944, 0.001943,  0.001944};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[2].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.062481, 0.062481, 0.050534, 0.044039, 0.050534, 0.062481, 0.050536,
                      0.044041, 0.050536, 0.062481, 0.041453, 0.023803, 0.017463, 0.023803,
                      0.041453, 0.062481, 0.041455, 0.023805, 0.017465, 0.023805, 0.041455,
                      0.062481, 0.019953, 0.00258,  0.004763, 0.00258,  0.019953, 0.062481,
                      0.019955, 0.002581, 0.004764, 0.002581, 0.019955, 0.062481, 0.005491,
                      0.00374,  0.005491, 0.062481, 0.01548,  0.016228, 0.01548};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 4
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[3].getMatrix(Side::Front, PropertySimple::T);

    correctResults = {0.765126, 0.82303,  3.858763,  5.115548, 3.858763,  0.82303,  0.01198,
                      0.008255, 0.01198,  0.764539,  5.213146, 8.4662,    9.65615,  8.4662,
                      5.213146, 0.764539, 0.007232,  0.002492, 0.001496,  0.002492, 0.007232,
                      0.764539, 9.187354, 10.653438, 8.403912, 10.653438, 9.187354, 0.764539,
                      0.001852, 0.000594, 0.000949,  0.000594, 0.001852,  0.727829, 0.045386,
                      0.033435, 0.045386, 0.727829,  0.001944, 0.001943,  0.001944};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[3].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.062481, 0.062481, 0.050534, 0.044039, 0.050534, 0.062481, 0.050536,
                      0.044041, 0.050536, 0.062481, 0.041453, 0.023803, 0.017463, 0.023803,
                      0.041453, 0.062481, 0.041455, 0.023805, 0.017465, 0.023805, 0.041455,
                      0.062481, 0.019953, 0.00258,  0.004763, 0.00258,  0.019953, 0.062481,
                      0.019955, 0.002581, 0.004764, 0.002581, 0.019955, 0.062481, 0.005491,
                      0.00374,  0.005491, 0.062481, 0.01548,  0.016228, 0.01548};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }
}
