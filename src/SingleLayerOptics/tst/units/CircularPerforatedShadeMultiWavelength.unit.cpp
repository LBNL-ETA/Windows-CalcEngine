#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestCircularPerforatedShadeMultiWavelength : public testing::Test
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

        const auto ratio = 0.49;

        const auto aMaterial =
          Material::dualBandMaterial(Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis, ratio);

        // make cell geometry
        const auto x = 22.5;        // mm
        const auto y = 38.1;        // mm
        const auto thickness = 5;   // mm
        const auto radius = 8.35;   // mm

        const auto aBSDF = CBSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Layer =
          CBSDFLayerMaker::getCircularPerforatedLayer(aMaterial, aBSDF, x, y, thickness, radius);
    }

public:
    std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestCircularPerforatedShadeMultiWavelength, TestCircularPerforatedMultiWavelength)
{
    SCOPED_TRACE("Begin Test: Perforated layer (multi range) - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    std::vector<BSDFIntegrator> aResults = aLayer->getWavelengthResults();

    const auto wavelengths{aLayer->getBandWavelengths()};
    const std::vector<double> correctWavelengths{0.3, 0.38, 0.780002, 2.5};

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
      3.324467, 3.234713, 3.234713, 3.234713, 3.234713, 3.234713, 3.234713, 3.234713, 3.234713,
      2.599524, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524,
      2.599524, 2.599524, 2.599524, 1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 1.953460,
      1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 0.001248, 0.001248, 0.001248,
      0.001248, 0.001248, 0.001248, 0.001248, 0.001248};

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

    // Front reflectance
    auto aRf = aResults[0].getMatrix(Side::Front, PropertySimple::R);

    calculatedResults.clear();
    correctResults = {0.188652, 0.194951, 0.194951, 0.194951, 0.194951, 0.194951, 0.194951,
                      0.194951, 0.194951, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737,
                      0.202737, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737,
                      0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.215334,
                      0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.253400, 0.253400,
                      0.253400, 0.253400, 0.253400, 0.253400, 0.253400, 0.253400};

    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aRf(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 2
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[1].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    calculatedResults.clear();
    correctResults = {3.370933, 3.282731, 3.282731, 3.282731, 3.282731, 3.282731, 3.282731,
                      3.282731, 3.282731, 2.649459, 2.649459, 2.649459, 2.649459, 2.649459,
                      2.649459, 2.649459, 2.649459, 2.649459, 2.649459, 2.649459, 2.649459,
                      2.006498, 2.006498, 2.006498, 2.006498, 2.006498, 2.006498, 2.006498,
                      2.006498, 2.006498, 2.006498, 2.006498, 2.006498, 0.063662, 0.063662,
                      0.063662, 0.063662, 0.063662, 0.063662, 0.063662, 0.063662};

    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aT(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], 1e-5);
    }

    // Front reflectance
    aRf = aResults[1].getMatrix(Side::Front, PropertySimple::R);

    calculatedResults.clear();
    correctResults = {0.142186, 0.146934, 0.146934, 0.146934, 0.146934, 0.146934, 0.146934,
                      0.146934, 0.146934, 0.152802, 0.152802, 0.152802, 0.152802, 0.152802,
                      0.152802, 0.152802, 0.152802, 0.152802, 0.152802, 0.152802, 0.152802,
                      0.162296, 0.162296, 0.162296, 0.162296, 0.162296, 0.162296, 0.162296,
                      0.162296, 0.162296, 0.162296, 0.162296, 0.162296, 0.190986, 0.190986,
                      0.190986, 0.190986, 0.190986, 0.190986, 0.190986, 0.190986};

    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aRf(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 3
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[2].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    calculatedResults.clear();

    correctResults = {3.324467, 3.234713, 3.234713, 3.234713, 3.234713, 3.234713, 3.234713,
                      3.234713, 3.234713, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524,
                      2.599524, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524,
                      1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 1.953460,
                      1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 0.001248, 0.001248,
                      0.001248, 0.001248, 0.001248, 0.001248, 0.001248, 0.001248};

    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aT(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], 1e-5);
    }

    // Front reflectance
    aRf = aResults[2].getMatrix(Side::Front, PropertySimple::R);

    calculatedResults.clear();

    correctResults = {0.188652, 0.194951, 0.194951, 0.194951, 0.194951, 0.194951, 0.194951,
                      0.194951, 0.194951, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737,
                      0.202737, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737,
                      0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.215334,
                      0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.253400, 0.253400,
                      0.253400, 0.253400, 0.253400, 0.253400, 0.253400, 0.253400};

    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aRf(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 4
    ///////////////////////////////////////////////////////////////////////

    aT = aResults[3].getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    calculatedResults.clear();

    correctResults = {3.324467, 3.234713, 3.234713, 3.234713, 3.234713, 3.234713, 3.234713,
                      3.234713, 3.234713, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524,
                      2.599524, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524, 2.599524,
                      1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 1.953460,
                      1.953460, 1.953460, 1.953460, 1.953460, 1.953460, 0.001248, 0.001248,
                      0.001248, 0.001248, 0.001248, 0.001248, 0.001248, 0.001248};

    for(size_t i = 0; i < size; ++i)
    {
        calculatedResults.push_back(aT(i, i));
    }

    EXPECT_EQ(correctResults.size(), calculatedResults.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], calculatedResults[i], 1e-5);
    }

    // Front reflectance
    aRf = aResults[3].getMatrix(Side::Front, PropertySimple::R);

    calculatedResults.clear();

    correctResults = {0.188652, 0.194951, 0.194951, 0.194951, 0.194951, 0.194951, 0.194951,
                      0.194951, 0.194951, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737,
                      0.202737, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737, 0.202737,
                      0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.215334,
                      0.215334, 0.215334, 0.215334, 0.215334, 0.215334, 0.253400, 0.253400,
                      0.253400, 0.253400, 0.253400, 0.253400, 0.253400, 0.253400};

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
