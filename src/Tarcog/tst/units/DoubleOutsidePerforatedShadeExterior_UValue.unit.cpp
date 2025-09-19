#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

/// This example is compared with fortran version of WINDOW

class TestDoubleOutsidePerforatedShadeExterior_UValue : public testing::Test
{
    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

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
        constexpr auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        constexpr auto shadeLayerConductance = 0.3;

        // make cell geometry

        // 31006 represents material properties at the time of the creation of this unit test
        constexpr auto thickness_31006{0.0006};
        constexpr FenestrationCommon::Perforated::Geometry aGeometry{
          .type = FenestrationCommon::Perforated::Type::Circular,
          .SpacingX = 0.00169,
          .SpacingY = 0.00169,
          .DimensionX = 0.00116,
          .DimensionY = 0.00116};

        const auto effectiveLayerPerforated{
          EffectiveLayers::makePerforatedValues(thickness_31006, aGeometry)};

        constexpr auto Ef = 0.752239525318;
        constexpr auto Eb = 0.752239525318;
        constexpr auto Tirf = 0.164178311825;
        constexpr auto Tirb = 0.164178311825;

        auto aLayer1 =
          Tarcog::ISO15099::Layers::shading(effectiveLayerPerforated.effectiveThickness(),
                                            shadeLayerConductance,
                                            effectiveLayerPerforated.getEffectiveOpenness(),
                                            Ef,
                                            Tirf,
                                            Eb,
                                            Tirb);

        aLayer1->setSolarHeatGain(0.324484854937, solarRadiation);

        constexpr auto gapThickness = 0.0127;
        const auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        constexpr auto solidLayerThickness = 0.003048;   // [m]
        constexpr auto solidLayerConductance = 1.0;
        auto aLayer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aLayer2 != nullptr);

        aLayer2->setSolarHeatGain(0.034704498947, solarRadiation);

        constexpr auto windowWidth = 1.0;
        constexpr auto windowHeight = 1.0;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aLayer1, GapLayer1, aLayer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem & GetSystem() const
    {
        return *m_TarcogSystem;
    }
};

TEST_F(TestDoubleOutsidePerforatedShadeExterior_UValue, Test1)
{
    SCOPED_TRACE("Begin Test: Outside perforated shade.");

    auto aSystem = GetSystem();

    const auto uval = aSystem.getUValue();

    // Fortran version of tarcog in WINDOW gives 3.215803
    EXPECT_NEAR(3.215808, uval, 1e-6);

    const auto effectiveLayerConductivities{
      aSystem.getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue)};

    const std::vector correctEffectConductivites{0.230300, 1.0};
    EXPECT_EQ(correctEffectConductivites.size(), effectiveLayerConductivities.size());
    for(size_t i = 0u; i < correctEffectConductivites.size(); ++i)
    {
        EXPECT_NEAR(correctEffectConductivites[i], effectiveLayerConductivities[i], 1e-6);
    }

    const auto heatflow =
      aSystem.getHeatFlow(Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Indoor);

    // Fortran version of tarcog in WINDOW gives 125.416325
    EXPECT_NEAR(125.416522, heatflow, 1e-6);
}
