#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapBetweenDarkGlassPanesNoVentilationWinterValidation : public testing::Test
{
private:
    std::unique_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////

        auto outdoorAirTemperature = 273.15;   // Kelvins
        auto outdoorAirSpeed = 2.75;           // meters per second
        auto tSky = 273.15;                    // Kelvins
        auto solarRadiation = 0.0;
        auto Outdoor =
          Tarcog::ISO15099::Environments::outdoor(outdoorAirTemperature,
                                                  outdoorAirSpeed,
                                                  solarRadiation,
                                                  tSky,
                                                  Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto indoorAirTemperature = 298.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(indoorAirTemperature);
        auto convective_heat_transfer_coefficient_indoor =
          3.9;   // + radiative_heat_transfer_coefficient (4.1 W/(m²K) from EN 410) = 8 W/(m²K)
                 // The convective_heat_transfer_coefficient_indoor is NOT USED if
                 // Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH. It is used in the
                 // cases `::HPrescribed` and `::HcPrescribed`
        Indoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                               convective_heat_transfer_coefficient_indoor);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;

        const auto solidLayer1Thickness = 0.004;   // [m]
        const auto solidLayer1Conductance = 1.0;
        auto emissivity1 = 0.84;
        auto transmittance1 = 0;
        auto solidLayer1 = Tarcog::ISO15099::Layers::solid(solidLayer1Thickness,
                                                           solidLayer1Conductance,
                                                           emissivity1,
                                                           transmittance1,
                                                           emissivity1,
                                                           transmittance1);
        solidLayer1->setSolarHeatGain(0.2, solarRadiation);
        ASSERT_TRUE(solidLayer1 != nullptr);

        auto gapThickness = 0.01;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);
        auto gapAirSpeed = 0;
        auto forcedGapLayer = Tarcog::ISO15099::Layers::forcedVentilationGap(
          gapLayer, gapAirSpeed, indoorAirTemperature);
        ASSERT_TRUE(forcedGapLayer != nullptr);

        const auto solidLayer2Thickness = 0.004;   // [m]
        const auto solidLayer2Conductance = 1.0;
        auto emissivity2 = 0.84;
        auto transmittance2 = 0;
        auto solidLayer2 = Tarcog::ISO15099::Layers::solid(solidLayer2Thickness,
                                                           solidLayer2Conductance,
                                                           emissivity2,
                                                           transmittance2,
                                                           emissivity2,
                                                           transmittance2);
        solidLayer2->setSolarHeatGain(0.2, solarRadiation);
        ASSERT_TRUE(solidLayer2 != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({solidLayer1, forcedGapLayer, solidLayer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////

        m_TarcogSystem = std::make_unique<CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
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

    [[nodiscard]] double GetOutdoorConvectiveHeatTransferCoefficient() const
    {
        return m_TarcogSystem->CSingleSystem::getHc(Tarcog::ISO15099::Environment::Outdoor);
    };

    [[nodiscard]] double GetOutdoorRadiativeHeatTransferCoefficient() const
    {
        return m_TarcogSystem->CSingleSystem::getHr(Tarcog::ISO15099::Environment::Outdoor);
    };

    [[nodiscard]] double GetOutdoorHeatTransferCoefficient() const
    {
        return m_TarcogSystem->CSingleSystem::getH(Tarcog::ISO15099::Environment::Outdoor);
    };

    [[nodiscard]] double GetIndoorConvectiveHeatTransferCoefficient() const
    {
        return m_TarcogSystem->CSingleSystem::getHc(Tarcog::ISO15099::Environment::Indoor);
    };

    [[nodiscard]] double GetIndoorRadiativeHeatTransferCoefficient() const
    {
        return m_TarcogSystem->CSingleSystem::getHr(Tarcog::ISO15099::Environment::Indoor);
    };

    [[nodiscard]] double GetIndoorHeatTransferCoefficient() const
    {
        return m_TarcogSystem->CSingleSystem::getH(Tarcog::ISO15099::Environment::Indoor);
    };
};

TEST_F(TestGapBetweenDarkGlassPanesNoVentilationWinterValidation, GapLayerSurfaceIRFlow)
{
    SCOPED_TRACE("Begin Test: Test gap layer surface temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Front);
    auto backIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(342.30366908251926, frontIRRadiationFlow, 1e-4);
    EXPECT_NEAR(384.12123352518552, backIRRadiationFlow, 1e-4);
}

TEST_F(TestGapBetweenDarkGlassPanesNoVentilationWinterValidation, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(0, gainEnergy, 1e-4);
}

TEST_F(TestGapBetweenDarkGlassPanesNoVentilationWinterValidation, FirstLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getSurfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getSurfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(276.83340353098015, frontTemperature, 1e-4);
    EXPECT_NEAR(277.11277736659969, backTemperature, 1e-4);
}

TEST_F(TestGapBetweenDarkGlassPanesNoVentilationWinterValidation, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getSurfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getSurfaceTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->averageLayerTemperature();
    auto averageTemperature = aLayer->averageSurfaceTemperature();
    EXPECT_NEAR(277.11277736659969, frontTemperature, 1e-4);
    EXPECT_NEAR(288.37356601094808, backTemperature, 1e-4);
    EXPECT_NEAR(282.74317168877388, layerTemperature, 1e-4);
    EXPECT_NEAR(282.74317168877388, averageTemperature, 1e-4);
}

TEST_F(TestGapBetweenDarkGlassPanesNoVentilationWinterValidation, SecondLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Shade Temperatures");

    auto aLayer = GetSecondLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getSurfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getSurfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(288.37356601094808, frontTemperature, 1e-4);
    EXPECT_NEAR(288.65293984656751, backTemperature, 1e-4);
}

TEST_F(TestGapBetweenDarkGlassPanesNoVentilationWinterValidation, HeatTransferCoefficients)
{
    SCOPED_TRACE("Begin Test: Test The Heat Transfer Coefficients");

    auto outdoorConvectiveHeatTransferCoefficient = GetOutdoorConvectiveHeatTransferCoefficient();
    auto outdoorRadiativeHeatTransferCoefficient = GetOutdoorRadiativeHeatTransferCoefficient();
    auto outdoorHeatTransferCoefficient = GetOutdoorHeatTransferCoefficient();
    auto indoorConvectiveHeatTransferCoefficient = GetIndoorConvectiveHeatTransferCoefficient();
    auto indoorRadiativeHeatTransferCoefficient = GetIndoorRadiativeHeatTransferCoefficient();
    auto indoorHeatTransferCoefficient = GetIndoorHeatTransferCoefficient();

    EXPECT_NEAR(15, outdoorConvectiveHeatTransferCoefficient, 1e-4);
    EXPECT_NEAR(3.9616636645399086, outdoorRadiativeHeatTransferCoefficient, 1e-4);
    EXPECT_NEAR(18.961663664539909, outdoorHeatTransferCoefficient, 1e-4);
    EXPECT_NEAR(2.541395138863741, indoorConvectiveHeatTransferCoefficient, 1e-4);
    EXPECT_NEAR(4.8128237212777485, indoorRadiativeHeatTransferCoefficient, 1e-4);
    EXPECT_NEAR(7.35421886014149, indoorHeatTransferCoefficient, 1e-4);
}