#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example/test case on multlayer specular
// Difference from BSDF layer is that properties can be calculated at any custom angle

class MultiPaneSpecular_102_103_EN410 : public testing::Test
{
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

    static CSeries loadSolarRadiationFile()
    {
        CSeries aSolarRadiation(
          {{0.300, 0.0005}, {0.320, 0.0069}, {0.340, 0.0122}, {0.360, 0.0145}, {0.380, 0.0177},
           {0.400, 0.0235}, {0.420, 0.0268}, {0.440, 0.0294}, {0.460, 0.0343}, {0.480, 0.0339},
           {0.500, 0.0326}, {0.520, 0.0318}, {0.540, 0.0321}, {0.560, 0.0312}, {0.580, 0.0294},
           {0.600, 0.0289}, {0.620, 0.0289}, {0.640, 0.0280}, {0.660, 0.0273}, {0.680, 0.0246},
           {0.700, 0.0237}, {0.720, 0.0220}, {0.740, 0.0230}, {0.760, 0.0199}, {0.780, 0.0211},
           {0.800, 0.0330}, {0.850, 0.0453}, {0.900, 0.0381}, {0.950, 0.0220}, {1.000, 0.0329},
           {1.050, 0.0306}, {1.100, 0.0185}, {1.150, 0.0136}, {1.200, 0.0210}, {1.250, 0.0211},
           {1.300, 0.0166}, {1.350, 0.0042}, {1.400, 0.0010}, {1.450, 0.0044}, {1.500, 0.0095},
           {1.550, 0.0123}, {1.600, 0.0110}, {1.650, 0.0106}, {1.700, 0.0093}, {1.750, 0.0068},
           {1.800, 0.0024}, {1.850, 0.0005}, {1.900, 0.0002}, {1.950, 0.0012}, {2.000, 0.0030},
           {2.050, 0.0037}, {2.100, 0.0057}, {2.200, 0.0066}, {2.300, 0.0060}, {2.400, 0.0041},
           {2.500, 0.0006}

          });

        return aSolarRadiation;
    }

protected:
    void SetUp() override
    {
        double thickness = 3.048e-3;   // [m]
        const auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);


        thickness = 5.715e-3;   // [m]
        const auto aMaterial_103 =
          Material::nBandMaterial(SpectralSample::NFRC_103(), thickness, MaterialType::Monolithic);


        const auto layer102 = SpecularLayer::createLayer(aMaterial_102);
        const auto layer103 = SpecularLayer::createLayer(aMaterial_103);

        m_Layer = CMultiPaneSpecular::create({layer102, layer103});

        const CalculationProperties input{
          loadSolarRadiationFile(),
          loadSolarRadiationFile().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] std::shared_ptr<CMultiPaneSpecular> getLayer() const
    {
        return m_Layer;
    }
};

TEST_F(MultiPaneSpecular_102_103_EN410, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.621741, T, 1e-6);

    const double Rf = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.119697, Rf, 1e-6);

    const double Rb = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.110757, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.113512, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.145050, Abs2, 1e-6);

    const double Them = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.523013, Them, 1e-6);

    const double Rfhem = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.194344, Rfhem, 1e-6);

    const double Rbhem = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.178996, Rbhem, 1e-6);
}