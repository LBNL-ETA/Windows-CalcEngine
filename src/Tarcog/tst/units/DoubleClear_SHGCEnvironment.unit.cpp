#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

class TestDoubleClearSHGCEnvironment : public testing::Test
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

        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight, tilt);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->setAbsorptances({0.096489921212, 0.072256758809});
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem & GetSystem() const
    {
        return *m_TarcogSystem;
    };
};

TEST_F(TestDoubleClearSHGCEnvironment, UValue)
{
    SCOPED_TRACE("Begin Test: Double Clear - U-value run");

    constexpr auto Tolerance = 1e-6;

    auto aSystem{GetSystem()};

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    auto Temperature = aSystem.getTemperatures(aRun);
    std::vector correctTemperature = {304.025047, 303.955156, 300.484758, 300.414867};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    auto SolidLayerConductivities = aSystem.getSolidEffectiveLayerConductivities(aRun);
    std::vector<double> correctSolidConductivities{1, 1};
    EXPECT_EQ(SolidLayerConductivities.size(), correctSolidConductivities.size());

    for(auto i = 0u; i < SolidLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctSolidConductivities[i], SolidLayerConductivities[i], Tolerance);
    }

    auto GapLayerConductivities = aSystem.getGapEffectiveLayerConductivities(aRun);
    std::vector<double> correctGapConductivities{0.083913};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    auto Radiosity = aSystem.getRadiosities(aRun);
    std::vector<double> correctRadiosity = {485.546128, 480.950664, 465.217923, 458.631346};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());
    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    auto effectiveSystemConductivity{aSystem.getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.119383, effectiveSystemConductivity, Tolerance);

    auto thickness{aSystem.thickness(aRun)};
    EXPECT_NEAR(0.018796, thickness, Tolerance);

    auto numOfIter = aSystem.getNumberOfIterations(aRun);
    EXPECT_EQ(1u, numOfIter);
}

TEST_F(TestDoubleClearSHGCEnvironment, SHGC)
{
    SCOPED_TRACE("Begin Test: Double Clear - SHGC run");

    constexpr auto Tolerance = 1e-6;

    auto aSystem{GetSystem()};

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    const auto aRun{Tarcog::ISO15099::System::SHGC};

    const std::vector correctTemperature{308.185604, 308.260088, 306.318284, 306.191403};
    testVectors("Temperature - SHGC Run", correctTemperature, aSystem.getTemperatures(aRun), Tolerance);

    const std::vector correctSolidLayerConductivities{1.0, 1.0};
    testVectors("Solid Conductivities - SHGC Run",
                correctSolidLayerConductivities,
                aSystem.getSolidEffectiveLayerConductivities(aRun),
                Tolerance);

    const std::vector correctGapConductivities{0.087241};
    testVectors("Gap Conductivities - SHGC Run",
                correctGapConductivities,
                aSystem.getGapEffectiveLayerConductivities(aRun),
                Tolerance);

    const std::vector correctRadiosity{508.280530, 510.189512, 500.936293, 489.338313};
    testVectors("Radiosity - SHGC Run", correctRadiosity, aSystem.getRadiosities(aRun), Tolerance);

    EXPECT_NEAR(0.658981, aSystem.getEffectiveSystemConductivity(aRun), Tolerance);
    EXPECT_NEAR(0.018796, aSystem.thickness(aRun), Tolerance);
    EXPECT_EQ(1u, aSystem.getNumberOfIterations(aRun));
}

TEST_F(TestDoubleClearSHGCEnvironment, SystemResults)
{
    SCOPED_TRACE("Begin Test: Double Clear - System results");

    constexpr auto Tolerance = 1e-6;

    auto aSystem{GetSystem()};

    EXPECT_NEAR(2.866261, aSystem.getUValue(), Tolerance);

    constexpr auto Tsol{0.703296};
    EXPECT_NEAR(0.763304, aSystem.getSHGC(Tsol), Tolerance);

    EXPECT_NEAR(575.826177, aSystem.relativeHeatGain(Tsol), Tolerance);
}

TEST_F(TestDoubleClearSHGCEnvironment, DeflectionUValue)
{
    SCOPED_TRACE("Begin Test: Double Clear Deflection - UValue");

    constexpr auto Tolerance = 1e-8;

    auto tarcogSystem{GetSystem()};

    constexpr auto initialTemperature{293.15};
    constexpr auto initialPressure{101325.0};
    tarcogSystem.setDeflectionProperties(initialTemperature, initialPressure);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    const std::vector correctMaxLayersDeflection{0.448510e-3, -0.448510e-3};
    testVectors("Max Layer Deflection",
                correctMaxLayersDeflection,
                tarcogSystem.getMaxLayerDeflections(aRun),
                Tolerance);

    const std::vector correctMeanLayersDeflection{0.187892e-3, -0.187892e-3};
    testVectors("Mean Layer Deflection",
                correctMeanLayersDeflection,
                tarcogSystem.getMeanLayerDeflections(aRun),
                Tolerance);

    const std::vector correctPanesLoad{20.52839045, -20.52839045};
    testVectors("Panes Load", correctPanesLoad, tarcogSystem.getPanesLoad(aRun), Tolerance);

    const std::vector correctMaxGapWidth{13.597019e-3};
    testVectors("Max Gap Width",
                correctMaxGapWidth, tarcogSystem.getMaxGapWidth(aRun),
                Tolerance);

    const std::vector correctMeanGapWidth{13.075785e-3};
    testVectors("Mean Gap Width",
                correctMeanGapWidth, tarcogSystem.getMeanGapWidth(aRun),
                Tolerance);
}

TEST_F(TestDoubleClearSHGCEnvironment, DeflectionSHGC)
{
    SCOPED_TRACE("Begin Test: Double Clear Deflection - SHGC");

    constexpr auto Tolerance = 1e-8;

    auto tarcogSystem{GetSystem()};

    constexpr auto initialTemperature{293.15};
    constexpr auto initialPressure{101325.0};
    tarcogSystem.setDeflectionProperties(initialTemperature, initialPressure);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::SHGC;

    const std::vector correctMaxLayersDeflection{0.696246e-3, -0.696246e-3};
    testVectors("Max Layer Deflection",
                correctMaxLayersDeflection,
                tarcogSystem.getMaxLayerDeflections(aRun),
                Tolerance);

    const std::vector correctMeanLayersDeflection{0.291675e-3, -0.291675e-3};
    testVectors("Mean Layer Deflection",
                correctMeanLayersDeflection,
                tarcogSystem.getMeanLayerDeflections(aRun),
                Tolerance);

    const std::vector correctPanesLoad{32.14789366, -32.14789366};
    testVectors("Panes Load", correctPanesLoad, tarcogSystem.getPanesLoad(aRun), Tolerance);

    const std::vector correctMaxGapWidth{14.092491e-3};
    testVectors("Max Gap Width",
                correctMaxGapWidth, tarcogSystem.getMaxGapWidth(aRun),
                Tolerance);

    const std::vector correctMeanGapWidth{13.283351e-3};
    testVectors("Mean Gap Width",
                correctMeanGapWidth, tarcogSystem.getMeanGapWidth(aRun),
                Tolerance);
}