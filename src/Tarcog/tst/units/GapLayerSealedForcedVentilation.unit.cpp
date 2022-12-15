#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestGapLayerSealedForcedVentilation : public testing::Test
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

        auto sealedLayer =
          Tarcog::ISO15099::Layers::sealedLayer(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(sealedLayer != nullptr);

        auto gapThickness = 0.0127;
        auto gapAirSpeed = 0.5;
        auto gap = Tarcog::ISO15099::Layers::forcedVentilationGap(gapThickness, gapAirSpeed);
        ASSERT_TRUE(gap != nullptr);

        double windowWidth = 1;
        double windowHeight = 1;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({solidLayer, gap, sealedLayer});

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

    std::shared_ptr<Tarcog::ISO15099::CIGUShadeLayer> GetSealedLayer() const
    {
        auto sealedLayer = m_TarcogSystem->getSolidLayers()[1];
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUShadeLayer>(sealedLayer)
               != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUShadeLayer>(sealedLayer);
    };
};

TEST_F(TestGapLayerSealedForcedVentilation, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Gap Layer - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(132.26008267306551, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerSealedForcedVentilation, SolidTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Gap Layer - Solid Temperatures");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(262.13857535301037, frontTemperature, 1e-4);
    EXPECT_NEAR(263.30867938548238, backTemperature, 1e-4);
}

TEST_F(TestGapLayerSealedForcedVentilation, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Gap Layer - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(263.30867938548238, frontTemperature, 1e-4);
    EXPECT_NEAR(284.72087553180552, backTemperature, 1e-4);
    EXPECT_NEAR(284.71692368656022, layerTemperature, 1e-4);
    EXPECT_NEAR(274.01477745864395, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerSealedForcedVentilation, ClosedShadeTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Gap Layer - Closed Shade Temperatures");

    auto aLayer = GetSealedLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(284.72087553180552, frontTemperature, 1e-4);
    EXPECT_NEAR(285.13511319074695, backTemperature, 1e-4);
}

TEST_F(TestGapLayerSealedForcedVentilation, AirflowReferencePoint)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Gap Layer - Airflow Reference Point");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto airflowReferencePoint = aLayer->getAirflowReferencePoint(0.5);
    EXPECT_NEAR(6912.4781021678982, airflowReferencePoint, 1e-4);
}
