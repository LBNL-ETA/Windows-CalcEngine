#include <cmath>
#include <stdexcept>

#include <gtest/gtest.h>

#include "CompoundProductTestHelper.hpp"

using namespace CompoundProduct;
using namespace CompoundProductTest;

namespace
{
    const std::vector<double> wavelengths{0.4, 0.6};

    // Thicknesses of the layers of the tests. The unit is free.
    constexpr double GlassThickness{6.0};
    constexpr double InterlayerThickness{0.76};

    Monolithic glass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.04, 0.05}),
                .bulk = {.tau = series(wavelengths, {0.9, 0.6}), .thickness = GlassThickness}};
    }

    Interface coating()
    {
        return {.tf = series(wavelengths, {0.5, 0.6}),
                .tb = series(wavelengths, {0.45, 0.55}),
                .rf = series(wavelengths, {0.1, 0.2}),
                .rb = series(wavelengths, {0.3, 0.25})};
    }

    Bulk glassBulk()
    {
        return glass().bulk;
    }

    Assembly uncoatedPane()
    {
        return Assembly(uncoatedInterface(glass())).add(glassBulk(), uncoatedInterface(glass()));
    }

    Assembly coatedPane()
    {
        return Assembly(uncoatedInterface(glass())).add(glassBulk(), coating());
    }
}   // namespace

TEST(CompoundProductConstruction, MonolithicClosedForm)
{
    const auto result{uncoatedPane().externalProperties()};

    // T = t^2 * tau / (1 - r^2 * tau^2), R = r + t^2 * r * tau^2 / (1 - r^2 * tau^2)
    const auto transmittance{[](const double rfl, const double tau) {
        return (1 - rfl) * (1 - rfl) * tau / (1 - rfl * rfl * tau * tau);
    }};
    const auto reflectance{[](const double rfl, const double tau) {
        return rfl + (1 - rfl) * (1 - rfl) * rfl * tau * tau / (1 - rfl * rfl * tau * tau);
    }};

    expectNear(
      result.Tf, {transmittance(0.04, 0.9), transmittance(0.05, 0.6)}, 1e-12);
    expectNear(result.Tb, result.Tf, 1e-12);
    expectNear(result.Rf, {reflectance(0.04, 0.9), reflectance(0.05, 0.6)}, 1e-12);
    expectNear(result.Rb, result.Rf, 1e-12);
}

TEST(CompoundProductConstruction, FlippedAssemblyIsTheProductSeenFromTheBack)
{
    const auto original{coatedPane().externalProperties()};
    const auto reversed{coatedPane().flipped().externalProperties()};

    expectNear(reversed, original.flipped(), 1e-12);
    expectNear(reversed.Tf, original.Tb, 1e-12);
    expectNear(reversed.Rf, original.Rb, 1e-12);
}

TEST(CompoundProductConstruction, FlippedDoesNotChangeTheAssembly)
{
    const auto pane{coatedPane()};
    const auto before{pane.externalProperties()};

    const auto reversed{pane.flipped()};

    expectNear(pane.externalProperties(), before, 1e-12);
    expectNear(reversed.flipped().externalProperties(), before, 1e-12);
}

// Two interfaces with different front and back transmittances and one bulk layer between them.
// D = 1 - rbA * rfB * tau^2
// Tf = tfA * tfB * tau / D,  Tb = tbA * tbB * tau / D
// Rf = rfA + tfA * tbA * rfB * tau^2 / D,  Rb = rbB + tfB * tbB * rbA * tau^2 / D
TEST(CompoundProductConstruction, JoinOfTwoInterfacesWithFourProperties)
{
    const std::vector<double> single{0.5};
    const Interface front{.tf = series(single, {0.7}),
                          .tb = series(single, {0.6}),
                          .rf = series(single, {0.1}),
                          .rb = series(single, {0.2})};
    const Interface back{.tf = series(single, {0.5}),
                         .tb = series(single, {0.4}),
                         .rf = series(single, {0.3}),
                         .rb = series(single, {0.25})};
    const Bulk between{.tau = series(single, {0.9}), .thickness = GlassThickness};

    const auto result{Assembly(front).add(between, back).externalProperties()};

    const auto denominator{1 - 0.2 * 0.3 * 0.81};
    expectNear(result.Tf, {0.7 * 0.5 * 0.9 / denominator}, 1e-12);
    expectNear(result.Tb, {0.6 * 0.4 * 0.9 / denominator}, 1e-12);
    expectNear(result.Rf, {0.1 + 0.7 * 0.6 * 0.3 * 0.81 / denominator}, 1e-12);
    expectNear(result.Rb, {0.25 + 0.5 * 0.4 * 0.2 * 0.81 / denominator}, 1e-12);
}

// A part that is known only by its external properties gives the same product as the same part
// given by its components.
TEST(CompoundProductConstruction, AssemblyAcceptsPartsKnownAsAWhole)
{
    const Bulk between{.tau = series(wavelengths, {0.97, 0.7}), .thickness = GlassThickness};
    const auto fromComponents{uncoatedPane().add(between, coatedPane()).externalProperties()};

    const auto fromWholeParts{Assembly(uncoatedPane().externalProperties())
                                .add(between, coatedPane().externalProperties())
                                .externalProperties()};

    expectNear(fromWholeParts, fromComponents, 1e-12);
}

// An assembly that is added as a part gives the same product as its components added one by one.
TEST(CompoundProductConstruction, AssemblyAcceptsAnAssemblyAsPart)
{
    const Bulk between{.tau = series(wavelengths, {0.97, 0.7}), .thickness = GlassThickness};

    const auto result{coatedPane().add(between, uncoatedPane()).externalProperties()};

    const auto expected{Assembly(uncoatedInterface(glass()))
                          .add(glassBulk(), coating())
                          .add(between, uncoatedInterface(glass()))
                          .add(glassBulk(), uncoatedInterface(glass()))
                          .externalProperties()};
    expectNear(result, expected, 1e-12);
}

// The components of an assembly are read back in the order in which they lie in the product.
TEST(CompoundProductConstruction, AssemblyShowsItsComponents)
{
    const auto pane{coatedPane()};

    expectNear(pane.front(), uncoatedInterface(glass()), 1e-12);
    ASSERT_EQ(pane.segments().size(), 1U);
    expectNear(pane.segments()[0].bulk.tau, glassBulk().tau, 1e-12);
    expectNear(pane.segments()[0].backInterface, coating(), 1e-12);
}

// The flipped pane starts with the coating seen from its other side.
TEST(CompoundProductConstruction, FlippedAssemblyShowsItsComponentsInReverseOrder)
{
    const auto flippedPane{coatedPane().flipped()};

    expectNear(flippedPane.front(), coating().flipped(), 1e-12);
    ASSERT_EQ(flippedPane.segments().size(), 1U);
    expectNear(flippedPane.segments()[0].bulk.tau, glassBulk().tau, 1e-12);
    expectNear(flippedPane.segments()[0].backInterface, uncoatedInterface(glass()), 1e-12);
}

// Nothing inside a part that is known only as a whole is known. It is one interface with the
// external properties of the part, and it has no segment.
TEST(CompoundProductConstruction, PartKnownAsAWholeIsOneInterface)
{
    const auto whole{coatedPane().externalProperties()};

    const Assembly part{whole};

    EXPECT_TRUE(part.segments().empty());
    expectNear(part.front().tf, whole.Tf, 1e-12);
    expectNear(part.front().tb, whole.Tb, 1e-12);
    expectNear(part.front().rf, whole.Rf, 1e-12);
    expectNear(part.front().rb, whole.Rb, 1e-12);
}

// An assembly that is added as a part keeps all its components.
TEST(CompoundProductConstruction, AddedAssemblyKeepsItsComponents)
{
    const auto laminate{coatedPane().add(glassBulk(), uncoatedPane())};

    ASSERT_EQ(laminate.segments().size(), 3U);
    expectNear(laminate.segments()[0].backInterface, coating(), 1e-12);
    expectNear(laminate.segments()[1].backInterface, uncoatedInterface(glass()), 1e-12);
    expectNear(laminate.segments()[2].backInterface, uncoatedInterface(glass()), 1e-12);
}

TEST(CompoundProductConstruction, AssemblyAddDoesNotChangeTheAssembly)
{
    const Assembly surface{uncoatedInterface(glass())};
    const auto before{surface.externalProperties()};

    const auto longer{surface.add(glassBulk(), coating())};

    expectNear(surface.externalProperties(), before, 1e-12);
    expectNear(longer.externalProperties(), coatedPane().externalProperties(), 1e-12);
}

TEST(CompoundProductConstruction, TwoIndexMatchedPanesEqualOneThickPane)
{
    const auto surface{uncoatedInterface(glass())};
    const auto two{Assembly(surface)
                     .add(glassBulk(), nonReflectingInterface(wavelengths))
                     .add(glassBulk(), surface)
                     .externalProperties()};
    const auto thick{Assembly(surface)
                       .add(glassBulk().atThickness(2.0 * GlassThickness), surface)
                       .externalProperties()};

    expectNear(two, thick, 1e-12);
}

// The glass of the tests is 6 thick. At twice the thickness the internal transmittance is
// squared, at half the thickness it is the square root. The result holds the new thickness, so
// that a second change needs no old thickness either.
TEST(CompoundProductConstruction, BulkAtAnotherThickness)
{
    const auto pane{glassBulk()};

    const auto thicker{pane.atThickness(12.0)};
    const auto thinner{pane.atThickness(3.0)};

    expectNear(pane.tau, {0.9, 0.6}, 1e-12);
    expectNear(thicker.tau, {0.81, 0.36}, 1e-12);
    expectNear(thinner.tau, {std::sqrt(0.9), std::sqrt(0.6)}, 1e-12);
    EXPECT_DOUBLE_EQ(thicker.thickness, 12.0);
    EXPECT_DOUBLE_EQ(thinner.thickness, 3.0);
    expectNear(thicker.atThickness(6.0).tau, pane.tau, 1e-12);
}

// A layer without a thickness cannot be changed to another thickness.
TEST(CompoundProductConstruction, BulkWithoutThicknessCannotChangeIt)
{
    const Bulk withoutThickness{.tau = glassBulk().tau, .thickness = 0.0};

    EXPECT_THROW((void)withoutThickness.atThickness(6.0), std::runtime_error);
    EXPECT_THROW((void)glassBulk().atThickness(-1.0), std::runtime_error);
}

TEST(CompoundProductConstruction, PartsAtDifferentWavelengthsThrow)
{
    const Bulk otherWavelengths{.tau = series({0.4, 0.7}, {0.9, 0.6}), .thickness = GlassThickness};
    const auto surface{uncoatedInterface(glass())};

    const auto pane{Assembly(surface).add(otherWavelengths, surface)};

    EXPECT_THROW((void)pane.externalProperties(), std::runtime_error);
}

TEST(CompoundProductConstruction, CommonWavelengths)
{
    const auto product{[](const std::vector<double> & lambdas) {
        const std::vector<double> zeros(lambdas.size(), 0.0);
        return ExternalProperties{.Tf = series(lambdas, zeros),
                                  .Tb = series(lambdas, zeros),
                                  .Rf = series(lambdas, zeros),
                                  .Rb = series(lambdas, zeros)};
    }};

    const auto result{commonWavelengths({product({0.3, 0.5, 0.7}), product({0.4, 0.5, 2.5})})};

    const std::vector<double> expected{0.3, 0.4, 0.5, 0.7, 2.5};
    EXPECT_EQ(result, expected);
}
