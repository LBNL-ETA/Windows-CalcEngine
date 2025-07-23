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

        auto roomTemperature = 294.15;
        auto Indoor = Environments::indoor(roomTemperature);
        assert(Indoor != nullptr);

        auto airTemperature = 255.15;
        auto airSpeed = 5.5;
        auto tSky = 255.15;
        auto solarRadiation = 0.0;
        auto Outdoor = Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, SkyModel::AllSpecified);
        assert(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::CalculateH);

        auto emissivityFront = 0.52204944036998135;
        auto emissivityBack = 0.52204944036998102;
        auto transmittanceFront = 0.44074853220969445;
        auto transmittanceBack = 0.44074853220969445;

        auto materialThickness = 0.0001;
        auto materialConductance = 160.0;

        FenestrationCommon::Venetian::Geometry geometry{0.05, 0.07, 45, 0.00};
        EffectiveLayers::EffectiveHorizontalVenetian effectiveVenetian{
          1.0, 1.0, materialThickness, geometry};

        auto shadingLayer = Layers::shading(effectiveVenetian.effectiveThickness(),
                                            materialConductance,
                                            effectiveVenetian.getEffectiveOpenness(),
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

        auto roomTemperature = 294.15;
        auto Indoor = Environments::indoor(roomTemperature);
        assert(Indoor != nullptr);

        auto airTemperature = 255.15;
        auto airSpeed = 5.5;
        auto tSky = 255.15;
        auto solarRadiation = 0.0;
        auto Outdoor = Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, SkyModel::AllSpecified);
        assert(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::HcPrescribed,
                                modifier.hcRatio);

        auto solidLayer = Layers::solid(0.003048, 1.0, modifier.emissivityRatio, 0.0, 0.84, 0.0);
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

    const std::vector correctTemperature{257.504498, 257.795556, 276.669580, 277.055281};
    Helper::testVectors("Temperature", correctTemperature, aDoubleSystem->getTemperatures(), Tolerance);

    const std::vector correctRadiosity{279.595282, 248.498982, 318.811755, 348.524663};
    Helper::testVectors("Radiosity", correctRadiosity, aDoubleSystem->getRadiosities(), Tolerance);

    auto aModifier = aDoubleSystem->getShadingModifier(Tarcog::ISO15099::Environment::Outdoor);

    const auto aSingleSystem = buildSingleLayerSystemWithModifier(aModifier);

    EXPECT_NEAR(3.244674, aSingleSystem->getUValue(), Tolerance);
}
