#include <cmath>
#include <stdexcept>

#include <gtest/gtest.h>

#include "CompoundProductTestHelper.hpp"

using namespace CompoundProduct;
using namespace CompoundProductTest;

namespace
{
    const std::vector<double> wavelengths{0.4, 0.6};

    Monolithic glass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.04, 0.05}),
                .internalTransmittance = series(wavelengths, {0.9, 0.6})};
    }

    Interface coating()
    {
        return {.transmittanceFront = series(wavelengths, {0.5, 0.6}),
                .transmittanceBack = series(wavelengths, {0.45, 0.55}),
                .reflectanceFront = series(wavelengths, {0.1, 0.2}),
                .reflectanceBack = series(wavelengths, {0.3, 0.25})};
    }

    Bulk glassBulk()
    {
        return {.internalTransmittance = glass().internalTransmittance};
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
      result.transmittanceFront, {transmittance(0.04, 0.9), transmittance(0.05, 0.6)}, 1e-12);
    expectNear(result.transmittanceBack, result.transmittanceFront, 1e-12);
    expectNear(result.reflectanceFront, {reflectance(0.04, 0.9), reflectance(0.05, 0.6)}, 1e-12);
    expectNear(result.reflectanceBack, result.reflectanceFront, 1e-12);
}

TEST(CompoundProductConstruction, FlippedAssemblyIsTheProductSeenFromTheBack)
{
    const auto original{coatedPane().externalProperties()};
    const auto reversed{coatedPane().flipped().externalProperties()};

    expectNear(reversed, original.flipped(), 1e-12);
    expectNear(reversed.transmittanceFront, original.transmittanceBack, 1e-12);
    expectNear(reversed.reflectanceFront, original.reflectanceBack, 1e-12);
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
    const Interface front{.transmittanceFront = series(single, {0.7}),
                          .transmittanceBack = series(single, {0.6}),
                          .reflectanceFront = series(single, {0.1}),
                          .reflectanceBack = series(single, {0.2})};
    const Interface back{.transmittanceFront = series(single, {0.5}),
                         .transmittanceBack = series(single, {0.4}),
                         .reflectanceFront = series(single, {0.3}),
                         .reflectanceBack = series(single, {0.25})};
    const Bulk between{.internalTransmittance = series(single, {0.9})};

    const auto result{Assembly(front).add(between, back).externalProperties()};

    const auto denominator{1 - 0.2 * 0.3 * 0.81};
    expectNear(result.transmittanceFront, {0.7 * 0.5 * 0.9 / denominator}, 1e-12);
    expectNear(result.transmittanceBack, {0.6 * 0.4 * 0.9 / denominator}, 1e-12);
    expectNear(result.reflectanceFront, {0.1 + 0.7 * 0.6 * 0.3 * 0.81 / denominator}, 1e-12);
    expectNear(result.reflectanceBack, {0.25 + 0.5 * 0.4 * 0.2 * 0.81 / denominator}, 1e-12);
}

// A part that is known only by its external properties gives the same product as the same part
// given by its components.
TEST(CompoundProductConstruction, AssemblyAcceptsPartsKnownAsAWhole)
{
    const Bulk between{.internalTransmittance = series(wavelengths, {0.97, 0.7})};
    const auto fromComponents{uncoatedPane().add(between, coatedPane()).externalProperties()};

    const auto fromWholeParts{Assembly(uncoatedPane().externalProperties())
                                .add(between, coatedPane().externalProperties())
                                .externalProperties()};

    expectNear(fromWholeParts, fromComponents, 1e-12);
}

// An assembly that is added as a part gives the same product as its components added one by one.
TEST(CompoundProductConstruction, AssemblyAcceptsAnAssemblyAsPart)
{
    const Bulk between{.internalTransmittance = series(wavelengths, {0.97, 0.7})};

    const auto result{coatedPane().add(between, uncoatedPane()).externalProperties()};

    const auto expected{Assembly(uncoatedInterface(glass()))
                          .add(glassBulk(), coating())
                          .add(between, uncoatedInterface(glass()))
                          .add(glassBulk(), uncoatedInterface(glass()))
                          .externalProperties()};
    expectNear(result, expected, 1e-12);
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
    const auto thick{
      Assembly(surface).add(withThickness(glassBulk(), 3.0, 6.0), surface).externalProperties()};

    expectNear(two, thick, 1e-12);
}

TEST(CompoundProductConstruction, WithThickness)
{
    const Bulk pane{.internalTransmittance = glass().internalTransmittance};

    expectNear(withThickness(pane, 3.0, 6.0).internalTransmittance, {0.81, 0.36}, 1e-12);
    expectNear(withThickness(pane, 3.0, 1.5).internalTransmittance,
               {std::sqrt(0.9), std::sqrt(0.6)},
               1e-12);
    EXPECT_THROW((void)withThickness(pane, 0.0, 6.0), std::runtime_error);
}

TEST(CompoundProductConstruction, PartsAtDifferentWavelengthsThrow)
{
    const Bulk otherWavelengths{.internalTransmittance = series({0.4, 0.7}, {0.9, 0.6})};
    const auto surface{uncoatedInterface(glass())};

    const auto pane{Assembly(surface).add(otherWavelengths, surface)};

    EXPECT_THROW((void)pane.externalProperties(), std::runtime_error);
}

TEST(CompoundProductConstruction, CommonWavelengths)
{
    const auto product{[](const std::vector<double> & lambdas) {
        const std::vector<double> zeros(lambdas.size(), 0.0);
        return ExternalProperties{.transmittanceFront = series(lambdas, zeros),
                                  .transmittanceBack = series(lambdas, zeros),
                                  .reflectanceFront = series(lambdas, zeros),
                                  .reflectanceBack = series(lambdas, zeros)};
    }};

    const auto result{commonWavelengths({product({0.3, 0.5, 0.7}), product({0.4, 0.5, 2.5})})};

    const std::vector<double> expected{0.3, 0.4, 0.5, 0.7, 2.5};
    EXPECT_EQ(result, expected);
}
