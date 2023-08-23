#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

// Example of double clear window with inital guess for solution
class TestDoubleClearUValueEnvironment : public testing::Test
{
private:
    /////////////////////////////////////////////////////////
    /// Outdoor
    /////////////////////////////////////////////////////////
    double airTemperature = 255.15;   // Kelvins
    double airSpeed = 5.5;            // meters per second
    double tSky = 255.15;             // Kelvins
    double solarRadiation = 789.0;

    /////////////////////////////////////////////////////////
    /// Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    /////////////////////////////////////////////////////////
    /// IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.003048;   // [m]
    double solidLayerConductance = 1.0;      // [W/m2K]

    double gapThickness = 0.0127;

    double windowWidth = 1.0;
    double windowHeight = 1.0;
    double tilt = 90.0;

    std::unique_ptr<Tarcog::ISO15099::CSystem> tarcogSystem;

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
        aSolidLayer1->setSolarAbsorptance(0.096498350373052627, solarRadiation);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer2->setSolarAbsorptance(0.072264769695765979, solarRadiation);

        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight, tilt);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        tarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }

public:
    Tarcog::ISO15099::CSystem & getSystem()
    {
        return *tarcogSystem;
    };
};

TEST_F(TestDoubleClearUValueEnvironment, StandardRun)
{
    SCOPED_TRACE("Begin Test: Double Clear");

    constexpr auto Tolerance = 1e-6;

    auto tarcogSystem = getSystem();

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    std::vector correctTemperature = {258.791640, 259.116115, 279.323983, 279.648458};
    testVectors("Temperature - Uvalue run",
                correctTemperature,
                tarcogSystem.getTemperatures(aRun),
                Tolerance);

    std::vector correctSolidConductivities{1.0, 1.0};
    testVectors("Solid Conductivities - Uvalue run",
                correctSolidConductivities,
                tarcogSystem.getSolidEffectiveLayerConductivities(aRun),
                Tolerance);

    std::vector correctGapConductivities{0.066904};
    testVectors("Gap Conductivities - Uvalue run",
                correctGapConductivities,
                tarcogSystem.getGapEffectiveLayerConductivities(aRun),
                Tolerance);

    std::vector correctRadiosity = {252.066216, 267.938384, 332.786197, 359.178924};
    testVectors(
      "Radiosity - Uvalue run", correctRadiosity, tarcogSystem.getRadiosities(aRun), Tolerance);

    auto effectiveSystemConductivity{tarcogSystem.getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.095937, effectiveSystemConductivity, Tolerance);

    auto thickness{tarcogSystem.thickness(aRun)};
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    auto numOfIter = tarcogSystem.getNumberOfIterations(aRun);
    EXPECT_EQ(24u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    aRun = Tarcog::ISO15099::System::SHGC;

    correctTemperature = {261.920381, 262.408833, 284.753241, 285.038766};
    testVectors(
      "Temperature - SHGC Run", correctTemperature, tarcogSystem.getTemperatures(aRun), Tolerance);

    correctSolidConductivities = {1.0, 1.0};
    testVectors("Solid Conductivities - SHGC Run",
                correctSolidConductivities,
                tarcogSystem.getSolidEffectiveLayerConductivities(aRun),
                Tolerance);

    correctGapConductivities = {0.069446};
    testVectors("Gap Conductivities - SHGC Run",
                correctGapConductivities,
                tarcogSystem.getGapEffectiveLayerConductivities(aRun),
                Tolerance);

    correctRadiosity = {262.585532, 283.163764, 358.428554, 382.293524};
    testVectors(
      "Radiosity - SHGC Run", correctRadiosity, tarcogSystem.getRadiosities(aRun), Tolerance);

    effectiveSystemConductivity = tarcogSystem.getEffectiveSystemConductivity(aRun);
    EXPECT_NEAR(0.052983, effectiveSystemConductivity, Tolerance);

    thickness = tarcogSystem.thickness(aRun);
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    numOfIter = tarcogSystem.getNumberOfIterations(aRun);
    EXPECT_EQ(25u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    auto Uvalue = tarcogSystem.getUValue();
    EXPECT_NEAR(Uvalue, 2.729619, Tolerance);

    constexpr double Tsol{0.703296};
    auto SHGC = tarcogSystem.getSHGC(Tsol);
    EXPECT_NEAR(SHGC, 0.755625, Tolerance);

    auto relativeHeatGain = tarcogSystem.relativeHeatGain(Tsol);
    EXPECT_NEAR(569.194784, relativeHeatGain, Tolerance);
}

TEST_F(TestDoubleClearUValueEnvironment, EnabledDeflection)
{
    SCOPED_TRACE("Begin Test: Double Clear Deflection");

    constexpr auto Tolerance = 1e-8;

    auto tarcogSystem = getSystem();

    constexpr auto initialTemperature{293.15};
    constexpr auto initialPressure{101325.0};
    tarcogSystem.setDeflectionProperties(initialTemperature, initialPressure);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    const std::vector<double> correctMaxLayersDeflection{-1.190817e-3, 1.190817e-3};
    testVectors("Max Layer Deflection",
                correctMaxLayersDeflection,
                tarcogSystem.getMaxLayerDeflections(aRun),
                Tolerance);

    const std::vector<double> correctMeanLayersDeflection{-0.498864e-3, 0.498864e-3};
    testVectors("Mean Layer Deflection",
                correctMeanLayersDeflection,
                tarcogSystem.getMeanLayerDeflections(aRun),
                Tolerance);

    const std::vector<double> correctPanesLoad{-55.34465653, 55.34465653};
    testVectors("Panes Load", correctPanesLoad, tarcogSystem.getPanesLoad(aRun), Tolerance);
}