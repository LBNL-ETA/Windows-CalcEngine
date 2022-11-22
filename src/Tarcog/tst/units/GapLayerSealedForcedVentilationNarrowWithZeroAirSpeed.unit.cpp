#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestGapLayerSealedForcedVentilationNarrowWithZeroAirSpeed : public testing::Test
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

        auto closedShadeLayer =
          Tarcog::ISO15099::Layers::closedShading(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(closedShadeLayer != nullptr);

        auto gapThickness = 0.00001;
        auto gapAirSpeed = 0.0;
        Tarcog::ISO15099::ForcedVentilation forcedVentilation = {gapAirSpeed};
        auto gap = Tarcog::ISO15099::Layers::forcedVentilationGap(gapThickness, forcedVentilation);
        ASSERT_TRUE(gap != nullptr);

        double windowWidth = 1;
        double windowHeight = 1;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({solidLayer, gap, closedShadeLayer});

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

    std::shared_ptr<Tarcog::ISO15099::CIGUShadeLayer> GetClosedShadeLayer() const
    {
        auto closedShadeLayer = m_TarcogSystem->getSolidLayers()[1];
        assert(std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUShadeLayer>(closedShadeLayer)
               != nullptr);
        return std::dynamic_pointer_cast<Tarcog::ISO15099::CIGUShadeLayer>(closedShadeLayer);
    };
};

TEST_F(TestGapLayerSealedForcedVentilationNarrowWithZeroAirSpeed, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Narrow Gap Layer With Zero Air Speed - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(0.0, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerSealedForcedVentilationNarrowWithZeroAirSpeed, SolidTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Narrow Gap Layer With Zero Air Speed - Solid Temperatures");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(262.8455712342456, frontTemperature, 1e-4);
    EXPECT_NEAR(264.13464915091032, backTemperature, 1e-4);
}

TEST_F(TestGapLayerSealedForcedVentilationNarrowWithZeroAirSpeed, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Narrow Gap Layer With Zero Air Speed - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(264.13464915091032, frontTemperature, 1e-4);
    EXPECT_NEAR(264.23102533430392, backTemperature, 1e-4);
    EXPECT_NEAR(264.18283724260709, layerTemperature, 1e-4);
    EXPECT_NEAR(264.18283724260709, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerSealedForcedVentilationNarrowWithZeroAirSpeed, ClosedShadeTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Narrow Gap Layer With Zero Air Speed - Closed Shade Temperatures");

    auto aLayer = GetClosedShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(264.23102533430392, frontTemperature, 1e-4);
    EXPECT_NEAR(265.52010325096921, backTemperature, 1e-4);
}

TEST_F(TestGapLayerSealedForcedVentilationNarrowWithZeroAirSpeed, AirflowReferencePoint)
{
    SCOPED_TRACE("Begin Test: Test Sealed Forced Ventilated Narrow Gap Layer With Zero Air Speed - Airflow Reference Point");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto airflowReferencePoint = aLayer->getAirflowReferencePoint(0.5);
    EXPECT_NEAR(6911.5329001843065, airflowReferencePoint, 1e-4);
}