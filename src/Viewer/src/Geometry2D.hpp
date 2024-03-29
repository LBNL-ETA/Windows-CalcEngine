#ifndef GEOMETRY2D_H
#define GEOMETRY2D_H

#include <vector>
#include <memory>

#include <WCECommon.hpp>

namespace Viewer
{
    class CViewSegment2D;
    class CPoint2D;
    class CSegment2D;

    class CGeometry2D
    {
    public:
        CGeometry2D();

        void appendSegment(const CViewSegment2D & t_Segment);
        void appendGeometry2D(const CGeometry2D & t_Geometry2D);
        FenestrationCommon::SquareMatrix viewFactors();

        // Shifts all segments for given coordinates
        CGeometry2D Translate(double const t_x, double const t_y) const;

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
        bool thirdSurfaceShadowing(CViewSegment2D const & t_Segment1,
                                   CViewSegment2D const & t_Segment2) const;

        // Simplified version for fast calculation of third surface shadowing. Used for subsurfaces
        bool thirdSurfaceShadowingSimple(const CViewSegment2D & t_Segment1,
                                         const CViewSegment2D & t_Segment2) const;

        // Calculate view factor between two segments. This routine will check third surface
        // shadowing as well. It will divide segments into subsurfaces by default
        double viewFactorCoeff(const CViewSegment2D & t_Segment1,
                               const CViewSegment2D & t_Segment2) const;

        // returns Y coordinate of intersection with Y axis for line going through t_Point and for
        // given tangens of profile angle static double intersectionWithYAxis( double const tanPhi,
        // CPoint2D const& t_Point );

        // Checks if view factors are valid for current geometry
        void checkViewFactors();

        std::vector<CViewSegment2D> m_Segments;

        // Holds state for the view factors. No need to recalculate them every time since it is
        // time consuming operation.
        FenestrationCommon::SquareMatrix m_ViewFactors;
        bool m_ViewFactorsCalculated;
    };

}   // namespace Viewer

#endif
