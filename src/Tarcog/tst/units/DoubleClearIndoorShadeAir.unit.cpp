#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

class TestDoubleClearIndoorShadeAir : public testing::Test
{
    std::unique_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature = 255.15;   // Kelvins
        constexpr auto airSpeed = 5.5;            // meters per second
        constexpr auto tSky = 255.15;             // Kelvins
        constexpr auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        constexpr auto roomTemperature = 295.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness = 0.005715;   // [m]
        constexpr auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer1 != nullptr);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        constexpr auto shadeLayerThickness = 0.01;
        constexpr auto shadeLayerConductance = 160.0;
        constexpr auto PermeabilityFactor = 0.2;

        EffectiveLayers::ShadeOpenness openness{.Dl = 0.1, .Dr = 0.1, .Dtop = 0.1, .Dbot = 0.1};

        auto effectiveLayer{
          EffectiveLayers::makeCommonValues(shadeLayerThickness, PermeabilityFactor, openness)};

        auto layer3 = Tarcog::ISO15099::Layers::shading(effectiveLayer.thickness,
                                                        shadeLayerConductance,
                                                        effectiveLayer.openness);

        ASSERT_TRUE(layer3 != nullptr);

        constexpr auto gapThickness = 0.0127;
        auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap1 != nullptr);

        auto gap2 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap2 != nullptr);

        constexpr auto windowWidth = 1.0;
        constexpr auto windowHeight = 1.0;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, layer2, gap2, layer3});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSingleSystem & GetSystem() const
    {
        return *m_TarcogSystem;
    }
};

TEST_F(TestDoubleClearIndoorShadeAir, Test1)
{
    SCOPED_TRACE("Begin Test: Indoor Shade - Air");

    constexpr auto Tolerance = 1e-6;

    const auto aSystem = GetSystem();

    const auto temperature = aSystem.getTemperatures();
    const auto radiosity = aSystem.getRadiosities();

    const std::vector correctTemp = {
      258.263891, 258.783935, 276.426524, 276.946569, 288.264550, 288.268356};
    const std::vector correctJ = {
      250.329019, 264.865975, 320.451170, 341.531072, 383.499870, 397.713907};

    Helper::testVectors("Temperatures", correctTemp, temperature, Tolerance);
    Helper::testVectors("Radiosities", correctJ, radiosity, Tolerance);

    const auto ventilatedFlow = aSystem.getVentilationFlow(Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(42.278080126434745, ventilatedFlow, Tolerance);
}
