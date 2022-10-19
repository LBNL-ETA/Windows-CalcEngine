#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestGapLayerAtEdgeForcedVentilation : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

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

        auto layer = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer != nullptr);

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;
        auto Atop = 0.1;
        auto Abot = 0.1;
        auto Aleft = 0.1;
        auto Aright = 0.1;
        auto Afront = 0.2;

        EffectiveLayers::ShadeOpenness openness{Afront, Aleft, Aright, Atop, Abot};

        double windowWidth = 1;
        double windowHeight = 1;

        EffectiveLayers::EffectiveLayerOther effectiveLayer{
          windowWidth, windowHeight, shadeLayerThickness, openness};

        EffectiveLayers::EffectiveOpenness effOpenness{effectiveLayer.getEffectiveOpenness()};

        auto shadeLayer = Tarcog::ISO15099::Layers::shading(
          shadeLayerThickness, shadeLayerConductance, effOpenness);

        ASSERT_TRUE(shadeLayer != nullptr);

        auto gapThickness = 0.0127;
        auto gapAirSpeed = 0.5;
        auto gapVerticalDirection = Tarcog::ISO15099::AirVerticalDirection::Up;
        Tarcog::ISO15099::ForcedVentilation forcedVentilation = {gapAirSpeed, gapVerticalDirection};
        auto gap = Tarcog::ISO15099::Layers::forcedVentilationGap(gapThickness, forcedVentilation);
        ASSERT_TRUE(gap != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer, gap, shadeLayer});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    std::shared_ptr<Tarcog::ISO15099::CIGUVentilatedGapLayer> GetGap() const
    {
        auto layer = m_TarcogSystem->getGapLayers()[0];
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUVentilatedGapLayer>(layer)
               != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUVentilatedGapLayer>(layer);
    };
};

TEST_F(TestGapLayerAtEdgeForcedVentilation, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(-173.19394352133912, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilation, Temperature)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer - Temperature");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(261.96466446784217, frontTemperature, 1e-4);
    EXPECT_NEAR(274.85315869592796, backTemperature, 1e-4);
    EXPECT_NEAR(268.40891158188504, layerTemperature, 1e-4);
    EXPECT_NEAR(268.40891158188504, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilation, AirflowReferencePoint)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer - Airflow Reference Point");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto airflowReferencePoint = aLayer->getAirflowReferencePoint(0.5);
    EXPECT_NEAR(6911.7392493523666, airflowReferencePoint, 1e-4);
}