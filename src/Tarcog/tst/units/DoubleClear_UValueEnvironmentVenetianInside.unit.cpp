#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

class DoubleClear_UValueEnvironmentVenetianInside : public testing::Test
{
private:
    /////////////////////////////////////////////////////////
    /// Outdoor
    /////////////////////////////////////////////////////////
    double airTemperature = 255.15;   // Kelvins
    double airSpeed = 5.5;            // meters per second
    double tSky = 255.15;             // Kelvins
    double solarRadiation = 0.0;

    /////////////////////////////////////////////////////////
    /// Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    /////////////////////////////////////////////////////////
    /// IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.003048;   // [m]
    double solidLayerConductance = 1.0;      // [W/m2K]
    double shadeLayerConductance = 0.12;

    double gapThickness = 0.0127;
    double shadeGapThickness = 0.0381;

    double windowWidth = 1.0;
    double windowHeight = 1.0;
    double tilt = 90.0;

    std::unique_ptr<Tarcog::ISO15099::CSystem> system;

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

        // Venetian
        const auto matThickness{0.0029};   // m
        const FenestrationCommon::Venetian::Geometry venetianGeometry{0.0508, 0.012, 0.0, 0.0};

        EffectiveLayers::EffectiveHorizontalVenetian effectiveVenetian{
          matThickness, venetianGeometry};

        // From window
        auto Ef = 0.762609422207;
        auto Eb = 0.762609422207;
        auto Tirf = 0.224916040897;
        auto Tirb = 0.224916040897;

        auto venetianLayer =
          Tarcog::ISO15099::Layers::shading(effectiveVenetian.effectiveThickness(),
                                            shadeLayerConductance,
                                            effectiveVenetian.getEffectiveOpenness(),
                                            Ef,
                                            Tirf,
                                            Eb,
                                            Tirb);

        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);

        auto gapLayerShade = Tarcog::ISO15099::Layers::gap(shadeGapThickness);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight, tilt);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2, gapLayerShade, venetianLayer});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        system = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }

public:
    Tarcog::ISO15099::CSystem & getSystem()
    {
        return *system;
    };
};

TEST_F(DoubleClear_UValueEnvironmentVenetianInside, StandardRun)
{
    SCOPED_TRACE("Begin Test: Double Clear");

    constexpr auto Tolerance = 1e-6;

    auto tarcogSystem = getSystem();

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    std::vector correctTemperature = {258.04303308701242,
                                      258.30068157152357,
                                      274.87318943566243,
                                      275.13083792017358,
                                      285.74307808549537,
                                      287.8240450610885};
    Helper::testVectors("Temperature - Uvalue run",
                        correctTemperature,
                        tarcogSystem.getTemperatures(aRun),
                        Tolerance);

    std::vector correctSolidConductivities{1.0, 1.0, 0.064723381535680649};
    Helper::testVectors("Solid Conductivities - Uvalue run",
                        correctSolidConductivities,
                        tarcogSystem.getSolidEffectiveLayerConductivities(aRun),
                        Tolerance);

    std::vector correctGapConductivities{0.064778086753446609, 0.26242346851182713};
    Helper::testVectors("Gap Conductivities - Uvalue run",
                        correctGapConductivities,
                        tarcogSystem.getGapEffectiveLayerConductivities(aRun),
                        Tolerance);

    std::vector correctRadiosity = {249.60517454445562,
                                    262.2154667364166,
                                    313.8292357698283,
                                    334.95846689103632,
                                    387.89324931163679,
                                    377.3686622916473};
    Helper::testVectors(
      "Radiosity - Uvalue run", correctRadiosity, tarcogSystem.getRadiosities(aRun), Tolerance);

    auto effectiveSystemConductivity{tarcogSystem.getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.12232020362599783, effectiveSystemConductivity, Tolerance);

    auto thickness{tarcogSystem.thickness(aRun)};
    EXPECT_NEAR(0.0590804, thickness, Tolerance);

    auto numOfIter = tarcogSystem.getNumberOfIterations(aRun);
    EXPECT_EQ(65u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    auto Uvalue = tarcogSystem.getUValue();
    EXPECT_NEAR(Uvalue, 2.167445, Tolerance);

    constexpr double Tsol{0.703296};
    auto SHGC = tarcogSystem.getSHGC(Tsol);
    EXPECT_NEAR(SHGC, 0.0, Tolerance);

    auto relativeHeatGain = tarcogSystem.relativeHeatGain(Tsol);
    EXPECT_NEAR(16.86272, relativeHeatGain, Tolerance);
}
