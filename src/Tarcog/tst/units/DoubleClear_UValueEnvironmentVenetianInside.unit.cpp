#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

// Example of double clear window with inital guess for solution
class DoubleClear_UValueEnvironmentVenetianInside : public testing::Test
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
        const auto slatWidth{0.0508};      // m
        const auto slatSpacing{0.0432};    // m
        const auto slatTiltAngle{0.0};
        const auto curvatureRadius{0.0};   // m

        const auto frontOpenness{ThermalPermeability::Venetian::frontOpenness(
          slatTiltAngle, slatSpacing, matThickness, curvatureRadius, slatWidth)};

        const auto dl{0.0};
        const auto dr{0.0};
        const auto dtop{0.0};
        const auto dbot{0.0};

        EffectiveLayers::ShadeOpenness openness{frontOpenness, dl, dr, dtop, dbot};

        EffectiveLayers::EffectiveHorizontalVenetian effectiveVenetian{
          windowWidth, windowHeight, matThickness, openness, slatTiltAngle, slatWidth};

        // From window
        auto Ef = 0.518577;
        auto Eb = 0.518577;
        auto Tirf = 0.42419052;
        auto Tirb = 0.42419052;

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

    std::vector correctTemperature = {258.17414338086746,
                                      258.44349115078751,
                                      275.66951214625334,
                                      275.93885991617333,
                                      286.31519503530762,
                                      288.39614378854833};
    testVectors("Temperature - Uvalue run",
                correctTemperature,
                tarcogSystem.getTemperatures(aRun),
                Tolerance);

    std::vector correctSolidConductivities{1.0, 1.0, 0.068902110279170703};
    testVectors("Solid Conductivities - Uvalue run",
                correctSolidConductivities,
                tarcogSystem.getSolidEffectiveLayerConductivities(aRun),
                Tolerance);

    std::vector correctGapConductivities{0.065150412563458326, 0.28274394167757577};
    testVectors("Gap Conductivities - Uvalue run",
                correctGapConductivities,
                tarcogSystem.getGapEffectiveLayerConductivities(aRun),
                Tolerance);

    std::vector correctRadiosity = {250.03465513548156,
                                    263.21661641584001,
                                    317.15368152807775,
                                    339.64734212131299,
                                    397.07384888589741,
                                    371.75872636176621};
    testVectors(
      "Radiosity - Uvalue run", correctRadiosity, tarcogSystem.getRadiosities(aRun), Tolerance);

    auto effectiveSystemConductivity{tarcogSystem.getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.12831633907950135, effectiveSystemConductivity, Tolerance);

    auto thickness{tarcogSystem.thickness(aRun)};
    EXPECT_NEAR(0.059080400000000005, thickness, Tolerance);

    auto numOfIter = tarcogSystem.getNumberOfIterations(aRun);
    EXPECT_EQ(61u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    auto Uvalue = tarcogSystem.getUValue();
    EXPECT_NEAR(Uvalue, 2.2658638693694613, Tolerance);

    constexpr double Tsol{0.703296};
    auto SHGC = tarcogSystem.getSHGC(Tsol);
    EXPECT_NEAR(SHGC, 0.70329600000000003, Tolerance);

    auto relativeHeatGain = tarcogSystem.relativeHeatGain(Tsol);
    EXPECT_NEAR(527.63927883472888, relativeHeatGain, Tolerance);
}
