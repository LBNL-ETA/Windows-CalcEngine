#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestWovenShadeMultiWavelength : public testing::Test
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
        const auto diameter = 6.35;   // mm
        const auto spacing = 19.05;   // mm

        // create BSDF
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Layer = CBSDFLayerMaker::getWovenLayer(aMaterial, aBSDF, diameter, spacing);
    }

public:
    std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestWovenShadeMultiWavelength, TestWovenMultiWavelength)
{
    SCOPED_TRACE("Begin Test: Perforated layer (multi range) - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    auto aResults = aLayer->getWavelengthResults();

    const auto wavelengths{aLayer->getBandWavelengths()};
    const std::vector<double> correctWavelengths{0.3, 0.38, 0.780002, 2.5};

    EXPECT_EQ(wavelengths.size(), correctWavelengths.size());

    for(size_t i = 0u; i < correctWavelengths.size(); ++i)
    {
        EXPECT_NEAR(wavelengths[i], correctWavelengths[i], 1e-6);
    }

    size_t correctSize = correctWavelengths.size();

    EXPECT_EQ(correctSize, aResults.size());

    std::vector<double> correctResults;

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 1
    ///////////////////////////////////////////////////////////////////////

    auto aT = aResults[0].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size_t size = aT.size();

    correctResults = {5.786283, 6.076508, 6.075762, 6.060038, 6.075762, 6.076508, 6.075762,
                      6.060038, 6.075762, 5.1071,   5.132581, 5.008355, 4.795191, 5.008355,
                      5.132581, 5.1071,   5.132581, 5.008355, 4.795191, 5.008355, 5.132581,
                      3.786365, 3.977287, 3.388986, 1.346054, 3.388986, 3.977287, 3.786365,
                      3.977287, 3.388986, 1.346054, 3.388986, 3.977287, 0.03677,  0.013011,
                      0.03677,  0.013011, 0.03677,  0.013011, 0.03677,  0.013011};

    EXPECT_EQ(correctResults.size(), aT.size());

    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    auto aRf = aResults[0].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.136184, 0.138526, 0.138533, 0.138825, 0.138533, 0.138526, 0.138533,
                      0.138825, 0.138533, 0.142351, 0.14151,  0.143861, 0.148459, 0.143861,
                      0.14151,  0.142351, 0.14151,  0.143861, 0.148459, 0.143861, 0.14151,
                      0.145716, 0.139948, 0.150205, 0.193505, 0.150205, 0.139948, 0.145716,
                      0.139948, 0.150205, 0.193505, 0.150205, 0.139948, 0.217877, 0.241636,
                      0.217877, 0.241636, 0.217877, 0.241636, 0.217877, 0.241636};

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

    correctResults = {5.819182, 6.109986, 6.109241, 6.09359,  6.109241, 6.109986, 6.109241,
                      6.09359,  6.109241, 5.141467, 5.166734, 5.043095, 4.831082, 5.043095,
                      5.166734, 5.141467, 5.166734, 5.043095, 4.831082, 5.043095, 5.166734,
                      3.821344, 4.010806, 3.425052, 1.392962, 3.425052, 4.010806, 3.821344,
                      4.010806, 3.425052, 1.392962, 3.425052, 4.010806, 0.090127, 0.072878,
                      0.090127, 0.072878, 0.090127, 0.072878, 0.090127, 0.072878};

    EXPECT_EQ(correctResults.size(), aT.size());

    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = aResults[1].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.103285, 0.105049, 0.105053, 0.105273, 0.105053, 0.105049, 0.105053,
                      0.105273, 0.105053, 0.107984, 0.107356, 0.109122, 0.112568, 0.109122,
                      0.107356, 0.107984, 0.107356, 0.109122, 0.112568, 0.109122, 0.107356,
                      0.110737, 0.10643,  0.114139, 0.146597, 0.114139, 0.10643,  0.110737,
                      0.10643,  0.114139, 0.146597, 0.114139, 0.10643,  0.16452,  0.181769,
                      0.16452,  0.181769, 0.16452,  0.181769, 0.16452,  0.181769};

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

    correctResults = {5.786283, 6.076508, 6.075762, 6.060038, 6.075762, 6.076508, 6.075762,
                      6.060038, 6.075762, 5.1071,   5.132581, 5.008355, 4.795191, 5.008355,
                      5.132581, 5.1071,   5.132581, 5.008355, 4.795191, 5.008355, 5.132581,
                      3.786365, 3.977287, 3.388986, 1.346054, 3.388986, 3.977287, 3.786365,
                      3.977287, 3.388986, 1.346054, 3.388986, 3.977287, 0.03677,  0.013011,
                      0.03677,  0.013011, 0.03677,  0.013011, 0.03677,  0.013011};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = aResults[2].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.136184, 0.138526, 0.138533, 0.138825, 0.138533, 0.138526, 0.138533,
                      0.138825, 0.138533, 0.142351, 0.14151,  0.143861, 0.148459, 0.143861,
                      0.14151,  0.142351, 0.14151,  0.143861, 0.148459, 0.143861, 0.14151,
                      0.145716, 0.139948, 0.150205, 0.193505, 0.150205, 0.139948, 0.145716,
                      0.139948, 0.150205, 0.193505, 0.150205, 0.139948, 0.217877, 0.241636,
                      0.217877, 0.241636, 0.217877, 0.241636, 0.217877, 0.241636};

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

    correctResults = {5.786283, 6.076508, 6.075762, 6.060038, 6.075762, 6.076508, 6.075762,
                      6.060038, 6.075762, 5.1071,   5.132581, 5.008355, 4.795191, 5.008355,
                      5.132581, 5.1071,   5.132581, 5.008355, 4.795191, 5.008355, 5.132581,
                      3.786365, 3.977287, 3.388986, 1.346054, 3.388986, 3.977287, 3.786365,
                      3.977287, 3.388986, 1.346054, 3.388986, 3.977287, 0.03677,  0.013011,
                      0.03677,  0.013011, 0.03677,  0.013011, 0.03677,  0.013011};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = aResults[3].getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0.136184, 0.138526, 0.138533, 0.138825, 0.138533, 0.138526, 0.138533,
                      0.138825, 0.138533, 0.142351, 0.14151,  0.143861, 0.148459, 0.143861,
                      0.14151,  0.142351, 0.14151,  0.143861, 0.148459, 0.143861, 0.14151,
                      0.145716, 0.139948, 0.150205, 0.193505, 0.150205, 0.139948, 0.145716,
                      0.139948, 0.150205, 0.193505, 0.150205, 0.139948, 0.217877, 0.241636,
                      0.217877, 0.241636, 0.217877, 0.241636, 0.217877, 0.241636};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }
}

TEST_F(TestWovenShadeMultiWavelength, AtWavelength)
{
    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    constexpr size_t wavelengthIndex{3u};
    auto aResults = aLayer->getResultsAtWavelength(wavelengthIndex);

    std::vector<double> correctResults = {
      5.786283, 6.076508, 6.075762, 6.060038, 6.075762, 6.076508, 6.075762, 6.060038, 6.075762,
      5.1071,   5.132581, 5.008355, 4.795191, 5.008355, 5.132581, 5.1071,   5.132581, 5.008355,
      4.795191, 5.008355, 5.132581, 3.786365, 3.977287, 3.388986, 1.346054, 3.388986, 3.977287,
      3.786365, 3.977287, 3.388986, 1.346054, 3.388986, 3.977287, 0.03677,  0.013011, 0.03677,
      0.013011, 0.03677,  0.013011, 0.03677,  0.013011};

    const auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < correctResults.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }
}