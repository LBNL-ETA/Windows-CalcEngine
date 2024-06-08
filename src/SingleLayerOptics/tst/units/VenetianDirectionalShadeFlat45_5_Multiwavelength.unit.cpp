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
      0.806378, 0.864282, 3.889189, 5.140599, 3.889189, 0.864282,  0.050536, 0.044041,  0.050536,
      0.805791, 5.236172, 8.477179, 9.66359,  8.477179, 5.236172,  0.805791, 0.041455,  0.023805,
      0.017465, 0.023805, 0.041455, 0.805791, 9.196133, 10.647086, 8.389687, 10.647086, 9.196133,
      0.805791, 0.019955, 0.002581, 0.004764, 0.002581, 0.019955,  0.769082, 0.005491,  0.00374,
      0.005491, 0.769082, 0.005492, 0.003741, 0.005492};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    auto aRf = aResults[0].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.021229, 0.021229, 0.020108, 0.018988, 0.020108, 0.021229, 0.01497,
                      0.012135, 0.01497,  0.021229, 0.018427, 0.012824, 0.010023, 0.012824,
                      0.018427, 0.021229, 0.011098, 0.005263, 0.003608, 0.005263, 0.011098,
                      0.021229, 0.011175, 0.008933, 0.018988, 0.008933, 0.011175, 0.021229,
                      0.004234, 0.001919, 0.0034,   0.001919, 0.004234, 0.021229, 0.045386,
                      0.033435, 0.045386, 0.021229, 0.009895, 0.011646, 0.009895};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 2
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[1].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {0.796705, 0.854609, 3.880892,  5.133133, 3.880892,  0.854609, 0.042942,
                      0.037537, 0.042942, 0.796118,  5.229088, 8.472884,  9.660377, 8.472884,
                      5.229088, 0.796118, 0.035369,  0.020456, 0.015048,  0.020456, 0.035369,
                      0.796118, 9.192489, 10.649185, 8.393878, 10.649185, 9.192489, 0.796118,
                      0.017176, 0.005097, 0.00984,   0.005097, 0.017176,  0.759409, 0.033205,
                      0.049132, 0.033205, 0.759409,  0.034505, 0.049834,  0.034505};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[1].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.029405, 0.029405, 0.027853, 0.0263,   0.027853, 0.029405, 0.020844,
                      0.016818, 0.020844, 0.029405, 0.025524, 0.017763, 0.013883, 0.017763,
                      0.025524, 0.029405, 0.015307, 0.006807, 0.004486, 0.006807, 0.015307,
                      0.029405, 0.015478, 0.012374, 0.0263,   0.012374, 0.015478, 0.029405,
                      0.005353, 0.001874, 0.003143, 0.001874, 0.005353, 0.029405, 0.049792,
                      0.03048,  0.049792, 0.029405, 0.009067, 0.010814, 0.009067};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 3
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[2].getMatrix(Side::Front, PropertySimple::T);

    correctResults = {0.806378, 0.864282, 3.889189,  5.140599, 3.889189,  0.864282, 0.050536,
                      0.044041, 0.050536, 0.805791,  5.236172, 8.477179,  9.66359,  8.477179,
                      5.236172, 0.805791, 0.041455,  0.023805, 0.017465,  0.023805, 0.041455,
                      0.805791, 9.196133, 10.647086, 8.389687, 10.647086, 9.196133, 0.805791,
                      0.019955, 0.002581, 0.004764,  0.002581, 0.019955,  0.769082, 0.005491,
                      0.00374,  0.005491, 0.769082,  0.005492, 0.003741,  0.005492};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[2].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.021229, 0.021229, 0.020108, 0.018988, 0.020108, 0.021229, 0.01497,
                      0.012135, 0.01497,  0.021229, 0.018427, 0.012824, 0.010023, 0.012824,
                      0.018427, 0.021229, 0.011098, 0.005263, 0.003608, 0.005263, 0.011098,
                      0.021229, 0.011175, 0.008933, 0.018988, 0.008933, 0.011175, 0.021229,
                      0.004234, 0.001919, 0.0034,   0.001919, 0.004234, 0.021229, 0.045386,
                      0.033435, 0.045386, 0.021229, 0.009895, 0.011646, 0.009895};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Wavelength number 4
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[3].getMatrix(Side::Front, PropertySimple::T);

    correctResults = {0.806378, 0.864282, 3.889189,  5.140599, 3.889189,  0.864282, 0.050536,
                      0.044041, 0.050536, 0.805791,  5.236172, 8.477179,  9.66359,  8.477179,
                      5.236172, 0.805791, 0.041455,  0.023805, 0.017465,  0.023805, 0.041455,
                      0.805791, 9.196133, 10.647086, 8.389687, 10.647086, 9.196133, 0.805791,
                      0.019955, 0.002581, 0.004764,  0.002581, 0.019955,  0.769082, 0.005491,
                      0.00374,  0.005491, 0.769082,  0.005492, 0.003741,  0.005492};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[3].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.021229, 0.021229, 0.020108, 0.018988, 0.020108, 0.021229, 0.01497,
                      0.012135, 0.01497,  0.021229, 0.018427, 0.012824, 0.010023, 0.012824,
                      0.018427, 0.021229, 0.011098, 0.005263, 0.003608, 0.005263, 0.011098,
                      0.021229, 0.011175, 0.008933, 0.018988, 0.008933, 0.011175, 0.021229,
                      0.004234, 0.001919, 0.0034,   0.001919, 0.004234, 0.021229, 0.045386,
                      0.033435, 0.045386, 0.021229, 0.009895, 0.011646, 0.009895};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }
}
