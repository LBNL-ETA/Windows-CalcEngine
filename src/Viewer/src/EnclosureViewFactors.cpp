#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <vector>

#include "EnclosureViewFactors.hpp"
#include "SpatialGrid.hpp"
#include "ViewFactorClosure.hpp"
#include "ViewFactorMath.hpp"
#include "ViewerConstants.hpp"


using namespace FenestrationCommon;

// Serial, multi-enclosure view-factor engine. The pair logic mirrors
// CGeometry2D::checkViewFactors exactly (cross-string for clear pairs, subdivision for
// partially shadowed/blocked pairs), but it is built on the pure Viewer::detail free functions
// and supports multiple enclosures and an explicit blocker list. The EnclosureViewFactorsParity
// tests pin it to CGeometry2D. The spatial grid and threading land in later steps.
namespace Viewer
{
    namespace
    {
        struct EngineSegment
        {
            CSegment2D geometry;
            double length;
            std::size_t enclosureId;
        };

        struct Bounds
        {
            double minX;
            double minY;
            double maxX;
            double maxY;
        };

        // Axis-aligned bounding box of a segment pair. It contains the connecting rays and the
        // seg1-seg2 polygon, so it is the correct region to query the grid for both blocking tests.
        Bounds pairBounds(const CSegment2D & segmentA, const CSegment2D & segmentB)
        {
            const std::initializer_list<double> xCoords{segmentA.startPoint().x(),
                                                        segmentA.endPoint().x(),
                                                        segmentB.startPoint().x(),
                                                        segmentB.endPoint().x()};
            const std::initializer_list<double> yCoords{segmentA.startPoint().y(),
                                                        segmentA.endPoint().y(),
                                                        segmentB.startPoint().y(),
                                                        segmentB.endPoint().y()};
            return {.minX = std::min(xCoords),
                    .minY = std::min(yCoords),
                    .maxX = std::max(xCoords),
                    .maxY = std::max(yCoords)};
        }

        // CGeometry2D::pointInSegmentsView, over plain CSegment2D.
        bool pointInSegmentsView(const CSegment2D & segmentA,
                                 const CSegment2D & segmentB,
                                 const CPoint2D & point)
        {
            std::vector<CSegment2D> polygon;
            polygon.push_back(segmentA);
            const CSegment2D side2{segmentA.endPoint(), segmentB.startPoint()};
            if(side2.length() > 0)
            {
                polygon.push_back(side2);
            }
            polygon.push_back(segmentB);
            const CSegment2D side4{segmentB.endPoint(), segmentA.startPoint()};
            if(side4.length() > 0)
            {
                polygon.push_back(side4);
            }

            return std::ranges::all_of(polygon, [&](const CSegment2D & edge) {
                return detail::pointPosition(edge, point) == PointPosition::Visible;
            });
        }

        // CGeometry2D::thirdSurfaceShadowing (full test on the connecting rays), restricted to the
        // grid candidates for the pair's bounding box.
        bool thirdSurfaceShadowing(const CSegment2D & segmentA,
                                   const CSegment2D & segmentB,
                                   const std::vector<CSegment2D> & blockers,
                                   const SpatialGrid & grid)
        {
            std::vector<CSegment2D> rays;
            const CSegment2D r11{segmentA.startPoint(), segmentB.endPoint()};
            if(r11.length() > 0)
            {
                rays.push_back(r11);
            }
            const CSegment2D r22{segmentA.endPoint(), segmentB.startPoint()};
            if(r22.length() > 0)
            {
                rays.push_back(r22);
            }
            if(rays.empty())
            {
                return false;
            }

            const auto bounds = pairBounds(segmentA, segmentB);
            const auto candidates =
              grid.candidatesForBoundingBox(bounds.minX, bounds.minY, bounds.maxX, bounds.maxY);

            const auto blocks = [&](const std::size_t idx) {
                const auto & blocker = blockers[idx];
                if(blocker == segmentA || blocker == segmentB)
                {
                    return false;
                }
                const auto crossesRay = [&](const CSegment2D & ray) {
                    return ray.intersectionWithSegment(blocker);
                };
                return std::ranges::any_of(rays, crossesRay)
                       || pointInSegmentsView(segmentA, segmentB, blocker.startPoint())
                       || pointInSegmentsView(segmentA, segmentB, blocker.endPoint());
            };

            return std::ranges::any_of(candidates, blocks);
        }

        // CGeometry2D::thirdSurfaceShadowingSimple (center-line test, used while subdividing),
        // restricted to the grid candidates for the pair's bounding box.
        bool thirdSurfaceShadowingSimple(const CSegment2D & segmentA,
                                         const CSegment2D & segmentB,
                                         const std::vector<CSegment2D> & blockers,
                                         const SpatialGrid & grid)
        {
            const CSegment2D centerLine{segmentA.centerPoint(), segmentB.centerPoint()};
            const auto bounds = pairBounds(segmentA, segmentB);
            const auto candidates =
              grid.candidatesForBoundingBox(bounds.minX, bounds.minY, bounds.maxX, bounds.maxY);

            return std::ranges::any_of(candidates, [&](const std::size_t idx) {
                const auto & blocker = blockers[idx];
                return blocker != segmentA && blocker != segmentB
                       && centerLine.intersectionWithSegment(blocker);
            });
        }

        // CGeometry2D::viewFactorCoeff (subdivision path for partial / blocked pairs).
        double subdividedCoefficient(const CSegment2D & segmentA,
                                     const CSegment2D & segmentB,
                                     const std::vector<CSegment2D> & blockers,
                                     const SpatialGrid & grid,
                                     const std::size_t subdivision)
        {
            const auto subsA = detail::subdivide(segmentA, subdivision);
            const auto subsB = detail::subdivide(segmentB, subdivision);

            double coefficient = 0.0;
            for(const auto & subA : subsA)
            {
                for(const auto & subB : subsB)
                {
                    if(detail::selfShadow(subA, subB) == Shadowing::No
                       && !thirdSurfaceShadowingSimple(subA, subB, blockers, grid))
                    {
                        coefficient += detail::crossStringCoefficient(subA, subB);
                    }
                }
            }

            return coefficient < ViewerConstants::MIN_VIEW_COEFF ? 0.0 : coefficient;
        }

        // Raw cross-string coefficient for one pair (CGeometry2D::checkViewFactors body).
        double pairCoefficient(const CSegment2D & segmentA,
                               const CSegment2D & segmentB,
                               const std::vector<CSegment2D> & blockers,
                               const SpatialGrid & grid,
                               const std::size_t subdivision)
        {
            const auto shadow = detail::selfShadow(segmentA, segmentB);
            if(shadow == Shadowing::Total)
            {
                return 0.0;
            }

            const auto blocked = thirdSurfaceShadowing(segmentA, segmentB, blockers, grid);
            if(!blocked && shadow == Shadowing::No)
            {
                return detail::crossStringCoefficient(segmentA, segmentB);
            }
            return subdividedCoefficient(segmentA, segmentB, blockers, grid, subdivision);
        }

        std::vector<EngineSegment> buildSegments(const std::vector<RadiationSegment> & segments)
        {
            std::vector<EngineSegment> result;
            result.reserve(segments.size());
            for(const auto & segment : segments)
            {
                const CSegment2D geometry{segment.startPoint, segment.endPoint};
                result.push_back({.geometry = geometry,
                                  .length = geometry.length(),
                                  .enclosureId = segment.enclosureId});
            }
            return result;
        }

        // Blockers are all radiation segments plus the explicit blocking segments. A pair's own
        // two segments are excluded inside the blocking tests by value comparison.
        std::vector<CSegment2D> buildBlockers(const std::vector<EngineSegment> & segments,
                                              const std::vector<BlockingSegment> & blockers)
        {
            std::vector<CSegment2D> result;
            result.reserve(segments.size() + blockers.size());
            for(const auto & segment : segments)
            {
                result.push_back(segment.geometry);
            }
            for(const auto & blocker : blockers)
            {
                result.emplace_back(blocker.startPoint, blocker.endPoint);
            }
            return result;
        }

        // Per-row deficit (1 - row sum). For an open enclosure this is the view factor to the
        // environment; for a closed enclosure it is the small closure residual.
        std::vector<double> environmentViewFactors(const SquareMatrix & viewFactors)
        {
            const auto size = viewFactors.size();
            std::vector<double> result(size, 0.0);
            for(std::size_t row = 0; row < size; ++row)
            {
                double sum = 0.0;
                for(std::size_t col = 0; col < size; ++col)
                {
                    sum += viewFactors(row, col);
                }
                result[row] = 1.0 - sum;
            }
            return result;
        }
    }   // namespace

    EnclosureViewFactorResult
    computeEnclosureViewFactors(const std::vector<RadiationSegment> & segments,
                                const std::vector<BlockingSegment> & blockers,
                                const ViewFactorOptions & options)
    {
        const auto engineSegments = buildSegments(segments);
        const auto blockerSegments = buildBlockers(engineSegments, blockers);
        const auto cellsPerAxis = options.gridCellsPerAxis.value_or(
          SpatialGrid::suggestCellsPerAxis(blockerSegments.size()));
        const auto grid = SpatialGrid::build(blockerSegments, cellsPerAxis);
        const auto size = engineSegments.size();

        SquareMatrix viewFactors{size};

        // Each row is independent and writes only its own upper-triangle cells (row, col) and
        // their mirror (col, row) for col > row, so distinct rows touch disjoint cells - the
        // parallel pass is race-free without locks (the matrix is pre-sized and never resized).
        const auto computeRow = [&](const std::size_t row) {
            for(std::size_t col = row + 1; col < size; ++col)
            {
                if(engineSegments[row].enclosureId != engineSegments[col].enclosureId)
                {
                    continue;
                }

                const auto coefficient = pairCoefficient(engineSegments[row].geometry,
                                                          engineSegments[col].geometry,
                                                          blockerSegments,
                                                          grid,
                                                          options.subdivision);
                viewFactors(row, col) = coefficient / (2.0 * engineSegments[row].length);
                viewFactors(col, row) = coefficient / (2.0 * engineSegments[col].length);
            }
        };

        if(options.multithread && size > 1)
        {
            FenestrationCommon::executeInParallel<std::size_t>(0, size - 1, computeRow);
        }
        else
        {
            for(std::size_t row = 0; row < size; ++row)
            {
                computeRow(row);
            }
        }

        // Reciprocity is implicit (one coefficient feeds both off-diagonal entries). Optional
        // least-squares smoothing closes the closed enclosures (rows -> 1); open enclosures keep
        // their deficit as the environment view factor.
        if(options.leastSquaresSmoothing)
        {
            std::vector<double> areas;
            std::vector<std::size_t> enclosureIds;
            areas.reserve(size);
            enclosureIds.reserve(size);
            for(const auto & segment : engineSegments)
            {
                areas.push_back(segment.length);
                enclosureIds.push_back(segment.enclosureId);
            }
            viewFactors =
              smoothViewFactors(viewFactors, areas, enclosureIds, options.openEnclosureIds);
        }

        return {.viewFactors = viewFactors,
                .environmentViewFactor = environmentViewFactors(viewFactors)};
    }
}   // namespace Viewer
