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

    // Aleft  = Mleft  * height = 0.003 * 0.661 = 0.001983 m2
    // Aright = Mright * height = 0.003 * 0.661 = 0.001983 m2
    // Atop = Abot = Afront = 0
    //
    // Since Atop and Abot are zero, OPENING_TOLERANCE (1e-6) is used as minimum.
    // Aeq_bot = Aeq_top = 0.0009925 m2 (symmetric when top == bottom)
    EXPECT_NEAR(0.0009925, shadeOpenings.Aeq_bot(), 1e-6);
    EXPECT_NEAR(0.0009925, shadeOpenings.Aeq_top(), 1e-6);
    EXPECT_NEAR(0.0, shadeOpenings.frontPorosity(), 1e-8);
    EXPECT_TRUE(shadeOpenings.isOpen());
}
