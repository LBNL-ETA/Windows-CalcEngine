#include <gtest/gtest.h>

#include "CompoundProductTestHelper.hpp"

// Every test builds a product from chosen internal properties, deconstructs the result and
// expects to get the chosen internal properties back.

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
                .internalTransmittance = series(wavelengths, {0.9, 0.6})};
    }

    Monolithic backGlass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.045, 0.042}),
                .internalTransmittance = series(wavelengths, {0.95, 0.8})};
    }

    Bulk bulkOf(const Monolithic & monolithic)
    {
        return {.internalTransmittance = monolithic.internalTransmittance};
    }

    Bulk interlayer()
    {
        return {.internalTransmittance = series(wavelengths, {0.97, 0.7})};
    }

    Interface coating()
    {
        return {.transmittance = series(wavelengths, {0.5, 0.6}),
                .reflectanceFront = series(wavelengths, {0.1, 0.2}),
                .reflectanceBack = series(wavelengths, {0.3, 0.25})};
    }

    Interface otherCoating()
    {
        return {.transmittance = series(wavelengths, {0.7, 0.4}),
                .reflectanceFront = series(wavelengths, {0.15, 0.3}),
                .reflectanceBack = series(wavelengths, {0.2, 0.35})};
    }

    // Front pane with a coating towards the air and an interface to an interlayer.
    Stack coatedFrontPane()
    {
        return {.interfaces = {otherCoating(), nonReflectingInterface(wavelengths)},
                .bulks = {bulkOf(frontGlass())}};
    }

    // Back pane with an interface to an interlayer and a bare surface towards the air.
    Stack uncoatedBackPane()
    {
        return {.interfaces = {nonReflectingInterface(wavelengths), uncoatedInterface(backGlass())},
                .bulks = {bulkOf(backGlass())}};
    }
}   // namespace

// A coating behind the middle pane of a laminate of three panes. No case of the sources covers
// this product.
TEST(CompoundProductRoundTrip, InterfaceInsideLaminateOfThreePanes)
{
    const auto noReflection{nonReflectingInterface(wavelengths)};
    const auto middleGlass{bulkOf(backGlass())};
    const Stack whole{.interfaces = {otherCoating(),
                                     noReflection,
                                     noReflection,
                                     coating(),
                                     noReflection,
                                     uncoatedInterface(backGlass())},
                      .bulks = {bulkOf(frontGlass()),
                                interlayer(),
                                middleGlass,
                                interlayer(),
                                bulkOf(backGlass())}};
    const Stack frontPart{.interfaces = {otherCoating(), noReflection, noReflection},
                          .bulks = {bulkOf(frontGlass()), interlayer()}};

    const auto result{deconstructInterface(
      construct(whole),
      {.externalProperties = construct(frontPart), .bulkToUnknownInterface = middleGlass},
      {.externalProperties = construct(uncoatedBackPane()),
       .bulkToUnknownInterface = interlayer()})};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, LastInterface)
{
    const Stack stack{.interfaces = {otherCoating(), coating()}, .bulks = {bulkOf(frontGlass())}};
    const Stack frontPart{.interfaces = {otherCoating()}, .bulks = {}};

    const auto result{deconstructLastInterface(
      construct(stack),
      {.externalProperties = construct(frontPart),
       .bulkToUnknownInterface = bulkOf(frontGlass())})};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, FirstInterface)
{
    const Stack stack{.interfaces = {coating(), otherCoating()}, .bulks = {bulkOf(frontGlass())}};
    const Stack backPart{.interfaces = {otherCoating()}, .bulks = {}};

    const auto result{deconstructFirstInterface(
      construct(stack),
      {.externalProperties = construct(backPart),
       .bulkToUnknownInterface = bulkOf(frontGlass())})};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

// An interlayer between a coated front pane and a back pane. The case of the sources needs two
// uncoated panes.
TEST(CompoundProductRoundTrip, BulkBetweenCoatedPanes)
{
    const Stack whole{.interfaces = {otherCoating(),
                                     nonReflectingInterface(wavelengths),
                                     nonReflectingInterface(wavelengths),
                                     uncoatedInterface(backGlass())},
                      .bulks = {bulkOf(frontGlass()), interlayer(), bulkOf(backGlass())}};

    const auto result{deconstructBulk(
      construct(whole), construct(coatedFrontPane()), construct(uncoatedBackPane()))};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value.internalTransmittance, interlayer().internalTransmittance, Tolerance);
}

TEST(CompoundProductRoundTrip, Monolithic)
{
    const Stack stack{
      .interfaces = {uncoatedInterface(frontGlass()), uncoatedInterface(frontGlass())},
      .bulks = {bulkOf(frontGlass())}};

    const auto result{deconstructMonolithic(construct(stack))};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value.surfaceReflectance, frontGlass().surfaceReflectance, Tolerance);
    expectNear(result.value.internalTransmittance, frontGlass().internalTransmittance, Tolerance);
}

TEST(CompoundProductRoundTrip, CoatingOnBack)
{
    const Stack stack{.interfaces = {uncoatedInterface(frontGlass()), coating()},
                      .bulks = {bulkOf(frontGlass())}};

    const auto result{deconstructCoating(construct(stack), frontGlass(), Side::Back)};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, CoatingOnFront)
{
    const Stack stack{.interfaces = {coating(), uncoatedInterface(frontGlass())},
                      .bulks = {bulkOf(frontGlass())}};

    const auto result{deconstructCoating(construct(stack), frontGlass(), Side::Front)};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, Interlayer)
{
    const auto noReflection{nonReflectingInterface(wavelengths)};
    const Stack stack{.interfaces = {uncoatedInterface(frontGlass()),
                                     noReflection,
                                     noReflection,
                                     uncoatedInterface(backGlass())},
                      .bulks = {bulkOf(frontGlass()), interlayer(), bulkOf(backGlass())}};

    const auto result{deconstructInterlayer(construct(stack), frontGlass(), backGlass())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value.internalTransmittance, interlayer().internalTransmittance, Tolerance);
}

TEST(CompoundProductRoundTrip, EmbeddedCoating)
{
    const Stack stack{.interfaces = {uncoatedInterface(frontGlass()),
                                     coating(),
                                     nonReflectingInterface(wavelengths),
                                     uncoatedInterface(backGlass())},
                      .bulks = {bulkOf(frontGlass()), interlayer(), bulkOf(backGlass())}};

    const auto result{
      deconstructEmbeddedCoating(construct(stack), frontGlass(), backGlass(), interlayer())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, DoubleMonolithicUncoatedSurfaces)
{
    const Stack stack{.interfaces = {uncoatedInterface(frontGlass()),
                                     coating(),
                                     uncoatedInterface(backGlass())},
                      .bulks = {bulkOf(frontGlass()), bulkOf(backGlass())}};

    const auto result{deconstructDoubleMonolithic(construct(stack), frontGlass(), backGlass())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}

TEST(CompoundProductRoundTrip, DoubleMonolithicAbsorbingSurfaces)
{
    const Interface frontSurface{.transmittance = series(wavelengths, {0.6, 0.7}),
                                 .reflectanceFront = series(wavelengths, {0.2, 0.1}),
                                 .reflectanceBack = series(wavelengths, {0.15, 0.12})};
    const Interface backSurface{.transmittance = series(wavelengths, {0.66, 0.8}),
                                .reflectanceFront = series(wavelengths, {0.12, 0.07}),
                                .reflectanceBack = series(wavelengths, {0.18, 0.09})};
    const Stack stack{.interfaces = {frontSurface, coating(), backSurface},
                      .bulks = {bulkOf(frontGlass()), bulkOf(backGlass())}};

    const auto result{deconstructDoubleMonolithic(
      construct(stack), frontSurface, bulkOf(frontGlass()), backSurface, bulkOf(backGlass()))};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value, coating(), Tolerance);
}
