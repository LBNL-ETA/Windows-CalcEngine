#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

class TestDoubleClearDeflectionWithLoad : public testing::Test
{};

TEST_F(TestDoubleClearDeflectionWithLoad, U_ValueRun)
{
    SCOPED_TRACE("Begin Test: Double Clear - Deflection case with loads");

    /////////////////////////////////////////////////////////
    /// Outdoor
    /////////////////////////////////////////////////////////
    constexpr auto airTemperature{300};   // Kelvins
    constexpr auto airSpeed{5.5};         // meters per second
    constexpr auto tSky{255.15};          // Kelvins
    constexpr auto solarRadiation{0.0};
    constexpr auto outsidePressure{102000};

    auto Outdoor = Tarcog::ISO15099::Environments::outdoor(airTemperature,
                                                           airSpeed,
                                                           solarRadiation,
                                                           tSky,
                                                           Tarcog::ISO15099::SkyModel::AllSpecified,
                                                           outsidePressure);
    ASSERT_TRUE(Outdoor != nullptr);
    Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

    /////////////////////////////////////////////////////////
    /// Indoor
    /////////////////////////////////////////////////////////

    constexpr auto roomTemperature{250.0};
    constexpr auto roomPressure{100000};

    auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature, roomPressure);
    ASSERT_TRUE(Indoor != nullptr);

    /////////////////////////////////////////////////////////
    /// IGU
    /////////////////////////////////////////////////////////
    constexpr auto solidLayerThickness{0.003048};   // [m]
    constexpr auto solidLayerConductance{1.0};      // [W/m2K]

    auto aSolidLayer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
    aSolidLayer1->setSolarAbsorptance(0.099839858711, solarRadiation);

    auto aSolidLayer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
    aSolidLayer2->setSolarAbsorptance(0.076627746224, solarRadiation);

    constexpr auto gapThickness1{0.006};
    const auto gapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness1);
    ASSERT_TRUE(gapLayer1 != nullptr);

    constexpr auto windowWidth{1.0};
    constexpr auto windowHeight{5.0};
    Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
    aIGU.addLayers({aSolidLayer1, gapLayer1, aSolidLayer2});

    /////////////////////////////////////////////////////////
    /// System
    /////////////////////////////////////////////////////////
    Tarcog::ISO15099::CSystem tarcogSystem{aIGU, Indoor, Outdoor};
    tarcogSystem.setAppliedLoad({2000, 1000});
    tarcogSystem.setDeflectionProperties(320, 102000);

    constexpr auto Tolerance{1e-6};

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    constexpr auto aRun = Tarcog::ISO15099::System::Uvalue;

    const std::vector correctTemperature{292.076977, 291.610008, 272.796958, 272.329989};
    testVectors(
      "U-value temperatures", correctTemperature, tarcogSystem.getTemperatures(aRun), Tolerance);

    const std::vector correctDeflection{-55.488195e-3, -54.644421e-3};
    testVectors("U-value deflection",
                correctDeflection,
                tarcogSystem.getMaxLayerDeflections(Tarcog::ISO15099::System::Uvalue),
                Tolerance);
}
