#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCEGases.hpp>
#include <WCETarcog.hpp>
#include <WCECommon.hpp>

class TestDoubleOutsidePerforatedShade_SHGC : public testing::Test
{
    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature = 305.15;   // Kelvins
        constexpr auto airSpeed = 2.75;           // meters per second
        constexpr auto tSky = 305.15;             // Kelvins
        constexpr auto solarRadiation = 783.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        constexpr auto roomTemperature = 297.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        constexpr auto shadeLayerConductance = 0.12;

        // make cell geometry
        constexpr auto thickness_31111{0.00023};

        constexpr FenestrationCommon::Perforated::Geometry aGeometry{
          .type = FenestrationCommon::Perforated::Type::Circular,
          .SpacingX = 0.00169,
          .SpacingY = 0.00169,
          .DimensionX = 0.00116,
          .DimensionY = 0.00116};

        EffectiveLayers::EffectiveLayerPerforated effectiveLayerPerforated{thickness_31111,
                                                                           aGeometry};


        constexpr auto Ef = 0.640892;
        constexpr auto Eb = 0.623812;
        constexpr auto Tirf = 0.257367;
        constexpr auto Tirb = 0.257367;

        auto aLayer1 =
          Tarcog::ISO15099::Layers::shading(effectiveLayerPerforated.effectiveThickness(),
                                            shadeLayerConductance,
                                            effectiveLayerPerforated.getEffectiveOpenness(),
                                            Ef,
                                            Tirf,
                                            Eb,
                                            Tirb);

        aLayer1->setSolarHeatGain(0.106659, solarRadiation);

        constexpr auto gapThickness = 0.0127;
        auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        constexpr auto solidLayerThickness = 0.003048;   // [m]
        constexpr auto solidLayerConductance = 1.0;
        auto aLayer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aLayer2 != nullptr);

        aLayer2->setSolarHeatGain(0.034677, solarRadiation);


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

TEST_F(TestDoubleOutsidePerforatedShade_SHGC, Test1)
{
    SCOPED_TRACE("Begin Test: Outside perforated shade.");

    auto aSystem = GetSystem();

    const auto uval = aSystem.getUValue();
    EXPECT_NEAR(3.193057, uval, 1e-6);

    const auto effectiveLayerConductivities{
      aSystem.getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue)};

    const std::vector correctEffectConductivites{0.115592, 1.0};
    EXPECT_EQ(correctEffectConductivites.size(), effectiveLayerConductivities.size());
    for(size_t i = 0u; i < correctEffectConductivites.size(); ++i)
    {
        EXPECT_NEAR(correctEffectConductivites[i], effectiveLayerConductivities[i], 1e-6);
    }

    constexpr auto totSol{0.315236};
    const auto shgc{aSystem.getSHGC(totSol)};
    EXPECT_NEAR(0.348647, shgc, 1e-6);

    const auto heatflow =
      aSystem.getHeatFlow(Tarcog::ISO15099::System::SHGC, Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(-51.705046, heatflow, 1e-6);
}
