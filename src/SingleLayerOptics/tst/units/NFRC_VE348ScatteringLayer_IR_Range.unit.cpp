#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>
#include <WCESpectralAveraging.hpp>

#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestVE345ScatteringLayer_IR_Range : public testing::Test
{
private:
    CScatteringLayer m_Layer;


protected:
    virtual void SetUp()
    {
        const double thickness = 5.66e-3;   // [m]
        const auto aMaterial =
          Material::nBandMaterial(SpectralSample::NFRC_VE348(), thickness, MaterialType::Coated);

        const double blackBodyTemperature = 300;
        m_Layer = CScatteringLayer::createSpecularLayer(aMaterial);
        m_Layer.setBlackBodySource(blackBodyTemperature);
    }

public:
    CScatteringLayer & getLayer()
    {
        return m_Layer;
    }
};

TEST_F(TestVE345ScatteringLayer_IR_Range, TestFrontIR)
{
    SCOPED_TRACE("Begin Test: VE345 scattering layer IR properties.");

    auto aLayer{CScatteringLayerIR(getLayer())};

    const double emiss = aLayer.emissivity(Side::Front, EmissivityPolynomials::NFRC_301_Uncoated);
    EXPECT_NEAR(0.840263, emiss, 1e-6);

    const double transmittance = aLayer.transmittance(Side::Front);
    EXPECT_NEAR(0.0, transmittance, 1e-6);
}

TEST_F(TestVE345ScatteringLayer_IR_Range, TestBackIR)
{
    SCOPED_TRACE("Begin Test: VE345 scattering layer IR properties.");

    auto aLayer{CScatteringLayerIR(getLayer())};

    const double emiss = aLayer.emissivity(Side::Back, EmissivityPolynomials::NFRC_301_Coated);
    EXPECT_NEAR(0.088879, emiss, 1e-6);

    const double transmittance = aLayer.transmittance(Side::Front);
    EXPECT_NEAR(0.0, transmittance, 1e-6);
}
