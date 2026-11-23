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
        return {.transmittance = series(wavelengths, {0.5, 0.6}),
                .reflectanceFront = series(wavelengths, {0.1, 0.2}),
                .reflectanceBack = series(wavelengths, {0.3, 0.25})};
    }

    Stack monolithicStack()
    {
        return {.interfaces = {uncoatedInterface(glass()), uncoatedInterface(glass())},
                .bulks = {Bulk{.internalTransmittance = glass().internalTransmittance}}};
    }

    Stack coatedStack()
    {
        return {.interfaces = {uncoatedInterface(glass()), coating()},
                .bulks = {Bulk{.internalTransmittance = glass().internalTransmittance}}};
    }
}   // namespace

TEST(CompoundProductConstruction, MonolithicClosedForm)
{
    const auto result{construct(monolithicStack())};

    // T = t^2 * tau / (1 - r^2 * tau^2), R = r + t^2 * r * tau^2 / (1 - r^2 * tau^2)
    const auto transmittance{[](const double rfl, const double tau) {
        return (1 - rfl) * (1 - rfl) * tau / (1 - rfl * rfl * tau * tau);
    }};
    const auto reflectance{[](const double rfl, const double tau) {
        return rfl + (1 - rfl) * (1 - rfl) * rfl * tau * tau / (1 - rfl * rfl * tau * tau);
    }};

    expectNear(result.transmittance, {transmittance(0.04, 0.9), transmittance(0.05, 0.6)}, 1e-12);
    expectNear(result.reflectanceFront, {reflectance(0.04, 0.9), reflectance(0.05, 0.6)}, 1e-12);
    expectNear(result.reflectanceBack, result.reflectanceFront, 1e-12);
}

TEST(CompoundProductConstruction, FlippedStackSwapsReflectances)
{
    const auto original{construct(coatedStack())};
    const auto reversed{construct(flipped(coatedStack()))};

    expectNear(reversed.transmittance, original.transmittance, 1e-12);
    expectNear(reversed.reflectanceFront, original.reflectanceBack, 1e-12);
    expectNear(reversed.reflectanceBack, original.reflectanceFront, 1e-12);
}

TEST(CompoundProductConstruction, JoinedStacksEqualOneStack)
{
    const auto between{nonReflectingInterface(wavelengths)};
    const auto result{joined(monolithicStack(), coatedStack(), between)};

    ASSERT_EQ(result.interfaces.size(), 3U);
    ASSERT_EQ(result.bulks.size(), 2U);

    const Stack expected{
      .interfaces = {uncoatedInterface(glass()), between, coating()},
      .bulks = {Bulk{.internalTransmittance = glass().internalTransmittance},
                Bulk{.internalTransmittance = glass().internalTransmittance}}};

    expectNear(construct(result).transmittance, construct(expected).transmittance, 1e-12);
    expectNear(construct(result).reflectanceFront, construct(expected).reflectanceFront, 1e-12);
    expectNear(construct(result).reflectanceBack, construct(expected).reflectanceBack, 1e-12);
}

TEST(CompoundProductConstruction, TwoIndexMatchedPanesEqualOneThickPane)
{
    const Bulk pane{.internalTransmittance = glass().internalTransmittance};
    const Stack two{.interfaces = {uncoatedInterface(glass()),
                                   nonReflectingInterface(wavelengths),
                                   uncoatedInterface(glass())},
                    .bulks = {pane, pane}};
    const Stack thick{.interfaces = {uncoatedInterface(glass()), uncoatedInterface(glass())},
                      .bulks = {withThickness(pane, 3.0, 6.0)}};

    expectNear(construct(two).transmittance, construct(thick).transmittance, 1e-12);
    expectNear(construct(two).reflectanceFront, construct(thick).reflectanceFront, 1e-12);
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

TEST(CompoundProductConstruction, WrongInputThrows)
{
    const Stack missingInterface{
      .interfaces = {uncoatedInterface(glass())},
      .bulks = {Bulk{.internalTransmittance = glass().internalTransmittance}}};
    EXPECT_THROW((void)construct(missingInterface), std::runtime_error);

    const Stack otherWavelengths{
      .interfaces = {uncoatedInterface(glass()), uncoatedInterface(glass())},
      .bulks = {Bulk{.internalTransmittance = series({0.4, 0.7}, {0.9, 0.6})}}};
    EXPECT_THROW((void)construct(otherWavelengths), std::runtime_error);
}

TEST(CompoundProductConstruction, CommonWavelengths)
{
    const auto product{[](const std::vector<double> & lambdas) {
        const std::vector<double> zeros(lambdas.size(), 0.0);
        return ExternalProperties{.transmittance = series(lambdas, zeros),
                                  .reflectanceFront = series(lambdas, zeros),
                                  .reflectanceBack = series(lambdas, zeros)};
    }};

    const auto result{commonWavelengths({product({0.3, 0.5, 0.7}), product({0.4, 0.5, 2.5})})};

    const std::vector<double> expected{0.3, 0.4, 0.5, 0.7, 2.5};
    EXPECT_EQ(result, expected);
}
