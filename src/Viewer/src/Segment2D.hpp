#pragma once

#include <memory>
#include <optional>

#include "Point2D.hpp"

namespace Viewer
{
    // Intersection status is used to determine how line intersects the segment.
    // No - line does not intersect the segment
    // Point - line intersects the segment excatly at one of the points
    // Segment - line intersect the segment somewhere in the middle
    enum class IntersectionStatus
    {
        No,
        Point,
        Segment
    };

    // Representation of simple Segment in 2D space
    class CSegment2D
    {
    public:
        CSegment2D() = default;
        CSegment2D(const CPoint2D & t_StartPoint, const CPoint2D & t_EndPoint);

        [[nodiscard]] CPoint2D startPoint() const;
        [[nodiscard]] CPoint2D endPoint() const;
        [[nodiscard]] CPoint2D centerPoint() const;

        bool operator==(CSegment2D const & rhs) const;
        bool operator!=(CSegment2D const & rhs) const;

        [[nodiscard]] double length() const;
        [[nodiscard]] double angle() const;

        // Calculates if segment intersects with passed t_Segment
        [[nodiscard]] bool
          intersectionWithSegment(const CSegment2D & t_Segment) const;

        // For line made up of current segment, calculate how it intersects passed segment.
        [[nodiscard]] IntersectionStatus intersectionWithLine(const CSegment2D & t_Segment) const;

        // Dot product of two std::vectors
        [[nodiscard]] double dotProduct(const CSegment2D & t_Segment) const;

        // Translates segment for given coordinates
        [[nodiscard]] CSegment2D translate(double t_x, double t_y) const;

        // returns end point of the std::vector that starts at (0, 0)
        [[nodiscard]] CPoint2D intensity() const;

        //! Calculates unit normal to the surface segment
        [[nodiscard]] CPoint2D surfaceUnitNormal() const;
    protected:
        CPoint2D m_StartPoint;
        CPoint2D m_EndPoint;
        CPoint2D m_CenterPoint;

    private:
        double calculateLength(const CPoint2D & startPoint, const CPoint2D & endPoint);
        double calculateAngle(const CPoint2D & startPoint, const CPoint2D & endPoint);
        CPoint2D calculateCenter(const CPoint2D & startPoint, const CPoint2D & endPoint);

        // Calculates intesection point between two lines made up of std::vector. Even if two
        // std::vectors do not intersect, point of intersection will be returned
        [[nodiscard]] std::optional<CPoint2D>
          intersection(const CSegment2D & t_Segment) const;

        // test if x coordinate is in range of rectangle made up of segment
        [[nodiscard]] bool isInRectangleRange(const CPoint2D & t_Point) const;

        // Equation of LINE made up of two points
        // A, B and C coefficients of line that is made up of segment. Line equation is A*x + B*y =
        // C
        [[nodiscard]] double coeffA() const;
        [[nodiscard]] double coeffB() const;
        [[nodiscard]] double coeffC() const;

        double m_Length;
        double m_Angle{0.0};
    };

}   // namespace Viewer
