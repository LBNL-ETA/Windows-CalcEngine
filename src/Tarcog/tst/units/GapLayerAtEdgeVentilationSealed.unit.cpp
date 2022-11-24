#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestGapLayerAtEdgeVentilationSealed : public testing::Test
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

        auto solidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(solidLayer != nullptr);

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;

        auto shadeLayer =
          Tarcog::ISO15099::Layers::closedShading(shadeLayerThickness, shadeLayerConductance);

        ASSERT_TRUE(shadeLayer != nullptr);

        auto gapThickness = 0.0127;
        auto gap = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap != nullptr);

        double windowWidth = 1;
        double windowHeight = 1;

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
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUSolidLayer>(solidLayer) != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUSolidLayer>(solidLayer);
    };

    std::shared_ptr<Tarcog::ISO15099::CIGUVentilatedGapLayer> GetGap() const
    {
        auto gap = m_TarcogSystem->getGapLayers()[0];
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUVentilatedGapLayer>(gap) != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUVentilatedGapLayer>(gap);
    };

    std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer> GetShadeLayer() const
    {
        auto shadeLayer = m_TarcogSystem->getSolidLayers()[1];
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUSolidLayer>(shadeLayer) != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUSolidLayer>(shadeLayer);
    };
};

TEST_F(TestGapLayerAtEdgeVentilationSealed, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Sealed  Ventilated Gap Layer - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(0.01825302550352461, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeVentilationSealed, SolidTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed  Ventilated Gap Layer - Solid Temperatures");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(258.91171403153015, frontTemperature, 1e-4);
    EXPECT_NEAR(259.54021444301679, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeVentilationSealed, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed  Ventilated Gap Layer - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(259.54021444301679, frontTemperature, 1e-4);
    EXPECT_NEAR(280.27406284157792, backTemperature, 1e-4);
    EXPECT_NEAR(269.90934338470544, layerTemperature, 1e-4);
    EXPECT_NEAR(269.90713864229735, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeVentilationSealed, ShadeTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed  Ventilated Gap Layer - Shade Temperatures");

    auto aLayer = GetShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(280.27406284157792, frontTemperature, 1e-4);
    EXPECT_NEAR(280.28093506752992, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeVentilationSealed, AirflowReferencePoint)
{
    SCOPED_TRACE("Begin Test: Test Sealed  Ventilated Gap Layer - Airflow Reference Point");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto airflowReferencePoint = aLayer->getAirflowReferencePoint(0.5);
    EXPECT_NEAR(6911.8109575046556, airflowReferencePoint, 1e-4);
}
