#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer : public testing::Test
{
private:
    std::unique_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 305.15;   // Kelvins
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 255.15;             // Kelvins
        auto solarRadiation = 783.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 297.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        // IGU
        auto solidLayer1Thickness = 0.005715;   // [m]
        auto solidLayer1Conductance = 1.0;
        auto solidLayer1SolarAbsorptance = 0.094189159572;
        auto solidLayer1FrontEmissivity = 0.84;
        auto solidLayer1FrontIRTransmittance = 0.0;
        auto solidLayer1BackEmissivity = 0.84;
        auto solidLayer1BackIRTransmittance = 0.0;

        auto solidLayer = Tarcog::ISO15099::Layers::solid(solidLayer1Thickness,
                                                          solidLayer1Conductance,
                                                          solidLayer1FrontEmissivity,
                                                          solidLayer1FrontIRTransmittance,
                                                          solidLayer1BackEmissivity,
                                                          solidLayer1BackIRTransmittance);
        solidLayer->setSolarAbsorptance(solidLayer1SolarAbsorptance, solarRadiation);
        ASSERT_TRUE(solidLayer != nullptr);

        const auto solidLayer2Thickness = 0.01;
        const auto solidLayer2Conductance = 160.0;
        const auto solidLayer2FrontEmissivity = 0.84;
        const auto solidLayer2FrontTransmittance = 0.0;
        const auto solidLayer2BackEmissivity = 0.84;
        const auto solidLayer2BackTransmittance = 0.0;
        const auto solidLayer2SolarAbsorptance = 0.687443971634;

        auto shadeLayer = Tarcog::ISO15099::Layers::solid(solidLayer2Thickness,
                                                          solidLayer2Conductance,
                                                          solidLayer2FrontEmissivity,
                                                          solidLayer2FrontTransmittance,
                                                          solidLayer2BackEmissivity,
                                                          solidLayer2BackTransmittance);
        shadeLayer->setSolarAbsorptance(solidLayer2SolarAbsorptance, solarRadiation);

        ASSERT_TRUE(shadeLayer != nullptr);

        const auto gapThickness = 0.0127;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);

        auto gapAirSpeed = 0.5;
        auto gap =
          Tarcog::ISO15099::Layers::forcedVentilationGap(gapLayer, gapAirSpeed, roomTemperature);
        ASSERT_TRUE(gap != nullptr);

        const auto windowWidth{1.0};
        const auto windowHeight{1.0};

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({solidLayer, gap, shadeLayer});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] CSingleSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    };

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

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(-130.503973, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, SolidTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Solid Temperatures");

    auto aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(307.649406, frontTemperature, 1e-4);
    EXPECT_NEAR(308.491542, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, SolidRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Solid Radiosities");

    auto aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(485.193494, frontRadiosity, 1e-4);
    EXPECT_NEAR(534.495170, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(308.491542, frontTemperature, 1e-4);
    EXPECT_NEAR(329.185903, backTemperature, 1e-4);
    EXPECT_NEAR(308.524319, layerTemperature, 1e-4);
    EXPECT_NEAR(318.838697, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, GapRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Gap Radiosities");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(534.495170, frontRadiosity, 1e-4);
    EXPECT_NEAR(644.767023, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, ShadeTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Shade Temperatures");

    auto aLayer = GetSecondLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(329.185903, frontTemperature, 1e-4);
    EXPECT_NEAR(329.184144, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, ShadeRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Shade Radiosities");

    auto aLayer = GetSecondLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(644.767023, frontRadiosity, 1e-4);
    EXPECT_NEAR(629.962476, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, IndoorHeatFlow)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Indoor Heat Flow");

    auto aSystem = GetSystem();

    auto convectiveHF = aSystem->getConvectiveHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    auto radiativeHF = aSystem->getRadiationHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    auto totalHF = aSystem->getHeatFlow(Tarcog::ISO15099::Environment::Indoor);

    EXPECT_NEAR(-109.364638, convectiveHF, 1e-5);
    EXPECT_NEAR(-187.921002, radiativeHF, 1e-5);
    EXPECT_NEAR(-297.285640, totalHF, 1e-5);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, IndoorValues)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Indoor Values");

    auto aSystem = GetSystem();

    auto hInput = aSystem->getHInput(Tarcog::ISO15099::Environment::Indoor);
    auto hCoefficientModel = aSystem->getHCoefficientModel(Tarcog::ISO15099::Environment::Indoor);
    auto iRCalculatedOutside =
      aSystem->getIRCalculatedOutside(Tarcog::ISO15099::Environment::Indoor);

    EXPECT_NEAR(0, hInput, 1e-5);
    EXPECT_EQ(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH, hCoefficientModel);
    EXPECT_NEAR(0, iRCalculatedOutside, 1e-5);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, OutdoorValues)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Outdoor Values");

    auto aSystem = GetSystem();

    auto hInput = aSystem->getHInput(Tarcog::ISO15099::Environment::Outdoor);
    auto hCoefficientModel = aSystem->getHCoefficientModel(Tarcog::ISO15099::Environment::Outdoor);
    auto iRCalculatedOutside =
      aSystem->getIRCalculatedOutside(Tarcog::ISO15099::Environment::Outdoor);

    EXPECT_NEAR(0, hInput, 1e-5);
    EXPECT_EQ(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH, hCoefficientModel);
    EXPECT_NEAR(0, iRCalculatedOutside, 1e-5);
}