#include <array>
#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

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

    constexpr double thicknessILC2019_2 = 12.0e-3;
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
          SpectralSample::ILC2019_2(), thicknessILC2019_2, MaterialType::Coated);
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

    // Captured from post-fix run; regenerate via running the test suite with placeholder values.
    constexpr DirDiffResult expected_Nano_Solar_Front{
        .tDir = 0.055349,
        .tDif = 0.115868,
        .tHem = 0.171217,
        .rDir = 0.485549,
        .rDif = 0.051677,
        .rHem = 0.537226,
        .absDif = 0.293146,
        .absDir = 0.291556};
    constexpr DirDiffResult expected_Nano_Solar_Back{
        .tDir = 0.055309,
        .tDif = 0.109965,
        .tHem = 0.165275,
        .rDir = 0.111168,
        .rDif = 0.238957,
        .rHem = 0.350125,
        .absDif = 0.481437,
        .absDir = 0.484601};

    constexpr DirDiffResult expected_Nano_Visible_Front{
        .tDir = 0.127992,
        .tDif = 0.285586,
        .tHem = 0.413578,
        .rDir = 0.030987,
        .rDif = 0.067442,
        .rHem = 0.098429,
        .absDif = 0.496836,
        .absDir = 0.487993};
    constexpr DirDiffResult expected_Nano_Visible_Back{
        .tDir = 0.127137,
        .tDif = 0.283487,
        .tHem = 0.410625,
        .rDir = 0.042189,
        .rDif = 0.091119,
        .rHem = 0.133309,
        .absDif = 0.465504,
        .absDir = 0.456066};

    constexpr DirDiffResult expected_Small_Solar_Front{
        .tDir = 0.055349,
        .tDif = 0.115868,
        .tHem = 0.171217,
        .rDir = 0.485549,
        .rDif = 0.051677,
        .rHem = 0.537226,
        .absDif = 0.282919,
        .absDir = 0.291556};
    constexpr DirDiffResult expected_Small_Solar_Back{
        .tDir = 0.055309,
        .tDif = 0.109965,
        .tHem = 0.165275,
        .rDir = 0.111168,
        .rDif = 0.238957,
        .rHem = 0.350125,
        .absDif = 0.462846,
        .absDir = 0.484601};

    constexpr DirDiffResult expected_Small_Visible_Front{
        .tDir = 0.127992,
        .tDif = 0.285586,
        .tHem = 0.413578,
        .rDir = 0.030987,
        .rDif = 0.067442,
        .rHem = 0.098429,
        .absDif = 0.481412,
        .absDir = 0.487993};
    constexpr DirDiffResult expected_Small_Visible_Back{
        .tDir = 0.127137,
        .tDif = 0.283487,
        .tHem = 0.410625,
        .rDir = 0.042189,
        .rDif = 0.091119,
        .rHem = 0.133309,
        .absDif = 0.451411,
        .absDir = 0.456066};

}   // namespace

TEST(ILC2019_2_DirDiff, Nano_Solar)
{
    auto layer = makeLayer(BSDFBasis::Nano, Spectrum::Solar);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Nano_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Nano_Solar_Back);
}

TEST(ILC2019_2_DirDiff, Nano_Visible)
{
    auto layer = makeLayer(BSDFBasis::Nano, Spectrum::Visible);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Nano_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Nano_Visible_Back);
}

TEST(ILC2019_2_DirDiff, Small_Solar)
{
    auto layer = makeLayer(BSDFBasis::Small, Spectrum::Solar);
    expectClose(compute(*layer, Side::Front, solarMin, solarMax), expected_Small_Solar_Front);
    expectClose(compute(*layer, Side::Back, solarMin, solarMax), expected_Small_Solar_Back);
}

TEST(ILC2019_2_DirDiff, Small_Visible)
{
    auto layer = makeLayer(BSDFBasis::Small, Spectrum::Visible);
    expectClose(compute(*layer, Side::Front, visMin, visMax), expected_Small_Visible_Front);
    expectClose(compute(*layer, Side::Back, visMin, visMax), expected_Small_Visible_Back);
}

TEST(ILC2019_2_DirDiff, DISABLED_BasisComparisonTable)
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
                          << std::setprecision(6) << std::setw(12) << r.tDir << std::setw(12)
                          << r.tDif << std::setw(12) << r.tHem << std::setw(12) << r.rDir
                          << std::setw(12) << r.rDif << std::setw(12) << r.rHem << "\n";
            }
        }
    }
}
