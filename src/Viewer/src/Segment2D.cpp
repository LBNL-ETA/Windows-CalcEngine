#include <cmath>
#include <stdexcept>
#include <algorithm>

#include "Segment2D.hpp"
#include "ViewerConstants.hpp"

namespace Viewer
{
    CSegment2D::CSegment2D(const CPoint2D & t_StartPoint, const CPoint2D & t_EndPoint) :
        m_StartPoint(t_StartPoint),
        m_EndPoint(t_EndPoint),
        m_Length(calculateLength(m_StartPoint, m_EndPoint)),
        m_CenterPoint(calculateCenter(m_StartPoint, m_EndPoint))

    {}

    CPoint2D CSegment2D::startPoint() const
    {
        return m_StartPoint;
    }

    CPoint2D CSegment2D::endPoint() const
    {
        return m_EndPoint;
    }

    CPoint2D CSegment2D::centerPoint() const
    {
        return m_CenterPoint;
    }

    bool CSegment2D::operator==(CSegment2D const & rhs) const
    {
        return m_StartPoint == rhs.m_StartPoint && m_EndPoint == rhs.m_EndPoint;
    }

    bool CSegment2D::operator!=(CSegment2D const & rhs) const
    {
        return !(*this == rhs);
    }

    double CSegment2D::length() const
    {
        return m_Length;
    }

    bool CSegment2D::intersectionWithSegment(const CSegment2D & t_Segment) const
    {
        auto aInt = false;
        if(length() != 0)
        {
            auto aPoint = intersection(t_Segment);

            if(aPoint.has_value())
            {
                aInt = isInRectangleRange(aPoint.value())
                       && t_Segment.isInRectangleRange(aPoint.value());
            }
        }

        return aInt;
    }

    IntersectionStatus CSegment2D::intersectionWithLine(const CSegment2D & t_Segment) const
    {
        auto status = IntersectionStatus::No;

        if(length() != 0)
        {
            auto aPoint = intersection(t_Segment);

            if(aPoint.has_value())
            {
                auto aInt = t_Segment.isInRectangleRange(aPoint.value());
                if(aInt)
                {
                    status = IntersectionStatus::Segment;
                }
                if(t_Segment.startPoint().sameCoordinates(*aPoint)
                   || t_Segment.endPoint().sameCoordinates(*aPoint))
                {
                    status = IntersectionStatus::Point;
                }
            }
        }

        return status;
    }

    double CSegment2D::dotProduct(const CSegment2D & t_Segment) const
    {
        auto p1 = intensity();
        auto p2 = t_Segment.intensity();

        return p1.dotProduct(p2);
    }

    // Translates segment for given coordinates
    std::shared_ptr<CSegment2D> CSegment2D::translate(double const t_x, double const t_y) const
    {
        CPoint2D startPoint{m_StartPoint.x() + t_x, m_StartPoint.y() + t_y};
        CPoint2D endPoint{m_EndPoint.x() + t_x, m_EndPoint.y() + t_y};
        auto aSegment = std::make_shared<CSegment2D>(startPoint, endPoint);
        return aSegment;
    }

    double CSegment2D::calculateLength(const CPoint2D & startPoint, const CPoint2D & endPoint)
    {
        auto deltaX = endPoint.x() - startPoint.x();
        auto deltaY = endPoint.y() - startPoint.y();
        return std::sqrt(deltaX * deltaX + deltaY * deltaY);
    }

    CPoint2D CSegment2D::calculateCenter(const CPoint2D & startPoint, const CPoint2D & endPoint)
    {
        auto x = (endPoint.x() + startPoint.x()) / 2;
        auto y = (endPoint.y() + startPoint.y()) / 2;

        return {x, y};
    }

    std::optional<CPoint2D> CSegment2D::intersection(const CSegment2D & t_Segment) const
    {
        auto A1 = coeffA();
        auto A2 = t_Segment.coeffA();
        auto B1 = coeffB();
        auto B2 = t_Segment.coeffB();

        auto C1 = coeffC();
        auto C2 = t_Segment.coeffC();
        auto x = 0.0;
        auto y = 0.0;

        if(std::abs(A1) > ViewerConstants::DISTANCE_TOLERANCE)
        {
            auto t1 = C2 - C1 * A2 / A1;
            auto t2 = B2 - B1 * A2 / A1;
            // assert( t2 != 0 );
            if(std::abs(t2) > ViewerConstants::DISTANCE_TOLERANCE)
            {
                y = t1 / t2;
                x = (C1 - B1 * y) / A1;
            }
            else
            {
                return {};
            }
        }
        else
        {
            y = C1 / B1;
            x = (C2 - B2 * y) / A2;
        }

        return CPoint2D{x, y};
    }

    bool CSegment2D::isInRectangleRange(const CPoint2D & t_Point) const
    {
        // Should return in range only if point is not exactly on the line's point
        auto inXRange = false;
        auto inYRange = false;

        // Check X range
        auto const maxX = std::max(m_EndPoint.x(), m_StartPoint.x());
        auto const minX = std::min(m_EndPoint.x(), m_StartPoint.x());
        if(std::abs(maxX - minX) > ViewerConstants::DISTANCE_TOLERANCE)
        {
            if(t_Point.x() < (maxX - ViewerConstants::DISTANCE_TOLERANCE)
               && t_Point.x() > (minX + ViewerConstants::DISTANCE_TOLERANCE))
            {
                inXRange = true;
            }
        }
        else
        {
            if(std::abs(t_Point.x() - maxX) < ViewerConstants::DISTANCE_TOLERANCE)
            {
                inXRange = true;
            }
        }

        // Check Y range
        auto const maxY = std::max(m_EndPoint.y(), m_StartPoint.y());
        auto const minY = std::min(m_EndPoint.y(), m_StartPoint.y());
        if(std::abs(maxY - minY) > ViewerConstants::DISTANCE_TOLERANCE)
        {
            if(t_Point.y() < (maxY - ViewerConstants::DISTANCE_TOLERANCE)
               && t_Point.y() > (minY + ViewerConstants::DISTANCE_TOLERANCE))
            {
                inYRange = true;
            }
        }
        else
        {
            if(std::abs(t_Point.y() - maxY) < ViewerConstants::DISTANCE_TOLERANCE)
            {
                inYRange = true;
            }
        }

        return inXRange && inYRange;
    }

    double CSegment2D::coeffA() const
    {
        return m_StartPoint.y() - m_EndPoint.y();
    }

    double CSegment2D::coeffB() const
    {
        return m_EndPoint.x() - m_StartPoint.x();
    }

    double CSegment2D::coeffC() const
    {
        return coeffB() * m_StartPoint.y() + coeffA() * m_StartPoint.x();
    }

    CPoint2D CSegment2D::intensity() const
    {
        auto x = m_EndPoint.x() - m_StartPoint.x();
        auto y = m_EndPoint.y() - m_StartPoint.y();
        return {x, y};
    }

}   // namespace Viewer
