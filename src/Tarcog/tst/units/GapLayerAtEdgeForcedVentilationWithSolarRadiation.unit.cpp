#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestGapLayerAtEdgeForcedVentilationWithSolarRadiation : public testing::Test
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
        auto solarRadiation = 700.0;

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
        auto solidLayerSolarAbsorptance = 0.094189159572;

        auto solidLayer = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        solidLayer->setSolarAbsorptance(solidLayerSolarAbsorptance, solarRadiation);
        ASSERT_TRUE(solidLayer != nullptr);

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;
        auto shadeLayerSolarAbsorptance = 0.687443971634;
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
        shadeLayer->setSolarAbsorptance(shadeLayerSolarAbsorptance, solarRadiation);

        ASSERT_TRUE(shadeLayer != nullptr);

        auto gapThickness = 0.0127;
        auto gapAirSpeed = 0.5;
        Tarcog::ISO15099::ForcedVentilation forcedVentilation = {gapAirSpeed};
        auto gap = Tarcog::ISO15099::Layers::forcedVentilationGap(gapThickness, forcedVentilation);
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
    Tarcog::ISO15099::CSingleSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    };

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

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(16.449907386063828, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, SolidTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Solid Temperatures");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(268.85563831728786, frontTemperature, 1e-4);
    EXPECT_NEAR(270.97231009255381, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, SolidRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Solid Radiosities");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(287.28456598343178, frontRadiosity, 1e-4);
    EXPECT_NEAR(339.73558433006201, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(270.97231009255381, frontTemperature, 1e-4);
    EXPECT_NEAR(314.20685654711355, backTemperature, 1e-4);
    EXPECT_NEAR(293.77207512587205, layerTemperature, 1e-4);
    EXPECT_NEAR(292.58958331983365, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, GapRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Gap Radiosities");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(339.73558433006201, frontRadiosity, 1e-4);
    EXPECT_NEAR(518.55433901363858, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, ShadeTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Shade Temperatures");

    auto aLayer = GetShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(314.20685654711355, frontTemperature, 1e-4);
    EXPECT_NEAR(314.21313360489381, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, ShadeRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Shade Radiosities");

    auto aLayer = GetShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(518.55433901363858, frontRadiosity, 1e-4);
    EXPECT_NEAR(533.07538128082308, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, AirflowReferencePoint)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Airflow Reference Point");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto airflowReferencePoint = aLayer->getAirflowReferencePoint(0.5);
    EXPECT_NEAR(6912.8529362264735, airflowReferencePoint, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiation, IndoorHeatFlow)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation - Indoor Heat Flow");

    auto aSystem = GetSystem();

    auto convectiveHF = aSystem->getConvectiveHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    auto radiativeHF = aSystem->getRadiationHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    auto totalHF = aSystem->getHeatFlow(Tarcog::ISO15099::Environment::Indoor);

    EXPECT_NEAR(-57.440502097412043, convectiveHF, 1e-5);
    EXPECT_NEAR(-102.81512658453136, radiativeHF, 1e-5);
    EXPECT_NEAR(-160.25562868194339, totalHF, 1e-5);
}
