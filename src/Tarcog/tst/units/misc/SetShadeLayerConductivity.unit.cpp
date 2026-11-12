#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

// A shading layer with openings solves with an equivalent conductivity, mixed from the
// conductivity of the layer and of the gas next to it. Setting a new conductivity on such a
// layer (what WINDOW does with the effective conductivity it gets from THERM) must change the
// conductivity that enters that mix. The companion test SetSolidLayerConductivity covers a
// closed shade, which the engine builds as an ordinary solid layer.

namespace
{
    constexpr size_t shadeIndex{2u};
    constexpr double initialConductivity{0.45};
    constexpr double newConductivity{0.1};
    constexpr auto run{Tarcog::ISO15099::System::Uvalue};

    std::shared_ptr<Tarcog::ISO15099::CEnvironment> outdoorEnvironment()
    {
        constexpr auto airTemperature{255.15};
        constexpr auto airSpeed{5.5};
        constexpr auto skyTemperature{255.15};
        constexpr auto solarRadiation{0.0};
        auto outdoor{Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, skyTemperature, Tarcog::ISO15099::SkyModel::AllSpecified)};
        outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);
        return outdoor;
    }

    // Two glass panes and an indoor shade that lets air through.
    std::unique_ptr<Tarcog::ISO15099::CSystem> systemWithOpenShade(const double shadeConductivity)
    {
        constexpr auto glassThickness{0.003};
        constexpr auto glassConductivity{1.0};
        constexpr auto shadeThickness{0.03514};
        constexpr auto permeabilityFactor{0.5};
        constexpr auto emissivity{0.876};
        constexpr auto irTransmittance{0.1};
        constexpr auto roomTemperature{294.15};

        const auto effectiveLayer{EffectiveLayers::makeCommonValues(shadeThickness, permeabilityFactor)};
        const auto shade{Tarcog::ISO15099::Layers::shading(effectiveLayer.thickness,
                                                           shadeConductivity,
                                                           effectiveLayer.openness,
                                                           emissivity,
                                                           irTransmittance,
                                                           emissivity,
                                                           irTransmittance)};

        Tarcog::ISO15099::CIGU igu(1.0, 1.0);
        igu.addLayers({Tarcog::ISO15099::Layers::solid(glassThickness, glassConductivity),
                       Tarcog::ISO15099::Layers::gap(0.0127),
                       Tarcog::ISO15099::Layers::solid(glassThickness, glassConductivity),
                       Tarcog::ISO15099::Layers::gap(0.0381),
                       shade});

        return std::make_unique<Tarcog::ISO15099::CSystem>(
          igu, Tarcog::ISO15099::Environments::indoor(roomTemperature), outdoorEnvironment());
    }
}   // namespace

TEST(TestSetShadeLayerConductivity, TheShadeIsBuiltAsAShadingLayer)
{
    const auto system{systemWithOpenShade(initialConductivity)};

    EXPECT_TRUE(system->getSolidLayers(run)[shadeIndex]->isShadeLayer());
}

TEST(TestSetShadeLayerConductivity, SettingAConductivityChangesTheSolution)
{
    const auto system{systemWithOpenShade(initialConductivity)};
    const auto before{system->getTemperatures(run)};

    system->setSolidLayerConductivity(shadeIndex, newConductivity);
    const auto after{system->getTemperatures(run)};

    ASSERT_EQ(before.size(), after.size());
    // The shade's own faces move apart when its conductivity drops.
    const auto faceDifference{[](const std::vector<double> & temperatures) {
        return temperatures[2u * shadeIndex + 1u] - temperatures[2u * shadeIndex];
    }};
    EXPECT_GT(faceDifference(after), faceDifference(before) + 1e-3);
}

TEST(TestSetShadeLayerConductivity, SettingAConductivityEqualsBuildingTheLayerWithIt)
{
    const auto changed{systemWithOpenShade(initialConductivity)};
    // Solved once with the first value, as in the WINDOW loop, before the new one is set.
    const auto solvedFirst{changed->getTemperatures(run)};
    ASSERT_FALSE(solvedFirst.empty());
    changed->setSolidLayerConductivity(shadeIndex, newConductivity);

    const auto built{systemWithOpenShade(newConductivity)};

    Helper::testVectors("Temperatures", built->getTemperatures(run), changed->getTemperatures(run));
    Helper::testVectors("Radiosities", built->getRadiosities(run), changed->getRadiosities(run));
    EXPECT_NEAR(built->getUValue(), changed->getUValue(), 1e-6);
}

TEST(TestSetShadeLayerConductivity, SettingEveryConductivityEqualsBuildingTheLayersWithThem)
{
    const auto changed{systemWithOpenShade(initialConductivity)};
    changed->setSolidLayerConductivities({1.0, 1.0, newConductivity});

    const auto built{systemWithOpenShade(newConductivity)};

    Helper::testVectors("Temperatures", built->getTemperatures(run), changed->getTemperatures(run));
}

TEST(TestSetShadeLayerConductivity, MaterialConductivityIsTheValueSet)
{
    const auto system{systemWithOpenShade(initialConductivity)};
    EXPECT_NEAR(system->getSolidLayers(run)[shadeIndex]->materialConductivity(), initialConductivity, 1e-12);

    system->setSolidLayerConductivity(shadeIndex, newConductivity);
    const auto solved{system->getTemperatures(run)};
    ASSERT_FALSE(solved.empty());

    const auto layers{system->getSolidLayers(run)};
    EXPECT_NEAR(layers[shadeIndex]->materialConductivity(), newConductivity, 1e-12);
    // The value the layer solves with is mixed with the gas, so it differs from the one set.
    EXPECT_GT(std::abs(layers[shadeIndex]->getConductance() - newConductivity), 1e-6);
}

TEST(TestSetShadeLayerConductivity, MaterialConductivityOfAGlassLayerIsItsConductivity)
{
    const auto system{systemWithOpenShade(initialConductivity)};

    EXPECT_NEAR(system->getSolidLayers(run)[0u]->materialConductivity(), 1.0, 1e-12);
}
