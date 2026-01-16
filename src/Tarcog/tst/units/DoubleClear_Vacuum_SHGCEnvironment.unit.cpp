#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

class TestDoubleClearVacuumSHGCEnvironment : public testing::Test
{
private:
    /////////////////////////////////////////////////////////
    /// Outdoor
    /////////////////////////////////////////////////////////
    const double airTemperature{305.15};   // Kelvins
    const double airSpeed{2.75};           // meters per second
    const double tSky{305.15};             // Kelvins
    const double solarRadiation{783.0};    // W/m2

    /////////////////////////////////////////////////////////
    /// Indoor
    /////////////////////////////////////////////////////////

    const double roomTemperature{297.15};   // Kelvins

    /////////////////////////////////////////////////////////
    /// IGU
    /////////////////////////////////////////////////////////
    const double solidLayerThickness{0.003048};   // [m]
    const double solidLayerConductance{1.0};      // [W/m2K]

    const double gapThickness{0.0127};   // [m]

    const double windowWidth{1.0};    // [m]
    const double windowHeight{1.0};   // [m]
    const double tilt{90.0};          // [deg]

    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        // Add support pillars
        constexpr auto pillarHeight = 0.0001;
        constexpr auto gapPressure = 0.1333;
        constexpr auto pillarConductivity = 999.0;
        constexpr auto pillarArea = 0.03 * 0.03;
        constexpr auto pillarRadius = 0.0002;

        Tarcog::ISO15099::CylindricalPillar pillar{
          pillarHeight, pillarConductivity, pillarArea, pillarRadius};

        auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight, tilt);
        aIGU.addLayers({aSolidLayer1, pillarGap, aSolidLayer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->setAbsorptances({0.096498351223209947, 0.072264770003798204});
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem & GetSystem() const
    {
        return *m_TarcogSystem;
    };
};

TEST_F(TestDoubleClearVacuumSHGCEnvironment, UValue)
{
    SCOPED_TRACE("Begin Test: Double Clear - U-value run");

    constexpr auto Tolerance = 1e-6;

    auto aSystem{GetSystem()};

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    constexpr auto aRun{Tarcog::ISO15099::System::Uvalue};

    Helper::testVectors("Temperature - UValue Run",
                        {304.151914, 304.089895, 300.134801, 300.072782},
                        aSystem.getTemperatures(aRun),
                        Tolerance);

    Helper::testVectors("Solid Conductivities - UValue Run",
                        {1.000000, 1.000000},
                        aSystem.getSolidEffectiveLayerConductivities(aRun),
                        Tolerance);

    Helper::testVectors("Gap Conductivities - UValue Run",
                        {0.000514},
                        aSystem.getGapEffectiveLayerConductivities(aRun),
                        Tolerance);

    Helper::testVectors("Radiosity - UValue Run",
                        {486.225721, 481.394417, 463.483318, 456.867517},
                        aSystem.getRadiosities(aRun),
                        Tolerance);

    EXPECT_NEAR(0.030907, aSystem.getEffectiveSystemConductivity(aRun), Tolerance);

    EXPECT_NEAR(0.006196, aSystem.thickness(aRun), Tolerance);

    EXPECT_EQ(1U, aSystem.getNumberOfIterations(aRun));
}

TEST_F(TestDoubleClearVacuumSHGCEnvironment, SHGC)
{
    SCOPED_TRACE("Begin Test: Double Clear - SHGC run");

    constexpr auto Tolerance = 1e-6;

    auto aSystem{GetSystem()};

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    constexpr auto aRun{Tarcog::ISO15099::System::SHGC};

    Helper::testVectors("Temperature - SHGC Run",
                        {308.259264, 308.338358, 306.138494, 306.016204},
                        aSystem.getTemperatures(aRun),
                        Tolerance);

    Helper::testVectors("Solid Conductivities - SHGC Run",
                        {1.0, 1.0},
                        aSystem.getSolidEffectiveLayerConductivities(aRun),
                        Tolerance);

    Helper::testVectors("Gap Conductivities - SHGC Run",
                        {0.000538},
                        aSystem.getGapEffectiveLayerConductivities(aRun),
                        Tolerance);

    Helper::testVectors("Radiosity - SHGC Run",
                        {508.691418, 510.476417, 499.998638, 488.381035},
                        aSystem.getRadiosities(aRun),
                        Tolerance);

    EXPECT_NEAR(0.188977, aSystem.getEffectiveSystemConductivity(aRun), Tolerance);
    EXPECT_NEAR(0.006196, aSystem.thickness(aRun), Tolerance);
    EXPECT_EQ(1u, aSystem.getNumberOfIterations(aRun));
}

TEST_F(TestDoubleClearVacuumSHGCEnvironment, SystemResults)
{
    SCOPED_TRACE("Begin Test: Double Clear - System results");

    constexpr auto Tolerance = 1e-6;

    auto aSystem{GetSystem()};

    EXPECT_NEAR(2.543436344624626, aSystem.getUValue(), Tolerance);

    constexpr auto Tsol{0.7033};
    EXPECT_NEAR(0.764686, aSystem.getSHGC(Tsol), Tolerance);

    EXPECT_NEAR(574.317448, aSystem.relativeHeatGain(Tsol), Tolerance);
}