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
        double Tmat = 0.1;
        double Rfmat = 0.7;
        double Rbmat = 0.7;
        double minLambda = 0.3;
        double maxLambda = 2.5;
        std::shared_ptr<CMaterial> aSolarRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda);

        // Visible range
        Tmat = 0.2;
        Rfmat = 0.6;
        Rbmat = 0.6;
        minLambda = 0.38;
        maxLambda = 0.78;
        std::shared_ptr<CMaterial> aVisibleRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda);

        double ratio = 0.49;

        std::shared_ptr<CMaterial> aMaterial =
          std::make_shared<CMaterialDualBand>(aVisibleRangeMaterial, aSolarRangeMaterial, ratio);

        // make cell geometry
        double diameter = 6.35;   // mm
        double spacing = 19.05;   // mm

        // create BSDF
        auto aBSDF = std::make_shared<CBSDFHemisphere>(BSDFBasis::Quarter);

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
      5.781690252,    6.071360365,    6.070606009,    6.05489096,     6.070606009,
      6.071360365,    6.070606009,    6.05489096,     6.070606009,    5.09538153,
      5.120510232,    4.996212771,    4.783499092,    4.996212771,    5.120510232,
      5.09538153,     5.120510232,    4.996212771,    4.783499092,    4.996212771,
      5.120510232,    3.751813794,    3.940650006,    3.351110068,    1.311712884,
      3.351110068,    3.940650006,    3.751813794,    3.940650006,    3.351110068,
      1.311712884,    3.351110068,    3.940650006,    0.001248274063, 0.001248274063,
      0.001248274063, 0.001248274063, 0.001248274063, 0.001248274063, 0.001248274063,
      0.001248274063};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = (*aResults)[1]->getMatrix(Side::Front, PropertySimple::R);

    correctResults = {
      0.1407775749, 0.1436754658, 0.1436891016, 0.143973169,  0.1436891016, 0.1436754658,
      0.1436891016, 0.143973169,  0.1436891016, 0.1540708059, 0.1535808296, 0.1560044649,
      0.160152099,  0.1560044649, 0.1535808296, 0.1540708059, 0.1535808296, 0.1560044649,
      0.160152099,  0.1560044649, 0.1535808296, 0.1802685913, 0.1765865361, 0.1880817815,
      0.2278473164, 0.1880817815, 0.1765865361, 0.1802685913, 0.1765865361, 0.1880817815,
      0.2278473164, 0.1880817815, 0.1765865361, 0.2533996349, 0.2533996349, 0.2533996349,
      0.2533996349, 0.2533996349, 0.2533996349, 0.2533996349, 0.2533996349};

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

    // Front reflectance
    aRf = (*aResults)[2]->getMatrix(Side::Front, PropertySimple::R);

    correctResults = {
      0.1061032954, 0.10828742,   0.1082976973, 0.1085117973, 0.1082976973, 0.10828742,
      0.1082976973, 0.1085117973, 0.1082976973, 0.1161223315, 0.1157530391, 0.1175797199,
      0.1207057692, 0.1175797199, 0.1157530391, 0.1161223315, 0.1157530391, 0.1175797199,
      0.1207057692, 0.1175797199, 0.1157530391, 0.1358674604, 0.1330923154, 0.1417562195,
      0.1717272878, 0.1417562195, 0.1330923154, 0.1358674604, 0.1330923154, 0.1417562195,
      0.1717272878, 0.1417562195, 0.1330923154, 0.1909859317, 0.1909859317, 0.1909859317,
      0.1909859317, 0.1909859317, 0.1909859317, 0.1909859317, 0.1909859317};

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
      5.781690252,    6.071360365,    6.070606009,    6.05489096,     6.070606009,
      6.071360365,    6.070606009,    6.05489096,     6.070606009,    5.09538153,
      5.120510232,    4.996212771,    4.783499092,    4.996212771,    5.120510232,
      5.09538153,     5.120510232,    4.996212771,    4.783499092,    4.996212771,
      5.120510232,    3.751813794,    3.940650006,    3.351110068,    1.311712884,
      3.351110068,    3.940650006,    3.751813794,    3.940650006,    3.351110068,
      1.311712884,    3.351110068,    3.940650006,    0.001248274063, 0.001248274063,
      0.001248274063, 0.001248274063, 0.001248274063, 0.001248274063, 0.001248274063,
      0.001248274063};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-5);
    }

    // Front reflectance
    aRf = (*aResults)[3]->getMatrix(Side::Front, PropertySimple::R);

    correctResults = {
      0.1407775749, 0.1436754658, 0.1436891016, 0.143973169,  0.1436891016, 0.1436754658,
      0.1436891016, 0.143973169,  0.1436891016, 0.1540708059, 0.1535808296, 0.1560044649,
      0.160152099,  0.1560044649, 0.1535808296, 0.1540708059, 0.1535808296, 0.1560044649,
      0.160152099,  0.1560044649, 0.1535808296, 0.1802685913, 0.1765865361, 0.1880817815,
      0.2278473164, 0.1880817815, 0.1765865361, 0.1802685913, 0.1765865361, 0.1880817815,
      0.2278473164, 0.1880817815, 0.1765865361, 0.2533996349, 0.2533996349, 0.2533996349,
      0.2533996349, 0.2533996349, 0.2533996349, 0.2533996349, 0.2533996349};

    EXPECT_EQ(correctResults.size(), aRf.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRf(i, i), 1e-5);
    }
}
