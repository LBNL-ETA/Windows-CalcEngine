#pragma once

#include <cstddef>
#include <vector>

#include "Segment2D.hpp"

namespace Viewer
{
    // Uniform spatial grid over the blocker segments' bounding box, built once and queried
    // read-only. It narrows the third-surface blocking tests from "every blocker" to the few
    // whose cells overlap a segment pair's region. Each blocker is bucketed into the cells of its
    // bounding box, and candidatesForBoundingBox returns the blockers whose cells overlap the
    // queried box, so the result is always a superset of the truly relevant blockers (no false
    // negatives) - it only ever narrows the brute-force set, never changes the answer.
    class SpatialGrid
    {
    public:
        [[nodiscard]] static SpatialGrid build(const std::vector<CSegment2D> & blockers,
                                               std::size_t cellsPerAxis);

        // Heuristic cell count per axis from the blocker count (legacy sweet spot ~10-20).
        [[nodiscard]] static std::size_t suggestCellsPerAxis(std::size_t blockerCount);

        // De-duplicated indices of blockers whose cells overlap the given bounding box.
        [[nodiscard]] std::vector<std::size_t>
          candidatesForBoundingBox(double minX, double minY, double maxX, double maxY) const;

    private:
        SpatialGrid(double minX,
                    double minY,
                    double cellWidth,
                    double cellHeight,
                    std::size_t cellsPerAxis,
                    std::vector<std::vector<std::size_t>> buckets);

        [[nodiscard]] std::size_t columnOf(double xCoord) const;
        [[nodiscard]] std::size_t rowOf(double yCoord) const;

        double m_minX;
        double m_minY;
        double m_cellWidth;
        double m_cellHeight;
        std::size_t m_cellsPerAxis;
        std::vector<std::vector<std::size_t>> m_buckets;   // row-major, m_cellsPerAxis^2 cells
    };
}   // namespace Viewer
