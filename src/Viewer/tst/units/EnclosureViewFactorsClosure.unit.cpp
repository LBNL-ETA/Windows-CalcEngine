#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include "WCEViewer.hpp"


using namespace Viewer;
using namespace FenestrationCommon;

// Closure / environment / smoothing tests for the enclosure engine (A5). An open (auto) enclosure
// reports its row deficit as the view factor to the environment; a closed enclosure closes to one,
// and least-squares smoothing enforces that closure on closed enclosures while leaving open ones
// untouched.

namespace
{
    std::vector<RadiationSegment>
      segmentsFromEdges(const std::vector<std::pair<CPoint2D, CPoint2D>> & edges,
                        std::size_t enclosureId = 0)
    {
        std::vector<RadiationSegment> segments;
        for(const auto & [start, end] : edges)
        {
            segments.push_back({.startPoint = start,
                                .endPoint = end,
                                .emissivity = 0.9,
                                .enclosureId = enclosureId});
        }
        return segments;
    }

    std::vector<std::pair<CPoint2D, CPoint2D>> closedPolygon(const std::vector<CPoint2D> & vertices)
    {
        std::vector<std::pair<CPoint2D, CPoint2D>> edges;
        for(size_t idx = 0; idx < vertices.size(); ++idx)
        {
            edges.emplace_back(vertices[idx], vertices[(idx + 1) % vertices.size()]);
        }
        return edges;
    }

    // Three walls of a unit square, open at the top (the "room"): left, right, bottom. Normals
    // point inward, so the walls see each other and the remainder of each row goes to the opening.
    std::vector<RadiationSegment> openUShape(std::size_t enclosureId = 0)
    {
        return segmentsFromEdges({{CPoint2D{0, 0}, CPoint2D{0, 1}},     // 0: left wall
                                  {CPoint2D{1, 1}, CPoint2D{1, 0}},     // 1: right wall
                                  {CPoint2D{1, 0}, CPoint2D{0, 0}}},    // 2: bottom
                                 enclosureId);
    }

    double maxAbs(const std::vector<double> & values)
    {
        double worst = 0.0;
        for(const auto value : values)
        {
            worst = std::max(worst, std::abs(value));
        }
        return worst;
    }
}   // namespace

// The deficit on an open enclosure equals the view factor to the missing surface (the room). For a
// unit square F(bottom, top) = 0.414213562 and F(side, top) = 0.292893219, so removing the top
// leaves exactly those as the environment view factors.
TEST(EnclosureViewFactorsClosure, OpenEnclosureEnvironmentViewFactor)
{
    const auto result = computeEnclosureViewFactors(openUShape(), {});

    EXPECT_NEAR(0.292893219, result.environmentViewFactor[0], 1e-6);   // left wall -> room
    EXPECT_NEAR(0.292893219, result.environmentViewFactor[1], 1e-6);   // right wall -> room
    EXPECT_NEAR(0.414213562, result.environmentViewFactor[2], 1e-6);   // bottom -> room
}

// A closed convex enclosure already closes to one, so the environment view factor is ~zero.
TEST(EnclosureViewFactorsClosure, ClosedEnclosureHasZeroEnvironment)
{
    const auto square = segmentsFromEdges(closedPolygon({{0, 0}, {0, 1}, {1, 1}, {1, 0}}));
    const auto result = computeEnclosureViewFactors(square, {});

    for(const auto environment : result.environmentViewFactor)
    {
        EXPECT_NEAR(0.0, environment, 1e-9);
    }
}

// Smoothing drives a closed enclosure's rows to exactly one (environment view factor -> 0) while
// keeping the zero diagonal. The concave L-shape leaves a numerical residual that smoothing fixes.
TEST(EnclosureViewFactorsClosure, SmoothingClosesConcaveEnclosure)
{
    const auto lShape =
      segmentsFromEdges(closedPolygon({{0, 0}, {0, 3}, {3, 3}, {3, 1}, {1, 1}, {1, 0}}));

    const auto raw = computeEnclosureViewFactors(lShape, {});
    const auto smoothed =
      computeEnclosureViewFactors(lShape, {}, {.leastSquaresSmoothing = true});

    EXPECT_GT(maxAbs(raw.environmentViewFactor), 1e-6) << "fixture should have a closure residual";
    EXPECT_LT(maxAbs(smoothed.environmentViewFactor), maxAbs(raw.environmentViewFactor));
    EXPECT_NEAR(0.0, maxAbs(smoothed.environmentViewFactor), 1e-6);

    for(size_t idx = 0; idx < smoothed.viewFactors.size(); ++idx)
    {
        EXPECT_NEAR(0.0, smoothed.viewFactors(idx, idx), 1e-12);
    }
}

// Open enclosures are excluded from smoothing: their deficit is the environment view factor, so
// turning smoothing on (with the enclosure flagged open) must not change the matrix.
TEST(EnclosureViewFactorsClosure, SmoothingSkipsOpenEnclosure)
{
    const auto shape = openUShape(0);

    const auto without = computeEnclosureViewFactors(shape, {}).viewFactors;
    const auto with =
      computeEnclosureViewFactors(
        shape, {}, {.leastSquaresSmoothing = true, .openEnclosureIds = {0u}})
        .viewFactors;

    ASSERT_EQ(without.size(), with.size());
    for(size_t row = 0; row < without.size(); ++row)
    {
        for(size_t col = 0; col < without.size(); ++col)
        {
            EXPECT_EQ(without(row, col), with(row, col));
        }
    }
}
