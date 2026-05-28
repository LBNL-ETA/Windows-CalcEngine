#include <array>
#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>

#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

namespace
{
    struct DirDiffResult
    {
        double tDir;
        double tDif;
        double tHem;
        double rDir;
        double rDif;
        double rHem;
        double absDif;
        double absDir;
    };

    constexpr double thicknessILC2019_1 = 10.3e-3;
    constexpr double solarMin = 0.3;
    constexpr double solarMax = 2.5;
    constexpr double visMin = 0.38;
    constexpr double visMax = 0.78;
    constexpr double tol = 1e-6;

    std::unique_ptr<CMultiPaneBSDF> makeLayer(const BSDFBasis basis)
    {
        const auto material = Material::nBandMaterial(
          SpectralSample::ILC2019_1(), thicknessILC2019_1, MaterialType::Laminate);
        const auto bsdf = BSDFHemisphere::create(basis);
        const auto layer = CBSDFLayerMaker::getDirDifLayer(material, bsdf);
        auto multiLayer = CMultiPaneBSDF::create({layer});
        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        multiLayer->setCalculationProperties(input);
        return multiLayer;
    }

    DirDiffResult compute(CMultiPaneBSDF & layer,
                          const Side side,
                          const double minLambda,
                          const double maxLambda)
    {
        constexpr double theta = 0.0;
        constexpr double phi = 0.0;
        return {
          .tDir = layer.DirDir(minLambda, maxLambda, side, PropertySurface::T, theta, phi),
          .tDif = layer.DiffDiff(minLambda, maxLambda, side, PropertySurface::T),
          .tHem = layer.DirHem(minLambda, maxLambda, side, PropertySurface::T, theta, phi),
          .rDir = layer.DirDir(minLambda, maxLambda, side, PropertySurface::R, theta, phi),
          .rDif = layer.DiffDiff(minLambda, maxLambda, side, PropertySurface::R),
          .rHem = layer.DirHem(minLambda, maxLambda, side, PropertySurface::R, theta, phi),
          .absDif = layer.AbsDiff(minLambda, maxLambda, side, 1),
          .absDir = layer.Abs(minLambda, maxLambda, side, 1, theta, phi),
        };
    }

    void expectClose(const DirDiffResult & actual, const DirDiffResult & expected)
    {
        EXPECT_NEAR(actual.tDir, expected.tDir, tol);
        EXPECT_NEAR(actual.tDif, expected.tDif, tol);
        EXPECT_NEAR(actual.tHem, expected.tHem, tol);
        EXPECT_NEAR(actual.rDir, expected.rDir, tol);
        EXPECT_NEAR(actual.rDif, expected.rDif, tol);
        EXPECT_NEAR(actual.rHem, expected.rHem, tol);
        EXPECT_NEAR(actual.absDif, expected.absDif, tol);
        EXPECT_NEAR(actual.absDir, expected.absDir, tol);
    }

    // Captured from post-fix run; regenerate via running the test suite with the placeholder values.
    constexpr DirDiffResult expected_Nano_Solar_Front{
        .tDir = 0.037390,
        .tDif = 0.619279,
        .tHem = 0.645473,
        .rDir = 0.042876,
        .rDif = 0.162034,
        .rHem = 0.139472,
        .absDif = 0.218687,
        .absDir = 0.215054};
    constexpr DirDiffResult expected_Nano_Solar_Back{
        .tDir = 0.036865,
        .tDif = 0.618859,
        .tHem = 0.645036,
        .rDir = 0.042462,
        .rDif = 0.161804,
        .rHem = 0.139236,
        .absDif = 0.219338,
        .absDir = 0.215728};

    constexpr DirDiffResult expected_Nano_Visible_Front{
        .tDir = 0.015976,
        .tDif = 0.647792,
        .tHem = 0.674213,
        .rDir = 0.044192,
        .rDif = 0.197305,
        .rHem = 0.175458,
        .absDif = 0.154903,
        .absDir = 0.150328};
    constexpr DirDiffResult expected_Nano_Visible_Back{
        .tDir = 0.015669,
        .tDif = 0.647055,
        .tHem = 0.673447,
        .rDir = 0.043681,
        .rDif = 0.197098,
        .rHem = 0.175245,
        .absDif = 0.155846,
        .absDir = 0.151307};

    constexpr DirDiffResult expected_Small_Solar_Front{
        .tDir = 0.037390,
        .tDif = 0.579960,
        .tHem = 0.645473,
        .rDir = 0.042876,
        .rDif = 0.208457,
        .rHem = 0.139472,
        .absDif = 0.211583,
        .absDir = 0.215054};
    constexpr DirDiffResult expected_Small_Solar_Back{
        .tDir = 0.036865,
        .tDif = 0.579566,
        .tHem = 0.645036,
        .rDir = 0.042462,
        .rDif = 0.208239,
        .rHem = 0.139236,
        .absDif = 0.212195,
        .absDir = 0.215728};

    constexpr DirDiffResult expected_Small_Visible_Front{
        .tDir = 0.015976,
        .tDif = 0.606582,
        .tHem = 0.674213,
        .rDir = 0.044192,
        .rDif = 0.242605,
        .rHem = 0.175458,
        .absDif = 0.150813,
        .absDir = 0.150328};
    constexpr DirDiffResult expected_Small_Visible_Back{
        .tDir = 0.015669,
        .tDif = 0.605892,
        .tHem = 0.673447,
        .rDir = 0.043681,
        .rDif = 0.242410,
        .rHem = 0.175245,
        .absDif = 0.151698,
        .absDir = 0.151307};

    constexpr DirDiffResult expected_Quarter_Solar_Front{
        .tDir = 0.037390,
        .tDif = 0.575376,
        .tHem = 0.645473,
        .rDir = 0.042876,
        .rDif = 0.213031,
        .rHem = 0.139472,
        .absDif = 0.211593,
        .absDir = 0.215054};
    constexpr DirDiffResult expected_Quarter_Solar_Back{
        .tDir = 0.036865,
        .tDif = 0.574986,
        .tHem = 0.645036,
        .rDir = 0.042462,
        .rDif = 0.212815,
        .rHem = 0.139236,
        .absDif = 0.212200,
        .absDir = 0.215728};

    constexpr DirDiffResult expected_Quarter_Visible_Front{
        .tDir = 0.015976,
        .tDif = 0.601818,
        .tHem = 0.674213,
        .rDir = 0.044192,
        .rDif = 0.247122,
        .rHem = 0.175458,
        .absDif = 0.151060,
        .absDir = 0.150328};
    constexpr DirDiffResult expected_Quarter_Visible_Back{
        .tDir = 0.015669,
        .tDif = 0.601134,
        .tHem = 0.673447,
        .rDir = 0.043681,
        .rDif = 0.246927,
        .rHem = 0.175245,
        .absDif = 0.151939,
        .absDir = 0.151307};

    constexpr DirDiffResult expected_Half_Solar_Front{
        .tDir = 0.037390,
        .tDif = 0.579960,
        .tHem = 0.645473,
        .rDir = 0.042876,
        .rDif = 0.208457,
        .rHem = 0.139472,
        .absDif = 0.211583,
        .absDir = 0.215054};
    constexpr DirDiffResult expected_Half_Solar_Back{
        .tDir = 0.036865,
        .tDif = 0.579566,
        .tHem = 0.645036,
        .rDir = 0.042462,
        .rDif = 0.208239,
        .rHem = 0.139236,
        .absDif = 0.212195,
        .absDir = 0.215728};

    constexpr DirDiffResult expected_Half_Visible_Front{
        .tDir = 0.015976,
        .tDif = 0.606582,
        .tHem = 0.674213,
        .rDir = 0.044192,
        .rDif = 0.242605,
        .rHem = 0.175458,
        .absDif = 0.150813,
        .absDir = 0.150328};
    constexpr DirDiffResult expected_Half_Visible_Back{
        .tDir = 0.015669,
        .tDif = 0.605892,
        .tHem = 0.673447,
        .rDir = 0.043681,
        .rDif = 0.242410,
        .rHem = 0.175245,
        .absDif = 0.151698,
        .absDir = 0.151307};

    constexpr DirDiffResult expected_Full_Solar_Front{
        .tDir = 0.037390,
        .tDif = 0.581677,
        .tHem = 0.645473,
        .rDir = 0.042876,
        .rDif = 0.206398,
        .rHem = 0.139472,
        .absDif = 0.211924,
        .absDir = 0.215054};
    constexpr DirDiffResult expected_Full_Solar_Back{
        .tDir = 0.036865,
        .tDif = 0.581282,
        .tHem = 0.645036,
        .rDir = 0.042462,
        .rDif = 0.206180,
        .rHem = 0.139236,
        .absDif = 0.212538,
        .absDir = 0.215728};

    constexpr DirDiffResult expected_Full_Visible_Front{
        .tDir = 0.015976,
        .tDif = 0.608374,
        .tHem = 0.674213,
        .rDir = 0.044192,
        .rDif = 0.240611,
        .rHem = 0.175458,
        .absDif = 0.151015,
        .absDir = 0.150328};
    constexpr DirDiffResult expected_Full_Visible_Back{
        .tDir = 0.015669,
        .tDif = 0.607682,
        .tHem = 0.673447,
        .rDir = 0.043681,
        .rDif = 0.240415,
        .rHem = 0.175245,
        .absDif = 0.151903,
        .absDir = 0.151307};

}   // namespace

TEST(ILC2019_1_DirDiff, Nano_Solar)
{
    auto layer = makeLayer(BSDFBasis::Nano);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Nano_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Nano_Solar_Back);
}

TEST(ILC2019_1_DirDiff, Nano_Visible)
{
    auto layer = makeLayer(BSDFBasis::Nano);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Nano_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Nano_Visible_Back);
}

TEST(ILC2019_1_DirDiff, Small_Solar)
{
    auto layer = makeLayer(BSDFBasis::Small);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Small_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Small_Solar_Back);
}

TEST(ILC2019_1_DirDiff, Small_Visible)
{
    auto layer = makeLayer(BSDFBasis::Small);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Small_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Small_Visible_Back);
}

TEST(ILC2019_1_DirDiff, Quarter_Solar)
{
    auto layer = makeLayer(BSDFBasis::Quarter);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Quarter_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Quarter_Solar_Back);
}

TEST(ILC2019_1_DirDiff, Quarter_Visible)
{
    auto layer = makeLayer(BSDFBasis::Quarter);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Quarter_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Quarter_Visible_Back);
}

TEST(ILC2019_1_DirDiff, Half_Solar)
{
    auto layer = makeLayer(BSDFBasis::Half);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Half_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Half_Solar_Back);
}

TEST(ILC2019_1_DirDiff, Half_Visible)
{
    auto layer = makeLayer(BSDFBasis::Half);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Half_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Half_Visible_Back);
}

TEST(ILC2019_1_DirDiff, Full_Solar)
{
    auto layer = makeLayer(BSDFBasis::Full);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Full_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Full_Solar_Back);
}

TEST(ILC2019_1_DirDiff, Full_Visible)
{
    auto layer = makeLayer(BSDFBasis::Full);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Full_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Full_Visible_Back);
}

// Pure Lambertian diagnostic: T_dir=0, T_dif=1, R=0 across the whole spectrum.
// For a perfect Lambertian diffuser energy conservation requires DirHem(theta=0) = 1
// exactly for every basis. Any deviation isolates a basis-dependent error in the
// dir->dif distribution, independent of any real material or spectral integration.
// Disabled by default (prints, no assertions) - run with --gtest_also_run_disabled_tests.
// Convert to real EXPECT_NEAR(..., 1.0, tol) assertions if you want this in CI.
TEST(ILC2019_1_DirDiff, DISABLED_PureLambertianDiagnostic)
{
    std::vector<SpectralAveraging::MeasuredRow> rows;
    for(double wl = 0.300; wl <= 2.500 + 1e-9; wl += 0.100)
    {
        rows.push_back({wl, {0.0, 0.0, 0.0, 0.0}, {1.0, 1.0, 0.0, 0.0}});
    }
    const auto lambertianSample = SpectralAveraging::CSpectralSampleData::create(rows);

    constexpr std::array<std::pair<BSDFBasis, const char *>, 5> bases{
      {{BSDFBasis::Nano, "Nano"},
       {BSDFBasis::Small, "Small"},
       {BSDFBasis::Quarter, "Quarter"},
       {BSDFBasis::Half, "Half"},
       {BSDFBasis::Full, "Full"}}};

    std::cout << "\n=== Pure Lambertian (T_dif=1, all else 0) - DirHem(theta=0) ===\n";
    std::cout << "Expected: 1.000000 for every basis (energy conservation)\n";
    std::cout << std::left << std::setw(10) << "Basis" << std::right << std::setw(14)
              << "DirHem(T,F)" << std::setw(14) << "DirHem(T,B)" << "\n";

    for(const auto & [basis, name] : bases)
    {
        const auto material =
          Material::nBandMaterial(lambertianSample, 0.001, MaterialType::Laminate);
        const auto bsdf = BSDFHemisphere::create(basis);
        const auto layer = CBSDFLayerMaker::getDirDifLayer(material, bsdf);
        auto multiLayer = CMultiPaneBSDF::create({layer});
        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        multiLayer->setCalculationProperties(input);

        const double vF = multiLayer->DirHem(
          solarMin, solarMax, Side::Front, PropertySurface::T, 0.0, 0.0);
        const double vB = multiLayer->DirHem(
          solarMin, solarMax, Side::Back, PropertySurface::T, 0.0, 0.0);
        std::cout << std::left << std::setw(10) << name << std::right << std::fixed
                  << std::setprecision(6) << std::setw(14) << vF << std::setw(14) << vB << "\n";
    }
}

// Documentation-style test: prints integrated results for every basis on a single sample/range/side
// combination. Disabled by default (DISABLED_ prefix) - Full basis construction is ~3.5s wall-clock,
// total ~19s, which slows the suite. The same comparison is preserved as a static markdown artifact
// at D:\Documents\Direct-Diffuse\ILC2019_1_basis_comparison.md. Re-enable locally via
// `--gtest_also_run_disabled_tests` if you need a fresh dump.
TEST(ILC2019_1_DirDiff, DISABLED_BasisComparisonTable)
{
    constexpr std::array<std::pair<BSDFBasis, const char *>, 5> bases{
      {{BSDFBasis::Nano, "Nano"},
       {BSDFBasis::Small, "Small"},
       {BSDFBasis::Quarter, "Quarter"},
       {BSDFBasis::Half, "Half"},
       {BSDFBasis::Full, "Full"}}};

    constexpr std::array<std::tuple<const char *, double, double>, 2> ranges{
      {{"Solar", solarMin, solarMax}, {"Visible", visMin, visMax}}};

    constexpr std::array<std::pair<Side, const char *>, 2> sides{
      {{Side::Front, "Front"}, {Side::Back, "Back"}}};

    for(const auto & [rangeName, rangeMin, rangeMax] : ranges)
    {
        for(const auto & [side, sideName] : sides)
        {
            std::cout << "\n=== ILC2019_1 / " << rangeName << " / " << sideName
                      << " (theta=0, phi=0) ===\n";
            std::cout << std::left << std::setw(10) << "Basis" << std::right << std::setw(12)
                      << "Tdir" << std::setw(12) << "Tdif" << std::setw(12) << "Them"
                      << std::setw(12) << "Rdir" << std::setw(12) << "Rdif" << std::setw(12)
                      << "Rhem" << "\n";
            for(const auto & [basis, basisName] : bases)
            {
                auto layer = makeLayer(basis);
                const auto r = compute(*layer, side, rangeMin, rangeMax);
                std::cout << std::left << std::setw(10) << basisName << std::right << std::fixed
                          << std::setprecision(6) << std::setw(12) << r.tDir << std::setw(12)
                          << r.tDif << std::setw(12) << r.tHem << std::setw(12) << r.rDir
                          << std::setw(12) << r.rDif << std::setw(12) << r.rHem << "\n";
            }
        }
    }
}
