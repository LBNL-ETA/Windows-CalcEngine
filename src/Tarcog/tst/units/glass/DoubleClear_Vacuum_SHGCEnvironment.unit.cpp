#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

namespace
{
    Tarcog::ISO15099::CSystem makeDoubleClearSystem()
    {
        // Outdoor
        constexpr double airTemperature{305.15};
        constexpr double airSpeed{2.75};
        constexpr double tSky{305.15};
        constexpr double solarRadiation{783.0};

        // Indoor
        constexpr double roomTemperature{297.15};

        // IGU
        constexpr double solidLayerThickness{0.003048};
        constexpr double solidLayerConductance{1.0};
        constexpr double windowWidth{1.0};
        constexpr double windowHeight{1.0};
        constexpr double tilt{90.0};

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        // Pillar
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

        Tarcog::ISO15099::CSystem sys(aIGU, Indoor, Outdoor);
        sys.setAbsorptances({0.096498351223209947, 0.072264770003798204});
        return sys;
    }
}   // namespace

TEST(DoubleClearVacuumSHGCEnvironment, UValue)
{
    SCOPED_TRACE("Begin Test: Double Clear - U-value run");
    constexpr auto Tolerance = 1e-6;

    auto aSystem = makeDoubleClearSystem();

    constexpr auto aRun{Tarcog::ISO15099::System::Uvalue};

    Helper::testVectors("Temperature - UValue Run",
                        {304.150642, 304.088544, 300.138331, 300.076232},
                        aSystem.getTemperatures(aRun),
                        Tolerance);

    Helper::testVectors("Solid Conductivities - UValue Run",
                        {1.000000, 1.000000},
                        aSystem.getSolidEffectiveLayerConductivities(aRun),
                        Tolerance);

    Helper::testVectors("Gap Conductivities - UValue Run",
                        {0.000516},
                        aSystem.getGapEffectiveLayerConductivities(aRun),
                        Tolerance);

    Helper::testVectors("Radiosity - UValue Run",
                        {486.218900, 481.389973, 463.500787, 456.885279},
                        aSystem.getRadiosities(aRun),
                        Tolerance);

    EXPECT_NEAR(0.030982, aSystem.getEffectiveSystemConductivity(aRun), Tolerance);
    EXPECT_NEAR(0.006196, aSystem.thickness(aRun), Tolerance);
    EXPECT_EQ(1U, aSystem.getNumberOfIterations(aRun));
}

TEST(DoubleClearVacuumSHGCEnvironment, SHGC)
{
    SCOPED_TRACE("Begin Test: Double Clear - SHGC run");
    constexpr auto Tolerance = 1e-6;

    auto aSystem = makeDoubleClearSystem();

    constexpr auto aRun{Tarcog::ISO15099::System::SHGC};

    Helper::testVectors("Temperature - SHGC Run",
                        {308.258554, 308.337603, 306.140245, 306.017910},
                        aSystem.getTemperatures(aRun),
                        Tolerance);

    Helper::testVectors("Solid Conductivities - SHGC Run",
                        {1.0, 1.0},
                        aSystem.getSolidEffectiveLayerConductivities(aRun),
                        Tolerance);

    Helper::testVectors("Gap Conductivities - SHGC Run",
                        {0.000539},
                        aSystem.getGapEffectiveLayerConductivities(aRun),
                        Tolerance);

    Helper::testVectors("Radiosity - SHGC Run",
                        {508.687455, 510.473663, 500.007766, 488.390349},
                        aSystem.getRadiosities(aRun),
                        Tolerance);

    EXPECT_NEAR(0.189222, aSystem.getEffectiveSystemConductivity(aRun), Tolerance);
    EXPECT_NEAR(0.006196, aSystem.thickness(aRun), Tolerance);
    EXPECT_EQ(1U, aSystem.getNumberOfIterations(aRun));
}