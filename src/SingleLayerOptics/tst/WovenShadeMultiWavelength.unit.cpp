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
        const auto aBSDF = CBSDFHemisphere::create(BSDFBasis::Quarter);

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

    std::shared_ptr<std::vector<std::shared_ptr<CBSDFIntegrator>>> aResults =
      aLayer->getWavelengthResults();

    size_t correctSize = 4;

    EXPECT_EQ(correctSize, aResults->size());

    std::vector<double> correctResults;

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 1
    ///////////////////////////////////////////////////////////////////////

    auto aT = (*aResults)[0]->getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size_t size = aT.size();

    correctResults = {5.780996767,
                      6.070652604,
                      6.069898181,
                      6.054181733,
                      6.069898181,
                      6.070652604,
                      6.069898181,
                      6.054181733,
                      6.069898181,
                      5.09462256,
                      5.119753676,
                      4.995444276,
                      4.782710166,
                      4.995444276,
                      5.119753676,
                      5.09462256,
                      5.119753676,
                      4.995444276,
                      4.782710166,
                      4.995444276,
                      5.119753676,
                      3.750925772,
                      3.939780122,
                      3.350183557,
                      1.310590483,
                      3.350183557,
                      3.939780122,
                      3.750925772,
                      3.939780122,
                      3.350183557,
                      1.310590483,
                      3.350183557,
                      3.939780122,
                      0,
                      0,
                      0,
                      0,
                      0,
                      0,
                      0,
                      0};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    auto aRf = (*aResults)[0]->getMatrix(Side::Front, PropertySimple::R);

    correctResults = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 2
    ///////////////////////////////////////////////////////////////////////

    aT = (*aResults)[1]->getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {
      5.786840839,   6.076510952,   6.075756596,   6.060041547,   6.075756596,   6.076510952,
      6.075756596,   6.060041547,   6.075756596,   5.100533545,   5.125662822,   5.001365523,
      4.788651107,   5.001365523,   5.125662822,   5.100533545,   5.125662822,   5.001365523,
      4.788651107,   5.001365523,   5.125662822,   3.767445256,   3.960125221,   3.373357384,
      1.327344345,   3.373357384,   3.960125221,   3.767445256,   3.960125221,   3.373357384,
      1.327344345,   3.373357384,   3.960125221,   0.04389839072, 0.03328401306, 0.04389839072,
      0.03328401306, 0.04389839072, 0.03328401306, 0.04389839072, 0.03328401306};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = (*aResults)[1]->getMatrix(Side::Front, PropertySimple::R);

    correctResults = {
      0.1356269881, 0.138524879,  0.1385385148, 0.1388225822, 0.1385385148, 0.138524879,
      0.1385385148, 0.1388225822, 0.1385385148, 0.1489187911, 0.1484282395, 0.1508517131,
      0.1550000842, 0.1508517131, 0.1484282395, 0.1489187911, 0.1484282395, 0.1508517131,
      0.1550000842, 0.1508517131, 0.1484282395, 0.1646371297, 0.157111321,  0.1658344657,
      0.2122158549, 0.1658344657, 0.157111321,  0.1646371297, 0.157111321,  0.1658344657,
      0.2122158549, 0.1658344657, 0.157111321,  0.2107495182, 0.2213638959, 0.2107495182,
      0.2213638959, 0.2107495182, 0.2213638959, 0.2107495182, 0.2213638959};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 3
    ///////////////////////////////////////////////////////////////////////

    aT = (*aResults)[2]->getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {
      5.81917028,    6.109554159,   6.108803161,   6.09315808,    6.108803161,   6.109554159,
      6.108803161,   6.09315808,    6.108803161,   5.136136815,   5.161145261,   5.037444874,
      4.825752232,   5.037444874,   5.161145261,   5.136136815,   5.161145261,   5.037444874,
      4.825752232,   5.037444874,   5.161145261,   3.806817435,   3.997606119,   3.412959698,
      1.378435422,   3.412959698,   3.997606119,   3.806817435,   3.997606119,   3.412959698,
      1.378435422,   3.412959698,   3.997606119,   0.09473427962, 0.08717526634, 0.09473427962,
      0.08717526634, 0.09473427962, 0.08717526634, 0.09473427962, 0.08717526634};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = (*aResults)[2]->getMatrix(Side::Front, PropertySimple::R);

    correctResults = {
      0.1032975474, 0.105481672,  0.1054919493, 0.1057060493, 0.1054919493, 0.105481672,
      0.1054919493, 0.1057060493, 0.1054919493, 0.1133155213, 0.1129458008, 0.1147723613,
      0.1178989589, 0.1147723613, 0.1129458008, 0.1133155213, 0.1129458008, 0.1147723613,
      0.1178989589, 0.1147723613, 0.1129458008, 0.1252649508, 0.1196304232, 0.1262321515,
      0.1611247782, 0.1262321515, 0.1196304232, 0.1252649508, 0.1196304232, 0.1262321515,
      0.1611247782, 0.1262321515, 0.1196304232, 0.1599136293, 0.1674726426, 0.1599136293,
      0.1674726426, 0.1599136293, 0.1674726426, 0.1599136293, 0.1674726426};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////
    //  Wavelength number 4
    ///////////////////////////////////////////////////////////////////////

    aT = (*aResults)[3]->getMatrix(Side::Front, PropertySimple::T);

    // Test only diagonal of transmittance matrix
    size = aT.size();

    correctResults = {
      5.786840839,   6.076510952,   6.075756596,   6.060041547,   6.075756596,   6.076510952,
      6.075756596,   6.060041547,   6.075756596,   5.100533545,   5.125662822,   5.001365523,
      4.788651107,   5.001365523,   5.125662822,   5.100533545,   5.125662822,   5.001365523,
      4.788651107,   5.001365523,   5.125662822,   3.767445256,   3.960125221,   3.373357384,
      1.327344345,   3.373357384,   3.960125221,   3.767445256,   3.960125221,   3.373357384,
      1.327344345,   3.373357384,   3.960125221,   0.04389839072, 0.03328401306, 0.04389839072,
      0.03328401306, 0.04389839072, 0.03328401306, 0.04389839072, 0.03328401306};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = (*aResults)[3]->getMatrix(Side::Front, PropertySimple::R);

    correctResults = {
      0.1356269881, 0.138524879,  0.1385385148, 0.1388225822, 0.1385385148, 0.138524879,
      0.1385385148, 0.1388225822, 0.1385385148, 0.1489187911, 0.1484282395, 0.1508517131,
      0.1550000842, 0.1508517131, 0.1484282395, 0.1489187911, 0.1484282395, 0.1508517131,
      0.1550000842, 0.1508517131, 0.1484282395, 0.1646371297, 0.157111321,  0.1658344657,
      0.2122158549, 0.1658344657, 0.157111321,  0.1646371297, 0.157111321,  0.1658344657,
      0.2122158549, 0.1658344657, 0.157111321,  0.2107495182, 0.2213638959, 0.2107495182,
      0.2213638959, 0.2107495182, 0.2213638959, 0.2107495182, 0.2213638959};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }
}
