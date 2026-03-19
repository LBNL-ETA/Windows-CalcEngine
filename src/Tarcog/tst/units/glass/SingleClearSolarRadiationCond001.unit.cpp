#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

class TestSingleClearSolarCond100 : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 300.0;   // Kelvins
        auto airSpeed = 5.5;           // meters per second
        auto tSky = 270.0;             // Kelvins
        auto solarRadiation = 1000.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        auto roomTemperature = 294.15;
        auto aIndoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(aIndoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 0.01;
        auto solarAbsorptance = 0.094189159572;

        auto aSolidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer->setSolarHeatGain(solarAbsorptance, solarRadiation);
        ASSERT_TRUE(aSolidLayer != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer);

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::unique_ptr<Tarcog::ISO15099::CSingleSystem>(
          new Tarcog::ISO15099::CSingleSystem(aIGU, aIndoor, Outdoor));
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSingleSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    }
};

TEST_F(TestSingleClearSolarCond100, TestTempAndRad)
{
    SCOPED_TRACE("Begin Test: Single Clear (Solar Radiation) - Temperatures and Radiosity.");

    constexpr auto Tolerance = 1e-6;

    const auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    const auto Temperature = aSystem->getTemperatures();
    const std::vector correctTemperature{299.465898, 300.261869};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    const auto Radiosity = aSystem->getRadiosities();
    const std::vector correctRadiosity{443.871080, 455.028489};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }
}

TEST_F(TestSingleClearSolarCond100, TestIndoor)
{
    SCOPED_TRACE("Begin Test: Single Clear (Solar Radiation) - Indoor heat flow.");

    constexpr auto Tolerance = 1e-6;

    const auto aSystem = GetSystem();

    const auto convectiveHF = aSystem->getConvectiveHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    const auto radiativeHF = aSystem->getRadiationHeatFlow(Tarcog::ISO15099::Environment::Indoor);
    const auto totalHF = aSystem->getHeatFlow(Tarcog::ISO15099::Environment::Indoor);

    EXPECT_NEAR(-13.913388, convectiveHF, Tolerance);
    EXPECT_NEAR(-30.569739, radiativeHF, Tolerance);
    EXPECT_NEAR(-44.483127, totalHF, Tolerance);
}
