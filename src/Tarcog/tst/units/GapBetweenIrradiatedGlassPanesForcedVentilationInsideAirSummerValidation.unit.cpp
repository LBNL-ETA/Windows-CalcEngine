#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapBetweenIrradiatedGlassPanesForcedVentilationInsideAirSummerValidation : public testing::Test
{
private:
    std::unique_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////
        auto outdoorAirTemperature = 303.15;       // Kelvins
        auto outdoorAirSpeed = 2.75;                // meters per second
        auto tSky = 303.15;                 // Kelvins
        auto solarRadiation = 1000.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          outdoorAirTemperature, outdoorAirSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto indoorAirTemperature = 298.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(indoorAirTemperature);

        auto convective_heat_transfer_coefficient_indoor = 3.9; // + radiative_heat_transfer_coefficient (4.1 W/(m²K) from EN 410) = 8 W/(m²K)
        Indoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH, convective_heat_transfer_coefficient_indoor);


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
        solidLayer1->setSolarAbsorptance(0.2, solarRadiation);
        ASSERT_TRUE(solidLayer1 != nullptr);

        auto gapThickness = 0.01;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);
        auto gapAirSpeed = 1;
        auto forcedGapLayer =
          Tarcog::ISO15099::Layers::forcedVentilationGap(gapLayer, gapAirSpeed, indoorAirTemperature);
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
        solidLayer2->setSolarAbsorptance(0.2, solarRadiation);
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
};

TEST_F(TestGapBetweenIrradiatedGlassPanesForcedVentilationInsideAirSummerValidation, GapLayerSurfaceIRFlow)
{
    SCOPED_TRACE("Begin Test: Test gap layer surface temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Front);
    auto backIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(300.51850897417813, frontIRRadiationFlow, 1e-4);
    EXPECT_NEAR(352.30917088728245, backIRRadiationFlow, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesForcedVentilationInsideAirSummerValidation, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(123.35122562642526, gainEnergy, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesForcedVentilationInsideAirSummerValidation, FirstLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(263.04855139463018, frontTemperature, 1e-4);
    EXPECT_NEAR(263.33651241783423, backTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesForcedVentilationInsideAirSummerValidation, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->layerTemperature();
    auto averageTemperature = aLayer->averageTemperature();
    EXPECT_NEAR(263.33651241783423, frontTemperature, 1e-4);
    EXPECT_NEAR(282.70879216106016, backTemperature, 1e-4);
    EXPECT_NEAR(285.74858839456721, layerTemperature, 1e-4);
    EXPECT_NEAR(273.02265228944719, averageTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassPanesForcedVentilationInsideAirSummerValidation, SecondLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Shade Temperatures");

    auto aLayer = GetSecondLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(282.70879216106016, frontTemperature, 1e-4);
    EXPECT_NEAR(283.10709823028276, backTemperature, 1e-4);
}