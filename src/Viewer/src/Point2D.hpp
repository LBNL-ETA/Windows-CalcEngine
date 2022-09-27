#ifndef POINT2D_H
#define POINT2D_H

#include <memory>

namespace Viewer
{
    ////////////////////////////////////////////////////////////////////////////////////////
    // CPoint2D
    ////////////////////////////////////////////////////////////////////////////////////////

    // Simple representation of point in Cartesian coordinate system
    class CPoint2D
    {
    public:
        CPoint2D() = default;
        CPoint2D(double x, double y);
        [[nodiscard]] double x() const;
        [[nodiscard]] double y() const;

        static CPoint2D createPointFromPolarCoordinates(double theta, double radius);

        ///////////////////////////////////////////////////////////////////////////////////
        // brief Test if two points are withing certain tolerance.
        // param t_Point
        // return
        ///////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] bool sameCoordinates(CPoint2D const & t_Point) const;

        [[nodiscard]] double dotProduct(CPoint2D const & t_Point) const;

        bool operator==(CPoint2D const & rhs) const;
        bool operator!=(CPoint2D const & rhs) const;

        // True if current point is left from passed point (t_Point)
        [[nodiscard]] bool isLeft(CPoint2D const & t_Point) const;

        // Translates point for given coordinates
        [[nodiscard]] CPoint2D translate(double t_x, double t_y) const;

    protected:
        double m_x{0.0};
        double m_y{0.0};
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    // PointsProfile2DCompare
    ////////////////////////////////////////////////////////////////////////////////////////

    // Compare position of the points relative to profile angle. Used in conjuction with stl
    // algorithms for point positioning.
    class PointsProfile2DCompare
    {
    public:
        explicit PointsProfile2DCompare(double t_ProfileAngle);

        bool operator()(const CPoint2D & t_Point1,
                        const CPoint2D & t_Point2) const;

    private:
        double m_ProfileAngle;
    };
}   // namespace Viewer

#endif
