#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapBetweenIndoorOutdoorAndVeryThickGlass : public testing::Test
{
private:
    std::unique_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // This test is used to determine the external heat transfer coefficient between a very thick glass pane and exterior and the internal heat transfer coefficient between a very thick glass pane and the interior.
        /////////////////////////////////////////////////////////


        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////

        auto outdoorAirTemperature = 273.15;       // Kelvins
        auto outdoorAirSpeed = 2.75;                // meters per second
        auto tSky = 273.15;                 // Kelvins
        auto solarRadiation = 0.0;
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

        const auto solidLayer1Thickness = 1.0;   // [m]
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

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({solidLayer1});

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
};

TEST_F(TestGapBetweenIndoorOutdoorAndVeryThickGlass, FirstLayerSurfaceTemperatures)
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