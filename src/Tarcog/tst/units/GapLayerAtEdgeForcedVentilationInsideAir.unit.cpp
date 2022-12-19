#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;

class TestGapLayerAtEdgeForcedVentilationInsideAir : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////
        auto outdoorTemperature = 255.15;   // Kelvins
        auto airSpeed = 5.5;                // meters per second
        auto tSky = 255.15;                 // Kelvins
        auto solarRadiation = 0.0;

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

        auto roomTemperature = 295.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        // IGU
        const auto layer1Thickness = 0.005715;   // [m]
        const auto layer1Conductance = 1.0;

        auto solidLayer1 =
          Tarcog::ISO15099::Layers::solid(layer1Thickness, layer1Conductance);
        ASSERT_TRUE(solidLayer1 != nullptr);

        const auto layer2Thickness = 0.01;
        const auto layer2Conductance = 160.0;

        auto shadeLayer =
          Tarcog::ISO15099::Layers::solid(layer2Thickness, layer2Conductance);
        ASSERT_TRUE(shadeLayer != nullptr);

        auto gapThickness = 0.0127;
        auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        // Now add forced ventilation to the gap
        auto gapAirSpeed = 0.5;
        auto gap = Tarcog::ISO15099::Layers::forcedVentilationGap(
          GapLayer1, gapAirSpeed, roomTemperature);
        ASSERT_TRUE(gap != nullptr);

        double windowWidth = 1;
        double windowHeight = 1;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({solidLayer1, gap, shadeLayer});

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

TEST_F(TestGapLayerAtEdgeForcedVentilationInsideAir, GapLayerSurfaceIRFlow)
{
    SCOPED_TRACE("Begin Test: Test gap layer surface temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Front);
    auto backIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(286.605527, frontIRRadiationFlow, 1e-4);
    EXPECT_NEAR(359.726794, backIRRadiationFlow, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationInsideAir, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(131.545905, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationInsideAir, FirstLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(262.168262, frontTemperature, 1e-4);
    EXPECT_NEAR(263.343360, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationInsideAir, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(263.343360, frontTemperature, 1e-4);
    EXPECT_NEAR(284.923046, backTemperature, 1e-4);
    EXPECT_NEAR(284.769550, layerTemperature, 1e-4);
    EXPECT_NEAR(274.133203, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationInsideAir, SecondLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Shade Temperatures");

    auto aLayer = GetSecondLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(284.923046, frontTemperature, 1e-4);
    EXPECT_NEAR(284.927676, backTemperature, 1e-4);
}