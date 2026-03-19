#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

// This test is created to accommodate same test in WC so we can confirm the results (Shading
// material is NFRC 31100 - White Venetian)
class TestShadeOutVenetian : public testing::Test
{
protected:
    static std::unique_ptr<Tarcog::ISO15099::CSingleSystem> buildDoubleLayerSystem()
    {
        using namespace Tarcog::ISO15099;

        constexpr auto roomTemperature = 294.15;
        auto Indoor = Environments::indoor(roomTemperature);
        assert(Indoor != nullptr);

        constexpr auto airTemperature = 255.15;
        constexpr auto airSpeed = 5.5;
        constexpr auto tSky = 255.15;
        constexpr auto solarRadiation = 0.0;
        auto Outdoor = Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, SkyModel::AllSpecified);
        assert(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::CalculateH);

        constexpr auto emissivityFront = 0.52204944036998135;
        constexpr auto emissivityBack = 0.52204944036998102;
        constexpr auto transmittanceFront = 0.44074853220969445;
        constexpr auto transmittanceBack = 0.44074853220969445;

        constexpr auto materialThickness = 0.0001;
        constexpr auto materialConductance = 160.0;

        FenestrationCommon::Venetian::Geometry geometry{
          .SlatWidth = 0.05, .SlatSpacing = 0.07, .SlatTiltAngle = 45, .CurvatureRadius = 0.00};

        const auto effectiveVenetian{
          EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry)};

        auto shadingLayer = Layers::shading(effectiveVenetian.thickness,
                                            materialConductance,
                                            effectiveVenetian.openness,
                                            emissivityFront,
                                            transmittanceFront,
                                            emissivityBack,
                                            transmittanceBack);
        assert(shadingLayer != nullptr);

        auto solidLayer = Layers::solid(0.003048, 1.0, 0.84, 0.0, 0.84, 0.0);
        auto gap = Layers::gap(0.0127);

        CIGU igu{1.0, 1.0};
        igu.addLayers({shadingLayer, gap, solidLayer});

        auto system = std::make_unique<CSingleSystem>(igu, Indoor, Outdoor);
        system->solve();
        return system;
    }

    static std::unique_ptr<Tarcog::ISO15099::CSingleSystem>
      buildSingleLayerSystemWithModifier(const Tarcog::ISO15099::ShadingModifier & modifier)
    {
        using namespace Tarcog::ISO15099;

        constexpr auto roomTemperature = 294.15;
        auto Indoor = Environments::indoor(roomTemperature);
        assert(Indoor != nullptr);

        constexpr auto airTemperature = 255.15;
        constexpr auto airSpeed = 5.5;
        constexpr auto tSky = 255.15;
        constexpr auto solarRadiation = 0.0;
        auto Outdoor = Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, SkyModel::AllSpecified);
        assert(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::HcPrescribed, 26 * modifier.hcRatio);

        auto solidLayer =
          Layers::solid(0.003048, 1.0, modifier.emissivityRatio * 0.84, 0.0, 0.84, 0.0);
        assert(solidLayer != nullptr);

        CIGU igu{1.0, 1.0};
        igu.addLayers({solidLayer});

        auto system = std::make_unique<CSingleSystem>(igu, Indoor, Outdoor);
        system->solve();
        return system;
    }
};

TEST_F(TestShadeOutVenetian, Test1)
{
    SCOPED_TRACE("Begin Test: ShadeOutVenetian - U-value");

    constexpr auto Tolerance = 1e-6;

    const auto aDoubleSystem = buildDoubleLayerSystem();

    EXPECT_NEAR(3.244674, aDoubleSystem->getUValue(), Tolerance);

    const std::vector correctTemperatureDouble{257.504498, 257.795556, 276.669580, 277.055281};
    Helper::testVectors("Temperatures with shading",
                        correctTemperatureDouble,
                        aDoubleSystem->getTemperatures(),
                        Tolerance);

    const std::vector correctRadiosityDouble{279.595282, 248.498982, 318.811755, 348.524663};
    Helper::testVectors("Radiosities with shading",
                        correctRadiosityDouble,
                        aDoubleSystem->getRadiosities(),
                        Tolerance);

    auto aModifier = aDoubleSystem->getShadingModifier(Tarcog::ISO15099::Environment::Outdoor);

    EXPECT_NEAR(0.155924, aModifier.hcRatio, Tolerance);
    EXPECT_NEAR(0.509054, aModifier.emissivityRatio, Tolerance);

    // Now we are removing shading system and applying shading modifier equations which should
    // lead to the same glass surface temperatures and radiosities
    const auto aSingleSystem = buildSingleLayerSystemWithModifier(aModifier);

    EXPECT_NEAR(3.244674, aSingleSystem->getUValue(), Tolerance);

    // Temperatures of the glass layers should match temperatures of the glass layer in the double
    // layer construction.
    const std::vector correctTemperatureSingle{276.669580, 277.055281};
    Helper::testVectors("Temperatures with shading modifier",
                        correctTemperatureSingle,
                        aSingleSystem->getTemperatures(),
                        Tolerance);

    // Radiosities of the surfaces should match radiosities of the surfaces that are on
    // the exterior and the interior side (first and last surface)
    const std::vector correctRadiositySingle{279.595282, 348.524663};
    Helper::testVectors("Radiosities with shading modifier",
                        correctRadiositySingle,
                        aSingleSystem->getRadiosities(),
                        Tolerance);
}
