#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir
    : public testing::Test
{
    std::unique_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////

        constexpr auto outdoorAirTemperature = 298.15;   // Kelvins
        constexpr auto outdoorAirSpeed = 2.75;           // meters per second
        constexpr auto tSky = outdoorAirTemperature;     // Kelvins
        constexpr auto solarRadiation = 1000.0;
        auto Outdoor =
          Tarcog::ISO15099::Environments::outdoor(outdoorAirTemperature,
                                                  outdoorAirSpeed,
                                                  solarRadiation,
                                                  tSky,
                                                  Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        constexpr auto roomTemperature = 298.15;
        const auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////

        constexpr auto windowWidth = 1.0;
        constexpr auto windowHeight = 1.0;

        constexpr auto shadeLayerConductance = 0.12;
        // make cell geometry
        constexpr auto thickness_31111{0.00023};

        constexpr FenestrationCommon::Perforated::Geometry geometry{
          .type = FenestrationCommon::Perforated::Type::Circular,
          .SpacingX = 0.00169,
          .SpacingY = 0.00169,
          .DimensionX = 0.00116,
          .DimensionY = 0.00116};

        const auto effectiveLayerPerforated{
          EffectiveLayers::makePerforatedValues(thickness_31111, geometry)};

        constexpr auto Ef = 0.640892;
        constexpr auto Eb = 0.623812;
        constexpr auto Tirf = 0.257367;
        constexpr auto Tirb = 0.257367;
        auto shadeLayer =
          Tarcog::ISO15099::Layers::shading(effectiveLayerPerforated.thickness,
                                            shadeLayerConductance,
                                            effectiveLayerPerforated.openness,
                                            Ef,
                                            Tirf,
                                            Eb,
                                            Tirb);
        shadeLayer->setSolarHeatGain(0.35, solarRadiation);
        ASSERT_TRUE(shadeLayer != nullptr);

        constexpr auto gapThickness = 0.05;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);
        constexpr auto gapAirSpeed = 0.1;
        auto forcedGapLayer = Tarcog::ISO15099::Layers::forcedVentilationGap(
          gapLayer, gapAirSpeed, outdoorAirTemperature);
        ASSERT_TRUE(forcedGapLayer != nullptr);

        constexpr auto solidLayerThickness = 0.003048;   // [m]
        constexpr auto solidLayerConductance = 1.0;
        auto solidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        solidLayer->setSolarHeatGain(0.04, solarRadiation);
        ASSERT_TRUE(solidLayer != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({shadeLayer, forcedGapLayer, solidLayer});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////

        m_TarcogSystem = std::make_unique<CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] CIGUSolidLayer * GetFirstLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[0].get();
    }

    [[nodiscard]] CIGUGapLayer * GetGap() const
    {
        return m_TarcogSystem->getGapLayers()[0].get();
    }

    [[nodiscard]] CIGUSolidLayer * GetSecondLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[1].get();
    }
};

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir,
       GapLayerSurfaceIRFlow)
{
    SCOPED_TRACE("Begin Test: Test gap layer surface temperatures");

    auto * const aLayer = GetGap();

    // Airflow iterations are set to 1e-4, and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    const auto frontIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Front);
    const auto backIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(514.09979648823617, frontIRRadiationFlow, 1e-4);
    EXPECT_NEAR(494.62579436827116, backIRRadiationFlow, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gain Energy");

    auto * const aLayer = GetGap();

    // Airflow iterations are set to 1e-4, and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    const auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(-45.333884920391, gainEnergy, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir,
       FirstLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto * const aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4, and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    const auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    const auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(312.84342610457981, frontTemperature, 1e-4);
    EXPECT_NEAR(313.12896009393842, backTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gap Temperatures");

    auto * const aLayer = GetGap();

    // Airflow iterations are set to 1e-4, and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    const auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    const auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    const auto layerTemperature = aLayer->averageLayerTemperature();
    const auto averageTemperature = aLayer->averageSurfaceTemperature();
    EXPECT_NEAR(313.12896009393842, frontTemperature, 1e-4);
    EXPECT_NEAR(305.04357261282178, backTemperature, 1e-4);
    EXPECT_NEAR(302.78306814785384, layerTemperature, 1e-4);
    EXPECT_NEAR(309.0862663533801, averageTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir,
       SecondLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Shade Temperatures");

    auto * const aLayer = GetSecondLayer();

    // Airflow iterations are set to 1e-4, and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    const auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    const auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(305.04357261282178, frontTemperature, 1e-4);
    EXPECT_NEAR(304.94803305914581, backTemperature, 1e-4);
}
