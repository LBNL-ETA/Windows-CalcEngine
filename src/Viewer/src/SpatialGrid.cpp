#include <algorithm>
#include <cmath>
#include <limits>

#include "SpatialGrid.hpp"
#include "ViewerConstants.hpp"


namespace Viewer
{
    namespace
    {
        struct Extent
        {
            double minX;
            double minY;
            double maxX;
            double maxY;
        };

        Extent boundsOf(const std::vector<CSegment2D> & segments)
        {
            if(segments.empty())
            {
                return {.minX = 0.0, .minY = 0.0, .maxX = 0.0, .maxY = 0.0};
            }

            auto extent = Extent{.minX = std::numeric_limits<double>::max(),
                                 .minY = std::numeric_limits<double>::max(),
                                 .maxX = std::numeric_limits<double>::lowest(),
                                 .maxY = std::numeric_limits<double>::lowest()};
            for(const auto & segment : segments)
            {
                for(const auto & point : {segment.startPoint(), segment.endPoint()})
                {
                    extent.minX = std::min(extent.minX, point.x());
                    extent.minY = std::min(extent.minY, point.y());
                    extent.maxX = std::max(extent.maxX, point.x());
                    extent.maxY = std::max(extent.maxY, point.y());
                }
            }
            return extent;
        }

        // Maps a coordinate to a cell index, clamped into [0, count - 1]. A non-positive cell size
        // (degenerate extent along that axis) collapses to a single row/column.
        std::size_t cellIndex(double coord, double minCoord, double cellSize, std::size_t count)
        {
            if(cellSize <= 0.0)
            {
                return 0;
            }
            const auto raw = static_cast<long long>(std::floor((coord - minCoord) / cellSize));
            if(raw < 0)
            {
                return 0;
            }
            if(raw >= static_cast<long long>(count))
            {
                return count - 1;
            }
            return static_cast<std::size_t>(raw);
        }
    }   // namespace

    SpatialGrid::SpatialGrid(double minX,
                             double minY,
                             double cellWidth,
                             double cellHeight,
                             std::size_t cellsPerAxis,
                             std::vector<std::vector<std::size_t>> buckets) :
        m_minX(minX),
        m_minY(minY),
        m_cellWidth(cellWidth),
        m_cellHeight(cellHeight),
        m_cellsPerAxis(cellsPerAxis),
        m_buckets(std::move(buckets))
    {}

    std::size_t SpatialGrid::suggestCellsPerAxis(const std::size_t blockerCount)
    {
        // The measured runtime optimum is a flat basin around ~8 cells per axis and is nearly
        // independent of the blocker count (blockers lie on a ~1D boundary, so the cost balance is
        // set by geometry, not count). A fixed target is therefore both simpler and closer to the
        // optimum than sqrt(count). Never use more cells than blockers.
        constexpr std::size_t target = 8;
        return std::clamp<std::size_t>(blockerCount, 1, target);
    }

    SpatialGrid SpatialGrid::build(const std::vector<CSegment2D> & blockers,
                                   const std::size_t cellsPerAxis)
    {
        const auto cells = std::max<std::size_t>(cellsPerAxis, 1);
        const auto extent = boundsOf(blockers);

        const auto width = extent.maxX - extent.minX;
        const auto height = extent.maxY - extent.minY;
        const auto cellWidth =
          width > ViewerConstants::DISTANCE_TOLERANCE ? width / static_cast<double>(cells) : 0.0;
        const auto cellHeight =
          height > ViewerConstants::DISTANCE_TOLERANCE ? height / static_cast<double>(cells) : 0.0;

        std::vector<std::vector<std::size_t>> buckets(cells * cells);
        for(std::size_t idx = 0; idx < blockers.size(); ++idx)
        {
            const auto & blocker = blockers[idx];
            const auto startPoint = blocker.startPoint();
            const auto endPoint = blocker.endPoint();

            const auto colLo =
              cellIndex(std::min(startPoint.x(), endPoint.x()), extent.minX, cellWidth, cells);
            const auto colHi =
              cellIndex(std::max(startPoint.x(), endPoint.x()), extent.minX, cellWidth, cells);
            const auto rowLo =
              cellIndex(std::min(startPoint.y(), endPoint.y()), extent.minY, cellHeight, cells);
            const auto rowHi =
              cellIndex(std::max(startPoint.y(), endPoint.y()), extent.minY, cellHeight, cells);

            for(auto row = rowLo; row <= rowHi; ++row)
            {
                for(auto col = colLo; col <= colHi; ++col)
                {
                    buckets[row * cells + col].push_back(idx);
                }
            }
        }

        return SpatialGrid{
          extent.minX, extent.minY, cellWidth, cellHeight, cells, std::move(buckets)};
    }

    std::size_t SpatialGrid::columnOf(const double xCoord) const
    {
        return cellIndex(xCoord, m_minX, m_cellWidth, m_cellsPerAxis);
    }

    std::size_t SpatialGrid::rowOf(const double yCoord) const
    {
        return cellIndex(yCoord, m_minY, m_cellHeight, m_cellsPerAxis);
    }

    std::vector<std::size_t> SpatialGrid::candidatesForBoundingBox(const double minX,
                                                                   const double minY,
                                                                   const double maxX,
                                                                   const double maxY) const
    {
        const auto colLo = columnOf(minX);
        const auto colHi = columnOf(maxX);
        const auto rowLo = rowOf(minY);
        const auto rowHi = rowOf(maxY);

        std::vector<std::size_t> result;
        for(auto row = rowLo; row <= rowHi; ++row)
        {
            for(auto col = colLo; col <= colHi; ++col)
            {
                const auto & bucket = m_buckets[row * m_cellsPerAxis + col];
                result.insert(result.end(), bucket.begin(), bucket.end());
            }
        }

        std::ranges::sort(result);
        const auto duplicates = std::ranges::unique(result);
        result.erase(duplicates.begin(), duplicates.end());
        return result;
    }
}   // namespace Viewer
