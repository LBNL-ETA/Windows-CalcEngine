#include <cmath>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include "WCEViewer.hpp"


using namespace Viewer;
using namespace FenestrationCommon;

// Cross-check parity: the new computeEnclosureViewFactors engine must reproduce the legacy
// CGeometry2D::viewFactors() raw matrix exactly on a single enclosure with no extra blockers.
// CGeometry2D is the oracle, so this proves the lift-to-pure engine is faithful.

namespace
{
    void expectEnginesAgree(const std::vector<std::pair<CPoint2D, CPoint2D>> & edges)
    {
        CGeometry2D legacy;
        std::vector<RadiationSegment> segments;
        for(const auto & [start, end] : edges)
        {
            legacy.appendSegment(CViewSegment2D{start, end});
            segments.push_back(
              {.startPoint = start, .endPoint = end, .emissivity = 0.9, .enclosureId = 0u});
        }

        const auto legacyVF = legacy.viewFactors();

        // Raw options: no closure / smoothing, serial, so the result matches CGeometry2D's raw
        // matrix. Subdivision is left at its default to match CGeometry2D.
        const ViewFactorOptions rawOptions{
          .enforceClosure = false, .leastSquaresSmoothing = false, .multithread = false};
        const auto result = computeEnclosureViewFactors(segments, {}, rawOptions);

        ASSERT_EQ(legacyVF.size(), result.viewFactors.size());
        for(size_t row = 0; row < legacyVF.size(); ++row)
        {
            for(size_t col = 0; col < legacyVF.size(); ++col)
            {
                EXPECT_NEAR(legacyVF(row, col), result.viewFactors(row, col), 1e-12)
                  << "mismatch at (" << row << ", " << col << ")";
            }
        }
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
}   // namespace

TEST(EnclosureViewFactorsParity, UnitSquare)
{
    expectEnginesAgree(closedPolygon({{0, 0}, {0, 1}, {1, 1}, {1, 0}}));
}

TEST(EnclosureViewFactorsParity, RegularOctagon)
{
    std::vector<CPoint2D> vertices;
    constexpr double pi = 3.14159265358979323846;
    for(size_t idx = 0; idx < 8; ++idx)
    {
        const double angle = -2.0 * pi * static_cast<double>(idx) / 8.0;
        vertices.emplace_back(std::cos(angle), std::sin(angle));
    }
    expectEnginesAgree(closedPolygon(vertices));
}

TEST(EnclosureViewFactorsParity, ConcaveLShape)
{
    expectEnginesAgree(closedPolygon({{0, 0}, {0, 3}, {3, 3}, {3, 1}, {1, 1}, {1, 0}}));
}

TEST(EnclosureViewFactorsParity, BlockingSurfaceOpenGeometry)
{
    // The 3-segment geometry from Enclosure2DViewFactorsBlockingSurface1 (middle segment blocks).
    expectEnginesAgree({
      {CPoint2D{10, 0}, CPoint2D{0, 0}},
      {CPoint2D{0, 5}, CPoint2D{10, 5}},
      {CPoint2D{5, 2}, CPoint2D{0, 2}},
    });
}

// New capability beyond CGeometry2D: two separate enclosures in one call must not see each
// other (block-diagonal matrix), and each closed block must still close to one.
TEST(EnclosureViewFactors, MultiEnclosureIsBlockDiagonal)
{
    std::vector<RadiationSegment> segments;
    const auto addSquare = [&](double offsetX, std::size_t enclosureId) {
        const std::vector<CPoint2D> vertices{
          {offsetX, 0}, {offsetX, 1}, {offsetX + 1, 1}, {offsetX + 1, 0}};
        for(size_t idx = 0; idx < vertices.size(); ++idx)
        {
            segments.push_back({.startPoint = vertices[idx],
                                .endPoint = vertices[(idx + 1) % vertices.size()],
                                .emissivity = 0.9,
                                .enclosureId = enclosureId});
        }
    };
    addSquare(0.0, 0);
    addSquare(10.0, 1);

    const ViewFactorOptions rawOptions{
      .enforceClosure = false, .leastSquaresSmoothing = false, .multithread = false};
    const auto viewFactors = computeEnclosureViewFactors(segments, {}, rawOptions).viewFactors;

    ASSERT_EQ(8u, viewFactors.size());
    for(size_t row = 0; row < 8; ++row)
    {
        double withinSum = 0.0;
        for(size_t col = 0; col < 8; ++col)
        {
            const bool sameEnclosure = (row < 4) == (col < 4);
            if(sameEnclosure)
            {
                withinSum += viewFactors(row, col);
            }
            else
            {
                EXPECT_EQ(0.0, viewFactors(row, col))
                  << "cross-enclosure leak at (" << row << ", " << col << ")";
            }
        }
        EXPECT_NEAR(1.0, withinSum, 1e-9) << "row " << row << " block does not close to one";
    }
}
