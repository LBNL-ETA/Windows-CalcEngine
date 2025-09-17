#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCEGases.hpp>
#include <WCETarcog.hpp>

// This test is used to compare two different approaches of generating gaps (thickness vs dcenter).
// Results should remain the same.
class TestShadeDCenter : public testing::Test
{
    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUpSystem(const Tarcog::ISO15099::GapLayer & gapLayer)
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 255.15;   // Kelvins
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 255.15;             // Kelvins
        auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////

        auto windowWidth = 2.0;
        auto windowHeight = 2.0;

        constexpr auto solidLayerThickness = 0.005715;   // [m]
        constexpr auto solidLayerConductance = 1.0;
        auto aLayer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        const auto shadeLayerConductance = 5.0;
        const auto shadeThickness = 0.06;

        const auto dl{0.0};
        const auto dr{0.0};
        const auto dtop{0.0};
        const auto dbot{0.0};
        const auto PermeabilityFactor = 0.3;   // fraction

        EffectiveLayers::ShadeOpenness openness{dl, dr, dtop, dbot};
        EffectiveLayers::EffectiveLayerCommon effLayer{
          shadeThickness, PermeabilityFactor, openness};

        const auto Ef = 0.9;
        const auto Eb = 0.9;
        const auto Tirf = 0;
        const auto Tirb = 0;

        auto shadeLayer = Tarcog::ISO15099::Layers::shading(shadeThickness,
                                                            shadeLayerConductance,
                                                            effLayer.getEffectiveOpenness(),
                                                            Ef,
                                                            Tirf,
                                                            Eb,
                                                            Tirb);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aLayer1, gapLayer, shadeLayer});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem & GetSystem() const
    {
        return *m_TarcogSystem;
    };
};

TEST_F(TestShadeDCenter, GapLayerAsThickness)
{
    SCOPED_TRACE("Begin Test: Gap entered as thickness.");

    // Gap thickness is measured from surface-to-surface.
    constexpr auto gapThickness = 0.0127;
    auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);

    SetUpSystem(GapLayer1);

    auto & aSystem = GetSystem();

    auto effectiveLayerConductivities{
      aSystem.getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue)};

    const auto systemKeff{aSystem.getEffectiveSystemConductivity(Tarcog::ISO15099::System::Uvalue)};
    EXPECT_NEAR(0.311294, systemKeff, 1e-6);

    const auto uval = aSystem.getUValue();
    EXPECT_NEAR(4.021995, uval, 1e-6);

    const auto heatflow =
      aSystem.getHeatFlow(Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(85.917632, heatflow, 1e-6);
}

TEST_F(TestShadeDCenter, GapLayerAsDcenter)
{
    SCOPED_TRACE("Begin Test: Gap entered as dcenter.");

    // In this case, gap is measured as distance from surface-to-center. In this case shade layer
    // thickness is 0.06 m which makes dcenter 0.03 m bigger than gap thickness making gap thickness
    // 0.0427 m.constexpr
    constexpr auto gapThickness = 0.0427;
    auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness, true);

    SetUpSystem(GapLayer1);

    auto & aSystem = GetSystem();

    auto effectiveLayerConductivities{
      aSystem.getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue)};

    const auto systemKeff{aSystem.getEffectiveSystemConductivity(Tarcog::ISO15099::System::Uvalue)};
    EXPECT_NEAR(0.311294, systemKeff, 1e-6);

    const auto uval = aSystem.getUValue();
    EXPECT_NEAR(4.021995, uval, 1e-6);

    const auto heatflow =
      aSystem.getHeatFlow(Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(85.917632, heatflow, 1e-6);
}
