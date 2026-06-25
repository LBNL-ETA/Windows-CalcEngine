#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include "WCEViewer.hpp"
#include "../../src/SpatialGrid.hpp"


using namespace Viewer;

// Opt-in micro-benchmark (DISABLED_ so it never runs in CI). Run it explicitly with:
//   Windows-CalcEngine_tests --gtest_also_run_disabled_tests \
//       --gtest_filter=EnclosureViewFactorsBench.DISABLED_GridDensitySweep
// It reproduces the conrad "speed vs grid density" tables on a blocking-heavy fixture and is the
// tool for choosing the grid-density heuristic. It asserts nothing about wall-clock time.

namespace
{
    // A comb of tall teeth. Each tooth blocks the line of sight between teeth on either side, so
    // most pairs hit the third-surface / subdivision path. Subdividing every edge makes the
    // blockers short, so the grid can cull candidates in both axes.
    std::vector<RadiationSegment> makeCombEnclosure(std::size_t teeth, std::size_t subdivisionPerEdge)
    {
        constexpr double pitch = 1.0;
        constexpr double width = 0.4;
        constexpr double height = 3.0;

        std::vector<RadiationSegment> segments;
        const auto addEdge = [&](const CPoint2D & from, const CPoint2D & to) {
            for(std::size_t step = 0; step < subdivisionPerEdge; ++step)
            {
                const auto t0 = static_cast<double>(step) / static_cast<double>(subdivisionPerEdge);
                const auto t1 =
                  static_cast<double>(step + 1) / static_cast<double>(subdivisionPerEdge);
                segments.push_back(
                  {.startPoint = {from.x() + (to.x() - from.x()) * t0,
                                  from.y() + (to.y() - from.y()) * t0},
                   .endPoint = {from.x() + (to.x() - from.x()) * t1,
                                from.y() + (to.y() - from.y()) * t1},
                   .emissivity = 0.9,
                   .enclosureId = 0u});
            }
        };

        for(std::size_t tooth = 0; tooth < teeth; ++tooth)
        {
            const auto xLeft = static_cast<double>(tooth) * pitch;
            const auto xRight = xLeft + width;
            addEdge({xLeft, 0.0}, {xLeft, height});      // left side, upward
            addEdge({xLeft, height}, {xRight, height});  // top
            addEdge({xRight, height}, {xRight, 0.0});    // right side, downward
        }
        return segments;
    }

    double maxAbsDifference(const FenestrationCommon::SquareMatrix & lhs,
                            const FenestrationCommon::SquareMatrix & rhs)
    {
        double worst = 0.0;
        for(std::size_t row = 0; row < lhs.size(); ++row)
        {
            for(std::size_t col = 0; col < lhs.size(); ++col)
            {
                worst = std::max(worst, std::abs(lhs(row, col) - rhs(row, col)));
            }
        }
        return worst;
    }
}   // namespace

// Realistic-scale timing: THERM enclosures are ~100 segments or fewer, computed once per
// simulation. This reports serial vs parallel (auto grid density) across that range so the
// optimization effort can be judged against the real workload, not the stress fixture.
TEST(EnclosureViewFactorsBench, DISABLED_RealisticSizes)
{
    std::cout << "\nhardware threads = " << std::thread::hardware_concurrency() << "\n";
    std::cout << "segments   serial(ms)   parallel(ms)\n";

    const std::vector<std::pair<std::size_t, std::size_t>> shapes{
      {8, 2}, {8, 4}, {10, 5}, {14, 5}};   // ~48, ~96, ~150, ~210 segments
    for(const auto & [teeth, subdivision] : shapes)
    {
        const auto segments = makeCombEnclosure(teeth, subdivision);

        const auto timeIt = [&](bool multithread) {
            const ViewFactorOptions options{.multithread = multithread};
            const auto start = std::chrono::steady_clock::now();
            const auto result = computeEnclosureViewFactors(segments, {}, options);
            const auto stop = std::chrono::steady_clock::now();
            return std::chrono::duration<double, std::milli>(stop - start).count();
        };

        const auto serialMs = timeIt(false);
        const auto parallelMs = timeIt(true);
        std::cout << segments.size() << "\t" << serialMs << "\t" << parallelMs << "\n";
    }
}

TEST(EnclosureViewFactorsBench, DISABLED_GridDensitySweep)
{
    const auto segments = makeCombEnclosure(8, 4);   // ~96 segments, realistic THERM scale
    std::cout << "\nsegments = " << segments.size() << ", hardware threads = "
              << std::thread::hardware_concurrency() << "\n";

    const auto timeRun = [&](std::size_t cells, bool multithread) {
        const ViewFactorOptions options{
          .gridCellsPerAxis = cells, .multithread = multithread};
        const auto start = std::chrono::steady_clock::now();
        const auto viewFactors = computeEnclosureViewFactors(segments, {}, options).viewFactors;
        const auto stop = std::chrono::steady_clock::now();
        const auto milliseconds = std::chrono::duration<double, std::milli>(stop - start).count();
        return std::pair{milliseconds, viewFactors};
    };

    std::cout << "N/axis   serial(ms)   parallel(ms)   speedup   maxDiff(par-vs-serial)\n";
    for(const std::size_t cells : {1u, 2u, 4u, 8u, 16u, 32u, 64u})
    {
        const auto [serialMs, serialVf] = timeRun(cells, false);
        const auto [parallelMs, parallelVf] = timeRun(cells, true);
        std::cout << cells << "\t" << serialMs << "\t" << parallelMs << "\t"
                  << (serialMs / parallelMs) << "\t" << maxAbsDifference(parallelVf, serialVf)
                  << "\n";
    }

    std::cout << "suggestCellsPerAxis(" << segments.size()
              << ") = " << SpatialGrid::suggestCellsPerAxis(segments.size()) << "\n";
}
