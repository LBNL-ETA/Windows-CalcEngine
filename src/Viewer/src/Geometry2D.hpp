#pragma once

#include <optional>
#include <vector>

#include <WCECommon.hpp>

namespace Viewer
{
    class CViewSegment2D;
    class CPoint2D;
    class CSegment2D;

    class CGeometry2D
    {
    public:
        void appendSegment(const CViewSegment2D & t_Segment);
        void appendGeometry2D(const CGeometry2D & t_Geometry2D);
        FenestrationCommon::SquareMatrix viewFactors();

        // Shifts all segments for given coordinates
        [[nodiscard]] CGeometry2D Translate(double t_x, double t_y) const;

        // First and last point function are necessary to make connection between different
        // enclosures
        [[nodiscard]] CPoint2D firstPoint() const;
        [[nodiscard]] CPoint2D lastPoint() const;

        // Entry point of the enclosure. Important since enclosure can be entered in reverse way.
        [[nodiscard]] CPoint2D entryPoint() const;

        // Exit point of the enclosure. Important since enclosure can be entered in reverse way.
        [[nodiscard]] CPoint2D exitPoint() const;

        [[nodiscard]] const std::vector<CViewSegment2D> & segments() const;

    private:
        // Determines if point is laying inside rectangle made by two segments. That would mean that
        // point is in blocking view of two segments
        static bool pointInSegmentsView(CViewSegment2D const & t_Segment1,
                                        CViewSegment2D const & t_Segment2,
                                        CPoint2D const & t_Point);

        // Calculates third surface blocking.
        [[nodiscard]] bool thirdSurfaceShadowing(CViewSegment2D const & t_Segment1,
                                                 CViewSegment2D const & t_Segment2) const;

        // Simplified version for fast calculation of third surface shadowing. Used for subsurfaces
        [[nodiscard]] bool thirdSurfaceShadowingSimple(const CViewSegment2D & t_Segment1,
                                                       const CViewSegment2D & t_Segment2) const;

        // Calculate view factor between two segments. This routine will check third surface
        // shadowing as well. It will divide segments into subsurfaces by default
        [[nodiscard]] double viewFactorCoeff(const CViewSegment2D & t_Segment1,
                                             const CViewSegment2D & t_Segment2) const;

        // returns Y coordinate of intersection with Y axis for line going through t_Point and for
        // given tangens of profile angle static double intersectionWithYAxis( double const tanPhi,
        // CPoint2D const& t_Point );

        // Computes the view-factor matrix for the current geometry (no caching).
        [[nodiscard]] FenestrationCommon::SquareMatrix calculateViewFactors() const;

        std::vector<CViewSegment2D> m_Segments;

        // Cached view factors; empty until first computed, cleared whenever segments change.
        // Recalculating them is expensive, so the result is memoized here.
        std::optional<FenestrationCommon::SquareMatrix> m_ViewFactors;
    };

}   // namespace Viewer
