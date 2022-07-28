#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestVenetianUniformShadeFlat45_5_Multiwavelength : public testing::Test
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
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0;
        const size_t numOfSlatSegments = 5;

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
                                                    DistributionMethod::UniformDiffuse,
                                                    true);
    }

public:
    std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestVenetianUniformShadeFlat45_5_Multiwavelength, TestVenetianMultiWavelength)
{
    SCOPED_TRACE("Begin Test: Venetian layer (multi range) - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    auto aResults = aLayer->getWavelengthResults();

    size_t correctSize = 4;

    EXPECT_EQ(correctSize, aResults.size());

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 1
    ///////////////////////////////////////////////////////////////////////

    auto aT = aResults[0].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size_t size = aT.size();

    std::vector<double> correctResults{
      0.80234,  0.860244, 3.88367,  5.136014, 3.88367,  0.860244,  0.054423, 0.050166,  0.054423,
      0.801753, 5.231931, 8.475046, 9.662109, 8.475046, 5.231931,  0.801753, 0.049212,  0.042728,
      0.040355, 0.042728, 0.049212, 0.801753, 9.194403, 10.673049, 8.440885, 10.673049, 9.194403,
      0.801753, 0.04129,  0.035089, 0.033237, 0.035089, 0.04129,   0.765044, 0.121845,  0.104227,
      0.121845, 0.765044, 0.027267, 0.024246, 0.027267};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    auto aRf = aResults[0].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.148682, 0.148682, 0.114522, 0.100372, 0.114522, 0.148682, 0.16834,
                      0.174328, 0.16834,  0.148682, 0.09467,  0.05513,  0.040658, 0.05513,
                      0.09467,  0.148682, 0.175812, 0.185946, 0.189655, 0.185946, 0.175812,
                      0.148682, 0.04636,  0.01122,  0.021997, 0.01122,  0.04636,  0.148682,
                      0.188194, 0.198337, 0.201428, 0.198337, 0.188194, 0.148682, 0.0856,
                      0.09856,  0.0856,   0.148682, 0.211514, 0.216673, 0.211514};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 2
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[1].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {0.822696, 0.8806,   3.899349, 5.149756, 3.899349, 0.8806,   0.076245,
                      0.072118, 0.076245, 0.822109, 5.244892, 8.482594, 9.667675, 8.482594,
                      5.244892, 0.822109, 0.071121, 0.06432,  0.061831, 0.06432,  0.071121,
                      0.822109, 9.20075,  10.66946, 8.433849, 10.66946, 9.20075,  0.822109,
                      0.062812, 0.055871, 0.053739, 0.055871, 0.062812, 0.7854,   0.107467,
                      0.09322,  0.107467, 0.7854,   0.046481, 0.042637, 0.046481};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = aResults[1].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.129988, 0.129988, 0.100123, 0.087752, 0.100123, 0.129988, 0.146686,
                      0.151645, 0.146686, 0.129988, 0.082767, 0.048199, 0.035546, 0.048199,
                      0.082767, 0.129988, 0.1529,   0.161474, 0.164612, 0.161474, 0.1529,
                      0.129988, 0.040531, 0.015128, 0.029658, 0.015128, 0.040531, 0.129988,
                      0.163375, 0.172244, 0.174984, 0.172244, 0.163375, 0.129988, 0.108043,
                      0.120069, 0.108043, 0.129988, 0.184338, 0.189303, 0.184338};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 3
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[2].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {0.80234,  0.860244, 3.88367,   5.136014, 3.88367,   0.860244, 0.054423,
                      0.050166, 0.054423, 0.801753,  5.231931, 8.475046,  9.662109, 8.475046,
                      5.231931, 0.801753, 0.049212,  0.042728, 0.040355,  0.042728, 0.049212,
                      0.801753, 9.194403, 10.673049, 8.440885, 10.673049, 9.194403, 0.801753,
                      0.04129,  0.035089, 0.033237,  0.035089, 0.04129,   0.765044, 0.121845,
                      0.104227, 0.121845, 0.765044,  0.027267, 0.024246,  0.027267};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = aResults[2].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.148682, 0.148682, 0.114522, 0.100372, 0.114522, 0.148682, 0.16834,
                      0.174328, 0.16834,  0.148682, 0.09467,  0.05513,  0.040658, 0.05513,
                      0.09467,  0.148682, 0.175812, 0.185946, 0.189655, 0.185946, 0.175812,
                      0.148682, 0.04636,  0.01122,  0.021997, 0.01122,  0.04636,  0.148682,
                      0.188194, 0.198337, 0.201428, 0.198337, 0.188194, 0.148682, 0.0856,
                      0.09856,  0.0856,   0.148682, 0.211514, 0.216673, 0.211514};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 4
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[3].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {0.80234,  0.860244, 3.88367,   5.136014, 3.88367,   0.860244, 0.054423,
                      0.050166, 0.054423, 0.801753,  5.231931, 8.475046,  9.662109, 8.475046,
                      5.231931, 0.801753, 0.049212,  0.042728, 0.040355,  0.042728, 0.049212,
                      0.801753, 9.194403, 10.673049, 8.440885, 10.673049, 9.194403, 0.801753,
                      0.04129,  0.035089, 0.033237,  0.035089, 0.04129,   0.765044, 0.121845,
                      0.104227, 0.121845, 0.765044,  0.027267, 0.024246,  0.027267};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = aResults[3].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.148682, 0.148682, 0.114522, 0.100372, 0.114522, 0.148682, 0.16834,
                      0.174328, 0.16834,  0.148682, 0.09467,  0.05513,  0.040658, 0.05513,
                      0.09467,  0.148682, 0.175812, 0.185946, 0.189655, 0.185946, 0.175812,
                      0.148682, 0.04636,  0.01122,  0.021997, 0.01122,  0.04636,  0.148682,
                      0.188194, 0.198337, 0.201428, 0.198337, 0.188194, 0.148682, 0.0856,
                      0.09856,  0.0856,   0.148682, 0.211514, 0.216673, 0.211514};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }
}
