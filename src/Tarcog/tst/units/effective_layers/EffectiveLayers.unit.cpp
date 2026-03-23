#include <gtest/gtest.h>

#include <WCETarcog.hpp>

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0029};   // m

    constexpr FenestrationCommon::Venetian::Geometry geometry{
      .SlatWidth = 0.0508, .SlatSpacing = 0.012, .SlatTiltAngle = 0.0, .CurvatureRadius = 0.0};

    const auto venetian =
      EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry);

    const auto effectiveThickness{venetian.thickness};

    EXPECT_NEAR(0.0021844, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.014265839, effectiveOpenness.Mfront, 1e-8);

    EXPECT_NEAR(0.805369, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer1)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0029};   // m

    constexpr FenestrationCommon::Venetian::Geometry geometry{0.0508, 0.012, 0.0, 0.0};

    const auto venetian =
      EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry);

    const auto effectiveThickness{venetian.thickness};

    EXPECT_NEAR(0.0021844, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.014265839, effectiveOpenness.Mfront, 1e-8);

    EXPECT_NEAR(0.805369, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0001};   // m
    constexpr FenestrationCommon::Venetian::Geometry geometry{0.0148, 0.012, 0.0, 0.0};

    const EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    const auto venetian =
      EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry, openness);

    EXPECT_NEAR(6.364e-4, venetian.thickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.015930, effectiveOpenness.Mfront, 1e-6);
    EXPECT_NEAR(0.991736, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, TestVenetianVerticalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0001};   // m
    const FenestrationCommon::Venetian::Geometry geometry{0.0762, 0.012, 0.0, 0.0};

    const EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    const auto venetian =
      EffectiveLayers::makeVerticalVenetianValues(materialThickness, geometry, openness);

    const auto effectiveThickness{venetian.thickness};

    EXPECT_NEAR(9.144e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.040908, effectiveOpenness.Mfront, 1e-6);
}

TEST(TestEffectiveLayers, TestEffectiveLayerCommon)
{
    SCOPED_TRACE("Begin Test: Effective layer common.");

    constexpr auto materialThickness{0.0001};   // m
    const auto permeabilityFactor{0.15};

    const auto common = EffectiveLayers::makeCommonValues(
      materialThickness, permeabilityFactor, EffectiveLayers::ShadeOpenness{0, 0, 0, 0});

    const auto effectiveThickness{common.thickness};

    EXPECT_NEAR(0.0001, effectiveThickness, 1e-6);

    const auto effectiveOpenness{common.openness};
    EXPECT_NEAR(8.0057885183479893e-3, effectiveOpenness.Mfront, 1e-8);
}

TEST(TestEffectiveLayers, TestVenetianVerticalEffectiveLayerWithTopAndBotOpenness45Deg)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0001};   // m
    const FenestrationCommon::Venetian::Geometry geometry{0.0762, 0.012, 45.0, 0.0};

    const EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    const auto venetian =
      EffectiveLayers::makeVerticalVenetianValues(materialThickness, geometry, openness);

    const auto effectiveThickness{venetian.thickness};

    EXPECT_NEAR(6.474269e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.040870, effectiveOpenness.Mfront, 1e-6);
}

TEST(TestEffectiveLayers, TestPerforatedEffectiveOpenness)
{
    SCOPED_TRACE("Begin Test: Perforated screen effective layer properties.");

    constexpr auto materialThickness{0.0006};   // m
    const FenestrationCommon::Perforated::Geometry geometry{
      FenestrationCommon::Perforated::Type::Circular, 0.0148, 0.0148, 0.012, 0.012};

    const EffectiveLayers::ShadeOpenness openness{0.005, 0.004, 0.01, 0.008};

    const auto perforated =
      EffectiveLayers::makePerforatedValues(materialThickness, geometry, openness);

    EXPECT_NEAR(6e-4, perforated.thickness, 1e-9);

    const auto effectiveOpenness{perforated.openness};
    EXPECT_NEAR(0.035287, effectiveOpenness.Mfront, 1e-6);
    EXPECT_NEAR(0.516332, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayer, TestLouveredShutterEffectiveOpenness)
{
    FenestrationCommon::LouveredShutter::Geometry geometry{
      .SlatWidth = 0.0889, .SlatThickness = 0.01, .SlatAngle = 87.0, .SlatSpacing = 0.0762};

    const auto louveredShutter = EffectiveLayers::makeLouveredShutterValues(geometry);

    EXPECT_NEAR(0.00161029, louveredShutter.thickness, 1e-8);

    const auto effectiveOpenness{louveredShutter.openness};
    EXPECT_NEAR(0.00503352, effectiveOpenness.Mfront, 1e-8);
    EXPECT_NEAR(0.295495, effectiveOpenness.PermeabilityFactor, 1e-6);
}

// Roller shade from AERC database (ID 3001): "3000 NET 1% N001 White/White"
// IDF values: thickness=8e-4, conductivity=0.3, IR transmittance=0.0333,
// emissivity=0.8948, permeability=0.001610
TEST(TestEffectiveLayers, TestRollerShadeEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Roller shade effective layer properties (AERC 3001).");

    constexpr auto materialThickness{0.0008};   // m
    constexpr auto permeabilityFactor{0.001610};

    const auto shade = EffectiveLayers::makeCommonValues(materialThickness, permeabilityFactor);

    EXPECT_NEAR(0.0008, shade.thickness, 1e-6);

    const auto effectiveOpenness{shade.openness};
    EXPECT_NEAR(3.470426e-05, effectiveOpenness.Mfront, 1e-8);
    EXPECT_NEAR(0.001610, effectiveOpenness.PermeabilityFactor, 1e-6);
}

// Roller shade from AERC database (ID 3001): "3000 NET 1% N001 White/White"
// with side openness. ShadeOpenness takes raw opening distances in meters.
// Left/right opening = 3 mm (0.003 m).
TEST(TestEffectiveLayers, TestRollerShadeWithSideOpenness)
{
    SCOPED_TRACE("Begin Test: Roller shade with side openness (AERC 3001).");

    constexpr auto materialThickness{0.0008};   // m
    constexpr auto permeabilityFactor{0.001610};
    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto shade =
      EffectiveLayers::makeCommonValues(materialThickness, permeabilityFactor, openness);

    EXPECT_NEAR(0.0008, shade.thickness, 1e-6);

    const auto & [Mfront, Mleft, Mright, Mtop, Mbot, PermFactor] = shade.openness;
    EXPECT_NEAR(3.470426e-05, Mfront, 1e-6);
    EXPECT_NEAR(0.003, Mleft, 1e-6);
    EXPECT_NEAR(0.003, Mright, 1e-6);
    EXPECT_NEAR(0.0, Mtop, 1e-6);
    EXPECT_NEAR(0.0, Mbot, 1e-6);
    EXPECT_NEAR(0.001610, PermFactor, 1e-6);

    // Coefficients for common shade model (BSDF/OtherShadingType)
    EXPECT_NEAR(0.078, shade.coeffs.C1, 1e-6);
    EXPECT_NEAR(1.2, shade.coeffs.C2, 1e-6);
    EXPECT_NEAR(1.0, shade.coeffs.C3, 1e-6);
    EXPECT_NEAR(1.0, shade.coeffs.C4, 1e-6);
}

// Cellular shade from AERC database (ID 1007): "Cell-in-cell Light color (HD)"
// IDF WindowMaterial:ComplexShade values:
//   type=OtherShadingType, thickness=3.514e-2, conductivity=9.857403e-2,
//   IR transmittance=2.856486e-4, front emissivity=0.87586, back emissivity=0.8758530,
//   TopOpeningRatio=0, BottomOpeningRatio=0,
//   LeftSideOpeningRatio=7.874016e-2, RightSideOpeningRatio=7.874016e-2,
//   FrontOpeningRatio=0, PermeabilityFactor=0
//
// ShadeOpenness takes raw opening distances in meters (not IDF ratios).
// Left/right opening = 3 mm (0.003 m).
TEST(TestEffectiveLayers, TestCellularShadeEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Cellular shade effective layer properties (AERC 1007).");

    constexpr auto materialThickness{3.514e-02};   // m
    constexpr auto permeabilityFactor{0.0};
    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto shade =
      EffectiveLayers::makeCommonValues(materialThickness, permeabilityFactor, openness);

    EXPECT_NEAR(3.514e-02, shade.thickness, 1e-6);

    const auto & [Mfront, Mleft, Mright, Mtop, Mbot, PermFactor] = shade.openness;
    EXPECT_NEAR(0.0, Mfront, 1e-8);
    EXPECT_NEAR(0.003, Mleft, 1e-8);
    EXPECT_NEAR(0.003, Mright, 1e-8);
    EXPECT_NEAR(0.0, Mtop, 1e-8);
    EXPECT_NEAR(0.0, Mbot, 1e-8);
    EXPECT_NEAR(0.0, PermFactor, 1e-8);

    // Coefficients for OtherShadingType (common shade model)
    EXPECT_NEAR(0.078, shade.coeffs.C1, 1e-6);
    EXPECT_NEAR(1.2, shade.coeffs.C2, 1e-6);
    EXPECT_NEAR(1.0, shade.coeffs.C3, 1e-6);
    EXPECT_NEAR(1.0, shade.coeffs.C4, 1e-6);
}

// Horizontal venetian blind from AERC database (ID 2001): "Off White Aluminum 24 mm (0)"
// IDF WindowMaterial:ComplexShade values:
//   type=VenetianHorizontal, slat width=0.024, slat spacing=0.019,
//   slat thickness=0.0001, slat angle=0, slat conductivity=160, slat curve=0.0488,
//   PermeabilityFactor=9.947644e-1, FrontOpeningRatio=1.595555e-2,
//   Left/RightSideOpeningRatio=7.874016e-2, Top/BottomOpeningRatio=0
//
// ShadeOpenness takes raw opening distances in meters.
// Left/right opening = 3 mm, top/bottom = 0.
// Note: venetian blinds ignore left/right openness (Mleft=Mright=0).
TEST(TestEffectiveLayers, TestHorizontalVenetianEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Horizontal venetian effective layer properties (AERC 2001).");

    constexpr auto slatThickness{0.0001};   // m (material thickness)
    constexpr FenestrationCommon::Venetian::Geometry geometry{
      .SlatWidth = 0.024,
      .SlatSpacing = 0.019,
      .SlatTiltAngle = 0.0,
      .CurvatureRadius = 0.0488};

    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto venetian =
      EffectiveLayers::makeHorizontalVenetianValues(slatThickness, geometry, openness);

    // t_eff = C4 * (SlatWidth * cos(0) + thickness * sin(0)) = 0.043 * 0.024 = 0.001032
    EXPECT_NEAR(0.001032, venetian.thickness, 1e-6);

    const auto & [Mfront, Mleft, Mright, Mtop, Mbot, PermFactor] = venetian.openness;

    // Venetian permeability from slat geometry
    EXPECT_NEAR(0.994764, PermFactor, 1e-5);

    // Mfront = C1 * pow(perm * pow(cos(0), C2), C3) = 0.016 * pow(0.994764, 0.53)
    EXPECT_NEAR(1.595555e-02, Mfront, 1e-6);

    // Venetian blinds: left/right always zero, top/bottom from openness
    EXPECT_NEAR(0.0, Mleft, 1e-8);
    EXPECT_NEAR(0.0, Mright, 1e-8);
    EXPECT_NEAR(0.0, Mtop, 1e-8);
    EXPECT_NEAR(0.0, Mbot, 1e-8);

    // Coefficients for horizontal venetian
    EXPECT_NEAR(0.016, venetian.coeffs.C1, 1e-6);
    EXPECT_NEAR(-0.63, venetian.coeffs.C2, 1e-6);
    EXPECT_NEAR(0.53, venetian.coeffs.C3, 1e-6);
    EXPECT_NEAR(0.043, venetian.coeffs.C4, 1e-6);
}

// Vertical venetian blind from AERC database (ID 8001): "White PVC Vertical Blind 2" (0)"
// IDF WindowMaterial:ComplexShade values:
//   type=VenetianVertical, slat width=0.0508, slat spacing=0.0432,
//   slat thickness=0.001, slat angle=0, slat conductivity=0.12, slat curve=0.067,
//   PermeabilityFactor=9.773756e-1, FrontOpeningRatio=4.074745e-2,
//   Left/RightSideOpeningRatio=7.874016e-2, Top/BottomOpeningRatio=0
//
// ShadeOpenness takes raw opening distances in meters.
// Left/right opening = 3 mm, top/bottom = 0.
// Note: venetian blinds ignore left/right openness (Mleft=Mright=0).
TEST(TestEffectiveLayers, TestVerticalVenetianEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Vertical venetian effective layer properties (AERC 8001).");

    constexpr auto slatThickness{0.001};   // m (material thickness)
    constexpr FenestrationCommon::Venetian::Geometry geometry{
      .SlatWidth = 0.0508,
      .SlatSpacing = 0.0432,
      .SlatTiltAngle = 0.0,
      .CurvatureRadius = 0.067};

    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto venetian =
      EffectiveLayers::makeVerticalVenetianValues(slatThickness, geometry, openness);

    // t_eff = C4 * (SlatWidth * cos(0) + thickness * sin(0)) = 0.012 * 0.0508 = 0.0006096
    EXPECT_NEAR(0.0006096, venetian.thickness, 1e-6);

    const auto & [Mfront, Mleft, Mright, Mtop, Mbot, PermFactor] = venetian.openness;

    // Venetian permeability from slat geometry
    EXPECT_NEAR(0.977376, PermFactor, 1e-6);

    // Mfront = C1 * pow(perm * pow(cos(0), C2), C3) = 0.041 * pow(0.977376, 0.27)
    EXPECT_NEAR(4.074745e-02, Mfront, 1e-6);

    // Vertical venetian: left/right always zero, top/bottom from openness
    EXPECT_NEAR(0.0, Mleft, 1e-6);
    EXPECT_NEAR(0.0, Mright, 1e-6);
    EXPECT_NEAR(0.0, Mtop, 1e-6);
    EXPECT_NEAR(0.0, Mbot, 1e-6);

    // Coefficients for vertical venetian
    EXPECT_NEAR(0.041, venetian.coeffs.C1, 1e-6);
    EXPECT_NEAR(0.0, venetian.coeffs.C2, 1e-6);
    EXPECT_NEAR(0.27, venetian.coeffs.C3, 1e-6);
    EXPECT_NEAR(0.012, venetian.coeffs.C4, 1e-6);
}

// Exterior roller shutter from AERC database (ID 100004): "RollerShutter_A150_AL_Light_NoInsul"
// IDF WindowMaterial:ComplexShade values:
//   type=OtherShadingType, thickness=8.856e-3, conductivity=1.295377e-1,
//   IR transmittance=3.238752e-5, front emissivity=0.90567, back emissivity=0.90954,
//   TopOpeningRatio=0, BottomOpeningRatio=0,
//   LeftSideOpeningRatio=2.362205e-1, RightSideOpeningRatio=2.362205e-1,
//   FrontOpeningRatio=0, PermeabilityFactor=0
//
// ShadeOpenness takes raw opening distances in meters.
// Left/right opening = 3 mm (0.003 m), gap = 12.7 mm.
// IDF ratio = 3 / 12.7 = 0.236221 (used only for E+ IDF export).
TEST(TestEffectiveLayers, TestRollerShutterEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Roller shutter effective layer properties (AERC 100004).");

    constexpr auto materialThickness{8.856e-03};   // m
    constexpr auto permeabilityFactor{0.0};
    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto shade =
      EffectiveLayers::makeCommonValues(materialThickness, permeabilityFactor, openness);

    EXPECT_NEAR(8.856e-03, shade.thickness, 1e-6);

    const auto & [Mfront, Mleft, Mright, Mtop, Mbot, PermFactor] = shade.openness;
    EXPECT_NEAR(0.0, Mfront, 1e-6);
    EXPECT_NEAR(0.003, Mleft, 1e-6);
    EXPECT_NEAR(0.003, Mright, 1e-6);
    EXPECT_NEAR(0.0, Mtop, 1e-6);
    EXPECT_NEAR(0.0, Mbot, 1e-6);
    EXPECT_NEAR(0.0, PermFactor, 1e-6);

    // Coefficients for OtherShadingType (common shade model)
    EXPECT_NEAR(0.078, shade.coeffs.C1, 1e-6);
    EXPECT_NEAR(1.2, shade.coeffs.C2, 1e-6);
    EXPECT_NEAR(1.0, shade.coeffs.C3, 1e-6);
    EXPECT_NEAR(1.0, shade.coeffs.C4, 1e-6);
}

// Louvered shutter from AERC database (ID 70000): "Louvered Shutter L1 - 0"
// IDF WindowMaterial:ComplexShade values:
//   type=LouveredShutter, layer thickness=8.89e-2, conductivity=1.723063,
//   PermeabilityFactor=8.434433e-1, FrontOpeningRatio=4.536313e-2,
//   Left/RightSideOpeningRatio=5.405406e-1, Top/BottomOpeningRatio=0
//
// Geometry from XML: Width=0.0889, Height(thickness)=0.011935, Angle=0, Spacing=0.0643
// Note: louvered shutter ignores all openness (Mleft=Mright=Mtop=Mbot=0).
TEST(TestEffectiveLayers, TestLouveredShutterEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Louvered shutter effective layer properties (AERC 70000).");

    constexpr FenestrationCommon::LouveredShutter::Geometry geometry{
      .SlatWidth = 0.0889,
      .SlatThickness = 0.011935,
      .SlatAngle = 0.0,
      .SlatSpacing = 0.0643};

    constexpr EffectiveLayers::ShadeOpenness openness{
      .Dl = 0.003, .Dr = 0.003, .Dtop = 0.0, .Dbot = 0.0};

    const auto shutter = EffectiveLayers::makeLouveredShutterValues(geometry, openness);

    // t_eff = C4 * (SlatWidth * cos(0) + SlatThickness * |sin(0)|) = 0.11 * 0.0889
    EXPECT_NEAR(0.009779, shutter.thickness, 1e-6);

    const auto & [Mfront, Mleft, Mright, Mtop, Mbot, PermFactor] = shutter.openness;

    EXPECT_NEAR(0.843445, PermFactor, 1e-6);
    EXPECT_NEAR(4.536313e-02, Mfront, 1e-6);

    // Louvered shutter: all side/top/bottom openings zeroed
    EXPECT_NEAR(0.0, Mleft, 1e-6);
    EXPECT_NEAR(0.0, Mright, 1e-6);
    EXPECT_NEAR(0.0, Mtop, 1e-6);
    EXPECT_NEAR(0.0, Mbot, 1e-6);

    // Coefficients for louvered shutter
    EXPECT_NEAR(0.12, shutter.coeffs.C1, 1e-6);
    EXPECT_NEAR(0.82, shutter.coeffs.C2, 1e-6);
    EXPECT_NEAR(0.059, shutter.coeffs.C3, 1e-6);
    EXPECT_NEAR(0.11, shutter.coeffs.C4, 1e-6);
}

TEST(TestEffectiveLayers, RadiusFromRise)
{
    constexpr double curvature{23.88962765};
    constexpr double slatWidth{14.8};
    auto test{ThermalPermeability::Venetian::calculateRise(curvature, slatWidth)};
    EXPECT_NEAR(1.175, test, 1e-6);
}
