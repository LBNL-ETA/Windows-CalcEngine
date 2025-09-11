///////////////////////////////////////////////////////////////////////////////////////
/// Example demonstrating dual-band material behavior with a homogeneous diffuse layer.
/// Shows how closely integrated optical properties (transmittance, reflectance) match
/// expected values across solar and visible wavelength ranges.
///////////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

class MultiPaneBSDF_Homogeneous_Dual_Band_Material : public testing::Test
{
private:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        const auto solarRadiation{StandardData::solarRadiationASTM_E891_87_Table1()};

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        // Solar range
        constexpr auto Tsol = 0.7;
        constexpr auto Rsol = 0.1;

        // Visible range
        constexpr auto Tvis = 0.6;
        constexpr auto Rvis = 0.2;

        auto aMaterial = Material::dualBandMaterial(Tsol, Tsol, Rsol, Rsol, Tvis, Tvis, Rvis, Rvis);

        auto diffuseLayer = CBSDFLayerMaker::getHomogeneousDiffuseLayer(aMaterial, aBSDF);

        m_Layer = CMultiPaneBSDF::create({diffuseLayer});

        const CalculationProperties input{StandardData::Photopic::solarRadiation(),
                                          StandardData::Photopic::solarRadiation().getXArray(),
                                          StandardData::Photopic::detectorData()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneBSDF_Homogeneous_Dual_Band_Material, TestSolarRange)
{
    SCOPED_TRACE("Begin Test: Specular and homogeneous diffuse IGU - BSDF.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    const CalculationProperties input{
      StandardData::solarRadiationASTM_E891_87_Table1(),
      StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
    aLayer.setCalculationProperties(input);

    constexpr double theta = 0;
    constexpr double phi = 0;

    const double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.717971, tauHem, 1e-6);

    const double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.102062, rhoHem, 1e-6);
}

TEST_F(MultiPaneBSDF_Homogeneous_Dual_Band_Material, TestVisibleRange)
{
    SCOPED_TRACE("Begin Test: Specular and homogeneous diffuse IGU - BSDF.");

    constexpr double minLambda = 0.38;
    constexpr double maxLambda = 0.78;

    CMultiPaneBSDF & aLayer = getLayer();

    const CalculationProperties inputVisible{StandardData::Photopic::solarRadiation(),
                                             StandardData::Photopic::wavelengthSetPhotopic(),
                                             StandardData::Photopic::detectorData()};
    aLayer.setCalculationProperties(inputVisible);

    constexpr double theta = 0;
    constexpr double phi = 0;

    const double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.615025, tauHem, 1e-6);

    const double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.205008, rhoHem, 1e-6);
}
