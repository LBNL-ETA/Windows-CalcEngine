#include <gtest/gtest.h>

#include "CompoundProductTestHelper.hpp"

// Every test builds a product from chosen internal properties, deconstructs the result and
// expects to get the chosen internal properties back. The coatings have different front and back
// transmittances, so that every deconstruction is tested with four properties.

using namespace CompoundProduct;
using namespace CompoundProductTest;
using FenestrationCommon::Side;

namespace
{
    const std::vector<double> wavelengths{0.4, 0.6};
    constexpr double Tolerance{1e-9};

    Monolithic frontGlass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.04, 0.05}),
                .tau = series(wavelengths, {0.9, 0.6})};
    }

    Monolithic backGlass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.045, 0.042}),
                .tau = series(wavelengths, {0.95, 0.8})};
    }

    Bulk bulkOf(const Monolithic & monolithic)
    {
        return {.tau = monolithic.tau};
    }

    Bulk interlayer()
    {
        return {.tau = series(wavelengths, {0.97, 0.7})};
    }

    Interface coating()
    {
        return {.tf = series(wavelengths, {0.5, 0.6}),
                .tb = series(wavelengths, {0.45, 0.55}),
                .rf = series(wavelengths, {0.1, 0.2}),
                .rb = series(wavelengths, {0.3, 0.25})};
    }

    Interface otherCoating()
    {
        return {.tf = series(wavelengths, {0.7, 0.4}),
                .tb = series(wavelengths, {0.65, 0.5}),
                .rf = series(wavelengths, {0.15, 0.3}),
                .rb = series(wavelengths, {0.2, 0.35})};
    }

    Interface noReflection()
    {
        return nonReflectingInterface(wavelengths);
    }

    Interface surfaceOf(const Monolithic & monolithic)
    {
        return uncoatedInterface(monolithic);
    }

    // Front pane with a coating towards the air and an interface to an interlayer.
    Assembly coatedFrontPane()
    {
        return Assembly(otherCoating()).add(bulkOf(frontGlass()), noReflection());
    }

    // Back pane with an interface to an interlayer and a bare surface towards the air.
    Assembly uncoatedBackPane()
    {
        return Assembly(noReflection()).add(bulkOf(backGlass()), surfaceOf(backGlass()));
    }
}   // namespace

// A coating behind the middle pane of a laminate of three panes. No case of the sources covers
// this product.
TEST(CompoundProductRoundTrip, InterfaceInsideLaminateOfThreePanes)
{
    const auto middleGlass{bulkOf(backGlass())};
    const auto frontPart{coatedFrontPane().add(interlayer(), noReflection())};
    const auto whole{
      frontPart.add(middleGlass, coating()).add(interlayer(), uncoatedBackPane())};

    const auto result{whole.externalProperties().deconstructPartInTheMiddle(
      frontPart, middleGlass, interlayer(), uncoatedBackPane())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, Assembly(coating()).externalProperties(), Tolerance);
}

// The remaining part of the two tests below has two interfaces and one bulk layer. A removal
// returns it as a whole.
TEST(CompoundProductRoundTrip, PartBehind)
{
    const Assembly frontPart{otherCoating()};
    const auto remainingPart{Assembly(coating()).add(bulkOf(backGlass()), surfaceOf(backGlass()))};
    const auto whole{frontPart.add(bulkOf(frontGlass()), remainingPart)};

    const auto result{
      whole.externalProperties().deconstructPartBehind(frontPart, bulkOf(frontGlass()))};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, remainingPart.externalProperties(), Tolerance);
}

// A laminate with a different front pane. The front surface and the glass of the old front pane
// are removed. The remaining part (coating, interlayer, back pane) is used as a whole and is
// never taken apart.
TEST(CompoundProductRoundTrip, RemainingPartIsUsedInAnAssembly)
{
    const auto behindFrontGlass{Assembly(coating()).add(interlayer(), uncoatedBackPane())};
    const Assembly oldFrontSurface{surfaceOf(frontGlass())};
    const auto measuredLaminate{
      oldFrontSurface.add(bulkOf(frontGlass()), behindFrontGlass).externalProperties()};

    const auto remaining{
      measuredLaminate.deconstructPartBehind(oldFrontSurface, bulkOf(frontGlass()))};
    const auto newLaminate{Assembly(surfaceOf(backGlass()))
                             .add(bulkOf(backGlass()), remaining.value)
                             .externalProperties()};

    const auto expected{Assembly(surfaceOf(backGlass()))
                          .add(bulkOf(backGlass()), behindFrontGlass)
                          .externalProperties()};
    EXPECT_TRUE(remaining.diagnostics.empty());
    expectNear(newLaminate, expected, Tolerance);
}

TEST(CompoundProductRoundTrip, PartInFrontOf)
{
    const Assembly backPart{otherCoating()};
    const auto remainingPart{
      Assembly(surfaceOf(frontGlass())).add(bulkOf(frontGlass()), coating())};
    const auto whole{remainingPart.add(bulkOf(backGlass()), backPart)};

    const auto result{
      whole.externalProperties().deconstructPartInFrontOf(bulkOf(backGlass()), backPart)};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, remainingPart.externalProperties(), Tolerance);
}

// An interlayer between a coated front pane and a back pane. The case of the sources needs two
// uncoated panes.
TEST(CompoundProductRoundTrip, BulkBetweenCoatedPanes)
{
    const auto whole{coatedFrontPane().add(interlayer(), uncoatedBackPane())};

    const auto result{
      whole.externalProperties().deconstructBulk(coatedFrontPane(), uncoatedBackPane())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value.tau, interlayer().tau, Tolerance);
}

// A known part that is known only as a whole is given as an Assembly of its external properties.
TEST(CompoundProductRoundTrip, KnownPartGivenByItsExternalProperties)
{
    const auto whole{coatedFrontPane().add(interlayer(), uncoatedBackPane())};
    const Assembly frontPartAsAWhole{coatedFrontPane().externalProperties()};

    const auto result{
      whole.externalProperties().deconstructPartBehind(frontPartAsAWhole, interlayer())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, uncoatedBackPane().externalProperties(), Tolerance);
}

TEST(CompoundProductRoundTrip, Monolithic)
{
    const auto pane{
      Assembly(surfaceOf(frontGlass())).add(bulkOf(frontGlass()), surfaceOf(frontGlass()))};

    const auto result{pane.externalProperties().deconstructMonolithic()};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value.surfaceReflectance, frontGlass().surfaceReflectance, Tolerance);
    expectNear(result.value.tau, frontGlass().tau, Tolerance);
}

TEST(CompoundProductRoundTrip, CoatingOnBack)
{
    const auto pane{Assembly(surfaceOf(frontGlass())).add(bulkOf(frontGlass()), coating())};

    const auto result{pane.externalProperties().deconstructCoating(frontGlass(), Side::Back)};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, CoatingOnFront)
{
    const auto pane{Assembly(coating()).add(bulkOf(frontGlass()), surfaceOf(frontGlass()))};

    const auto result{pane.externalProperties().deconstructCoating(frontGlass(), Side::Front)};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, Interlayer)
{
    const auto laminate{Assembly(surfaceOf(frontGlass()))
                          .add(bulkOf(frontGlass()), noReflection())
                          .add(interlayer(), uncoatedBackPane())};

    const auto result{
      laminate.externalProperties().deconstructInterlayer(frontGlass(), backGlass())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value.tau, interlayer().tau, Tolerance);
}

TEST(CompoundProductRoundTrip, EmbeddedCoating)
{
    const auto laminate{Assembly(surfaceOf(frontGlass()))
                          .add(bulkOf(frontGlass()), coating())
                          .add(interlayer(), uncoatedBackPane())};

    const auto result{laminate.externalProperties().deconstructEmbeddedCoating(
      frontGlass(), backGlass(), interlayer())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, DoubleMonolithicUncoatedSurfaces)
{
    const auto layers{Assembly(surfaceOf(frontGlass()))
                        .add(bulkOf(frontGlass()), coating())
                        .add(bulkOf(backGlass()), surfaceOf(backGlass()))};

    const auto result{
      layers.externalProperties().deconstructDoubleMonolithic(frontGlass(), backGlass())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, DoubleMonolithicAbsorbingSurfaces)
{
    const Interface frontSurface{.tf = series(wavelengths, {0.6, 0.7}),
                                 .tb = series(wavelengths, {0.6, 0.7}),
                                 .rf = series(wavelengths, {0.2, 0.1}),
                                 .rb = series(wavelengths, {0.15, 0.12})};
    const Interface backSurface{.tf = series(wavelengths, {0.66, 0.8}),
                                .tb = series(wavelengths, {0.66, 0.8}),
                                .rf = series(wavelengths, {0.12, 0.07}),
                                .rb = series(wavelengths, {0.18, 0.09})};
    const auto layers{Assembly(frontSurface)
                        .add(bulkOf(frontGlass()), coating())
                        .add(bulkOf(backGlass()), backSurface)};

    const auto result{layers.externalProperties().deconstructDoubleMonolithic(frontSurface,
                                                  bulkOf(frontGlass()),
                                                  backSurface,
                                                  bulkOf(backGlass()))};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

// A measured pane is calculated for another thickness. Thickness belongs to the bulk layer only:
// the pane is deconstructed, the bulk layer is changed, and the pane is constructed again.
// This is newSubstrateThickness of the sources.
TEST(CompoundProductRoundTrip, MeasuredPaneAtAnotherThickness)
{
    constexpr double MeasuredThickness{3.0};
    constexpr double NewThickness{6.0};
    const auto measured{Assembly(surfaceOf(frontGlass()))
                          .add(bulkOf(frontGlass()), surfaceOf(frontGlass()))
                          .externalProperties()};

    const auto glass{measured.deconstructMonolithic().value};
    const auto result{
      Assembly(uncoatedInterface(glass))
        .add(withThickness(bulkOf(glass), MeasuredThickness, NewThickness), uncoatedInterface(glass))
        .externalProperties()};

    const auto expected{
      Assembly(surfaceOf(frontGlass()))
        .add(withThickness(bulkOf(frontGlass()), MeasuredThickness, NewThickness),
             surfaceOf(frontGlass()))
        .externalProperties()};
    expectNear(result, expected, Tolerance);
}

// A laminate is constructed from two measured panes and a known interlayer. Each pane is
// deconstructed, and the laminate is written as an assembly of the results. The interfaces
// between glass and interlayer do not reflect. This is genlam of the sources for a laminate.
TEST(CompoundProductRoundTrip, LaminateFromTwoMeasuredPanes)
{
    const auto measuredFront{Assembly(surfaceOf(frontGlass()))
                               .add(bulkOf(frontGlass()), surfaceOf(frontGlass()))
                               .externalProperties()};
    const auto measuredBack{Assembly(surfaceOf(backGlass()))
                              .add(bulkOf(backGlass()), surfaceOf(backGlass()))
                              .externalProperties()};

    const auto front{measuredFront.deconstructMonolithic().value};
    const auto back{measuredBack.deconstructMonolithic().value};
    const auto laminate{Assembly(uncoatedInterface(front))
                          .add(bulkOf(front), noReflection())
                          .add(interlayer(), noReflection())
                          .add(bulkOf(back), uncoatedInterface(back))
                          .externalProperties()};

    const auto expected{Assembly(surfaceOf(frontGlass()))
                          .add(bulkOf(frontGlass()), noReflection())
                          .add(interlayer(), noReflection())
                          .add(bulkOf(backGlass()), surfaceOf(backGlass()))
                          .externalProperties()};
    expectNear(laminate, expected, Tolerance);
}
