#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter : public testing::Test
{
private:
    std::unique_ptr<CSingleSystem> m_TarcogSystem;

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

    [[nodiscard]] CIGUSolidLayer * GetSolidLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[0].get();
    };

    [[nodiscard]] CIGUGapLayer * GetGap() const
    {
        return m_TarcogSystem->getGapLayers()[0].get();
    };

    [[nodiscard]] CIGUSolidLayer * GetShadeLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[1].get();
    };
};

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(23.541540, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, SolidTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Solid Temperatures");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(269.257146, frontTemperature, 1e-4);
    EXPECT_NEAR(271.441992, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, SolidRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Solid Radiosities");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(288.774350, frontRadiosity, 1e-4);
    EXPECT_NEAR(342.842260, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(271.441992, frontTemperature, 1e-4);
    EXPECT_NEAR(315.512561, backTemperature, 1e-4);
    EXPECT_NEAR(295.166009, layerTemperature, 1e-4);
    EXPECT_NEAR(293.477276, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, GapRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Gap Radiosities");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(342.842260, frontRadiosity, 1e-4);
    EXPECT_NEAR(526.815620, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, ShadeTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Shade Temperatures");

    auto aLayer = GetShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(315.512561, frontTemperature, 1e-4);
    EXPECT_NEAR(315.517885, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, ShadeRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Shade Radiosities");

    auto aLayer = GetShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(526.815620, frontRadiosity, 1e-4);
    EXPECT_NEAR(542.719352, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, IndoorHeatFlow)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Indoor Heat Flow");

    auto aSystem = GetSystem();

    auto convectiveHF = aSystem->getConvectiveHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    auto radiativeHF = aSystem->getRadiationHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    auto totalHF = aSystem->getHeatFlow(Tarcog::ISO15099::Environment::Indoor);

    EXPECT_NEAR(-54.740403, convectiveHF, 1e-5);
    EXPECT_NEAR(-100.677878, radiativeHF, 1e-5);
    EXPECT_NEAR(-155.418281, totalHF, 1e-5);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, IndoorValues)
{
    SCOPED_TRACE(
      "Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Indoor Values");

    auto aSystem = GetSystem();

    auto hInput = aSystem->getHInput(Tarcog::ISO15099::Environment::Indoor);
    auto hCoefficientModel = aSystem->getHCoefficientModel(Tarcog::ISO15099::Environment::Indoor);
    auto iRCalculatedOutside =
      aSystem->getIRCalculatedOutside(Tarcog::ISO15099::Environment::Indoor);

    EXPECT_NEAR(0, hInput, 1e-5);
    EXPECT_EQ(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH, hCoefficientModel);
    EXPECT_NEAR(0, iRCalculatedOutside, 1e-5);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInWinter, OutdoorValues)
{
    SCOPED_TRACE(
      "Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In Winter - Outdoor Values");

    auto aSystem = GetSystem();

    auto hInput = aSystem->getHInput(Tarcog::ISO15099::Environment::Outdoor);
    auto hCoefficientModel = aSystem->getHCoefficientModel(Tarcog::ISO15099::Environment::Outdoor);
    auto iRCalculatedOutside =
      aSystem->getIRCalculatedOutside(Tarcog::ISO15099::Environment::Outdoor);

    EXPECT_NEAR(0, hInput, 1e-5);
    EXPECT_EQ(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH, hCoefficientModel);
    EXPECT_NEAR(0, iRCalculatedOutside, 1e-5);
}