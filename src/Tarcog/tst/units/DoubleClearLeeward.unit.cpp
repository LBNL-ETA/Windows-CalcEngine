#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

// Example of double clear window with inital guess for solution
class TestDoubleClearLeeward : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 255.15;   // Kelvins
        auto airSpeed = 5.5;            // meters per second
        auto pressure = 101325.0;
        auto airDirection = Tarcog::ISO15099::AirHorizontalDirection::Leeward;
        auto tSky = 255.15;   // Kelvins
        auto solarRadiation = 789.0;

        auto Outdoor =
          Tarcog::ISO15099::Environments::outdoor(airTemperature,
                                                  airSpeed,
                                                  solarRadiation,
                                                  tSky,
                                                  Tarcog::ISO15099::SkyModel::AllSpecified,
                                                  pressure,
                                                  airDirection);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.005715;   // [m]
        auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        layer1->setSolarHeatGain(0.166707709432, solarRadiation);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        layer2->setSolarHeatGain(0.112737670541, solarRadiation);

        auto gapThickness = 0.012;
        auto gap = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap, layer2});

        // Alternative way of adding layers
        // aIGU.addLayer(layer1);
        // aIGU.addLayer(gap);
        // aIGU.addLayer(layer2);

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    std::shared_ptr<Tarcog::ISO15099::CSystem> GetSystem() const
    {
        return m_TarcogSystem;
    };
};

TEST_F(TestDoubleClearLeeward, Test1)
{
    SCOPED_TRACE("Begin Test: Double Clear - Surface temperatures");

    constexpr double Tolerance = 1e-6;

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    auto Temperature = aSystem->getTemperatures(aRun);
    std::vector<double> correctTemperature = {258.756688, 259.359226, 279.178508, 279.781047};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    auto Radiosity = aSystem->getRadiosities(aRun);
    std::vector<double> correctRadiosity = {251.950834, 268.667346, 332.299341, 359.731704};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());
    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    auto numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(25u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    aRun = Tarcog::ISO15099::System::SHGC;

    Temperature = aSystem->getTemperatures(aRun);
    correctTemperature = {264.022835, 265.134421, 287.947302, 288.428859};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    Radiosity = aSystem->getRadiosities(aRun);
    correctRadiosity = {269.869356, 295.289318, 374.655898, 397.518721};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(25u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    auto Uvalue = aSystem->getUValue();
    EXPECT_NEAR(Uvalue, 2.703359, Tolerance);

    auto SHGC = aSystem->getSHGC(0.606897);
    EXPECT_NEAR(SHGC, 0.690096, Tolerance);
}
