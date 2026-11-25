#include <stdexcept>

#include <gtest/gtest.h>

#include "CompoundProductTestHelper.hpp"

// Measured data are six wavelengths taken from the sample products that came with the reference
// implementation of the equations (substrate, coated, laminate and laminate with embedded
// coating). Expected values were calculated with that reference implementation, which does not
// treat opaque wavelengths in any special way. Tolerances are therefore switched off where the
// results are compared with it.

using namespace CompoundProduct;
using namespace CompoundProductTest;
using FenestrationCommon::Side;

namespace
{
    const std::vector<double> wavelengths{0.300, 0.320, 0.400, 0.550, 1.000, 2.000};
    constexpr double Tolerance{1e-6};

    // Thicknesses of the layers of the tests. The unit is free.
    constexpr double GlassThickness{6.0};
    constexpr double InterlayerThickness{0.76};

    ExternalProperties product(const std::vector<double> & transmittance,
                               const std::vector<double> & reflectanceFront,
                               const std::vector<double> & reflectanceBack)
    {
        return {.Tf = series(wavelengths, transmittance),
                .Tb = series(wavelengths, transmittance),
                .Rf = series(wavelengths, reflectanceFront),
                .Rb = series(wavelengths, reflectanceBack)};
    }

    ExternalProperties substrate()
    {
        return product({0.202, 0.505, 0.908, 0.912, 0.864, 0.875},
                       {0.055, 0.070, 0.086, 0.083, 0.078, 0.073},
                       {0.055, 0.070, 0.086, 0.083, 0.078, 0.073});
    }

    //! Coating is on the back side.
    ExternalProperties coated()
    {
        return product({0.001, 0.009, 0.342, 0.733, 0.005, 0.001},
                       {0.276, 0.338, 0.300, 0.062, 0.934, 0.973},
                       {0.060, 0.084, 0.230, 0.076, 0.788, 0.851});
    }

    ExternalProperties laminate()
    {
        return product({0.003, 0.004, 0.656, 0.902, 0.804, 0.679},
                       {0.056, 0.058, 0.069, 0.084, 0.078, 0.063},
                       {0.056, 0.058, 0.069, 0.084, 0.078, 0.063});
    }

    ExternalProperties laminateWithEmbeddedCoating()
    {
        return product({0.002, 0.002, 0.237, 0.636, 0.005, 0.000},
                       {0.060, 0.083, 0.244, 0.154, 0.787, 0.851},
                       {0.056, 0.058, 0.195, 0.171, 0.808, 0.583});
    }

    //! Plain equations without any treatment of opaque wavelengths.
    Tolerances noTolerances()
    {
        return {.limitOfDetection = 0.0,
                .sensitivityThreshold = 0.0,
                .interlayerSensitivityThreshold = 0.0,
                .clampToPhysical = false};
    }

    Monolithic glass()
    {
        return substrate().deconstructMonolithic(GlassThickness, ReflectanceChoice::Average, noTolerances()).value;
    }

    Bulk interlayer()
    {
        return laminate().deconstructInterlayer(glass(), glass(), InterlayerThickness, noTolerances()).value;
    }
}   // namespace

TEST(CompoundProductReference, Monolithic)
{
    const auto result{glass()};

    expectNear(result.surfaceReflectance,
               {0.052608665446713956,
                0.05447522467348425,
                0.04520248065341318,
                0.04351384323754312,
                0.04300465712468566,
                0.039903761949652325},
               Tolerance);
    expectNear(result.bulk.tau,
               {0.22502547379278057,
                0.5643323096637146,
                0.9939982951865283,
                0.9949988624052964,
                0.9418484363499485,
                0.9478873722548796},
               Tolerance);
}

TEST(CompoundProductReference, Coating)
{
    const auto result{coated().deconstructCoating(glass(), Side::Back, noTolerances())};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value.tf,
               {0.004630088170958243,
                0.01658043287962344,
                0.35585801667909017,
                0.7695219497853232,
                0.005324501462207354,
                0.0010561628636654079},
               Tolerance);
    expectNear(result.value.rf,
               {4.851706169980125,
                0.9788959726713669,
                0.27935015832964183,
                0.020392120836534217,
                1.0526731237242772,
                1.082893696756826},
               Tolerance);
    expectNear(result.value.rb,
               {0.05999994214394423,
                0.08399514823891294,
                0.2242728377537766,
                0.050467324546399225,
                0.7879988732290513,
                0.850999958391055},
               Tolerance);
}

TEST(CompoundProductReference, Interlayer)
{
    expectNear(interlayer().tau,
               {0.06600837724246904,
                0.014048964284214338,
                0.7275305568309383,
                0.994046815431529,
                0.9882269212002436,
                0.8191284738150678},
               Tolerance);
}

TEST(CompoundProductReference, EmbeddedCoating)
{
    const auto result{laminateWithEmbeddedCoating().deconstructEmbeddedCoating(
      glass(), glass(), interlayer(), noTolerances())};

    // Reflectance on the interlayer side is 17 and 63 at the first two wavelengths, where the
    // laminate is opaque. Comparison there is relative to the size of the number.
    expectNear(result.value.tf,
               {0.6662455635617535,
                0.499025315004059,
                0.3555399988619099,
                0.6990997357872569,
                0.0058088685500785366,
                0.0},
               Tolerance);
    expectNear(result.value.rf,
               {0.16255442535381898,
                0.10001120922756838,
                0.21554319595457888,
                0.10043493871615375,
                0.8848601427193691,
                0.946110787527845},
               Tolerance);
    expectNear(result.value.rb,
               {17.12119251941439,
                62.70525331305453,
                0.3061845187143756,
                0.12044005726366992,
                0.9307569006592888,
                0.9548550579931994},
               1e-4);
}

TEST(CompoundProductReference, DoubleMonolithicISO9050Example)
{
    const std::vector<double> single{0.3};
    const ExternalProperties measured{.Tf = series(single, {0.269229}),
                                      .Tb = series(single, {0.269229}),
                                      .Rf = series(single, {0.242135}),
                                      .Rb = series(single, {0.236891})};
    const Interface frontSurface{.tf = series(single, {0.6}),
                                 .tb = series(single, {0.6}),
                                 .rf = series(single, {0.2}),
                                 .rb = series(single, {0.15})};
    const Interface backSurface{.tf = series(single, {0.66}),
                                .tb = series(single, {0.66}),
                                .rf = series(single, {0.12}),
                                .rb = series(single, {0.18})};

    const auto result{
      measured.deconstructDoubleMonolithic(frontSurface,
                                  Bulk{.tau = series(single, {0.94}), .thickness = GlassThickness},
                                  backSurface,
                                  Bulk{.tau = series(single, {0.83}), .thickness = GlassThickness})};

    EXPECT_TRUE(result.diagnostics.empty());
    expectNear(result.value.tf, {0.85}, 1e-5);
    expectNear(result.value.tb, {0.85}, 1e-5);
    expectNear(result.value.rf, {0.07}, 1e-5);
    expectNear(result.value.rb, {0.09}, 1e-5);
}

///////////////////////////////////////////////////////////////////////////
/// Default tolerances
///////////////////////////////////////////////////////////////////////////

TEST(CompoundProductDiagnostics, CoatingBehindWeaklyTransmittingSubstrate)
{
    const auto substrateGlass{substrate().deconstructMonolithic(GlassThickness).value};
    const auto result{coated().deconstructCoating(substrateGlass, Side::Back)};

    // Substrate internal transmittance at 0.300 is 0.225, which is below the threshold. Plain
    // equation gives substrate side reflectance of 4.85 there.
    EXPECT_TRUE(
      hasWarning(result.diagnostics, 0.300, Warning::InsensitiveCoatingSubstrateSide));
    EXPECT_TRUE(hasWarning(result.diagnostics, 0.300, Warning::UnphysicalClamped));
    EXPECT_FALSE(hasWarning(result.diagnostics, 0.550, Warning::UnphysicalClamped));
    EXPECT_FALSE(
      hasWarning(result.diagnostics, 0.550, Warning::InsensitiveCoatingSubstrateSide));

    for(const auto & point : result.value.rf)
    {
        EXPECT_GE(point.value(), 0.0);
        EXPECT_LE(point.value(), 1.0);
    }
    EXPECT_NEAR(result.value.rf[3].value(), 0.020392120836534217, 1e-6);
}

TEST(CompoundProductDiagnostics, OpaqueMonolithic)
{
    const std::vector<double> lambdas{0.3, 0.5, 5.0};
    const ExternalProperties measured{.Tf = series(lambdas, {0.0, 0.9, 0.0}),
                                      .Tb = series(lambdas, {0.0, 0.9, 0.0}),
                                      .Rf = series(lambdas, {0.07, 0.08, 0.0}),
                                      .Rb = series(lambdas, {0.07, 0.08, 0.0})};

    const auto result{measured.deconstructMonolithic(GlassThickness)};

    ASSERT_EQ(result.diagnostics.size(), 2U);
    EXPECT_TRUE(hasWarning(result.diagnostics, 0.3, Warning::OpaqueLayer));
    EXPECT_TRUE(hasWarning(result.diagnostics, 5.0, Warning::NoData));
    EXPECT_NEAR(result.value.surfaceReflectance[0].value(), 0.07, 1e-12);
    EXPECT_NEAR(result.value.bulk.tau[0].value(), 0.0, 1e-12);
    EXPECT_NEAR(result.value.surfaceReflectance[2].value(), 0.0, 1e-12);
}

TEST(CompoundProductDiagnostics, OpaqueSubstrateUnderCoating)
{
    const std::vector<double> single{0.3};
    const Monolithic opaque{.surfaceReflectance = series(single, {0.07}),
                            .bulk = {.tau = series(single, {0.0}), .thickness = GlassThickness}};
    const ExternalProperties measured{.Tf = series(single, {0.0}),
                                      .Tb = series(single, {0.0}),
                                      .Rf = series(single, {0.07}),
                                      .Rb = series(single, {0.4})};

    const auto result{measured.deconstructCoating(opaque, Side::Back)};

    EXPECT_TRUE(hasWarning(result.diagnostics, 0.3, Warning::OpaqueSubstrate));
    EXPECT_NEAR(result.value.tf[0].value(), 0.0, 1e-12);
    EXPECT_NEAR(result.value.rb[0].value(), 0.4, 1e-12);
}

TEST(CompoundProductDiagnostics, OpaqueLaminate)
{
    const auto substrateGlass{substrate().deconstructMonolithic(GlassThickness).value};
    const auto result{laminate().deconstructInterlayer(substrateGlass, substrateGlass, InterlayerThickness)};

    // Laminate transmittance at 0.300 is 0.003, which is above the limit of detection, but the
    // panes do not transmit enough for a reliable result.
    EXPECT_TRUE(hasWarning(result.diagnostics, 0.300, Warning::InsensitiveInterlayer));
    EXPECT_FALSE(hasWarning(result.diagnostics, 0.550, Warning::InsensitiveInterlayer));
    EXPECT_NEAR(result.value.tau[3].value(), 0.994046815431529, 1e-6);
}

TEST(CompoundProductDiagnostics, DifferentWavelengthsThrow)
{
    const Monolithic other{.surfaceReflectance = series({0.3, 0.5}, {0.04, 0.04}),
                           .bulk = {.tau = series({0.3, 0.5}, {0.9, 0.9}), .thickness = GlassThickness}};

    EXPECT_THROW((void)coated().deconstructCoating(other, Side::Back), std::runtime_error);
}
