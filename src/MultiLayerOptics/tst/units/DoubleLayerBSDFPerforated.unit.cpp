#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace SingleLayerOptics;
using namespace MultiLayerOptics;

// Example that tests interreflectance between two adjacent layers. This procedure will be used to
// calculate other multilayer properties
// Construction is NFRC=102 - NFRC=102
class TestDoubleLayerBSDFPerforated : public testing::Test
{
private:
    std::shared_ptr<CBSDFDoubleLayer> m_DoubleLayer;

protected:
    virtual void SetUp()
    {
        // Create lambda matrix
        std::vector<BSDFDefinition> aDefinitions{
          {0, 1}, {15, 1}, {30, 1}, {45, 1}, {60, 1}, {75, 1}, {86.25, 1}};

        // Create BSDF out of previous definitions
        const auto aBSDF = BSDFHemisphere::create(aDefinitions);

        CSeries aSolarRadiation{StandardData::solarRadiationASTM_E891_87_Table1()};

        const auto aMeasurements = SpectralSample::NFRC_102();

        double thickness = 3.048e-3;   // [m]
        MaterialType aType = MaterialType::Monolithic;

        auto aMaterial =
          SingleLayerOptics::Material::nBandMaterial(aMeasurements, thickness, aType);

        auto aLayer102 = CBSDFLayerMaker::getSpecularLayer(aMaterial, aBSDF);
        aLayer102->setSourceData(aSolarRadiation);

        // Perforated cell
        // create material
        double Tmat = 0.2;
        double Rfmat = 0.75;
        double Rbmat = 0.66;
        std::shared_ptr<CMaterial> perfMaterial =
          std::make_shared<CMaterialSingleBand>(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        double x = 22.5;        // mm
        double y = 38.1;        // mm
        thickness = 5;          // mm
        double radius = 8.35;   // mm

        // get shading BSDF layer
        auto aShade =
          CBSDFLayerMaker::getCircularPerforatedLayer(perfMaterial, aBSDF, x, y, thickness, radius);

        BSDFIntegrator aLayer1 = aLayer102->getResults();
        BSDFIntegrator aLayer2 = aShade->getResults();

        m_DoubleLayer = std::make_shared<CBSDFDoubleLayer>(aLayer1, aLayer2);
    }

public:
    std::shared_ptr<CBSDFDoubleLayer> getDoubleLayer() const
    {
        return m_DoubleLayer;
    }
};

TEST_F(TestDoubleLayerBSDFPerforated, TestDoubleLayerBSDF)
{
    SCOPED_TRACE("Begin Test: Double Layer BSDF.");

    BSDFIntegrator aLayer = getDoubleLayer()->value();

    // Front transmittance
    auto Tf = aLayer.getMatrix(Side::Front, PropertySurface::T);
    size_t matrixSize = Tf.size();

    // Test matrix
    size_t size = 7;

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctTf{
      {4.02734229, 0.04807644, 0.04923533, 0.05014887, 0.04956643, 0.04042293, 0.01069836},
      {0.04660969, 0.52907808, 0.0489721, 0.04988075, 0.04930142, 0.04020681, 0.01064117},
      {0.04638265, 0.04758648, 0.29694739, 0.04963778, 0.04906128, 0.04001096, 0.01058933},
      {0.04633115, 0.04753364, 0.04867944, 0.22773067, 0.0490068, 0.03996653, 0.01057757},
      {0.04660871, 0.0478184, 0.04897107, 0.0498797, 0.18009053, 0.04020597, 0.01064094},
      {0.04372094, 0.04485568, 0.04593693, 0.04678926, 0.04624584, 0.03771489, 0.00998165},
      {0.04372094, 0.04485568, 0.04593693, 0.04678926, 0.04624584, 0.03771489, 0.00998165}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctTf(i, j), Tf(i, j), 1e-6);
        }
    }

    // Front reflectance
    auto Rf = aLayer.getMatrix(Side::Front, PropertySurface::R);
    matrixSize = Rf.size();

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctRf{
      {1.53453038, 0.14025367, 0.14363451, 0.14629956, 0.14460041, 0.11792603, 0.0312104},
      {0.13649545, 0.32414895, 0.14341371, 0.14607467, 0.14437812, 0.11774475, 0.03116242},
      {0.13559035, 0.13910948, 0.25137713, 0.14510605, 0.14342076, 0.11696399, 0.03095578},
      {0.13266706, 0.13611032, 0.13939128, 0.25120164, 0.14032865, 0.11444228, 0.03028838},
      {0.12275277, 0.12593872, 0.12897449, 0.13136754, 0.33299805, 0.10588994, 0.02802492},
      {0.08779424, 0.09007286, 0.09224408, 0.09395562, 0.0928644, 0.94925871, 0.02004375},
      {0.02323569, 0.02383875, 0.02441339, 0.02486637, 0.02457757, 0.02004375, 14.46404805}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctRf(i, j), Rf(i, j), 1e-5);
        }
    }

    // Back Transmittance
    auto Tb = aLayer.getMatrix(Side::Back, PropertySurface::T);
    matrixSize = Tb.size();

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctTb{
      {4.02734229, 0.04789303, 0.0491344, 0.05109201, 0.05490417, 0.05872637, 0.05872637},
      {0.04678819, 0.52907808, 0.04905886, 0.05101347, 0.05481977, 0.05863609, 0.05863609},
      {0.04647794, 0.04750232, 0.29694739, 0.0506752, 0.05445626, 0.05824728, 0.05824728},
      {0.04547589, 0.04647818, 0.04768287, 0.22773067, 0.0532822, 0.05699148, 0.05699148},
      {0.04207745, 0.04300484, 0.04411951, 0.04587732, 0.18009053, 0.05273248, 0.05273248},
      {0.03009429, 0.03075757, 0.03155479, 0.03281200, 0.03526022, 0.03771489, 0.03771489},
      {0.00796478, 0.00814032, 0.00835132, 0.00868405, 0.00933200, 0.00998165, 0.00998165}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctTb(i, j), Tb(i, j), 1e-5);
        }
    }

    // Back Reflectance
    auto Rb = aLayer.getMatrix(Side::Back, PropertySurface::R);
    matrixSize = Rb.size();

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctRb{
      {0.25123617, 0.16414142, 0.16913772, 0.17605257, 0.1881739, 0.21323834, 0.21323834},
      {0.1597431, 0.17423888, 0.16904768, 0.17595894, 0.18807328, 0.21313072, 0.21313072},
      {0.15966527, 0.16397411, 0.17383866, 0.17587408, 0.1879821, 0.21303319, 0.21303319},
      {0.15964762, 0.16395606, 0.16894756, 0.17935988, 0.18796142, 0.21301106, 0.21301106},
      {0.15974276, 0.1640533, 0.16904732, 0.17595857, 0.19114974, 0.2131303, 0.21313030},
      {0.15875288, 0.16304161, 0.16800941, 0.17487930, 0.18691309, 0.21188976, 0.21188976},
      {0.15875288, 0.16304161, 0.16800941, 0.17487930, 0.18691309, 0.21188976, 0.21188976}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctRb(i, j), Rb(i, j), 1e-5);
        }
    }
}
