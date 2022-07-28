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
        const auto aBSDF = CBSDFHemisphere::create(BSDFBasis::Quarter);

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
    //  Wavelength number 1
    ///////////////////////////////////////////////////////////////////////

    auto aT = aResults[0].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size_t size = aT.size();

    std::vector<double> correctResults{
      0.810591, 0.868495, 3.878222, 5.126954, 3.878222, 0.868495,  0.054218, 0.050793,  0.054218,
      0.810004, 5.221758, 8.462545, 9.651114, 8.462545, 5.221758,  0.810004, 0.046418,  0.029956,
      0.028063, 0.029956, 0.046418, 0.810004, 9.182664, 10.646963, 8.394372, 10.646963, 9.182664,
      0.810004, 0.028635, 0.017473, 0.015928, 0.017473, 0.028635,  0.773294, 0.126744,  0.126608,
      0.126744, 0.773294, 0.009315, 0.00915,  0.009315};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    auto aRf = aResults[0].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.196291, 0.196291, 0.150276, 0.138258, 0.150276, 0.196291, 0.158764,
                      0.13836,  0.158764, 0.196291, 0.126729, 0.070042, 0.053571, 0.070042,
                      0.126729, 0.196291, 0.130237, 0.074786, 0.054869, 0.074786, 0.130237,
                      0.196291, 0.060051, 0.006725, 0.012817, 0.006725, 0.060051, 0.196291,
                      0.062692, 0.00811,  0.014968, 0.00811,  0.062692, 0.196291, 0.036972,
                      0.04909,  0.036972, 0.196291, 0.046854, 0.046395, 0.046854};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 2
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[1].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {0.836275, 0.894179, 3.893461,  5.13866,  3.893461,  0.894179, 0.088417,
                      0.088474, 0.088417, 0.835689,  5.232171, 8.466077,  9.653035, 8.466077,
                      5.232171, 0.835689, 0.083366,  0.061722, 0.058886,  0.061722, 0.083366,
                      0.835689, 9.185161, 10.647909, 8.398011, 10.647909, 9.185161, 0.835689,
                      0.059735, 0.027508, 0.024511,  0.027508, 0.059735,  0.798979, 0.093292,
                      0.079357, 0.093292, 0.798979,  0.01731,  0.018209,  0.01731};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[1].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.165903, 0.165903, 0.127956, 0.117843, 0.127956, 0.165903, 0.134906,
                      0.117926, 0.134906, 0.165903, 0.108225, 0.060355, 0.046206, 0.060355,
                      0.108225, 0.165903, 0.111117, 0.064266, 0.047276, 0.064266, 0.111117,
                      0.165903, 0.051783, 0.014108, 0.027952, 0.014108, 0.051783, 0.165903,
                      0.05396,  0.016014, 0.030913, 0.016014, 0.05396,  0.165903, 0.107196,
                      0.181881, 0.107196, 0.165903, 0.128611, 0.180142, 0.128611};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 3
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[2].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {0.810591, 0.868495, 3.878222,  5.126954, 3.878222,  0.868495, 0.054218,
                      0.050793, 0.054218, 0.810004,  5.221758, 8.462545,  9.651114, 8.462545,
                      5.221758, 0.810004, 0.046418,  0.029956, 0.028063,  0.029956, 0.046418,
                      0.810004, 9.182664, 10.646963, 8.394372, 10.646963, 9.182664, 0.810004,
                      0.028635, 0.017473, 0.015928,  0.017473, 0.028635,  0.773294, 0.126744,
                      0.126608, 0.126744, 0.773294,  0.009315, 0.00915,   0.009315};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[2].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.196291, 0.196291, 0.150276, 0.138258, 0.150276, 0.196291, 0.158764,
                      0.13836,  0.158764, 0.196291, 0.126729, 0.070042, 0.053571, 0.070042,
                      0.126729, 0.196291, 0.130237, 0.074786, 0.054869, 0.074786, 0.130237,
                      0.196291, 0.060051, 0.006725, 0.012817, 0.006725, 0.060051, 0.196291,
                      0.062692, 0.00811,  0.014968, 0.00811,  0.062692, 0.196291, 0.036972,
                      0.04909,  0.036972, 0.196291, 0.046854, 0.046395, 0.046854};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 4
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[3].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {0.810591, 0.868495, 3.878222,  5.126954, 3.878222,  0.868495, 0.054218,
                      0.050793, 0.054218, 0.810004,  5.221758, 8.462545,  9.651114, 8.462545,
                      5.221758, 0.810004, 0.046418,  0.029956, 0.028063,  0.029956, 0.046418,
                      0.810004, 9.182664, 10.646963, 8.394372, 10.646963, 9.182664, 0.810004,
                      0.028635, 0.017473, 0.015928,  0.017473, 0.028635,  0.773294, 0.126744,
                      0.126608, 0.126744, 0.773294,  0.009315, 0.00915,   0.009315};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Front reflectance
    aRf = aResults[3].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.196291, 0.196291, 0.150276, 0.138258, 0.150276, 0.196291, 0.158764,
                      0.13836,  0.158764, 0.196291, 0.126729, 0.070042, 0.053571, 0.070042,
                      0.126729, 0.196291, 0.130237, 0.074786, 0.054869, 0.074786, 0.130237,
                      0.196291, 0.060051, 0.006725, 0.012817, 0.006725, 0.060051, 0.196291,
                      0.062692, 0.00811,  0.014968, 0.00811,  0.062692, 0.196291, 0.036972,
                      0.04909,  0.036972, 0.196291, 0.046854, 0.046395, 0.046854};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-6);
    }
}
