#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CBaseIGULayer;

class TestGapLayerInBetweenForcedVentilation : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
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

        auto roomTemperature = 295.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        // IGU
        auto solidLayerThickness = 0.005715;   // [m]
        auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer1 != nullptr);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer2 != nullptr);

        auto layer3 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer2 != nullptr);

        const double windowWidth = 1;
        const double windowHeight = 1;

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
        aIGU.addLayers({layer1, gap1, layer2, gap2, layer3});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] CBaseIGULayer * GetGap1() const
    {
        return m_TarcogSystem->getGapLayers()[0].get();
    };

    [[nodiscard]] CBaseIGULayer * GetGap2() const
    {
        return m_TarcogSystem->getGapLayers()[1].get();
    };
};

TEST_F(TestGapLayerInBetweenForcedVentilation, VentilationFlow)
{
    SCOPED_TRACE("Begin Test: Test Ventilated Gap Layer - Intial Airflow");

    auto aLayer = GetGap1();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(134.001098, gainEnergy, 1e-4);

    aLayer = GetGap2();
    ASSERT_TRUE(aLayer != nullptr);
    gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(43.748528, gainEnergy, 1e-4);
}
