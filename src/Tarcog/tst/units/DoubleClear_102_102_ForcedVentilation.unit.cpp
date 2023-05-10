#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

[[nodiscard]] std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment>
  nfrcWinterEnvironmentConditions()
{
    constexpr auto airTemperature = 255.15;   // Kelvins
    constexpr auto airSpeed = 5.5;            // meters per second
    constexpr auto tSky = 255.15;             // Kelvins
    constexpr auto solarRadiation = 0.0;

    auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
      airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
    Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);
    return Outdoor;
}

[[nodiscard]] std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment>
  nfrcSummerEnvironmentConditions()
{
    constexpr auto airTemperature = 305.15;   // Kelvins
    constexpr auto airSpeed = 2.75;           // meters per second
    constexpr auto tSky = 305.15;             // Kelvins
    constexpr auto solarRadiation = 783.0;

    auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
      airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
    Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);
    return Outdoor;
}

[[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> nfrcWinterRoomConditions()
{
    constexpr auto roomTemperature = 294.15;   // Kelvins
    constexpr auto roomPressure = 101325;      // Pascals

    auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature, roomPressure);
    return Indoor;
}

[[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> nfrcSummerRoomConditions()
{
    constexpr auto roomTemperature = 297.15;   // Kelvins
    return Tarcog::ISO15099::Environments::indoor(roomTemperature);
}

[[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer> createSolidLayer102()
{
    constexpr auto solidLayerThickness = 0.003048;   // [m]
    constexpr auto solidLayerConductance = 1.0;      // [W/m2K]

    return Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
}

[[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> createForcedVentilationGap()
{
    constexpr auto gapThickness = 0.0127;          // [m]
    auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
    constexpr auto gapAirSpeed = 1;                // [m/s]
    constexpr auto inletAirTemperature = 294.15;   // [K]
    return Tarcog::ISO15099::Layers::forcedVentilationGap(
      gapLayer, gapAirSpeed, inletAirTemperature);
}

class TestDoubleClear102_102_ForcedVentilation : public testing::Test
{
};

TEST_F(TestDoubleClear102_102_ForcedVentilation, WinterSystem)
{
    constexpr auto Tolerance = 1e-6;

    auto Outdoor = nfrcWinterEnvironmentConditions();
    auto Indoor = nfrcWinterRoomConditions();

    /////////////////////////////////////////////////////////
    /// IGU
    /////////////////////////////////////////////////////////

    auto aSolidLayer1{createSolidLayer102()};
    auto aSolidLayer2{createSolidLayer102()};
    auto forcedGapLayer{createForcedVentilationGap()};

    constexpr auto windowWidth = 1.0;    // [m]
    constexpr auto windowHeight = 1.0;   // [m]
    Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
    aIGU.addLayers({aSolidLayer1, forcedGapLayer, aSolidLayer2});

    /////////////////////////////////////////////////////////
    /// System
    /////////////////////////////////////////////////////////
    auto aSystem = std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    ASSERT_TRUE(aSystem != nullptr);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    auto Temperature = aSystem->getTemperatures(aRun);
    std::vector correctTemperature = {263.818291, 264.593199, 285.521498, 285.704784};
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
    std::vector correctGapConductivities{0.095385};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    auto radiosity = aSystem->getRadiosities(aRun);
    std::vector correctRadiosity = {269.153038, 291.534322, 363.160726, 385.242148};
    ASSERT_EQ(correctRadiosity.size(), radiosity.size());
    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], radiosity[i], Tolerance);
    }

    auto effectiveSystemConductivity{aSystem->getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.051642, effectiveSystemConductivity, Tolerance);

    auto thickness{aSystem->thickness(aRun)};
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    auto numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(35u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    aRun = Tarcog::ISO15099::System::SHGC;

    Temperature = aSystem->getTemperatures(aRun);
    correctTemperature = {263.818291, 264.593199, 285.521498, 285.704784};
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
    correctGapConductivities = {0.095385};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    radiosity = aSystem->getRadiosities(aRun);
    correctRadiosity = {269.153038, 291.534322, 363.160726, 385.242148};
    ASSERT_EQ(correctRadiosity.size(), radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], radiosity[i], Tolerance);
    }

    effectiveSystemConductivity = aSystem->getEffectiveSystemConductivity(aRun);
    EXPECT_NEAR(0.051642, effectiveSystemConductivity, Tolerance);

    thickness = aSystem->thickness(aRun);
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(35u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    auto Uvalue = aSystem->getUValue();
    EXPECT_NEAR(Uvalue, 1.541882, Tolerance);

    auto SHGC = aSystem->getSHGC(0.703296);
    EXPECT_NEAR(SHGC, 0, Tolerance);

    auto relativeHeatGain = aSystem->relativeHeatGain(0.703296);
    EXPECT_NEAR(relativeHeatGain, 11.995845, Tolerance);
}

TEST_F(TestDoubleClear102_102_ForcedVentilation, SummerSystem)
{
    constexpr auto Tolerance = 1e-6;

    auto Outdoor = nfrcSummerEnvironmentConditions();
    auto Indoor = nfrcSummerRoomConditions();

    /////////////////////////////////////////////////////////
    /// IGU
    /////////////////////////////////////////////////////////

    constexpr auto nfrcSolarRadiation{783.0};                          // [W/m2]
    constexpr auto solarAbsorptanceCoefficientLayer1{0.096489921212};   // [-]
    auto aSolidLayer1{createSolidLayer102()};
    aSolidLayer1->setSolarAbsorptance(solarAbsorptanceCoefficientLayer1, nfrcSolarRadiation);

    constexpr auto solarAbsorptanceCoefficientLayer2{0.072256758809};   // [-]
    auto aSolidLayer2{createSolidLayer102()};
    aSolidLayer2->setSolarAbsorptance(solarAbsorptanceCoefficientLayer2, nfrcSolarRadiation);
    auto forcedGapLayer{createForcedVentilationGap()};

    constexpr auto windowWidth = 1.0;    // [m]
    constexpr auto windowHeight = 1.0;   // [m]
    Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
    aIGU.addLayers({aSolidLayer1, forcedGapLayer, aSolidLayer2});

    /////////////////////////////////////////////////////////
    /// System
    /////////////////////////////////////////////////////////
    auto aSystem = std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    ASSERT_TRUE(aSystem != nullptr);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    auto Temperature = aSystem->getTemperatures(aRun);
    std::vector correctTemperature = {302.086918, 301.897092, 297.931895, 297.917002};
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
    std::vector correctGapConductivities{0.107560};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    auto radiosity = aSystem->getRadiosities(aRun);
    std::vector correctRadiosity = {475.269393, 467.626523, 450.058720, 445.890087};
    ASSERT_EQ(correctRadiosity.size(), radiosity.size());
    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], radiosity[i], Tolerance);
    }

    auto effectiveSystemConductivity{aSystem->getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.022025, effectiveSystemConductivity, Tolerance);

    auto thickness{aSystem->thickness(aRun)};
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    auto numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(53u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    aRun = Tarcog::ISO15099::System::SHGC;

    Temperature = aSystem->getTemperatures(aRun);
    correctTemperature = {305.253340, 305.144629, 302.096850, 302.073888};
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
    correctGapConductivities = {0.109772};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    radiosity = aSystem->getRadiosities(aRun);
    correctRadiosity = {492.161590, 488.898798, 474.889057, 467.271297};
    ASSERT_EQ(correctRadiosity.size(), radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], radiosity[i], Tolerance);
    }

    effectiveSystemConductivity = aSystem->getEffectiveSystemConductivity(aRun);
    EXPECT_NEAR(0.211770, effectiveSystemConductivity, Tolerance);

    thickness = aSystem->thickness(aRun);
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(50u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    auto Uvalue = aSystem->getUValue();
    EXPECT_NEAR(Uvalue, 0.610775, Tolerance);

    auto SHGC = aSystem->getSHGC(0.703296);
    EXPECT_NEAR(SHGC, 0.742805, Tolerance);

    auto relativeHeatGain = aSystem->relativeHeatGain(0.703296);
    EXPECT_NEAR(relativeHeatGain, 543.413812, Tolerance);
}