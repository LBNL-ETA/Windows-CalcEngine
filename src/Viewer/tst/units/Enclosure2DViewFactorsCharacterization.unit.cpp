#include <cmath>
#include <vector>
#include <gtest/gtest.h>

#include "WCEViewer.hpp"
#include "WCECommon.hpp"


using namespace Viewer;
using namespace FenestrationCommon;

// Characterization (golden-master) tests for the existing CGeometry2D::viewFactors().
//
// These lock the current behavior on larger fixtures than the small Enclosure2D* tests,
// ahead of the planned viewer refactor (remove pointers, lift the math into pure free
// functions, add a spatial grid and threading). The assertions are physical invariants that
// hold by construction and therefore need no transcription of golden matrices:
//   - the diagonal is zero (a segment does not see itself),
//   - every coefficient lies in [0, 1],
//   - reciprocity F(i, j) * L_i == F(j, i) * L_j holds exactly,
//   - a closed convex enclosure has every row summing to one (all radiation lands somewhere).
// Any refactor that changes the numbers will break at least one of these.

namespace
{
    constexpr double pi = 3.14159265358979323846;

    // Builds a closed regular polygon enclosure. Vertices are generated clockwise so the
    // segment normals point inward (matching the winding of the unit-square fixture), which is
    // required for the segments to see each other across the enclosure interior.
    CGeometry2D makeRegularPolygon(const size_t numSides, const double radius)
    {
        std::vector<CPoint2D> vertices;
        vertices.reserve(numSides);
        for(size_t idx = 0; idx < numSides; ++idx)
        {
            const double angle = -2.0 * pi * static_cast<double>(idx) / static_cast<double>(numSides);
            vertices.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
        }

        CGeometry2D enclosure;
        for(size_t idx = 0; idx < numSides; ++idx)
        {
            const auto & startPoint = vertices[idx];
            const auto & endPoint = vertices[(idx + 1) % numSides];
            enclosure.appendSegment(CViewSegment2D{startPoint, endPoint});
        }

        return enclosure;
    }

    double rowSum(const SquareMatrix & matrix, const size_t row)
    {
        double sum = 0.0;
        for(size_t col = 0; col < matrix.size(); ++col)
        {
            sum += matrix(row, col);
        }
        return sum;
    }

    // Asserts the invariants shared by every closed convex enclosure.
    void expectClosedConvexInvariants(const CGeometry2D & enclosure)
    {
        CGeometry2D working{enclosure};
        const auto viewFactors = working.viewFactors();
        const auto & segments = working.segments();
        const auto size = viewFactors.size();

        for(size_t row = 0; row < size; ++row)
        {
            EXPECT_NEAR(0.0, viewFactors(row, row), 1e-9) << "diagonal must be zero at " << row;

            for(size_t col = 0; col < size; ++col)
            {
                const auto value = viewFactors(row, col);
                EXPECT_GE(value, -1e-9) << "negative view factor at (" << row << ", " << col << ")";
                EXPECT_LE(value, 1.0 + 1e-9) << "view factor above one at (" << row << ", " << col << ")";
            }

            // Reciprocity: F(i, j) * L_i == F(j, i) * L_j.
            const auto lengthRow = segments[row].length();
            for(size_t col = row + 1; col < size; ++col)
            {
                const auto lengthCol = segments[col].length();
                EXPECT_NEAR(viewFactors(row, col) * lengthRow,
                            viewFactors(col, row) * lengthCol,
                            1e-9)
                  << "reciprocity broken at (" << row << ", " << col << ")";
            }

            // Closure: a closed enclosure has each row summing to one.
            EXPECT_NEAR(1.0, rowSum(viewFactors, row), 1e-6) << "row " << row << " does not close to one";
        }
    }
}   // namespace

TEST(Enclosure2DViewFactorsCharacterization, RegularOctagonInvariants)
{
    SCOPED_TRACE("Characterization: closed regular octagon (clean cross-string path).");
    expectClosedConvexInvariants(makeRegularPolygon(8, 1.0));
}

TEST(Enclosure2DViewFactorsCharacterization, RegularPolygon24Invariants)
{
    SCOPED_TRACE("Characterization: closed 24-gon (larger clean fixture).");
    expectClosedConvexInvariants(makeRegularPolygon(24, 2.5));
}

// Concave (L-shaped) closed enclosure. Here some non-adjacent pairs are blocked, so this
// exercises the self-shadow / third-surface path. Reciprocity, the zero diagonal, and the
// [0, 1] range still hold by construction, and at least one non-adjacent pair must be fully
// blocked (zero view factor).
TEST(Enclosure2DViewFactorsCharacterization, ConcaveLShapeInvariants)
{
    SCOPED_TRACE("Characterization: closed concave L-shape (blocked / partial path).");

    // Clockwise traversal of an L-shape so normals point inward.
    const std::vector<CPoint2D> vertices{
      {0.0, 0.0}, {0.0, 3.0}, {3.0, 3.0}, {3.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}};

    CGeometry2D enclosure;
    for(size_t idx = 0; idx < vertices.size(); ++idx)
    {
        const auto & startPoint = vertices[idx];
        const auto & endPoint = vertices[(idx + 1) % vertices.size()];
        enclosure.appendSegment(CViewSegment2D{startPoint, endPoint});
    }

    const auto viewFactors = enclosure.viewFactors();
    const auto & segments = enclosure.segments();
    const auto size = viewFactors.size();

    size_t blockedPairs = 0;
    for(size_t row = 0; row < size; ++row)
    {
        EXPECT_NEAR(0.0, viewFactors(row, row), 1e-9) << "diagonal must be zero at " << row;

        const auto lengthRow = segments[row].length();
        for(size_t col = row + 1; col < size; ++col)
        {
            const auto value = viewFactors(row, col);
            EXPECT_GE(value, -1e-9);
            EXPECT_LE(value, 1.0 + 1e-9);

            const auto lengthCol = segments[col].length();
            EXPECT_NEAR(value * lengthRow, viewFactors(col, row) * lengthCol, 1e-9)
              << "reciprocity broken at (" << row << ", " << col << ")";

            if(value == 0.0)
            {
                ++blockedPairs;
            }
        }
    }

    EXPECT_GT(blockedPairs, 0u) << "the concave enclosure must produce at least one blocked pair";
}
