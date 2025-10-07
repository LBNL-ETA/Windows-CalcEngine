#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "../../../helper/optical/standardData.hpp"
#include "../../../helper/optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create scattered multilayer.

class MultiPaneScattered_21515 : public testing::Test
{
    std::unique_ptr<CMultiLayerScattered> m_Layer;

protected:
    void SetUp() override
    {
        // Create material from samples
        auto thickness = 0.18e-3;   // [m]
        auto aMaterial_21515 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_21515(), thickness, MaterialType::Monolithic);

        CScatteringLayer Layer21515 = CScatteringLayer::createSpecularLayer(aMaterial_21515);

        // Equivalent BSDF layer
        m_Layer = CMultiLayerScattered::create(Layer21515);

        m_Layer->setSourceData(StandardData::solarRadiationASTM_E891_87_Table1());
    }

public:
    CMultiLayerScattered & getLayer()
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneScattered_21515, TestSpecular1)
{
    SCOPED_TRACE("Begin Test: Laminate layer - Angular Dependency.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;

    CMultiLayerScattered & aLayer = getLayer();

    Side aSide = Side::Front;
    std::vector<double> thetaAngles{0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
    double phi = 0;

    const std::vector<double> correctResults{0.865999,
                                             0.863266,
                                             0.862713,
                                             0.860582,
                                             0.854166,
                                             0.837063,
                                             0.793620,
                                             0.685154,
                                             0.436450,
                                             0.000000};

    std::vector<double> results;

    for(const auto & theta : thetaAngles)
    {
        results.push_back(aLayer.getPropertySurface(
          minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi));
    }


    EXPECT_EQ(results.size(), correctResults.size());

    for(size_t i = 0u; i < correctResults.size(); ++i)
    {
        EXPECT_NEAR(results[i], correctResults[i], 1e-6);
    }
}
