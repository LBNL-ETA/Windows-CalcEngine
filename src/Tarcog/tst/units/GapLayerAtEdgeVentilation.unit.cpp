#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestGapLayerAtEdgeVentilation : public testing::Test
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

        auto solidLayer = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(solidLayer != nullptr);

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
        auto gap = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({solidLayer, gap, shadeLayer});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer> GetSolidLayer() const
    {
        auto solidLayer = m_TarcogSystem->getSolidLayers()[0];
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUSolidLayer>(solidLayer)
               != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUSolidLayer>(solidLayer);
    };

    std::shared_ptr<Tarcog::ISO15099::CIGUVentilatedGapLayer> GetGap() const
    {
        auto gap = m_TarcogSystem->getGapLayers()[0];
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUVentilatedGapLayer>(gap)
               != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUVentilatedGapLayer>(gap);
    };

    std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer> GetShadeLayer() const
    {
        auto shadeLayer = m_TarcogSystem->getSolidLayers()[1];
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUSolidLayer>(shadeLayer)
               != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUSolidLayer>(shadeLayer);
    };
};

TEST_F(TestGapLayerAtEdgeVentilation, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Ventilated Gap Layer - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(98.518206944209908, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeVentilation, SolidTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Ventilated Gap Layer - Solid Temperatures");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(261.30757062774973, frontTemperature, 1e-4);
    EXPECT_NEAR(262.33797584175301, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeVentilation, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Ventilated Gap Layer - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(262.33797584175301, frontTemperature, 1e-4);
    EXPECT_NEAR(283.91677515043358, backTemperature, 1e-4);
    EXPECT_NEAR(282.17807803748229, layerTemperature, 1e-4);
    EXPECT_NEAR(273.12737549609329, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeVentilation, ShadeTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Ventilated Gap Layer - Shade Temperatures");

    auto aLayer = GetShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(283.91677515043358, frontTemperature, 1e-4);
    EXPECT_NEAR(283.92316397867683, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeVentilation, AirflowReferencePoint)
{
    SCOPED_TRACE("Begin Test: Test Ventilated Gap Layer - Airflow Reference Point");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto airflowReferencePoint = aLayer->getAirflowReferencePoint(0.5);
    EXPECT_NEAR(6912.3686896869631, airflowReferencePoint, 1e-4);
}
