#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;

class TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////
        auto outdoorTemperature = 293.15;   // Kelvins
        auto airSpeed = 5.5;                // meters per second
        auto tSky = 293.15;                 // Kelvins
        auto solarRadiation = 1000.0;

        auto Outdoor =
          Tarcog::ISO15099::Environments::outdoor(outdoorTemperature,
                                                  airSpeed,
                                                  solarRadiation,
                                                  tSky,
                                                  Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 293.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        // IGU
        const auto layer1Thickness = 0.006;   // [m]
        const auto layer1Conductance = 1.0;

        auto solidLayer1 =
          Tarcog::ISO15099::Layers::solid(layer1Thickness, layer1Conductance);
        ASSERT_TRUE(solidLayer1 != nullptr);

        const auto shadeLayerThickness = 0.002;
        const auto shadeLayerConductance = 0.16;

        auto shadeLayer =
          Tarcog::ISO15099::Layers::solid(shadeLayerThickness, shadeLayerConductance);
        ASSERT_TRUE(shadeLayer != nullptr);

        auto gapThickness = 0.05;
        auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        // Now add forced ventilation to the gap
        auto gapAirSpeed = 0.5;
        auto gap = Tarcog::ISO15099::Layers::forcedVentilationGap(
          GapLayer1, gapAirSpeed, outdoorTemperature);
        ASSERT_TRUE(gap != nullptr);

        double windowWidth = 1;
        double windowHeight = 1;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({shadeLayer, gap, solidLayer1});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] CIGUSolidLayer * GetFirstLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[0].get();
    };

    [[nodiscard]] CIGUGapLayer * GetGap() const
    {
        return m_TarcogSystem->getGapLayers()[0].get();
    };

    [[nodiscard]] CIGUSolidLayer * GetSecondLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[1].get();
    };
};

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir, GapLayerSurfaceIRFlow)
{
    SCOPED_TRACE("Begin Test: Test gap layer surface temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Front);
    auto backIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(261.579394, frontIRRadiationFlow, 1e-4);
    EXPECT_NEAR(316.739416, backIRRadiationFlow, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(-75.720712, gainEnergy, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir, FirstLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(257.561838, frontTemperature, 1e-4);
    EXPECT_NEAR(257.964452, backTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(257.964452, frontTemperature, 1e-4);
    EXPECT_NEAR(275.631339, backTemperature, 1e-4);
    EXPECT_NEAR(260.505441, layerTemperature, 1e-4);
    EXPECT_NEAR(266.797896, averageTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedExteriorShadingAndGlassForcedVentilationOutsideAir, SecondLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Shade Temperatures");

    auto aLayer = GetSecondLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(275.631339, frontTemperature, 1e-4);
    EXPECT_NEAR(275.640475, backTemperature, 1e-4);
}