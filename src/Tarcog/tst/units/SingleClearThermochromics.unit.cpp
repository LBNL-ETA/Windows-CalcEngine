#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

class TestSingleClearThermochromics : public testing::Test
{
    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 300;   // Kelvins
        auto airSpeed = 5.5;         // meters per second
        auto airDirection = Tarcog::ISO15099::AirHorizontalDirection::Windward;
        auto tSky = 270;   // Kelvins
        auto solarRadiation = 789;

        std::shared_ptr<Tarcog::ISO15099::CEnvironment> Outdoor =
          std::make_shared<Tarcog::ISO15099::COutdoorEnvironment>(
            airTemperature,
            airSpeed,
            solarRadiation,
            airDirection,
            tSky,
            Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;
        std::shared_ptr<Tarcog::ISO15099::CEnvironment> Indoor =
          std::make_shared<Tarcog::ISO15099::CIndoorEnvironment>(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1;

        auto transmittance = 0;
        auto emissivity = 0.84;

        // Thermochromics property of the surface emissivity
        std::vector<std::pair<double, double>> emissivities = {
          {288.15, 0.84}, {293.15, 0.74}, {296.15, 0.64}, {300.15, 0.54}, {303.15, 0.44}};

        std::vector<std::pair<double, double>> transmittances = {
          {288.15, 0}, {293.15, 0}, {303.15, 0}};

        // Creates thermochromic surface on indoor side with variable emissivity and constant
        // transmittance
        auto frontSurface = std::make_shared<Tarcog::ISO15099::ISurface>(emissivity, transmittance);
        auto backSurface =
          std::make_shared<Tarcog::ISO15099::ISurface>(emissivities, transmittances);

        auto aSolidLayer = std::make_shared<Tarcog::ISO15099::CIGUSolidLayer>(
          solidLayerThickness, solidLayerConductance, frontSurface, backSurface);
        ASSERT_TRUE(aSolidLayer != nullptr);
        aSolidLayer->setSolarAbsorptance(0.094189159572, solarRadiation);

        const auto windowWidth = 1.0;
        const auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer);

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    Tarcog::ISO15099::CSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    };
};

TEST_F(TestSingleClearThermochromics, Test1)
{
    SCOPED_TRACE("Begin Test: Single Clear Thermochromics - U-value");

    constexpr auto Tolerance{1e-6};

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    /////////////////////////////////////////////////////////////////////////
    ///  U-value run
    /////////////////////////////////////////////////////////////////////////
    auto aSolidLayers = aSystem->getSolidLayers(Tarcog::ISO15099::System::Uvalue);

    auto aLayer = *aSolidLayers[0];

    auto emissivity = aLayer.getSurface(Side::Back)->getEmissivity();
    EXPECT_NEAR(emissivity, 0.613717, Tolerance);

    auto Temperature = aSystem->getTemperatures(Tarcog::ISO15099::System::Uvalue);
    std::vector<double> correctTemperature{297.207035, 297.144700};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    auto Radiosity = aSystem->getRadiosities(Tarcog::ISO15099::System::Uvalue);
    std::vector<double> correctRadiosity{432.444545, 439.201750};
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
    aSolidLayers = aSystem->getSolidLayers(Tarcog::ISO15099::System::SHGC);

    aLayer = *aSolidLayers[0];

    emissivity = aLayer.getSurface(Side::Back)->getEmissivity();
    EXPECT_NEAR(emissivity, 0.567105, Tolerance);

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
    ///  System properties
    /////////////////////////////////////////////////////////////////////////
    auto UValue = aSystem->getUValue();
    EXPECT_NEAR(UValue, 5.493806, Tolerance);

    auto SHGC = aSystem->getSHGC(0.831249);
    EXPECT_NEAR(SHGC, 0.850291, Tolerance);
}
