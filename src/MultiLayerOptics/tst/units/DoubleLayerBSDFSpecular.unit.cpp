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
class TestDoubleLayerBSDFSpecular : public testing::Test
{
private:
    std::shared_ptr<CBSDFDoubleLayer> m_DoubleLayer;

protected:
    virtual void SetUp()
    {
        // Create lambda matrix
        std::vector<BSDFDefinition> aDefinitions;
        aDefinitions.push_back(BSDFDefinition(0, 1));
        aDefinitions.push_back(BSDFDefinition(15, 1));
        aDefinitions.push_back(BSDFDefinition(30, 1));
        aDefinitions.push_back(BSDFDefinition(45, 1));
        aDefinitions.push_back(BSDFDefinition(60, 1));
        aDefinitions.push_back(BSDFDefinition(75, 1));
        aDefinitions.push_back(BSDFDefinition(86.25, 1));

        // Create BSDF from definitions
        const auto aBSDF = BSDFHemisphere::create(aDefinitions);

        CSeries aSolarRadiation{StandardData::solarRadiationASTM_E891_87_Table1()};

        const auto aMeasurements = SpectralSample::NFRC_102();

        double thickness = 3.048e-3;   // [m]
        MaterialType aType = MaterialType::Monolithic;

        auto aMaterial = Material::nBandMaterial(aMeasurements, thickness, aType);

        auto aLayer102 = CBSDFLayerMaker::getSpecularLayer(aMaterial, aBSDF);
        aLayer102->setSourceData(aSolarRadiation);

        BSDFIntegrator aLayer1 = aLayer102->getResults();
        BSDFIntegrator aLayer2 = aLayer102->getResults();

        m_DoubleLayer = std::make_shared<CBSDFDoubleLayer>(aLayer1, aLayer2);
    }

public:
    std::shared_ptr<CBSDFDoubleLayer> getDoubleLayer() const
    {
        return m_DoubleLayer;
    }
};

TEST_F(TestDoubleLayerBSDFSpecular, TestDoubleLayerBSDF)
{
    SCOPED_TRACE("Begin Test: Double Layer BSDF.");

    BSDFIntegrator aLayer = getDoubleLayer()->value();

    auto T = aLayer.getMatrix(Side::Front, PropertySurface::T);
    size_t matrixSize = T.size();

    // Test matrix
    size_t size = 7;

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctT{{13.06253965, 0, 0, 0, 0, 0, 0},
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
            EXPECT_NEAR(correctT(i, j), T(i, j), 1e-6);
        }
    }

    auto R = aLayer.getMatrix(Side::Front, PropertySurface::R);
    matrixSize = R.size();

    EXPECT_EQ(size, matrixSize);

    SquareMatrix correctR{{2.375120, 0, 0, 0, 0, 0, 0},
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
            EXPECT_NEAR(correctR(i, j), R(i, j), 1e-6);
        }
    }
}
