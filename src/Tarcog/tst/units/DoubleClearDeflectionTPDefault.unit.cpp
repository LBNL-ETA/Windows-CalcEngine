#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

class DoubleClearDeflectionTPDefault : public testing::Test
{};

TEST_F(DoubleClearDeflectionTPDefault, U_ValueRun)
{
    SCOPED_TRACE(
      "Begin Test: Double Clear - Calculated Deflection with temperature and pressure inputs");

    /////////////////////////////////////////////////////////
    /// Outdoor
    /////////////////////////////////////////////////////////
    auto airTemperature = 255.15;   // Kelvins
    auto airSpeed = 5.5;            // meters per second
    auto tSky = 255.15;             // Kelvins
    auto solarRadiation = 0.0;

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
    auto solidLayerThickness1 = 0.003048;   // [m]
    auto solidLayerThickness2 = 0.005715;
    auto solidLayerConductance = 1.0;

    auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);

    auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance);

    auto gapThickness = 0.0127;
    auto gap = Tarcog::ISO15099::Layers::gap(gapThickness);
    ASSERT_TRUE(gap != nullptr);

    double windowWidth = 1;
    double windowHeight = 1;
    Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
    aIGU.addLayers({layer1, gap, layer2});

    // Deflection properties
    auto Tini = 303.15;
    auto Pini = 101325.0;
    aIGU.setDeflectionProperties(Tini, Pini);

    /////////////////////////////////////////////////////////
    /// System
    /////////////////////////////////////////////////////////
    Tarcog::ISO15099::CSingleSystem tarcogSystem{aIGU, Indoor, Outdoor};

    tarcogSystem.solve();

    constexpr double Tolerance = 1e-6;

    auto Temperature = tarcogSystem.getTemperatures();
    std::vector correctTemperature = {258.795861, 259.120713, 279.023351, 279.632449};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    auto Radiosity = tarcogSystem.getRadiosities();
    std::vector correctRadiosity = {252.080153, 267.749407, 331.509846, 359.112232};
    Helper::testVectors("Radiosity", correctRadiosity, Radiosity, Tolerance);

    const auto MaxLayerDeflection = tarcogSystem.getMaxLayerDeflections();
    std::vector correctMaxLayerDeflection{-2.285680e-3, 0.48367382e-3};
    Helper::testVectors(
      "Max layer deflection", correctMaxLayerDeflection, MaxLayerDeflection, 1e-8);

    const auto MeanLayerDeflection = tarcogSystem.getMeanLayerDeflections();
    std::vector correctMeanLayerDeflection{-0.957652e-3, 0.202669e-3};
    Helper::testVectors(
      "Mean layer deflection", correctMeanLayerDeflection, MeanLayerDeflection, Tolerance);

    const auto MaxGapDeflection = tarcogSystem.getMaxGapWidth();
    std::vector correctMaxGapDeflection{9.930646e-3};
    Helper::testVectors("Maximum gap deflection", correctMaxGapDeflection, MaxGapDeflection, 1e-8);

    const auto MeanGapDeflection = tarcogSystem.getMeanGapWidth();
    std::vector correctMeanGapDeflection{11.539846e-3};
    Helper::testVectors(
      "Mean gap deflection", correctMeanGapDeflection, MeanGapDeflection, Tolerance);

    const auto GapPressure = tarcogSystem.getGapPressures();
    std::vector correctGapPressure{98976.323821};
    Helper::testVectors("Gaps pressure", correctGapPressure, GapPressure, 1e-6);

    const auto loads = tarcogSystem.getPanesLoad();
    std::vector correctLoads{-143.261021, 143.261021};
    Helper::testVectors("Panes load", correctLoads, loads, 1e-6);

    auto numOfIter = tarcogSystem.getNumberOfIterations();
    EXPECT_EQ(25u, numOfIter);
}

TEST_F(DoubleClearDeflectionTPDefault, SHGC_ValueRun)
{
    SCOPED_TRACE(
      "Begin Test: Double Clear - Calculated Deflection with temperature and pressure inputs");

    /////////////////////////////////////////////////////////
    /// Outdoor
    /////////////////////////////////////////////////////////
    auto airTemperature = 305.15;   // Kelvins
    auto airSpeed = 2.75;           // meters per second
    auto tSky = 305.15;             // Kelvins
    auto solarRadiation = 783.0;

    auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
      airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
    ASSERT_TRUE(Outdoor != nullptr);
    Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

    /////////////////////////////////////////////////////////
    /// Indoor
    /////////////////////////////////////////////////////////

    auto roomTemperature = 297.15;

    auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
    ASSERT_TRUE(Indoor != nullptr);

    /////////////////////////////////////////////////////////
    /// IGU
    /////////////////////////////////////////////////////////
    auto solidLayerThickness1 = 0.003048;   // [m]
    auto solidLayerThickness2 = 0.005715;
    auto solidLayerConductance = 1.0;

    auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);
    layer1->setSolarHeatGain(0.096058987081, solarRadiation);

    auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance);
    layer2->setSolarHeatGain(0.126891940832, solarRadiation);

    auto gapThickness = 0.0127;
    auto gap = Tarcog::ISO15099::Layers::gap(gapThickness);
    ASSERT_TRUE(gap != nullptr);

    double windowWidth = 1;
    double windowHeight = 1;
    Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
    aIGU.addLayers({layer1, gap, layer2});

    // Deflection properties
    auto Tini = 303.15;
    auto Pini = 101325.0;
    aIGU.setDeflectionProperties(Tini, Pini);

    /////////////////////////////////////////////////////////
    /// System
    /////////////////////////////////////////////////////////
    Tarcog::ISO15099::CSingleSystem tarcogSystem{aIGU, Indoor, Outdoor};

    tarcogSystem.solve();

    constexpr double Tolerance = 1e-6;

    auto Temperature = tarcogSystem.getTemperatures();
    std::vector correctTemperature = {308.954315, 309.077572, 309.485922, 309.218194};
    Helper::testVectors("Surface temperatures", correctTemperature, Temperature, Tolerance);

    auto Radiosity = tarcogSystem.getRadiosities();
    std::vector correctRadiosity = {512.583061, 517.782219, 519.766194, 506.137762};
    Helper::testVectors("Radiosity", correctRadiosity, Radiosity, Tolerance);

    const auto MaxLayerDeflection = tarcogSystem.getMaxLayerDeflections();
    std::vector correctMaxLayerDeflection{0.505696e-3, -0.080251e-03};
    Helper::testVectors(
      "Max layer deflection", correctMaxLayerDeflection, MaxLayerDeflection, 1e-8);

    const auto MeanLayerDeflection = tarcogSystem.getMeanLayerDeflections();
    std::vector correctMeanLayerDeflection{0.211849e-3, -0.033619e-03};
    Helper::testVectors(
      "Mean layer deflection", correctMeanLayerDeflection, MeanLayerDeflection, Tolerance);

    const auto MaxGapDeflection = tarcogSystem.getMaxGapWidth();
    std::vector correctMaxGapDeflection{0.013286};
    Helper::testVectors(
      "Maximum gap deflection", correctMaxGapDeflection, MaxGapDeflection, Tolerance);

    const auto MeanGapDeflection = tarcogSystem.getMeanGapWidth();
    std::vector correctMeanGapDeflection{0.012945};
    Helper::testVectors(
      "Mean gap deflection", correctMeanGapDeflection, MeanGapDeflection, Tolerance);

    const auto GapPressure = tarcogSystem.getGapPressures();
    std::vector correctGapPressure{101414.318183};
    Helper::testVectors("Gaps pressure", correctGapPressure, GapPressure, Tolerance);

    const auto loads = tarcogSystem.getPanesLoad();
    std::vector correctLoads{23.210613, -23.210613};
    Helper::testVectors("Panes load", correctLoads, loads, Tolerance);

    auto numOfIter = tarcogSystem.getNumberOfIterations();
    EXPECT_EQ(26u, numOfIter);
}