#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

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
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    const auto MaxLayerDeflection = tarcogSystem.getMaxLayerDeflections();
    std::vector correctMaxLayerDeflection{-2.285680e-3, 0.48367382e-3};
    ASSERT_EQ(correctMaxLayerDeflection.size(), MaxLayerDeflection.size());

    for(auto i = 0u; i < correctMaxLayerDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMaxLayerDeflection[i], MaxLayerDeflection[i], 1e-8);
    }

    const auto MeanLayerDeflection = tarcogSystem.getMeanLayerDeflections();
    std::vector correctMeanLayerDeflection{-0.957652e-3, 0.202669e-3};
    ASSERT_EQ(correctMeanLayerDeflection.size(), MeanLayerDeflection.size());

    for(auto i = 0u; i < correctMaxLayerDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMeanLayerDeflection[i], MeanLayerDeflection[i], Tolerance);
    }

    const auto MaxGapDeflection = tarcogSystem.getMaxGapDeflections();
    std::vector correctMaxGapDeflection{9.930646e-3};
    ASSERT_EQ(correctMaxGapDeflection.size(), MaxGapDeflection.size());

    for(auto i = 0u; i < correctMaxGapDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMaxGapDeflection[i], MaxGapDeflection[i], 1e-8);
    }

    const auto MeanGapDeflection = tarcogSystem.getMeanGapDeflections();
    std::vector correctMeanGapDeflection{11.539845643422444e-3};
    ASSERT_EQ(correctMeanGapDeflection.size(), MeanGapDeflection.size());

    for(auto i = 0u; i < correctMeanGapDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMeanGapDeflection[i], MeanGapDeflection[i], 1e-8);
    }

    auto numOfIter = tarcogSystem.getNumberOfIterations();
    EXPECT_EQ(25u, numOfIter);
}
