#include <memory>
#include <gtest/gtest.h>

#include "WCESpectralAveraging.hpp"
#include "WCEMultiLayerOptics.hpp"

using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace SingleLayerOptics;
using namespace MultiLayerOptics;

#include "standardData.hpp"
#include "spectralSampleData.hpp"

// Example that tests inter-reflectance between two adjacent layers. This procedure will be used to
// calculate other multilayer properties
// Construction is NFRC=102 - NFRC=102
class TestEquivalentBSDFDoubleSpecular : public testing::Test
{
private:
    std::shared_ptr<CEquivalentBSDFLayerSingleBand> m_EquivalentBSDFLayer;

protected:
    void SetUp() override
    {
        // Create lambda matrix
        const auto aBSDF =
          BSDFHemisphere::create({{0, 1}, {15, 1}, {30, 1}, {45, 1}, {60, 1}, {75, 1}, {86.25, 1}});

        auto aSolarRadiation{StandardData::solarRadiationASTM_E891_87_Table1()};

        auto aMeasurements_102{SpectralSample::NFRC_102()};
        constexpr auto thickness = 3.048e-3;   // [m]
        const MaterialType aType = MaterialType::Monolithic;
        const auto aMaterial =
          SingleLayerOptics::Material::nBandMaterial(aMeasurements_102, thickness, aType);

        auto aLayer102 = CBSDFLayerMaker::getSpecularLayer(aMaterial, aBSDF);
        aLayer102->setSourceData(aSolarRadiation);

        const auto aLayer1 = aLayer102->getResults();
        const auto aLayer2 = aLayer102->getResults();

        m_EquivalentBSDFLayer = std::make_shared<CEquivalentBSDFLayerSingleBand>(aLayer1);
        m_EquivalentBSDFLayer->addLayer(aLayer2);
    }

public:
    [[nodiscard]] std::shared_ptr<CEquivalentBSDFLayerSingleBand> getLayer() const
    {
        return m_EquivalentBSDFLayer;
    };
};

TEST_F(TestEquivalentBSDFDoubleSpecular, TestDoubleLayerBSDF)
{
    SCOPED_TRACE("Begin Test: Equivalent layer NFRC=102 - NFRC=102.");

    std::shared_ptr<CEquivalentBSDFLayerSingleBand> aLayer = getLayer();

    // Transmittance Front side
    SquareMatrix Tf = aLayer->getMatrix(Side::Front, PropertySurface::T);
    size_t matrixSize = Tf.size();

    // Test matrix
    size_t size = 7;

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctTf{{13.06253965, 0, 0, 0, 0, 0, 0},
                           {0, 1.71444156, 0, 0, 0, 0, 0},
                           {0, 0, 0.97702447, 0, 0, 0, 0},
                           {0, 0, 0, 0.8116755, 0, 0, 0},
                           {0, 0, 0, 0, 0.81270464, 0, 0},
                           {0, 0, 0, 0, 0, 0.807133, 0},
                           {0, 0, 0, 0, 0, 0, 0.93569046}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctTf(i, j), Tf(i, j), 1e-6);
        }
    }

    // Transmittance Back side
    SquareMatrix Tb = aLayer->getMatrix(Side::Back, PropertySurface::T);

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctTb{{13.06253965, 0, 0, 0, 0, 0, 0},
                           {0, 1.714453, 0, 0, 0, 0, 0},
                           {0, 0, 0.977050, 0, 0, 0, 0},
                           {0, 0, 0, 0.811720, 0, 0, 0},
                           {0, 0, 0, 0, 0.812780, 0, 0},
                           {0, 0, 0, 0, 0, 0.807264, 0},
                           {0, 0, 0, 0, 0, 0, 0.936161}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctTb(i, j), Tb(i, j), 1e-6);
        }
    }

    // Reflectance Front side
    SquareMatrix Rf = aLayer->getMatrix(Side::Front, PropertySurface::R);
    matrixSize = Rf.size();

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctRf{{2.375120, 0, 0, 0, 0, 0, 0},
                           {0, 0.312438, 0, 0, 0, 0, 0},
                           {0, 0, 0.183847, 0, 0, 0, 0},
                           {0, 0, 0, 0.181311, 0, 0, 0},
                           {0, 0, 0, 0, 0.319424, 0, 0},
                           {0, 0, 0, 0, 0, 1.160188, 0},
                           {0, 0, 0, 0, 0, 0, 15.183041}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctRf(i, j), Rf(i, j), 1e-6);
        }
    }

    // Reflectance Back side
    SquareMatrix Rb = aLayer->getMatrix(Side::Back, PropertySurface::R);
    matrixSize = Rb.size();

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctRb{{2.378934, 0, 0, 0, 0, 0, 0},
                           {0, 0.312940, 0, 0, 0, 0, 0},
                           {0, 0, 0.184137, 0, 0, 0, 0},
                           {0, 0, 0, 0.181565, 0, 0, 0},
                           {0, 0, 0, 0, 0.319714, 0, 0},
                           {0, 0, 0, 0, 0, 1.160607, 0},
                           {0, 0, 0, 0, 0, 0, 15.184551}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctRb(i, j), Rb(i, j), 1e-6);
        }
    }

    std::vector<double> A = aLayer->getLayerAbsorptances(1, Side::Front);
    std::vector correctAbs = {0.097101, 0.098421, 0.102437, 0.109394, 0.120057, 0.133151, 0.107444};

    for(size_t i = 0; i < size; i++)
    {
        EXPECT_NEAR(correctAbs[i], A[i], 1e-6);
    }

    A = aLayer->getLayerAbsorptances(1, Side::Back);
    correctAbs = {0.076519, 0.077444, 0.080005, 0.083088, 0.082612, 0.066930, 0.029791};

    for(size_t i = 0; i < size; i++)
    {
        EXPECT_NEAR(correctAbs[i], A[i], 1e-6);
    }

    A = aLayer->getLayerAbsorptances(2, Side::Front);
    correctAbs = {0.076620, 0.077547, 0.080113, 0.083205, 0.082734, 0.067030, 0.029824};

    for(size_t i = 0; i < size; i++)
    {
        EXPECT_NEAR(correctAbs[i], A[i], 1e-6);
    }

    A = aLayer->getLayerAbsorptances(2, Side::Back);
    correctAbs = {0.096998, 0.098315, 0.102323, 0.109268, 0.119921, 0.133028, 0.107370};

    for(size_t i = 0; i < size; i++)
    {
        EXPECT_NEAR(correctAbs[i], A[i], 1e-6);
    }
}
