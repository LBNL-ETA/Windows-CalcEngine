#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "CompoundProductTestHelper.hpp"

// First group (CompoundProductEquivalence): a case function and the general function are called
// on the same measured product and the same numbers are expected. The measured product is built
// from chosen internal properties at wavelengths where no warning is raised. The largest
// difference between the two results is recorded as the property "largestDifference" of the test.
//
// Second group (CompoundProductCaseRules): data at which a case function is expected to differ
// from the general function. These tests state what each case function adds.

using namespace CompoundProduct;
using namespace CompoundProductTest;
using FenestrationCommon::CSeries;
using FenestrationCommon::Side;

namespace
{
    const std::vector wavelengths{0.3, 0.55, 1.0, 2.5};
    constexpr double Tolerance{1e-12};

    Monolithic frontGlass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.05, 0.04, 0.042, 0.045}),
                .tau = series(wavelengths, {0.4, 0.9, 0.8, 0.6})};
    }

    Monolithic backGlass()
    {
        return {.surfaceReflectance = series(wavelengths, {0.048, 0.045, 0.041, 0.043}),
                .tau = series(wavelengths, {0.6, 0.95, 0.8, 0.7})};
    }

    Bulk bulkOf(const Monolithic & monolithic)
    {
        return {.tau = monolithic.tau};
    }

    Bulk interlayer()
    {
        return {.tau = series(wavelengths, {0.7, 0.97, 0.9, 0.8})};
    }

    Bulk fullyTransparentBulk()
    {
        return {.tau = series(wavelengths, {1.0, 1.0, 1.0, 1.0})};
    }

    Interface coating()
    {
        return {.tf = series(wavelengths, {0.2, 0.5, 0.6, 0.3}),
                .tb = series(wavelengths, {0.18, 0.45, 0.55, 0.35}),
                .rf = series(wavelengths, {0.4, 0.1, 0.2, 0.5}),
                .rb = series(wavelengths, {0.35, 0.3, 0.25, 0.45})};
    }

    Assembly bareSurfaceOf(const Monolithic & monolithic)
    {
        return Assembly(uncoatedInterface(monolithic));
    }

    // Front pane of a laminate: bare surface, glass, non-reflecting interface to the interlayer.
    Assembly frontPane()
    {
        return bareSurfaceOf(frontGlass())
          .add(bulkOf(frontGlass()), nonReflectingInterface(wavelengths));
    }

    // Back pane of a laminate: non-reflecting interface to the interlayer, glass, bare surface.
    Assembly backPane()
    {
        return Assembly(nonReflectingInterface(wavelengths))
          .add(bulkOf(backGlass()), uncoatedInterface(backGlass()));
    }

    double largestDifference(const CSeries & first, const CSeries & second)
    {
        const auto firstValues{first.getYArray()};
        const auto secondValues{second.getYArray()};
        if(firstValues.size() != secondValues.size())
        {
            return std::numeric_limits<double>::infinity();
        }
        return std::inner_product(
          firstValues.begin(),
          firstValues.end(),
          secondValues.begin(),
          0.0,
          [](const double largest, const double difference) {
              return std::max(largest, difference);
          },
          [](const double firstValue, const double secondValue) {
              return std::abs(firstValue - secondValue);
          });
    }

    //! Largest difference over the four properties between an interface and a part that
    //! consists of one interface.
    double largestDifference(const Interface & layerInterface, const ExternalProperties & part)
    {
        return std::max({largestDifference(layerInterface.tf, part.Tf),
                         largestDifference(layerInterface.tb, part.Tb),
                         largestDifference(layerInterface.rf, part.Rf),
                         largestDifference(layerInterface.rb, part.Rb)});
    }

    std::string scientific(const double value)
    {
        std::ostringstream stream;
        stream << std::scientific << std::setprecision(3) << value;
        return stream.str();
    }

    void expectNoDifference(const double difference)
    {
        ::testing::Test::RecordProperty("largestDifference", scientific(difference));
        EXPECT_LE(difference, Tolerance);
    }
}   // namespace

///////////////////////////////////////////////////////////////////////////
/// The case function and the general function give the same numbers
///////////////////////////////////////////////////////////////////////////

// The front part is the bare front surface of the substrate, the bulk layer is the glass of the
// substrate. The coating is the part that remains.
TEST(CompoundProductEquivalence, CoatingOnBackIsThePartBehindTheBareSurface)
{
    const auto substrate{frontGlass()};
    const auto measured{
      bareSurfaceOf(substrate).add(bulkOf(substrate), coating()).externalProperties()};

    const auto caseResult{measured.deconstructCoating(substrate, Side::Back)};
    const auto generalResult{
      measured.deconstructPartBehind(bareSurfaceOf(substrate), bulkOf(substrate))};

    EXPECT_TRUE(caseResult.diagnostics.empty());
    EXPECT_TRUE(generalResult.diagnostics.empty());
    expectNoDifference(largestDifference(caseResult.value, generalResult.value));
}

// The back part is the bare back surface of the substrate.
TEST(CompoundProductEquivalence, CoatingOnFrontIsThePartInFrontOfTheBareSurface)
{
    const auto substrate{frontGlass()};
    const auto measured{
      Assembly(coating()).add(bulkOf(substrate), uncoatedInterface(substrate)).externalProperties()};

    const auto caseResult{measured.deconstructCoating(substrate, Side::Front)};
    const auto generalResult{
      measured.deconstructPartInFrontOf(bulkOf(substrate), bareSurfaceOf(substrate))};

    EXPECT_TRUE(caseResult.diagnostics.empty());
    EXPECT_TRUE(generalResult.diagnostics.empty());
    expectNoDifference(largestDifference(caseResult.value, generalResult.value));
}

// Each pane is a known part that ends with a non-reflecting interface towards the interlayer.
TEST(CompoundProductEquivalence, InterlayerIsTheBulkBetweenTwoPanes)
{
    const auto measured{frontPane().add(interlayer(), backPane()).externalProperties()};

    const auto caseResult{measured.deconstructInterlayer(frontGlass(), backGlass())};
    const auto generalResult{measured.deconstructBulk(frontPane(), backPane())};

    EXPECT_TRUE(caseResult.diagnostics.empty());
    EXPECT_TRUE(generalResult.diagnostics.empty());
    expectNoDifference(largestDifference(caseResult.value.tau,
                                         generalResult.value.tau));
}

// The front part is the bare surface of the front pane and the bulk layer in front of the
// coating is the front glass. The bulk layer behind the coating is the interlayer and the back
// part is the back pane with its non-reflecting interface towards the interlayer.
TEST(CompoundProductEquivalence, EmbeddedCoatingIsTheInterfaceBetweenFrontSurfaceAndBackPane)
{
    const auto measured{bareSurfaceOf(frontGlass())
                          .add(bulkOf(frontGlass()), coating())
                          .add(interlayer(), backPane())
                          .externalProperties()};

    const auto caseResult{
      measured.deconstructEmbeddedCoating(frontGlass(), backGlass(), interlayer())};
    const auto generalResult{measured.deconstructPartInTheMiddle(
      bareSurfaceOf(frontGlass()), bulkOf(frontGlass()), interlayer(), backPane())};

    EXPECT_TRUE(caseResult.diagnostics.empty());
    EXPECT_TRUE(generalResult.diagnostics.empty());
    expectNoDifference(largestDifference(caseResult.value, generalResult.value));
}

// The known parts are the two bare outer surfaces, the bulk layers are the two thick layers.
TEST(CompoundProductEquivalence, DoubleMonolithicIsTheInterfaceBetweenTwoBareSurfaces)
{
    const auto measured{bareSurfaceOf(frontGlass())
                          .add(bulkOf(frontGlass()), coating())
                          .add(bulkOf(backGlass()), uncoatedInterface(backGlass()))
                          .externalProperties()};

    const auto caseResult{measured.deconstructDoubleMonolithic(frontGlass(), backGlass())};
    const auto generalResult{measured.deconstructPartInTheMiddle(bareSurfaceOf(frontGlass()),
                                                           bulkOf(frontGlass()),
                                                           bulkOf(backGlass()),
                                                           bareSurfaceOf(backGlass()))};

    EXPECT_TRUE(caseResult.diagnostics.empty());
    EXPECT_TRUE(generalResult.diagnostics.empty());
    expectNoDifference(largestDifference(caseResult.value, generalResult.value));
}

// Two general functions on the same laminate. An unknown bulk layer between two panes is also an
// unknown interface that has a fully transparent bulk layer on each side. That interface has the
// internal transmittance of the bulk layer as its two transmittances and does not reflect.
//
// The calculated reflectances are zero within the rounding of the computer and some of them are
// below zero by about 1e-13. With the default tolerances such a value is moved to zero and the
// warning UnphysicalClamped is raised. The results are compared without that correction.
TEST(CompoundProductEquivalence, BulkBetweenTwoPanesIsAnInterfaceThatDoesNotReflect)
{
    const auto measured{frontPane().add(interlayer(), backPane()).externalProperties()};
    const Tolerances withoutCorrection{.clampToPhysical = false};

    const auto asPart{measured.deconstructPartInTheMiddle(frontPane(),
                                                         fullyTransparentBulk(),
                                                         fullyTransparentBulk(),
                                                         backPane(),
                                                         withoutCorrection)};
    const auto corrected{measured.deconstructPartInTheMiddle(
      frontPane(), fullyTransparentBulk(), fullyTransparentBulk(), backPane())};
    EXPECT_FALSE(corrected.diagnostics.empty());
    for(const auto & diagnostic : corrected.diagnostics)
    {
        EXPECT_EQ(diagnostic.warning, Warning::UnphysicalClamped);
    }
    const auto asBulk{measured.deconstructBulk(frontPane(), backPane())};

    const auto & tau{asBulk.value.tau};
    const Interface expected{.tf = tau,
                             .tb = tau,
                             .rf = series(wavelengths, {0.0, 0.0, 0.0, 0.0}),
                             .rb = series(wavelengths, {0.0, 0.0, 0.0, 0.0})};
    EXPECT_TRUE(asPart.diagnostics.empty());
    EXPECT_TRUE(asBulk.diagnostics.empty());
    expectNoDifference(largestDifference(expected, asPart.value));
}

///////////////////////////////////////////////////////////////////////////
/// What the case functions add to the general functions
///////////////////////////////////////////////////////////////////////////

namespace
{
    // The first wavelength is an ordinary one. The second one triggers the rule of the test.
    const std::vector<double> twoWavelengths{0.55, 2.5};

    Monolithic glassWith(const double secondInternalTransmittance)
    {
        return {.surfaceReflectance = series(twoWavelengths, {0.04, 0.045}),
                .tau =
                  series(twoWavelengths, {0.9, secondInternalTransmittance})};
    }

    Interface coatingAtTwoWavelengths()
    {
        return {.tf = series(twoWavelengths, {0.5, 0.3}),
                .tb = series(twoWavelengths, {0.45, 0.35}),
                .rf = series(twoWavelengths, {0.1, 0.5}),
                .rb = series(twoWavelengths, {0.3, 0.45})};
    }

    ExternalProperties coatedOnBack(const Monolithic & substrate)
    {
        return bareSurfaceOf(substrate)
          .add(bulkOf(substrate), coatingAtTwoWavelengths())
          .externalProperties();
    }
}   // namespace

// Substrate that does not transmit. The general function reports an opaque layer and returns
// zeros. The case function returns the result of Optics 5.1: a coating that does not transmit,
// reflects everything towards the substrate, and has the measured reflectance on its air side.
TEST(CompoundProductCaseRules, CoatingOnOpaqueSubstrate)
{
    const auto substrate{glassWith(0.0)};
    const auto measured{coatedOnBack(substrate)};

    const auto caseResult{measured.deconstructCoating(substrate, Side::Back)};
    const auto generalResult{
      measured.deconstructPartBehind(bareSurfaceOf(substrate), bulkOf(substrate))};

    ASSERT_EQ(caseResult.diagnostics.size(), 1U);
    EXPECT_TRUE(hasWarning(caseResult.diagnostics, 2.5, Warning::OpaqueSubstrate));
    expectNear(caseResult.value.tf, {0.5, 0.0}, Tolerance);
    expectNear(caseResult.value.tb, {0.45, 0.0}, Tolerance);
    expectNear(caseResult.value.rf, {0.1, 1.0}, Tolerance);
    expectNear(caseResult.value.rb, {0.3, 0.45}, Tolerance);

    ASSERT_EQ(generalResult.diagnostics.size(), 1U);
    EXPECT_TRUE(hasWarning(generalResult.diagnostics, 2.5, Warning::OpaqueLayer));
    expectNear(generalResult.value.Tf, {0.5, 0.0}, Tolerance);
    expectNear(generalResult.value.Tb, {0.45, 0.0}, Tolerance);
    expectNear(generalResult.value.Rf, {0.1, 0.0}, Tolerance);
    expectNear(generalResult.value.Rb, {0.3, 0.0}, Tolerance);
}

// Substrate with an internal transmittance below the sensitivity threshold. The numbers are the
// same. The case function adds a warning.
TEST(CompoundProductCaseRules, CoatingOnSubstrateBelowSensitivityThreshold)
{
    const auto substrate{glassWith(0.2)};
    const auto measured{coatedOnBack(substrate)};

    const auto caseResult{measured.deconstructCoating(substrate, Side::Back)};
    const auto generalResult{
      measured.deconstructPartBehind(bareSurfaceOf(substrate), bulkOf(substrate))};

    ASSERT_EQ(caseResult.diagnostics.size(), 1U);
    EXPECT_TRUE(
      hasWarning(caseResult.diagnostics, 2.5, Warning::InsensitiveCoatingSubstrateSide));
    EXPECT_TRUE(generalResult.diagnostics.empty());
    expectNoDifference(largestDifference(caseResult.value, generalResult.value));
}

// Panes with an internal transmittance below the sensitivity threshold of the interlayer. The
// numbers are the same. The case function adds a warning.
TEST(CompoundProductCaseRules, InterlayerBetweenPanesBelowSensitivityThreshold)
{
    const auto glass{glassWith(0.2)};
    const Bulk interlayerBulk{.tau = series(twoWavelengths, {0.97, 0.8})};
    const auto front{
      bareSurfaceOf(glass).add(bulkOf(glass), nonReflectingInterface(twoWavelengths))};
    const auto back{front.flipped()};
    const auto measured{front.add(interlayerBulk, back).externalProperties()};

    const auto caseResult{measured.deconstructInterlayer(glass, glass)};
    const auto generalResult{measured.deconstructBulk(front, back)};

    ASSERT_EQ(caseResult.diagnostics.size(), 1U);
    EXPECT_TRUE(hasWarning(caseResult.diagnostics, 2.5, Warning::InsensitiveInterlayer));
    EXPECT_TRUE(generalResult.diagnostics.empty());
    expectNoDifference(largestDifference(caseResult.value.tau,
                                         generalResult.value.tau));
}

// Front glass that does not transmit. Optics 5.1 has no rule for an opaque layer in this case
// and returns values that are left over from a division by a very small number. The case
// function returns the result of the general function instead: zeros and the warning of an
// opaque layer.
TEST(CompoundProductCaseRules, EmbeddedCoatingBehindOpaqueFrontGlass)
{
    const auto front{glassWith(0.0)};
    const auto back{glassWith(0.8)};
    const Bulk interlayerBulk{.tau = series(twoWavelengths, {0.97, 0.8})};
    const auto backPart{Assembly(nonReflectingInterface(twoWavelengths))
                          .add(bulkOf(back), uncoatedInterface(back))};
    const auto measured{bareSurfaceOf(front)
                          .add(bulkOf(front), coatingAtTwoWavelengths())
                          .add(interlayerBulk, backPart)
                          .externalProperties()};

    const auto caseResult{measured.deconstructEmbeddedCoating(front, back, interlayerBulk)};
    const auto generalResult{measured.deconstructPartInTheMiddle(
      bareSurfaceOf(front), bulkOf(front), interlayerBulk, backPart)};

    ASSERT_EQ(caseResult.diagnostics.size(), 1U);
    EXPECT_TRUE(hasWarning(caseResult.diagnostics, 2.5, Warning::OpaqueLayer));
    expectNear(caseResult.value.tf, {0.5, 0.0}, Tolerance);
    expectNear(caseResult.value.tb, {0.45, 0.0}, Tolerance);
    expectNear(caseResult.value.rf, {0.1, 0.0}, Tolerance);
    expectNear(caseResult.value.rb, {0.3, 0.0}, Tolerance);
    ASSERT_EQ(generalResult.diagnostics.size(), 1U);
    EXPECT_TRUE(hasWarning(generalResult.diagnostics, 2.5, Warning::OpaqueLayer));
    expectNoDifference(largestDifference(caseResult.value, generalResult.value));
}

///////////////////////////////////////////////////////////////////////////
/// What the measurement says about an interlayer
///////////////////////////////////////////////////////////////////////////

// The laminate of these tests is not measured. It is constructed here from known parts, so that
// the true values inside it are known:
//
//      air |   front glass   |          interlayer          |   back glass   | air
//          ^                 ^                              ^                ^
//      bare surface      interface                      interface      bare surface
//                        r = 0.004                      r = 0.004
//
//          |<-- front pane ->|<---- part in the middle ---->|<- back pane -->|
//
// Input of every test: the external properties of this laminate (Tf, Tb, Rf, Rb), and the two
// glasses as they are known from the measurement of a single pane. The interlayer and its two
// interfaces are not given to the deconstruction.
//
// Output of every test: what the deconstruction returns. It is compared with the interlayer and
// the interfaces from which the laminate was constructed.
//
// Every list of numbers holds one value for each of the four wavelengths. Results of the
// deconstruction are stated to six decimals.

namespace
{
    constexpr double InterfaceReflectance{0.004};
    constexpr double SixDecimals{1e-6};

    // True value. An interface between glass and interlayer that reflects a little and does not
    // absorb.
    Interface glassToInterlayer()
    {
        const auto reflectance{series(wavelengths, std::vector<double>(4U, InterfaceReflectance))};
        const auto transmittance{
          series(wavelengths, std::vector<double>(4U, 1.0 - InterfaceReflectance))};
        return {.tf = transmittance, .tb = transmittance, .rf = reflectance, .rb = reflectance};
    }

    // True value. The part in the middle: interface, interlayer, interface.
    Assembly interlayerWithItsInterfaces()
    {
        return Assembly(glassToInterlayer()).add(interlayer(), glassToInterlayer());
    }

    // Input. The laminate as a measurement gives it.
    ExternalProperties laminateWithReflectingInterlayerInterfaces()
    {
        return bareSurfaceOf(frontGlass())
          .add(bulkOf(frontGlass()), interlayerWithItsInterfaces())
          .add(bulkOf(backGlass()), uncoatedInterface(backGlass()))
          .externalProperties();
    }

    // Output. The bare surface and the glass of each pane are removed from the laminate. No
    // assumption is made about what lies between the panes.
    ExternalProperties partBetweenThePanes(const ExternalProperties & laminate)
    {
        return laminate
          .deconstructPartInTheMiddle(bareSurfaceOf(frontGlass()),
                                      bulkOf(frontGlass()),
                                      bulkOf(backGlass()),
                                      bareSurfaceOf(backGlass()))
          .value;
    }
}   // namespace

// The case function (LaminateEquations (8)) is given everything that it accepts: the laminate
// and the two glasses. It has no argument for the interfaces between glass and interlayer,
// because it assumes that they do not reflect. It uses the transmittance of the laminate only.
// The light that is reflected at the interfaces is then counted as absorption, and the internal
// transmittance that it returns is lower than the true one.
TEST(CompoundProductInterlayerCheck, InterlayerCaseGivesTooLowTauWhenInterfacesReflect)
{
    const auto laminate{laminateWithReflectingInterlayerInterfaces()};

    const auto result{laminate.deconstructInterlayer(frontGlass(), backGlass())};

    // interlayer() is used above in laminateWithReflectingInterlayerInterfaces() but with small
    // reflectance interfaces.
    expectNear(interlayer().tau, {0.700000, 0.970000, 0.900000, 0.800000}, Tolerance);
    expectNear(result.value.tau, {0.694521, 0.962813, 0.893168, 0.793814}, SixDecimals);
}

// The removal of both panes makes no assumption about the interfaces. The part that remains is
// the interlayer together with its two interfaces, as a whole. With interfaces that do not
// reflect, this part would have the reflectance 0.
//
// The transmittance of this part differs in the fourth decimal from the result of the case
// function in the test above. Neither of the two is the internal transmittance of the
// interlayer, because both still hold the effect of the two interfaces.
TEST(CompoundProductInterlayerCheck, RemovingBothPanesLeavesInterlayerWithItsInterfaces)
{
    const auto laminate{laminateWithReflectingInterlayerInterfaces()};

    const auto middle{partBetweenThePanes(laminate)};

    expectNear(middle.Tf, {0.694417, 0.962270, 0.892826, 0.793621}, SixDecimals);
    expectNear(middle.Tb, {0.694417, 0.962270, 0.892826, 0.793621}, SixDecimals);
    expectNear(middle.Rf, {0.005944, 0.007734, 0.007214, 0.006540}, SixDecimals);
    expectNear(middle.Rb, {0.005944, 0.007734, 0.007214, 0.006540}, SixDecimals);
    expectNear(middle, interlayerWithItsInterfaces().externalProperties(), Tolerance);
}

// The part between the panes is read as two equal interfaces with a bulk layer between them, in
// the same way as a single pane in air. This gives the reflectance of the interfaces and the
// internal transmittance of the interlayer from which the laminate was constructed.
TEST(CompoundProductInterlayerCheck, PartBetweenThePanesGivesInterfaceReflectanceAndTau)
{
    const auto laminate{laminateWithReflectingInterlayerInterfaces()};

    const auto parts{partBetweenThePanes(laminate).deconstructMonolithic()};

    expectNear(parts.value.surfaceReflectance, {0.004, 0.004, 0.004, 0.004}, 1e-9);
    expectNear(parts.value.tau, {0.7, 0.97, 0.9, 0.8}, 1e-9);
}
