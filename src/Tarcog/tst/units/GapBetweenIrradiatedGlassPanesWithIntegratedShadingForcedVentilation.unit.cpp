#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CBaseLayer;
using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////

        auto airTemperature = 298.15;   // Kelvins
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 298.15;             // Kelvins
        auto solarRadiation = 1000.0;
        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 298.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////

        double windowWidth = 1;
        double windowHeight = 1;

        auto solidLayerThickness = 0.005715;   // [m]
        auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        layer1->setSolarHeatGain(0.03, solarRadiation);
        ASSERT_TRUE(layer1 != nullptr);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        layer2->setSolarHeatGain(0.03, solarRadiation);
        ASSERT_TRUE(layer2 != nullptr);

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;
        auto Atop = 0.1;
        auto Abot = 0.1;
        auto Aleft = 0.1;
        auto Aright = 0.1;
        auto PermeabilityFactor = 0.2;

        EffectiveLayers::ShadeOpenness openness{Aleft, Aright, Atop, Abot};
        EffectiveLayers::EffectiveLayerCommon effectiveLayer{
          windowWidth, windowHeight, shadeLayerThickness, PermeabilityFactor, openness};

        auto shadeLayer = Tarcog::ISO15099::Layers::shading(
          shadeLayerThickness, shadeLayerConductance, effectiveLayer.getEffectiveOpenness());
        shadeLayer->setSolarHeatGain(0.35, solarRadiation);
        ASSERT_TRUE(shadeLayer != nullptr);

        const auto gapThickness = 0.0127;
        const auto gapAirSpeed = 0.5;
        const auto aGap1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(aGap1 != nullptr);
        const auto gap1 = Tarcog::ISO15099::Layers::forcedVentilationGap(
          aGap1, gapAirSpeed, roomTemperature);
        ASSERT_TRUE(gap1 != nullptr);
        auto aGap2 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(aGap2 != nullptr);
        const auto gap2 = Tarcog::ISO15099::Layers::forcedVentilationGap(
          aGap2, gapAirSpeed, roomTemperature);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, shadeLayer, gap2, layer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////

        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve(); 
    }

public:
    [[nodiscard]] CBaseLayer * GetGap1() const
    {
        return m_TarcogSystem->getGapLayers()[0].get();
    };

    [[nodiscard]] CBaseLayer * GetGap2() const
    {
        return m_TarcogSystem->getGapLayers()[1].get();
    };

    [[nodiscard]] CIGUSolidLayer * GetFirstLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[0].get();
    };

    [[nodiscard]] CIGUSolidLayer * GetSecondLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[1].get();
    };

    [[nodiscard]] CIGUSolidLayer * GetThirdLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[2].get();
    };

};

TEST_F(TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation, VentilationFlow)
{
    SCOPED_TRACE("Begin Test: Test Ventilated Gap Layer - Intial Airflow");

    auto aLayer = GetGap1();

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(3.787673, gainEnergy, 1e-6);

    aLayer = GetGap2();
    ASSERT_TRUE(aLayer != nullptr);
    gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(-3.787673, gainEnergy, 1e-6);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation, FirstLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(306.040920, frontTemperature, 1e-4);
    EXPECT_NEAR(307.364599, backTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation, SecondLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto aLayer = GetSecondLayer();

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(334.871021, frontTemperature, 1e-6);
    EXPECT_NEAR(334.873976, backTemperature, 1e-6);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesWithIntegratedShadingForcedVentilation, ThirdLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto aLayer = GetThirdLayer();

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(318.084058, frontTemperature, 1e-6);
    EXPECT_NEAR(317.236037, backTemperature, 1e-6);
}
