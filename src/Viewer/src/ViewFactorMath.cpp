#include <stdexcept>

#include "ViewFactorMath.hpp"
#include "ViewerConstants.hpp"


namespace Viewer::detail
{
    namespace
    {
        // How many of the other segment's endpoints fall in this segment's self-shadow
        // (mirrors CViewSegment2D::isInSelfShadow): 0 -> No, 1 -> Partial, 2 -> Total.
        Shadowing segmentSelfShadow(const CSegment2D & viewer, const CSegment2D & other)
        {
            int invisibles = 0;
            const auto atStart = pointPosition(viewer, other.startPoint());
            const auto atEnd = pointPosition(viewer, other.endPoint());

            if(atStart == PointPosition::Invisible)
            {
                ++invisibles;
            }
            if(atEnd == PointPosition::Invisible)
            {
                ++invisibles;
            }
            if(invisibles == 1
               && (atStart == PointPosition::OnLine || atEnd == PointPosition::OnLine))
            {
                ++invisibles;
            }

            return static_cast<Shadowing>(invisibles);
        }
    }   // namespace

    PointPosition pointPosition(const CSegment2D & segment, const CPoint2D & point)
    {
        const auto start = segment.startPoint();
        const auto end = segment.endPoint();
        if(point.sameCoordinates(start) || point.sameCoordinates(end))
        {
            return PointPosition::OnLine;
        }

        const auto dx = end.x() - start.x();
        const auto dy = end.y() - start.y();
        const auto side = dx * (point.y() - start.y()) - dy * (point.x() - start.x());

        if(side > ViewerConstants::DISTANCE_TOLERANCE)
        {
            return PointPosition::Invisible;
        }
        if(side < -ViewerConstants::DISTANCE_TOLERANCE)
        {
            return PointPosition::Visible;
        }
        return PointPosition::OnLine;
    }

    Shadowing selfShadow(const CSegment2D & segmentA, const CSegment2D & segmentB)
    {
        const auto fromA = segmentSelfShadow(segmentA, segmentB);
        const auto fromB = segmentSelfShadow(segmentB, segmentA);

        if(fromA == Shadowing::Total || fromB == Shadowing::Total)
        {
            return Shadowing::Total;
        }
        if(fromA == Shadowing::No && fromB == Shadowing::No)
        {
            return Shadowing::No;
        }
        return Shadowing::Partial;
    }

    double crossStringCoefficient(const CSegment2D & segmentA, const CSegment2D & segmentB)
    {
        const auto r11 = CSegment2D{segmentA.startPoint(), segmentB.endPoint()}.length();
        const auto r22 = CSegment2D{segmentA.endPoint(), segmentB.startPoint()}.length();
        const auto r12 = CSegment2D{segmentA.startPoint(), segmentB.startPoint()}.length();
        const auto r21 = CSegment2D{segmentA.endPoint(), segmentB.endPoint()}.length();

        const auto coefficient = r12 + r21 - r22 - r11;
        return coefficient < ViewerConstants::MIN_VIEW_COEFF ? 0.0 : coefficient;
    }

    std::vector<CSegment2D> subdivide(const CSegment2D & segment, const std::size_t count)
    {
        if(count == 0)
        {
            throw std::runtime_error("Number of subsegments must be greater than zero.");
        }

        const auto start = segment.startPoint();
        const auto end = segment.endPoint();
        const auto dx = (end.x() - start.x()) / static_cast<double>(count);
        const auto dy = (end.y() - start.y()) / static_cast<double>(count);

        std::vector<CSegment2D> result;
        result.reserve(count);
        CPoint2D from = start;
        for(std::size_t idx = 1; idx <= count; ++idx)
        {
            const CPoint2D to{start.x() + static_cast<double>(idx) * dx,
                              start.y() + static_cast<double>(idx) * dy};
            result.emplace_back(from, to);
            from = to;
        }

        return result;
    }
}   // namespace Viewer::detail
