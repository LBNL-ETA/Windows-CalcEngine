#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

using Tarcog::ISO15099::CBaseLayer;
using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation
    : public testing::Test
{
    std::unique_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////

        constexpr auto airTemperature = 298.15;   // Kelvins
        constexpr auto airSpeed = 5.5;            // meters per second
        constexpr auto tSky = 298.15;             // Kelvins
        constexpr auto solarRadiation = 1000.0;
        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        constexpr auto roomTemperature = 298.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////

        constexpr double windowWidth = 1;
        constexpr double windowHeight = 1;

        constexpr auto solidLayerThickness = 0.005715;   // [m]
        constexpr auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        layer1->setSolarHeatGain(0.03, solarRadiation);
        ASSERT_TRUE(layer1 != nullptr);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        layer2->setSolarHeatGain(0.03, solarRadiation);
        ASSERT_TRUE(layer2 != nullptr);

        constexpr auto shadeLayerThickness = 0.01;
        constexpr auto shadeLayerConductance = 160.0;
        constexpr auto PermeabilityFactor = 0.2;

        EffectiveLayers::ShadeOpenness openness{.Dl = 0.1, .Dr = 0.1, .Dtop = 0.1, .Dbot = 0.1};
        EffectiveLayers::EffectiveLayerCommon effectiveLayer{
          shadeLayerThickness, PermeabilityFactor, openness};

        auto shadeLayer = Tarcog::ISO15099::Layers::shading(
          shadeLayerThickness, shadeLayerConductance, effectiveLayer.getEffectiveOpenness());
        shadeLayer->setSolarHeatGain(0.35, solarRadiation);
        ASSERT_TRUE(shadeLayer != nullptr);

        constexpr auto gapThickness = 0.0127;
        constexpr auto gapAirSpeed = 0.5;

        const auto aGap1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(aGap1 != nullptr);
        const auto gap1 =
          Tarcog::ISO15099::Layers::forcedVentilationGap(aGap1, gapAirSpeed, roomTemperature);
        ASSERT_TRUE(gap1 != nullptr);
        auto aGap2 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(aGap2 != nullptr);
        const auto gap2 =
          Tarcog::ISO15099::Layers::forcedVentilationGap(aGap2, gapAirSpeed, roomTemperature);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, shadeLayer, gap2, layer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////

        m_TarcogSystem = std::make_unique<CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] CBaseLayer * GetGap1() const
    {
        return m_TarcogSystem->getGapLayers()[0].get();
    }

    [[nodiscard]] CBaseLayer * GetGap2() const
    {
        return m_TarcogSystem->getGapLayers()[1].get();
    }

    [[nodiscard]] CIGUSolidLayer * GetFirstLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[0].get();
    }

    [[nodiscard]] CIGUSolidLayer * GetSecondLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[1].get();
    }

    [[nodiscard]] CIGUSolidLayer * GetThirdLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[2].get();
    }
};

TEST_F(TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation, VentilationFlow)
{
    SCOPED_TRACE("Begin Test: Test Ventilated Gap Layer - Intial Airflow");

    auto *aLayer = GetGap1();

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(3.375404, gainEnergy, 1e-6);

    aLayer = GetGap2();
    ASSERT_TRUE(aLayer != nullptr);
    gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(-3.375404, gainEnergy, 1e-6);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation,
       FirstLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    const auto *aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4, and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    const auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    const auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(306.035371, frontTemperature, 1e-6);
    EXPECT_NEAR(307.358052, backTemperature, 1e-6);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation,
       SecondLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    const auto *aLayer = GetSecondLayer();

    ASSERT_TRUE(aLayer != nullptr);
    const auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    const auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(334.905321, frontTemperature, 1e-6);
    EXPECT_NEAR(334.908295, backTemperature, 1e-6);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation,
       ThirdLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    const auto *aLayer = GetThirdLayer();

    ASSERT_TRUE(aLayer != nullptr);
    const auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    const auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(318.102821, frontTemperature, 1e-6);
    EXPECT_NEAR(317.253802, backTemperature, 1e-6);
}
