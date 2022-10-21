#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

class TestSingleClear : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 300.0;   // Kelvins
        auto airSpeed = 5.5;           // meters per second
        auto tSky = 270.0;             // Kelvins
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
        // IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1.0;

        auto aSolidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aSolidLayer != nullptr);
        aSolidLayer->setSolarAbsorptance(0.094189159572, solarRadiation);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::unique_ptr<Tarcog::ISO15099::CSystem>(
          new Tarcog::ISO15099::CSystem(aIGU, Indoor, Outdoor));
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    }
};

TEST_F(TestSingleClear, Test1)
{
    SCOPED_TRACE("Begin Test: Single Clear - U-value");

    constexpr auto Tolerance = 1e-6;

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    /////////////////////////////////////////////////////////////////////////
    ///  U-value run
    /////////////////////////////////////////////////////////////////////////
    auto Temperature = aSystem->getTemperatures(Tarcog::ISO15099::System::Uvalue);
    std::vector correctTemperature{297.207035, 297.14470};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    auto Radiosity = aSystem->getRadiosities(Tarcog::ISO15099::System::Uvalue);
    std::vector correctRadiosity{432.444545, 439.201749};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    auto numOfIterations = aSystem->getNumberOfIterations(Tarcog::ISO15099::System::Uvalue);
    EXPECT_EQ(24u, numOfIterations);

    /////////////////////////////////////////////////////////////////////////
    ///  SHGC run
    /////////////////////////////////////////////////////////////////////////
    Temperature = aSystem->getTemperatures(Tarcog::ISO15099::System::SHGC);
    correctTemperature = {299.116601, 299.121730};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    Radiosity = aSystem->getRadiosities(Tarcog::ISO15099::System::SHGC);
    correctRadiosity = {442.087153, 449.182158};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    numOfIterations = aSystem->getNumberOfIterations(Tarcog::ISO15099::System::SHGC);
    EXPECT_EQ(23u, numOfIterations);

    /////////////////////////////////////////////////////////////////////////
    ///  Heat flows
    /////////////////////////////////////////////////////////////////////////
    auto heatFlow =
      aSystem->getHeatFlow(Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(heatFlow, -20.450949, Tolerance);

    heatFlow = aSystem->getHeatFlow(Tarcog::ISO15099::System::Uvalue,
                                    Tarcog::ISO15099::Environment::Outdoor);
    EXPECT_NEAR(heatFlow, -20.450949, Tolerance);

    heatFlow =
      aSystem->getHeatFlow(Tarcog::ISO15099::System::SHGC, Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(heatFlow, -35.474877, Tolerance);

    heatFlow =
      aSystem->getHeatFlow(Tarcog::ISO15099::System::SHGC, Tarcog::ISO15099::Environment::Outdoor);
    EXPECT_NEAR(heatFlow, 38.840370, Tolerance);

    /////////////////////////////////////////////////////////////////////////
    //  System properties
    /////////////////////////////////////////////////////////////////////////
    auto UValue = aSystem->getUValue();
    EXPECT_NEAR(UValue, 5.493806, Tolerance);

    constexpr auto totalSolarTransmittance = 0.831249;

    auto SHGC = aSystem->getSHGC(totalSolarTransmittance);
    EXPECT_NEAR(SHGC, 0.850291, Tolerance);
}
