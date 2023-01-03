#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapLayerAtEdgeForcedVentilationWithZeroAirSpeed : public testing::Test
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

        auto gapAirSpeed = 0.0;
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
    [[nodiscard]] CIGUSolidLayer * GetSolidLayer1() const
    {
        return m_TarcogSystem->getSolidLayers()[0].get();
    };

    [[nodiscard]] CIGUGapLayer * GetGap() const
    {
        return m_TarcogSystem->getGapLayers()[0].get();
    };

    [[nodiscard]] CIGUSolidLayer * GetSolidLayer2() const
    {
        return m_TarcogSystem->getSolidLayers()[1].get();
    };
};

TEST_F(TestGapLayerAtEdgeForcedVentilationWithZeroAirSpeed, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer With Zero Air Speed - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(0.0, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithZeroAirSpeed, SolidLayer1Temperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer With Zero Air Speed - Solid layer 1 Temperatures");

    auto aLayer = GetSolidLayer1();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(259.139571, frontTemperature, 1e-4);
    EXPECT_NEAR(259.806240, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithZeroAirSpeed, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer With Zero Air Speed - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(259.806240, frontTemperature, 1e-4);
    EXPECT_NEAR(281.591652, backTemperature, 1e-4);
    EXPECT_NEAR(270.698946, layerTemperature, 1e-4);
    EXPECT_NEAR(270.698946, averageTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithZeroAirSpeed, SolidLayer2Temperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer With Zero Air Speed - Solid layer 2 Temperatures");

    auto aLayer = GetSolidLayer2();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(281.591652, frontTemperature, 1e-4);
    EXPECT_NEAR(281.598943, backTemperature, 1e-4);
}
