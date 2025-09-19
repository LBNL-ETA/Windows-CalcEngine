#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCEGases.hpp>
#include <WCETarcog.hpp>
#include <WCECommon.hpp>

class TestDoubleOutsideVenetianShade_UValue : public testing::Test
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
        constexpr auto shadeLayerConductance = 160.0;

        constexpr auto matThickness{0.0001};   // m

        constexpr FenestrationCommon::Venetian::Geometry aGeometry{.SlatWidth = 0.0148,
                                                                   .SlatSpacing = 0.0127,
                                                                   .SlatTiltAngle = 0.0,
                                                                   .CurvatureRadius =
                                                                     0.0331305656433105};

        const auto effectiveVenetian{
          EffectiveLayers::makeHorizontalVenetianValues(matThickness, aGeometry)};

        constexpr auto Ef = 0.5564947806702053;
        constexpr auto Eb = 0.5564947806702053;
        constexpr auto Tirf = 0.42293224373137134;
        constexpr auto Tirb = 0.42293224373137134;

        auto aLayer1 = Tarcog::ISO15099::Layers::shading(effectiveVenetian.thickness,
                                                         shadeLayerConductance,
                                                         effectiveVenetian.openness,
                                                         Ef,
                                                         Tirf,
                                                         Eb,
                                                         Tirb);

        constexpr auto gapThickness = 0.0127;
        auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        constexpr auto solidLayerThickness = 0.003048;   // [m]
        constexpr auto solidLayerConductance = 1.0;
        auto aLayer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aLayer2 != nullptr);

        constexpr auto windowWidth = 1.0;
        constexpr auto windowHeight = 1.0;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aLayer1, GapLayer1, aLayer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::unique_ptr<Tarcog::ISO15099::CSystem>(
          new Tarcog::ISO15099::CSystem(aIGU, Indoor, Outdoor));
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem & GetSystem() const
    {
        return *m_TarcogSystem;
    }
};

TEST_F(TestDoubleOutsideVenetianShade_UValue, Test1)
{
    SCOPED_TRACE("Begin Test: Outside venetian shade.");

    auto aSystem = GetSystem();

    const auto effectiveLayerConductivities{
      aSystem.getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue)};

    const std::vector correctEffectConductivites{1.878064, 1.0};
    EXPECT_EQ(correctEffectConductivites.size(), effectiveLayerConductivities.size());
    for(size_t i = 0u; i < correctEffectConductivites.size(); ++i)
    {
        EXPECT_NEAR(correctEffectConductivites[i], effectiveLayerConductivities[i], 1e-6);
    }

    const auto systemKeff{aSystem.getEffectiveSystemConductivity(Tarcog::ISO15099::System::Uvalue)};
    EXPECT_NEAR(0.106427, systemKeff, 1e-6);

    const auto uval = aSystem.getUValue();
    EXPECT_NEAR(3.239692, uval, 1e-6);

    const auto heatflow =
      aSystem.getHeatFlow(Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(126.347983, heatflow, 1e-6);
}
