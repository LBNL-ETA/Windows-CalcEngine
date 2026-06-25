#include <algorithm>
#include <array>
#include <vector>
#include <gtest/gtest.h>

#include "WCEViewer.hpp"
#include "../../src/SpatialGrid.hpp"


using namespace Viewer;

namespace
{
    // Four short blockers, one near each corner of [0, 10]^2.
    std::vector<CSegment2D> cornerBlockers()
    {
        return {
          CSegment2D{{1, 1}, {2, 1}},   // 0: lower-left
          CSegment2D{{8, 1}, {9, 1}},   // 1: lower-right
          CSegment2D{{1, 8}, {2, 8}},   // 2: upper-left
          CSegment2D{{8, 8}, {9, 8}},   // 3: upper-right
        };
    }

    // Ground truth: does the blocker's bounding box overlap the query box?
    bool bboxOverlaps(const CSegment2D & segment, const std::array<double, 4> & box)
    {
        const auto segMinX = std::min(segment.startPoint().x(), segment.endPoint().x());
        const auto segMaxX = std::max(segment.startPoint().x(), segment.endPoint().x());
        const auto segMinY = std::min(segment.startPoint().y(), segment.endPoint().y());
        const auto segMaxY = std::max(segment.startPoint().y(), segment.endPoint().y());
        return !(segMaxX < box[0] || segMinX > box[2] || segMaxY < box[1] || segMinY > box[3]);
    }

    bool contains(const std::vector<std::size_t> & values, std::size_t value)
    {
        return std::ranges::find(values, value) != values.end();
    }
}   // namespace

TEST(SpatialGrid, SingleCellReturnsAllBlockers)
{
    const auto blockers = cornerBlockers();
    const auto grid = SpatialGrid::build(blockers, 1);

    // A 1x1 grid puts everything in one cell, so any query returns every blocker (== brute force).
    const auto candidates = grid.candidatesForBoundingBox(1.0, 1.0, 2.0, 2.0);
    EXPECT_EQ(blockers.size(), candidates.size());
}

TEST(SpatialGrid, FinerGridNarrowsCandidates)
{
    const auto blockers = cornerBlockers();
    const auto grid = SpatialGrid::build(blockers, 4);

    // Query only the lower-left corner: the result must shrink but still contain that blocker.
    const auto candidates = grid.candidatesForBoundingBox(0.5, 0.5, 2.5, 2.5);
    EXPECT_LT(candidates.size(), blockers.size());
    EXPECT_TRUE(contains(candidates, 0u));
}

TEST(SpatialGrid, NeverProducesFalseNegative)
{
    const auto blockers = cornerBlockers();
    const auto grid = SpatialGrid::build(blockers, 4);

    const std::vector<std::array<double, 4>> queries{
      {0.5, 0.5, 2.5, 2.5}, {0, 0, 10, 10}, {7, 7, 9, 9}, {3, 3, 4, 4}, {0, 4, 10, 5}};
    for(const auto & box : queries)
    {
        const auto candidates =
          grid.candidatesForBoundingBox(box[0], box[1], box[2], box[3]);
        for(std::size_t idx = 0; idx < blockers.size(); ++idx)
        {
            if(bboxOverlaps(blockers[idx], box))
            {
                EXPECT_TRUE(contains(candidates, idx)) << "grid dropped overlapping blocker " << idx;
            }
        }
    }
}

TEST(SpatialGrid, HandlesDegenerateCollinearBlockers)
{
    // All blockers on a vertical line: the x extent is zero, which must not divide by zero.
    const std::vector<CSegment2D> blockers{
      CSegment2D{{5, 0}, {5, 1}}, CSegment2D{{5, 3}, {5, 4}}, CSegment2D{{5, 7}, {5, 8}}};
    const auto grid = SpatialGrid::build(blockers, 5);

    const auto candidates = grid.candidatesForBoundingBox(5.0, 0.0, 5.0, 8.0);
    for(std::size_t idx = 0; idx < blockers.size(); ++idx)
    {
        EXPECT_TRUE(contains(candidates, idx));
    }
}

TEST(SpatialGrid, SuggestCellsPerAxisIsClamped)
{
    EXPECT_EQ(1u, SpatialGrid::suggestCellsPerAxis(0));
    EXPECT_EQ(5u, SpatialGrid::suggestCellsPerAxis(5));    // fewer blockers than the target
    EXPECT_EQ(8u, SpatialGrid::suggestCellsPerAxis(100));  // capped at the target
    EXPECT_EQ(8u, SpatialGrid::suggestCellsPerAxis(100000));
}
