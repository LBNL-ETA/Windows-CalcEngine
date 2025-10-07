#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCEMultiLayerOptics.hpp>

#include "../../../helper/optical/standardData.hpp"
#include "../../../helper/optical/spectralSampleData.hpp"

using MultiLayerOptics::CMultiPaneBSDF;
using FenestrationCommon::CSeries;
using FenestrationCommon::MaterialType;
using FenestrationCommon::WavelengthRange;
using SpectralAveraging::CSpectralSampleData;

// Example on how to create multilayer BSDF from specular layers only

class MultiPaneBSDF_102_CondensedSpectrum_UV_QuarterBasis : public testing::Test
{
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        using SingleLayerOptics::BSDFHemisphere;
        using SingleLayerOptics::BSDFBasis;
        using SingleLayerOptics::CBSDFLayerMaker;
        using SingleLayerOptics::CalculationProperties;

        // Create material from samples
        constexpr double thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);

        const std::vector condensed{0.3,
                                    0.38,
                                    0.46,
                                    0.54,
                                    0.62,
                                    0.7,
                                    0.78,
                                    0.952,
                                    1.124,
                                    1.296,
                                    1.468,
                                    1.64,
                                    1.812,
                                    1.984,
                                    2.156,
                                    2.328,
                                    2.5};

        m_Layer = CMultiPaneBSDF::create({Layer_102}, condensed);

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneBSDF_102_CondensedSpectrum_UV_QuarterBasis, TestSpecular1)
{
    using FenestrationCommon::Side;
    using FenestrationCommon::PropertySurface;
    using FenestrationCommon::ScatteringSimple;

    constexpr double minLambda = 0.31;
    constexpr double maxLambda = 0.38;

    CMultiPaneBSDF & aLayer = getLayer();

    const double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.531278, tauDiff, 1e-6);

    const double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.146261, rhoDiff, 1e-6);

    const double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.322461, absDiff1, 1e-6);

    double theta = 0;
    double phi = 0;

    const auto absDiffFrontElectricity = aLayer.getAbsorptanceLayersElectricity(
      minLambda, maxLambda, Side::Front, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.0, absDiffFrontElectricity[0], 1e-6);

    double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.591581, tauHem, 1e-6);

    double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.591581, tauDir, 1e-6);

    double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.072544, rhoHem, 1e-6);

    double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.072544, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.335875, abs1, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.584809, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.077012, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.338179, abs1, 1e-6);
}
