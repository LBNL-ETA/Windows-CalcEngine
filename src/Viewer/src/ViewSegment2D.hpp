#ifndef VIEWSEGMENT2D_H
#define VIEWSEGMENT2D_H

#include <memory>
#include <vector>

#include "Segment2D.hpp"

namespace Viewer
{
    enum class Shadowing
    {
        No = 0,
        Partial,
        Total
    };

    enum class PointPosition
    {
        Visible,
        Invisible,
        OnLine
    };

    class CViewSegment2D : public CSegment2D, public std::enable_shared_from_this<CViewSegment2D>
    {
    public:
        CViewSegment2D() = default;
        CViewSegment2D(const CPoint2D & t_StartPoint, const CPoint2D & t_EndPoint);

        CViewSegment2D getNormal();

        bool operator==(CViewSegment2D const & rhs) const;
        bool operator!=(CViewSegment2D const & rhs) const;

        // Calculates view factor coefficient. It needs to be divided by segment length to get real
        // view factor.
        double viewFactorCoefficient(CSegment2D const & t_Segment) const;

        // Self shadowing between two segments
        Shadowing selfShadowing(CViewSegment2D const & t_Segment) const;

        // To determine position from the perspective of the segment
        PointPosition position(const CPoint2D & t_Point) const;

        // Divide segment into number of subsegments
        std::vector<CViewSegment2D> subSegments(size_t numSegments) const;

        // Translates segment for given coordinates
        CViewSegment2D translate(double t_x, double t_y);

    private:
        // How much segment is self shadowed (No, Partial, Total)
        Shadowing isInSelfShadow(CViewSegment2D const & t_Segment) const;
    };

}   // namespace Viewer

#endif
