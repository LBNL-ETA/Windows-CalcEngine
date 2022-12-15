#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

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
        auto solidLayerThickness = 0.005715;   // [m]
        auto solidLayerConductance = 1.0;
        auto solidLayerSolarAbsorptance = 0.094189159572;
        auto solidLayerFrontEmissivity = 0.84;
        auto solidLayerFrontIRTransmittance = 0.0;
        auto solidLayerBackEmissivity = 0.84;
        auto solidLayerBackIRTransmittance = 0.0;
        // reflectance = 1 - emissivity - transmittance

        auto solidLayer = Tarcog::ISO15099::Layers::solid(solidLayerThickness,
                                                          solidLayerConductance,
                                                          solidLayerFrontEmissivity,
                                                          solidLayerFrontIRTransmittance,
                                                          solidLayerBackEmissivity,
                                                          solidLayerBackIRTransmittance);
        solidLayer->setSolarAbsorptance(solidLayerSolarAbsorptance, solarRadiation);
        ASSERT_TRUE(solidLayer != nullptr);

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;
        auto shadeLayerFrontEmissivity = 0.84;
        auto shadeLayerFrontTransmittance = 0.0;
        auto shadeLayerBackEmissivity = 0.84;
        auto shadeLayerBackTransmittance = 0.0;
        // reflectance = 1 - emissivity - transmittance
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

        auto shadeLayer = Tarcog::ISO15099::Layers::shading(shadeLayerThickness,
                                                            shadeLayerConductance,
                                                            effOpenness,
                                                            shadeLayerFrontEmissivity,
                                                            shadeLayerFrontTransmittance,
                                                            shadeLayerBackEmissivity,
                                                            shadeLayerBackTransmittance);
        shadeLayer->setSolarAbsorptance(shadeLayerSolarAbsorptance, solarRadiation);

        ASSERT_TRUE(shadeLayer != nullptr);

        auto gapThickness = 0.0127;
        auto gapAirSpeed = 0.5;
        auto gap = Tarcog::ISO15099::Layers::forcedVentilationGap(gapThickness, gapAirSpeed);
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

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(-130.5048438198269, gainEnergy, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, SolidTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Solid Temperatures");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(307.64940595087643, frontTemperature, 1e-4);
    EXPECT_NEAR(308.49154217411314, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, SolidRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Solid Radiosities");

    auto aLayer = GetSolidLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(485.19373063498108, frontRadiosity, 1e-4);
    EXPECT_NEAR(534.49572555881934, backRadiosity, 1e-4);
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
    EXPECT_NEAR(308.49154217411314, frontTemperature, 1e-4);
    EXPECT_NEAR(329.18614190431651, backTemperature, 1e-4);
    EXPECT_NEAR(308.5243189875772, layerTemperature, 1e-4);
    EXPECT_NEAR(318.8388420392148, averageTemperature, 1e-4);
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
    EXPECT_NEAR(534.49572555881934, frontRadiosity, 1e-4);
    EXPECT_NEAR(644.76873455415853, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, ShadeTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Shade Temperatures");

    auto aLayer = GetShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->getTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->getTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(329.18614190431651, frontTemperature, 1e-4);
    EXPECT_NEAR(329.18394285290708, backTemperature, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, ShadeRadiosities)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Shade Radiosities");

    auto aLayer = GetShadeLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontRadiosity = aLayer->J(FenestrationCommon::Side::Front);
    auto backRadiosity = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(644.76873455415853, frontRadiosity, 1e-4);
    EXPECT_NEAR(629.96111066455899, backRadiosity, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, AirflowReferencePoint)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Airflow Reference Point");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto airflowReferencePoint = aLayer->getAirflowReferencePoint(0.5);
    EXPECT_NEAR(6913.4164769308836, airflowReferencePoint, 1e-4);
}

TEST_F(TestGapLayerAtEdgeForcedVentilationWithSolarRadiationInSummer, IndoorHeatFlow)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge With Solar Radiation In "
                 "Summer - Indoor Heat Flow");

    auto aSystem = GetSystem();

    auto convectiveHF = aSystem->getConvectiveHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    auto radiativeHF = aSystem->getRadiationHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    auto totalHF = aSystem->getHeatFlow(Tarcog::ISO15099::Environment::Indoor);

    EXPECT_NEAR(-109.36378592796353, convectiveHF, 1e-5);
    EXPECT_NEAR(-187.91963696055234, radiativeHF, 1e-5);
    EXPECT_NEAR(-297.28342288851587, totalHF, 1e-5);
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