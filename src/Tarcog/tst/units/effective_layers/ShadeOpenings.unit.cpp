#include <gtest/gtest.h>

#include <WCETarcog.hpp>

// Cellular shade from AERC database (ID 1007): "Cell-in-cell Light color (HD)"
// Window dimensions: width=8.78 m, height=0.661 m
// Left/right opening = 3 mm (0.003 m), dominant gap width = 38.1 mm (0.0381 m)
// Shade: non-porous (permeabilityFactor=0), no top/bottom openings
TEST(TestShadeOpenings, CellularShadeOpeningAreas)
{
    SCOPED_TRACE("Begin Test: Cellular shade opening areas (AERC 1007).");

    constexpr auto materialThickness{3.514e-02};   // m
    constexpr auto permeabilityFactor{0.0};
    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto shade =
      EffectiveLayers::makeCommonValues(materialThickness, permeabilityFactor, openness);

    constexpr auto width{8.78};     // m
    constexpr auto height{0.661};   // m

    const auto shadeOpenings =
      Tarcog::ISO15099::getShadeOpenings(width, height, shade.openness);

    // Afront = Mfront * width * height = 0 (non-porous cellular shade)
    // Aleft  = Mleft  * height = 0.003 * 0.661 = 0.001983 m2
    // Aright = Mright * height = 0.003 * 0.661 = 0.001983 m2
    // Atop = Abot = 0
    //
    // Since Atop and Abot are zero, OPENING_TOLERANCE (1e-6) is used as minimum.
    // Aeq_bot = Aeq_top = 0.0009925 m2 (symmetric when top == bottom)
    EXPECT_NEAR(0.0, shadeOpenings.effectiveFrontThermalOpennessArea(), 1e-6);
    EXPECT_NEAR(0.000993, shadeOpenings.Aeq_bot(), 1e-6);
    EXPECT_NEAR(0.000993, shadeOpenings.Aeq_top(), 1e-6);
    EXPECT_NEAR(0.0, shadeOpenings.frontPorosity(), 1e-6);
    EXPECT_TRUE(shadeOpenings.isOpen());
}

// Horizontal venetian blind from AERC database (ID 2001): "Off White Aluminum 24 mm (0)"
// Window dimensions: width=8.78 m, height=0.661 m
// Left/right opening = 3 mm, dominant gap width = 38.1 mm (0.0381 m)
// Venetian: Mleft=Mright=0 (slats block side flow), only Mfront contributes
TEST(TestShadeOpenings, HorizontalVenetianOpeningAreas)
{
    SCOPED_TRACE("Begin Test: Horizontal venetian opening areas (AERC 2001).");

    constexpr auto slatThickness{0.0001};   // m
    constexpr FenestrationCommon::Venetian::Geometry geometry{
      .SlatWidth = 0.024,
      .SlatSpacing = 0.019,
      .SlatTiltAngle = 0.0,
      .CurvatureRadius = 0.0488};

    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto venetian =
      EffectiveLayers::makeHorizontalVenetianValues(slatThickness, geometry, openness);

    constexpr auto width{8.78};     // m
    constexpr auto height{0.661};   // m

    const auto shadeOpenings =
      Tarcog::ISO15099::getShadeOpenings(width, height, venetian.openness);

    // Aleft = Aright = 0 (venetian ignores side openings)
    // Atop = Abot = 0
    // Afront = Mfront * width * height = 0.015956 * 8.78 * 0.661 = 0.092599 m2
    //
    // Since Atop and Abot are zero, OPENING_TOLERANCE (1e-6) is used as minimum.
    // Aeq_bot = Aeq_top = 0.023151 (confirmed against E+ A1eqin/A1eqout)
    EXPECT_NEAR(0.092599, shadeOpenings.effectiveFrontThermalOpennessArea(), 1e-6);
    EXPECT_NEAR(0.023151, shadeOpenings.Aeq_bot(), 1e-6);
    EXPECT_NEAR(0.023151, shadeOpenings.Aeq_top(), 1e-6);
    EXPECT_NEAR(0.994764, shadeOpenings.frontPorosity(), 1e-6);
    EXPECT_TRUE(shadeOpenings.isOpen());
}

// Roller shade from AERC database (ID 3001): "3000 NET 1% N001 White/White"
// Window dimensions: width=8.78 m, height=0.661 m
// Left/right opening = 3 mm (0.003 m), dominant gap width = 38.1 mm (0.0381 m)
// Roller shade: Mleft=Mright=0.003, small Mfront from permeability
TEST(TestShadeOpenings, RollerShadeOpeningAreas)
{
    SCOPED_TRACE("Begin Test: Roller shade opening areas (AERC 3001).");

    constexpr auto materialThickness{0.0008};   // m
    constexpr auto permeabilityFactor{0.001610};
    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto shade =
      EffectiveLayers::makeCommonValues(materialThickness, permeabilityFactor, openness);

    constexpr auto width{8.78};     // m
    constexpr auto height{0.661};   // m

    const auto shadeOpenings =
      Tarcog::ISO15099::getShadeOpenings(width, height, shade.openness);

    // Afront = Mfront * width * height = 3.470426e-5 * 8.78 * 0.661 = 0.000201 m2
    // Aleft  = Mleft  * height = 0.003 * 0.661 = 0.001983 m2
    // Aright = Mright * height = 0.003 * 0.661 = 0.001983 m2
    // Atop = Abot = 0
    //
    // Aeq_bot = Aeq_top (symmetric, to be confirmed against E+ debug)
    EXPECT_NEAR(0.000201, shadeOpenings.effectiveFrontThermalOpennessArea(), 1e-6);
    EXPECT_NEAR(0.001043, shadeOpenings.Aeq_bot(), 1e-6);
    EXPECT_NEAR(0.001043, shadeOpenings.Aeq_top(), 1e-6);
    // E+ debug: Ah=0.000201409, A1eqin=A1eqout=0.001042852
    EXPECT_NEAR(0.001610, shadeOpenings.frontPorosity(), 1e-6);
    EXPECT_TRUE(shadeOpenings.isOpen());
}
