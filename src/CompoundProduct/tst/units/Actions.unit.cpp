#include <cmath>

#include <gtest/gtest.h>

#include "CompoundProductTestHelper.hpp"

// Every test builds the measured products from chosen internal properties, runs an action and
// compares the result with the product that is constructed directly from the chosen properties.

using namespace CompoundProduct;
using namespace CompoundProductTest;
using FenestrationCommon::Side;

namespace
{
    const std::vector<double> wavelengths{0.4, 0.6};
    constexpr double Tolerance{1e-9};
    constexpr double OriginalGlassThickness{3.0};
    constexpr double OtherGlassThickness{4.0};
    constexpr double InterlayerThickness{0.76};

    Monolithic originalGlass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.04, 0.05}),
                .internalTransmittance = series(wavelengths, {0.9, 0.6})};
    }

    Monolithic otherGlass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.045, 0.042}),
                .internalTransmittance = series(wavelengths, {0.95, 0.8})};
    }

    Interface coating()
    {
        return {.transmittanceFront = series(wavelengths, {0.5, 0.6}),
                .transmittanceBack = series(wavelengths, {0.5, 0.6}),
                .reflectanceFront = series(wavelengths, {0.1, 0.2}),
                .reflectanceBack = series(wavelengths, {0.3, 0.25})};
    }

    Bulk bulkOf(const Monolithic & pane)
    {
        return {.internalTransmittance = pane.internalTransmittance};
    }

    ExternalProperties uncoatedPane(const Monolithic & glass)
    {
        return Assembly(uncoatedInterface(glass))
          .add(bulkOf(glass), uncoatedInterface(glass))
          .externalProperties();
    }

    ExternalProperties
      coatedPane(const Monolithic & glass, const Bulk & bulk, const Side coatedSide)
    {
        if(coatedSide == Side::Front)
        {
            return Assembly(coating()).add(bulk, uncoatedInterface(glass)).externalProperties();
        }
        return Assembly(uncoatedInterface(glass)).add(bulk, coating()).externalProperties();
    }

    void expectSameProduct(const ExternalProperties & actual,
                    const ExternalProperties & expected,
                    const double tolerance)
    {
        expectNear(actual.transmittanceFront, expected.transmittanceFront, tolerance);
        expectNear(actual.transmittanceBack, expected.transmittanceBack, tolerance);
        expectNear(actual.reflectanceFront, expected.reflectanceFront, tolerance);
        expectNear(actual.reflectanceBack, expected.reflectanceBack, tolerance);
    }
}   // namespace

namespace
{
    Interface otherCoating()
    {
        return {.transmittanceFront = series(wavelengths, {0.7, 0.4}),
                .transmittanceBack = series(wavelengths, {0.7, 0.4}),
                .reflectanceFront = series(wavelengths, {0.15, 0.3}),
                .reflectanceBack = series(wavelengths, {0.05, 0.2})};
    }

    Bulk interlayer()
    {
        return {.internalTransmittance = series(wavelengths, {0.97, 0.7})};
    }

    Bulk referenceInterlayer()
    {
        return {.internalTransmittance = series(wavelengths, {0.92, 0.75})};
    }

    //! Pane of original glass that carries coating() on the given side, with its substrate.
    CoatedPane coatingSource(const Side coatedSide)
    {
        return {.measured = coatedPane(originalGlass(), bulkOf(originalGlass()), coatedSide),
                .substrate = uncoatedPane(originalGlass()),
                .coatedSide = coatedSide,
                .thickness = OriginalGlassThickness};
    }

    UncoatedPane originalPane()
    {
        return {.measured = uncoatedPane(originalGlass()), .thickness = OriginalGlassThickness};
    }

    UncoatedPane otherPane()
    {
        return {.measured = uncoatedPane(otherGlass()), .thickness = OtherGlassThickness};
    }

    Interlayer interlayerLayer()
    {
        return {.internalTransmittance = interlayer().internalTransmittance,
                .thickness = InterlayerThickness};
    }

    //! Measured laminate of original glass with the given coating towards the interlayer,
    //! reference interlayer and other glass.
    ExternalProperties referenceLaminate(const Interface & embedded)
    {
        return Assembly(uncoatedInterface(originalGlass()))
          .add(bulkOf(originalGlass()), embedded)
          .add(referenceInterlayer(), nonReflectingInterface(wavelengths))
          .add(bulkOf(otherGlass()), uncoatedInterface(otherGlass()))
          .externalProperties();
    }

    //! Coating that touches an interlayer, deconstructed from a reference laminate that was
    //! measured with its coated pane in front.
    Interface embeddedCoating(const ExternalProperties & reference)
    {
        return reference
          .deconstructEmbeddedCoating(originalGlass(), otherGlass(), referenceInterlayer())
          .value;
    }
}   // namespace

class CompoundProductActions : public testing::TestWithParam<Side>
{};

TEST_P(CompoundProductActions, OnSubstrate)
{
    const auto coatedSide{GetParam()};

    const auto result{coatingSource(coatedSide).onSubstrate(otherPane())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectSameProduct(result.value.measured,
                      coatedPane(otherGlass(), bulkOf(otherGlass()), coatedSide),
                      Tolerance);
    expectSameProduct(result.value.substrate, uncoatedPane(otherGlass()), Tolerance);
    EXPECT_EQ(result.value.coatedSide, coatedSide);
    EXPECT_DOUBLE_EQ(result.value.thickness, OtherGlassThickness);
}

TEST_P(CompoundProductActions, OnTheSameGlassReturnsTheSamePane)
{
    const auto coatedSide{GetParam()};
    const auto pane{coatingSource(coatedSide)};

    const auto result{pane.onSubstrate(originalPane())};

    expectSameProduct(result.value.measured, pane.measured, Tolerance);
}

TEST_P(CompoundProductActions, CoatedPaneAtThickness)
{
    const auto coatedSide{GetParam()};
    constexpr double NewThickness{6.0};

    const auto result{coatingSource(coatedSide).atThickness(NewThickness)};

    const auto thickGlass{
      withThickness(bulkOf(originalGlass()), OriginalGlassThickness, NewThickness)};
    const auto surface{uncoatedInterface(originalGlass())};
    EXPECT_TRUE(result.diagnostics.empty());
    expectSameProduct(
      result.value.measured, coatedPane(originalGlass(), thickGlass, coatedSide), Tolerance);
    expectSameProduct(result.value.substrate,
                      Assembly(surface).add(thickGlass, surface).externalProperties(),
                      Tolerance);
    EXPECT_EQ(result.value.coatedSide, coatedSide);
    EXPECT_DOUBLE_EQ(result.value.thickness, NewThickness);
}

TEST_P(CompoundProductActions, CoatedPaneAtItsOwnThicknessIsTheSamePane)
{
    const auto coatedSide{GetParam()};
    const auto pane{coatingSource(coatedSide)};

    const auto result{pane.atThickness(OriginalGlassThickness)};

    expectSameProduct(result.value.measured, pane.measured, Tolerance);
    expectSameProduct(result.value.substrate, pane.substrate, Tolerance);
}

TEST_P(CompoundProductActions, CoatedPaneAtThicknessCanBeResizedAgain)
{
    const auto coatedSide{GetParam()};
    const auto pane{coatingSource(coatedSide)};

    const auto result{pane.atThickness(6.0).value.atThickness(OriginalGlassThickness)};

    expectSameProduct(result.value.measured, pane.measured, Tolerance);
}

TEST_P(CompoundProductActions, WithCoatingOnTheSameSide)
{
    const auto coatedSide{GetParam()};

    const auto result{otherPane().withCoating(coatingSource(coatedSide), coatedSide)};

    expectSameProduct(result.value.measured,
                      coatedPane(otherGlass(), bulkOf(otherGlass()), coatedSide),
                      Tolerance);
    expectSameProduct(result.value.substrate, uncoatedPane(otherGlass()), Tolerance);
    EXPECT_EQ(result.value.coatedSide, coatedSide);
    EXPECT_DOUBLE_EQ(result.value.thickness, OtherGlassThickness);
}

INSTANTIATE_TEST_SUITE_P(BothSides,
                         CompoundProductActions,
                         testing::Values(Side::Front, Side::Back));

TEST(CompoundProductActionsThickness, UncoatedPaneAtThickness)
{
    const auto result{originalPane().atThickness(6.0)};

    const auto surface{uncoatedInterface(originalGlass())};
    const auto thickGlass{withThickness(bulkOf(originalGlass()), OriginalGlassThickness, 6.0)};
    const auto expected{Assembly(surface).add(thickGlass, surface)};
    EXPECT_TRUE(result.diagnostics.empty());
    expectSameProduct(result.value.measured, expected.externalProperties(), Tolerance);
    EXPECT_DOUBLE_EQ(result.value.thickness, 6.0);
}

TEST(CompoundProductActionsCoating, WithCoatingOnTheOtherSideKeepsGlassSideTowardsGlass)
{
    const auto result{otherPane().withCoating(coatingSource(Side::Back), Side::Front)};

    const auto expected{
      Assembly(coating().flipped()).add(bulkOf(otherGlass()), uncoatedInterface(otherGlass()))};
    expectSameProduct(result.value.measured, expected.externalProperties(), Tolerance);
    EXPECT_EQ(result.value.coatedSide, Side::Front);
}

TEST(CompoundProductActionsCoating, CoatBothSides)
{
    const CoatedPane backSource{
      .measured = Assembly(uncoatedInterface(otherGlass()))
                    .add(bulkOf(otherGlass()), otherCoating())
                    .externalProperties(),
      .substrate = uncoatedPane(otherGlass()),
      .coatedSide = Side::Back,
      .thickness = OtherGlassThickness};

    const auto result{
      coatBothSides(uncoatedPane(originalGlass()), coatingSource(Side::Front), backSource)};

    const auto expected{Assembly(coating()).add(bulkOf(originalGlass()), otherCoating())};
    EXPECT_TRUE(result.diagnostics.empty());
    expectSameProduct(result.value, expected.externalProperties(), Tolerance);
}

TEST(CompoundProductActionsLaminate, MakeInterlayer)
{
    const auto noReflection{nonReflectingInterface(wavelengths)};
    const auto laminate{Assembly(uncoatedInterface(originalGlass()))
                          .add(bulkOf(originalGlass()), noReflection)
                          .add(interlayer(), noReflection)
                          .add(bulkOf(otherGlass()), uncoatedInterface(otherGlass()))};

    const auto result{makeInterlayer(laminate.externalProperties(),
                                     uncoatedPane(originalGlass()),
                                     uncoatedPane(otherGlass()),
                                     InterlayerThickness)};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(
      result.value.internalTransmittance, interlayer().internalTransmittance, Tolerance);
    EXPECT_DOUBLE_EQ(result.value.thickness, InterlayerThickness);
}

TEST(CompoundProductActionsDiagnostics, OpaqueSubstrateIsReported)
{
    const Monolithic opaqueGlass{.surfaceReflectance = series(wavelengths, {0.04, 0.05}),
                                 .internalTransmittance = series(wavelengths, {0.0, 0.6})};
    const CoatedPane pane{.measured = coatedPane(opaqueGlass, bulkOf(opaqueGlass), Side::Back),
                          .substrate = uncoatedPane(opaqueGlass),
                          .coatedSide = Side::Back,
                          .thickness = 3.0};

    const auto result{pane.onSubstrate(otherPane())};

    ASSERT_FALSE(result.diagnostics.empty());
    EXPECT_DOUBLE_EQ(result.diagnostics.front().wavelength, wavelengths.front());
    EXPECT_EQ(result.diagnostics.front().source, "substrate");
}

TEST(CompoundProductActionsOptions, SubstrateReflectanceChoiceIsUsed)
{
    auto measured{uncoatedPane(originalGlass())};
    measured.reflectanceBack = series(wavelengths, {0.09, 0.07});
    const UncoatedPane pane{.measured = measured, .thickness = 3.0};
    const ActionOptions frontReflectance{.substrateReflectance = ReflectanceChoice::Front,
                                         .tolerances = {}};

    const auto result{pane.atThickness(6.0, frontReflectance)};

    const auto glass{measured.deconstructMonolithic(ReflectanceChoice::Front).value};
    const auto expected{Assembly(uncoatedInterface(glass))
                          .add(withThickness(bulkOf(glass), 3.0, 6.0), uncoatedInterface(glass))};
    expectSameProduct(result.value.measured, expected.externalProperties(), Tolerance);
    const auto withAverage{pane.atThickness(6.0)};
    EXPECT_GT(std::abs(result.value.measured.reflectanceFront[0].value()
                       - withAverage.value.measured.reflectanceFront[0].value()),
              1e-4);
}

// Laminate. Every test compares the chain of calls with the assembly that is written out
// directly.

namespace
{
    ExternalProperties twoPanes(const Interface & frontSurface,
                                const Interface & frontPaneAtInterlayer,
                                const Interface & backPaneAtInterlayer,
                                const Interface & backSurface)
    {
        return Assembly(frontSurface)
          .add(bulkOf(originalGlass()), frontPaneAtInterlayer)
          .add(interlayer(), backPaneAtInterlayer)
          .add(bulkOf(otherGlass()), backSurface)
          .externalProperties();
    }

    Interface noReflection()
    {
        return nonReflectingInterface(wavelengths);
    }
}   // namespace

TEST(CompoundProductLaminate, SinglePaneIsThatPane)
{
    const auto result{Laminate(originalPane()).spectra()};

    EXPECT_TRUE(result.diagnostics.empty());
    expectSameProduct(result.value, uncoatedPane(originalGlass()), Tolerance);
}

TEST(CompoundProductLaminate, TwoUncoatedPanes)
{
    const auto result{Laminate(originalPane()).add(interlayerLayer(), otherPane()).spectra()};

    const auto expected{twoPanes(uncoatedInterface(originalGlass()),
                                     noReflection(),
                                     noReflection(),
                                     uncoatedInterface(otherGlass()))};
    EXPECT_TRUE(result.diagnostics.empty());
    expectSameProduct(result.value, expected, Tolerance);
}

TEST(CompoundProductLaminate, ThreePanes)
{
    const auto result{Laminate(originalPane())
                        .add(interlayerLayer(), otherPane())
                        .add(interlayerLayer(), originalPane())
                        .spectra()};

    const auto expected{Assembly(uncoatedInterface(originalGlass()))
                          .add(bulkOf(originalGlass()), noReflection())
                          .add(interlayer(), noReflection())
                          .add(bulkOf(otherGlass()), noReflection())
                          .add(interlayer(), noReflection())
                          .add(bulkOf(originalGlass()), uncoatedInterface(originalGlass()))};
    expectSameProduct(result.value, expected.externalProperties(), Tolerance);
}

TEST(CompoundProductLaminate, AddReturnsANewLaminateAndKeepsTheOriginal)
{
    const Laminate single{originalPane()};

    const auto extended{single.add(interlayerLayer(), otherPane())};

    expectSameProduct(single.spectra().value, uncoatedPane(originalGlass()), Tolerance);
    EXPECT_GT(std::abs(single.spectra().value.transmittanceFront[0].value()
                       - extended.spectra().value.transmittanceFront[0].value()),
              1e-4);
}

TEST(CompoundProductLaminate, CoatingsTowardsTheAirOnBothOuterSurfaces)
{
    const CoatedPane backPane{
      .measured = Assembly(uncoatedInterface(otherGlass()))
                    .add(bulkOf(otherGlass()), otherCoating())
                    .externalProperties(),
      .substrate = uncoatedPane(otherGlass()),
      .coatedSide = Side::Back,
      .thickness = OtherGlassThickness};

    const auto result{
      Laminate(coatingSource(Side::Front)).add(interlayerLayer(), backPane).spectra()};

    const auto expected{twoPanes(coating(), noReflection(), noReflection(), otherCoating())};
    expectSameProduct(result.value, expected, Tolerance);
}

TEST(CompoundProductLaminate, EmbeddedCoatingOnThePaneInFront)
{
    const auto result{
      Laminate(originalPane())
        .add(interlayerLayer(),
             otherPane(),
             {.onPaneInFront = embeddedCoating(referenceLaminate(otherCoating())),
              .onPaneBehind = std::nullopt})
        .spectra()};

    const auto expected{twoPanes(uncoatedInterface(originalGlass()),
                                     otherCoating(),
                                     noReflection(),
                                     uncoatedInterface(otherGlass()))};
    EXPECT_TRUE(result.diagnostics.empty());
    expectSameProduct(result.value, expected, Tolerance);
}

TEST(CompoundProductLaminate, EmbeddedCoatingOnThePaneBehind)
{
    // The glass of the back pane is the other glass. The coating comes from a reference
    // laminate whose coated pane is made of the original glass. The coating is flipped, so that
    // its side towards the glass stays towards the glass.
    const auto result{
      Laminate(originalPane())
        .add(interlayerLayer(),
             otherPane(),
             {.onPaneInFront = std::nullopt,
              .onPaneBehind = embeddedCoating(referenceLaminate(otherCoating())).flipped()})
        .spectra()};

    const auto expected{twoPanes(uncoatedInterface(originalGlass()),
                                     noReflection(),
                                     otherCoating().flipped(),
                                     uncoatedInterface(otherGlass()))};
    expectSameProduct(result.value, expected, Tolerance);
}

TEST(CompoundProductLaminate, ReferenceLaminateMeasuredWithCoatedPaneAtBack)
{
    // The reference laminate was measured from its other side. It is flipped before the
    // deconstruction, so that its coated pane is in front.
    const auto measuredFromTheBack{referenceLaminate(otherCoating()).flipped()};

    const auto result{
      Laminate(originalPane())
        .add(interlayerLayer(),
             otherPane(),
             {.onPaneInFront = embeddedCoating(measuredFromTheBack.flipped()),
              .onPaneBehind = std::nullopt})
        .spectra()};

    const auto expected{twoPanes(uncoatedInterface(originalGlass()),
                                     otherCoating(),
                                     noReflection(),
                                     uncoatedInterface(otherGlass()))};
    expectSameProduct(result.value, expected, Tolerance);
}

TEST(CompoundProductLaminate, PaneWithACoatingOnEachSide)
{
    // Front pane: coating() towards the air on its front, otherCoating() towards the interlayer.
    const auto result{
      Laminate(coatingSource(Side::Front))
        .add(interlayerLayer(),
             otherPane(),
             {.onPaneInFront = embeddedCoating(referenceLaminate(otherCoating())),
              .onPaneBehind = std::nullopt})
        .spectra()};

    const auto expected{twoPanes(
      coating(), otherCoating(), noReflection(), uncoatedInterface(otherGlass()))};
    expectSameProduct(result.value, expected, Tolerance);
}

TEST(CompoundProductLaminate, InterlayerAtNewThickness)
{
    const auto thick{interlayerLayer().atThickness(2.0 * InterlayerThickness)};

    const auto result{Laminate(originalPane()).add(thick, otherPane()).spectra()};

    const auto thickInterlayer{
      withThickness(interlayer(), InterlayerThickness, 2.0 * InterlayerThickness)};
    const auto expected{Assembly(uncoatedInterface(originalGlass()))
                          .add(bulkOf(originalGlass()), noReflection())
                          .add(thickInterlayer, noReflection())
                          .add(bulkOf(otherGlass()), uncoatedInterface(otherGlass()))};
    EXPECT_DOUBLE_EQ(thick.thickness, 2.0 * InterlayerThickness);
    expectSameProduct(result.value, expected.externalProperties(), Tolerance);
}

TEST(CompoundProductLaminate, CoatingMeasuredInTheAirCannotTouchAnInterlayer)
{
    // Back coating of the pane in front of the interlayer.
    EXPECT_THROW((void)Laminate(coatingSource(Side::Back)).add(interlayerLayer(), otherPane()),
                 std::runtime_error);
    // Front coating of the pane behind the interlayer.
    EXPECT_THROW((void)Laminate(originalPane()).add(interlayerLayer(), coatingSource(Side::Front)),
                 std::runtime_error);
}

TEST(CompoundProductLaminate, DiagnosticsNameThePane)
{
    const Monolithic opaqueGlass{.surfaceReflectance = series(wavelengths, {0.04, 0.05}),
                                 .internalTransmittance = series(wavelengths, {0.0, 0.6})};
    const UncoatedPane opaquePane{.measured = uncoatedPane(opaqueGlass), .thickness = 3.0};

    const auto result{Laminate(originalPane()).add(interlayerLayer(), opaquePane).spectra()};

    ASSERT_FALSE(result.diagnostics.empty());
    EXPECT_EQ(result.diagnostics.front().source, "pane[1].measured");
}
