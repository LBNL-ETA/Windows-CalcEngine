#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

// Example of double clear window with inital guess for solution
class TestDoubleClearUValueEnvironment : public testing::Test
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
        auto tSky = 255.15;             // Kelvins
        auto solarRadiation = 789.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
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
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1.0;      // [W/m2K]

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer1->setSolarAbsorptance(0.096489921212, solarRadiation);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer2->setSolarAbsorptance(0.072256758809, solarRadiation);

        auto gapThickness = 0.0127;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        // Alternative way of adding layers.
        // aIGU.addLayer(aSolidLayer1);
        // aIGU.addLayer(gapLayer);
        // aIGU.addLayer(aSolidLayer2);

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CSystem> GetSystem() const
    {
        return m_TarcogSystem;
    }
};

TEST_F(TestDoubleClearUValueEnvironment, Test1)
{
    SCOPED_TRACE("Begin Test: Double Clear - Surface temperatures");

    constexpr auto Tolerance = 1e-6;

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    auto Temperature = aSystem->getTemperatures(aRun);
    std::vector correctTemperature = {258.791640, 259.116115, 279.323983, 279.648458};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    auto SolidLayerConductivities = aSystem->getSolidEffectiveLayerConductivities(aRun);
    std::vector correctSolidConductivities{1.0, 1.0};
    EXPECT_EQ(SolidLayerConductivities.size(), correctSolidConductivities.size());

    for(auto i = 0u; i < SolidLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctSolidConductivities[i], SolidLayerConductivities[i], Tolerance);
    }

    auto GapLayerConductivities = aSystem->getGapEffectiveLayerConductivities(aRun);
    std::vector correctGapConductivities{0.066904};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    auto radiosity = aSystem->getRadiosities(aRun);
    std::vector correctRadiosity = {252.066216, 267.938384, 332.786197, 359.178924};
    ASSERT_EQ(correctRadiosity.size(), radiosity.size());
    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], radiosity[i], Tolerance);
    }

    auto effectiveSystemConductivity{aSystem->getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.095937, effectiveSystemConductivity, Tolerance);

    auto thickness{aSystem->thickness(aRun)};
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    auto numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(20u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    aRun = Tarcog::ISO15099::System::SHGC;

    Temperature = aSystem->getTemperatures(aRun);
    correctTemperature = {261.920088, 262.408524, 284.752662, 285.038190};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    SolidLayerConductivities = aSystem->getSolidEffectiveLayerConductivities(aRun);
    correctSolidConductivities = {1.0, 1.0};
    EXPECT_EQ(SolidLayerConductivities.size(), correctSolidConductivities.size());

    for(auto i = 0u; i < SolidLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctSolidConductivities[i], SolidLayerConductivities[i], Tolerance);
    }

    GapLayerConductivities = aSystem->getGapEffectiveLayerConductivities(aRun);
    correctGapConductivities = {0.069446};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    radiosity = aSystem->getRadiosities(aRun);
    correctRadiosity = {262.584530, 283.162254, 358.425764, 382.290985};
    ASSERT_EQ(correctRadiosity.size(), radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], radiosity[i], Tolerance);
    }

    effectiveSystemConductivity = aSystem->getEffectiveSystemConductivity(aRun);
    EXPECT_NEAR(0.052988, effectiveSystemConductivity, Tolerance);

    thickness = aSystem->thickness(aRun);
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(20u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    auto Uvalue = aSystem->getUValue();
    EXPECT_NEAR(Uvalue, 2.729619, Tolerance);

    auto SHGC = aSystem->getSHGC(0.703296);
    EXPECT_NEAR(SHGC, 0.755619, Tolerance);

    auto relativeHeatGain = aSystem->relativeHeatGain(0.703296);
    EXPECT_NEAR(relativeHeatGain, 569.190777, Tolerance);
}
