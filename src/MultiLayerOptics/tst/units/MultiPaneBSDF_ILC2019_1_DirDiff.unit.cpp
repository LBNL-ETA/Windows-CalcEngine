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

    enum class Spectrum
    {
        Solar,
        Visible
    };

    void applyCalculationProperties(CMultiPaneBSDF & layer, const Spectrum spec)
    {
        if(spec == Spectrum::Solar)
        {
            const CalculationProperties input{
              StandardData::solarRadiationASTM_E891_87_Table1(),
              StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
            layer.setCalculationProperties(input);
        }
        else
        {
            const CalculationProperties input{StandardData::Photopic::solarRadiation(),
                                              StandardData::Photopic::wavelengthSetPhotopic(),
                                              StandardData::Photopic::detectorData()};
            layer.setCalculationProperties(input);
        }
    }

    std::unique_ptr<CMultiPaneBSDF> makeLayer(const BSDFBasis basis, const Spectrum spec)
    {
        const auto material = Material::nBandMaterial(
          SpectralSample::ILC2019_1(), thicknessILC2019_1, MaterialType::Laminate);
        const auto bsdf = BSDFHemisphere::create(basis);
        const auto layer = CBSDFLayerMaker::getDirDifLayer(material, bsdf);
        auto multiLayer = CMultiPaneBSDF::create({layer});
        applyCalculationProperties(*multiLayer, spec);
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
          .tDif = layer.DirDiff(minLambda, maxLambda, side, PropertySurface::T, theta, phi),
          .tHem = layer.DirHem(minLambda, maxLambda, side, PropertySurface::T, theta, phi),
          .rDir = layer.DirDir(minLambda, maxLambda, side, PropertySurface::R, theta, phi),
          .rDif = layer.DirDiff(minLambda, maxLambda, side, PropertySurface::R, theta, phi),
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
        .tDir = 0.037403,
        .tDif = 0.608378,
        .tHem = 0.645781,
        .rDir = 0.042893,
        .rDif = 0.096643,
        .rHem = 0.139536,
        .absDif = 0.218327,
        .absDir = 0.214683};
    constexpr DirDiffResult expected_Nano_Solar_Back{
        .tDir = 0.036869,
        .tDif = 0.608475,
        .tHem = 0.645344,
        .rDir = 0.042460,
        .rDif = 0.096829,
        .rHem = 0.139289,
        .absDif = 0.218987,
        .absDir = 0.215367};

    constexpr DirDiffResult expected_Nano_Visible_Front{
        .tDir = 0.014420,
        .tDif = 0.681431,
        .tHem = 0.695851,
        .rDir = 0.044388,
        .rDif = 0.143986,
        .rHem = 0.188374,
        .absDif = 0.120319,
        .absDir = 0.115775};
    constexpr DirDiffResult expected_Nano_Visible_Back{
        .tDir = 0.014111,
        .tDif = 0.681062,
        .tHem = 0.695173,
        .rDir = 0.043839,
        .rDif = 0.144242,
        .rHem = 0.188081,
        .absDif = 0.121257,
        .absDir = 0.116746};

    constexpr DirDiffResult expected_Small_Solar_Front{
        .tDir = 0.037403,
        .tDif = 0.608378,
        .tHem = 0.645781,
        .rDir = 0.042893,
        .rDif = 0.096643,
        .rHem = 0.139536,
        .absDif = 0.211239,
        .absDir = 0.214683};
    constexpr DirDiffResult expected_Small_Solar_Back{
        .tDir = 0.036869,
        .tDif = 0.608475,
        .tHem = 0.645344,
        .rDir = 0.042460,
        .rDif = 0.096829,
        .rHem = 0.139289,
        .absDif = 0.211859,
        .absDir = 0.215367};

    constexpr DirDiffResult expected_Small_Visible_Front{
        .tDir = 0.014420,
        .tDif = 0.681431,
        .tHem = 0.695851,
        .rDir = 0.044388,
        .rDif = 0.143986,
        .rHem = 0.188374,
        .absDif = 0.117702,
        .absDir = 0.115775};
    constexpr DirDiffResult expected_Small_Visible_Back{
        .tDir = 0.014111,
        .tDif = 0.681062,
        .tHem = 0.695173,
        .rDir = 0.043839,
        .rDif = 0.144242,
        .rHem = 0.188081,
        .absDif = 0.118582,
        .absDir = 0.116746};

}   // namespace

TEST(ILC2019_1_DirDiff, Nano_Solar)
{
    auto layer = makeLayer(BSDFBasis::Nano, Spectrum::Solar);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Nano_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Nano_Solar_Back);
}

TEST(ILC2019_1_DirDiff, Nano_Visible)
{
    auto layer = makeLayer(BSDFBasis::Nano, Spectrum::Visible);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Nano_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Nano_Visible_Back);
}

TEST(ILC2019_1_DirDiff, Small_Solar)
{
    auto layer = makeLayer(BSDFBasis::Small, Spectrum::Solar);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Small_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Small_Solar_Back);
}

TEST(ILC2019_1_DirDiff, Small_Visible)
{
    auto layer = makeLayer(BSDFBasis::Small, Spectrum::Visible);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Small_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Small_Visible_Back);
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

    constexpr std::array<std::tuple<const char *, double, double, Spectrum>, 2> ranges{
      {{"Solar", solarMin, solarMax, Spectrum::Solar},
       {"Visible", visMin, visMax, Spectrum::Visible}}};

    constexpr std::array<std::pair<Side, const char *>, 2> sides{
      {{Side::Front, "Front"}, {Side::Back, "Back"}}};

    for(const auto & [rangeName, rangeMin, rangeMax, spec] : ranges)
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
                auto layer = makeLayer(basis, spec);
                const auto r = compute(*layer, side, rangeMin, rangeMax);
                std::cout << std::left << std::setw(10) << basisName << std::right << std::fixed
                          << std::setprecision(12) << std::setw(12) << r.tDir << std::setw(12)
                          << r.tDif << std::setw(12) << r.tHem << std::setw(12) << r.rDir
                          << std::setw(12) << r.rDif << std::setw(12) << r.rHem << "\n";
            }
        }
    }
}
