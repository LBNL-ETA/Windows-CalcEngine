#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>

#include "optical/spectralSampleData.hpp"
#include "optical/standardData.hpp"

using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace SingleLayerOptics;
using namespace MultiLayerOptics;

// Construction is NFRC=102 - Perforated - NFRC=102
class TestEquivalentBSDFTriplePerforatedInBetween : public testing::Test
{
private:
    std::shared_ptr<CEquivalentBSDFLayerSingleBand> m_EquivalentBSDFLayer;

protected:
    virtual void SetUp()
    {
        // Create lambda matrix
        std::vector<BSDFDefinition> aDefinitions{
          {0, 1}, {15, 1}, {30, 1}, {45, 1}, {60, 1}, {75, 1}, {86.25, 1}};

        const auto aBSDF = BSDFHemisphere::create(aDefinitions);

        auto aSolarRadiation{StandardData::solarRadiationASTM_E891_87_Table1()};

        std::shared_ptr<CSpectralSampleData> aMeasurements = SpectralSample::NFRC_102();
        double thickness = 3.048e-3;   // [m]
        MaterialType aType = MaterialType::Monolithic;
        auto aMaterial = Material::nBandMaterial(aMeasurements, thickness, aType);

        auto aLayer102 = CBSDFLayerMaker::getSpecularLayer(aMaterial, aBSDF);

        aLayer102->setSourceData(aSolarRadiation);

        // Perforated cell
        // create material
        double Tmat = 0.2;
        double Rfmat = 0.75;
        double Rbmat = 0.66;
        auto perfMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        double x = 22.5;        // mm
        double y = 38.1;        // mm
        thickness = 5;          // mm
        double radius = 8.35;   // mm
        auto aShade =
          CBSDFLayerMaker::getCircularPerforatedLayer(perfMaterial, aBSDF, x, y, thickness, radius);

        BSDFIntegrator aLayer1 = aLayer102->getResults();
        BSDFIntegrator aLayer2 = aShade->getResults();
        BSDFIntegrator aLayer3 = aLayer102->getResults();

        m_EquivalentBSDFLayer = std::make_shared<CEquivalentBSDFLayerSingleBand>(aLayer1);
        m_EquivalentBSDFLayer->addLayer(aLayer2);
        m_EquivalentBSDFLayer->addLayer(aLayer3);
    }

public:
    [[nodiscard]] std::shared_ptr<CEquivalentBSDFLayerSingleBand> getLayer() const
    {
        return m_EquivalentBSDFLayer;
    }
};

TEST_F(TestEquivalentBSDFTriplePerforatedInBetween, TestTripleLayerBSDF)
{
    SCOPED_TRACE("Begin Test: Equivalent layer NFRC=102 - Perforated - NFRC=102.");

    constexpr double kTol = 5e-5;

    CEquivalentBSDFLayerSingleBand aLayer = *getLayer();

    // Transmittance Front side
    SquareMatrix Tf = aLayer.getMatrix(Side::Front, PropertySurface::T);
    size_t matrixSize = Tf.size();

    // Test matrix
    size_t size = 7;

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctTf{{3.36513193e+00,
                            4.59380024e-02,
                            4.68677211e-02,
                            4.76966681e-02,
                            4.73625159e-02,
                            3.67802927e-02,
                            9.73430146e-03},
                           {4.46793459e-02,
                            4.46431913e-01,
                            4.65707959e-02,
                            4.73943209e-02,
                            4.70632076e-02,
                            3.65401206e-02,
                            9.67073732e-03},
                           {4.41904097e-02,
                            4.51474410e-02,
                            2.51387826e-01,
                            4.68740921e-02,
                            4.65474460e-02,
                            3.61326817e-02,
                            9.56290421e-03},
                           {4.31949401e-02,
                            4.41302523e-02,
                            4.50218651e-02,
                            1.90007197e-01,
                            4.54987154e-02,
                            3.53170393e-02,
                            9.34703564e-03},
                           {4.01801835e-02,
                            4.10510163e-02,
                            4.18811610e-02,
                            4.26217572e-02,
                            1.40277551e-01,
                            3.28605399e-02,
                            8.69689657e-03},
                           {2.71591829e-02,
                            2.77418896e-02,
                            2.82973794e-02,
                            2.87965021e-02,
                            2.86021785e-02,
                            2.21492350e-02,
                            5.86203413e-03},
                           {7.18797091e-03,
                            7.34219052e-03,
                            7.48920688e-03,
                            7.62130510e-03,
                            7.56987525e-03,
                            5.86203413e-03,
                            1.55145060e-03}};

    for(size_t i = 0; i < size; ++i)
        for(size_t j = 0; j < size; ++j)
            EXPECT_NEAR(correctTf(i, j), Tf(i, j), kTol);

    // Transmittance Back side
    SquareMatrix Tb = aLayer.getMatrix(Side::Back, PropertySurface::T);
    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctTb{{3.36513193e+00,
                            4.59095348e-02,
                            4.68120912e-02,
                            4.76335341e-02,
                            4.73314850e-02,
                            3.64804663e-02,
                            9.65494917e-03},
                           {4.47070507e-02,
                            4.46431913e-01,
                            4.65443606e-02,
                            4.73609334e-02,
                            4.70615300e-02,
                            3.62646620e-02,
                            9.59783423e-03},
                           {4.42429240e-02,
                            4.51730829e-02,
                            2.51387826e-01,
                            4.68676734e-02,
                            4.65722169e-02,
                            3.58806153e-02,
                            9.49619213e-03},
                           {4.32521911e-02,
                            4.41613623e-02,
                            4.50280310e-02,
                            1.90007197e-01,
                            4.55291599e-02,
                            3.50754692e-02,
                            9.28310153e-03},
                           {4.02065259e-02,
                            4.10524796e-02,
                            4.18588852e-02,
                            4.25932568e-02,
                            1.40277551e-01,
                            3.26140384e-02,
                            8.63165729e-03},
                           {2.73823994e-02,
                            2.79526110e-02,
                            2.84961724e-02,
                            2.89948279e-02,
                            2.88183577e-02,
                            2.21492350e-02,
                            5.86203413e-03},
                           {7.24704756e-03,
                            7.39796023e-03,
                            7.54181961e-03,
                            7.67379417e-03,
                            7.62708943e-03,
                            5.86203413e-03,
                            1.55145060e-03}};

    for(size_t i = 0; i < size; ++i)
        for(size_t j = 0; j < size; ++j)
            EXPECT_NEAR(correctTb(i, j), Tb(i, j), kTol);

    // Reflectance Front side
    SquareMatrix Rf = aLayer.getMatrix(Side::Front, PropertySurface::R);
    matrixSize = Rf.size();
    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctRf{
      {1.60088517, 0.14313834, 0.14652042, 0.14922345, 0.14757476, 0.11967244, 0.031672600},
      {0.13930281, 0.33400557, 0.14621983, 0.14891737, 0.14727177, 0.11942921, 0.031608230},
      {0.13831463, 0.14183139, 0.25742560, 0.14786150, 0.14622730, 0.11858444, 0.031384650},
      {0.13531849, 0.13875912, 0.14203821, 0.25617506, 0.14305981, 0.11601622, 0.030704950},
      {0.12527773, 0.12846280, 0.13149834, 0.13392429, 0.33732433, 0.10740501, 0.028425890},
      {0.08909441, 0.09136145, 0.09352206, 0.09524781, 0.09419309, 0.94992872, 0.020221080},
      {0.02357980, 0.02417979, 0.02475162, 0.02520836, 0.02492922, 0.02022108, 14.46409498}};

    for(size_t i = 0; i < size; ++i)
        for(size_t j = 0; j < size; ++j)
            EXPECT_NEAR(correctRf(i, j), Rf(i, j), kTol);

    // Reflectance Back side
    SquareMatrix Rb = aLayer.getMatrix(Side::Back, PropertySurface::R);
    matrixSize = Rb.size();
    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctRb{
      {1.58527489, 0.12471521, 0.12765346, 0.13000635, 0.12858011, 0.10418067, 0.027572540},
      {0.12137335, 0.31591820, 0.12738235, 0.12973031, 0.12830676, 0.10396204, 0.027514680},
      {0.12050431, 0.12355927, 0.23889038, 0.12880202, 0.12738835, 0.10322046, 0.027318410},
      {0.11789208, 0.12088088, 0.12372935, 0.23768222, 0.12462696, 0.10098352, 0.026726380},
      {0.10915298, 0.11191993, 0.11455697, 0.11666853, 0.32045439, 0.09349472, 0.024744390},
      {0.07756101, 0.07952931, 0.08140519, 0.08290615, 0.08199392, 0.94029819, 0.017588740},
      {0.02052736, 0.02104829, 0.02154476, 0.02194201, 0.02170057, 0.01758874, 14.46483641}};

    for(size_t i = 0; i < size; ++i)
        for(size_t j = 0; j < size; ++j)
            EXPECT_NEAR(correctRb(i, j), Rb(i, j), kTol);

    // Absorptances
    std::vector<double> A = aLayer.getLayerAbsorptances(1, Side::Front);
    std::vector<double> correctAbs = {
      0.14491971, 0.14746616, 0.1523858, 0.15909124, 0.16462775, 0.15497232, 0.09646037};
    for(size_t i = 0; i < size; i++)
        EXPECT_NEAR(correctAbs[i], A[i], kTol);

    A = aLayer.getLayerAbsorptances(1, Side::Back);
    correctAbs = {
      0.03637407, 0.03538488, 0.03443428, 0.03269092, 0.02777523, 0.01372161, 0.00363157};
    for(size_t i = 0; i < size; i++)
        EXPECT_NEAR(correctAbs[i], A[i], kTol);

    A = aLayer.getLayerAbsorptances(2, Side::Front);
    correctAbs = {
      0.03901251, 0.03987245, 0.0406922, 0.04141485, 0.04110882, 0.03205742, 0.00848434};
    for(size_t i = 0; i < size; i++)
        EXPECT_NEAR(correctAbs[i], A[i], kTol);

    A = aLayer.getLayerAbsorptances(2, Side::Back);
    correctAbs = {
      0.09677338, 0.09922121, 0.10155411, 0.10342485, 0.10229603, 0.08283503, 0.02192318};
    for(size_t i = 0; i < size; i++)
        EXPECT_NEAR(correctAbs[i], A[i], kTol);

    A = aLayer.getLayerAbsorptances(3, Side::Front);
    correctAbs = {0.03638587, 0.035405, 0.03446196, 0.03271799, 0.02778434, 0.01382314, 0.00365844};
    for(size_t i = 0; i < size; i++)
        EXPECT_NEAR(correctAbs[i], A[i], kTol);

    A = aLayer.getLayerAbsorptances(3, Side::Back);
    correctAbs = {
      0.13800596, 0.14037602, 0.14512686, 0.15169655, 0.15731107, 0.14898255, 0.09483209};
    for(size_t i = 0; i < size; i++)
        EXPECT_NEAR(correctAbs[i], A[i], kTol);
}
