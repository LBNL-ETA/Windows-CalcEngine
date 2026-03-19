#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

// Test that setSolidLayerConductivity updates the effective conductivity
// returned by getSolidEffectiveLayerConductivities.
// See: https://github.com/LBNL-ETA/Windows-CalcEngine/issues/243
class TestSetSolidLayerConductivity : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        constexpr auto airTemperature = 255.15;
        constexpr auto airSpeed = 5.5;
        constexpr auto tSky = 255.15;
        constexpr auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        constexpr auto roomTemperature = 294.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);

        // Glass layer 1
        constexpr auto glassThickness = 0.003;
        constexpr auto glassConductance = 1.0;
        auto glass1 = Tarcog::ISO15099::Layers::solid(glassThickness, glassConductance);

        // Glass layer 2
        auto glass2 = Tarcog::ISO15099::Layers::solid(glassThickness, glassConductance);

        // Shade layer (cellular shade) with initial conductivity of 0.45
        constexpr auto shadeThickness = 0.03514;
        constexpr auto initialConductivity = 0.45;
        constexpr auto permeabilityFactor = 0.0;

        const auto effectiveLayer{EffectiveLayers::makeCommonValues(shadeThickness, permeabilityFactor)};

        constexpr auto emissivity = 0.876;
        constexpr auto irTransmittance = 0.00028564859999999278;

        auto shade = Tarcog::ISO15099::Layers::shading(
          effectiveLayer.thickness,
          initialConductivity,
          effectiveLayer.openness,
          emissivity,
          irTransmittance,
          emissivity,
          irTransmittance);

        // Gaps
        constexpr auto gapThickness = 0.0127;
        auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        auto gap2 = Tarcog::ISO15099::Layers::gap(0.0381);

        constexpr double windowWidth = 1.0;
        constexpr double windowHeight = 1.0;

        Tarcog::ISO15099::CIGU igu(windowWidth, windowHeight);
        igu.addLayers({glass1, gap1, glass2, gap2, shade});

        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(igu, Indoor, Outdoor);
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    }
};

TEST_F(TestSetSolidLayerConductivity, ConductivityUpdateIsReflectedInEffectiveValues)
{
    SCOPED_TRACE("Test that setSolidLayerConductivity is reflected in getSolidEffectiveLayerConductivities");

    auto * sys = GetSystem();

    // Get initial effective conductivities
    const auto initialKeffs = sys->getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue);
    ASSERT_EQ(3u, initialKeffs.size());

    // Now simulate what THERM would do: set a different conductivity
    constexpr double thermComputedKeff = 0.0986;
    sys->setSolidLayerConductivity(2, thermComputedKeff);

    // Get updated effective conductivities
    const auto updatedKeffs = sys->getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue);
    ASSERT_EQ(3u, updatedKeffs.size());
    
    const std::vector correctKeffs{1.0, 1.0, 0.098693748135024753};
    Helper::testVectors("Effective conductivities after set", correctKeffs, updatedKeffs);
}
