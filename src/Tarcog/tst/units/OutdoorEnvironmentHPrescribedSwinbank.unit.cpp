#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestOutdoorEnvironmentHPrescribedSwingbank : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CEnvironment> Outdoor;
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 300.0;   // Kelvins
        auto airSpeed = 5.5;           // meters per second
        auto tSky = 270.0;             // Kelvins
        auto solarRadiation = 0.0;
        auto hout = 20.0;

        Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::Swinbank);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::HPrescribed, hout);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        auto roomTemperature = 294.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 100.0;

        auto aSolidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aSolidLayer != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        m_TarcogSystem->solve();
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CEnvironment> GetOutdoors() const
    {
        return Outdoor;
    }
};

TEST_F(TestOutdoorEnvironmentHPrescribedSwingbank, HPrescribed_Swinbank)
{
    SCOPED_TRACE("Begin Test: Outdoors -> H model = Prescribed; Sky Model = Swinbank");

    constexpr auto Tolerance = 1e-6;

    const auto aOutdoor = GetOutdoors();
    ASSERT_TRUE(aOutdoor != nullptr);

    const auto radiosity = aOutdoor->getEnvironmentIR();
    EXPECT_NEAR(459.2457, radiosity, Tolerance);

    const auto hc = aOutdoor->getHc();
    EXPECT_NEAR(14.895502, hc, Tolerance);

    const auto outIR = aOutdoor->getRadiationFlow();
    EXPECT_NEAR(-7.777658, outIR, Tolerance);

    const auto outConvection = aOutdoor->getConvectionConductionFlow();
    EXPECT_NEAR(-22.696081, outConvection, Tolerance);

    const auto totalHeatFlow = aOutdoor->getHeatFlow();
    EXPECT_NEAR(-30.473739, totalHeatFlow, Tolerance);
}
