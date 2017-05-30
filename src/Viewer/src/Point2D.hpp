#ifndef POINT2D_H
#define POINT2D_H

#include <memory>

namespace Viewer {

  ////////////////////////////////////////////////////////////////////////////////////////
  // CPoint2D
  ////////////////////////////////////////////////////////////////////////////////////////

  // Simple representation of point in Cartesian coordinate system
  class CPoint2D {
  public:
    CPoint2D( const double x, const double y );
    double x() const;
    double y() const;
    bool sameCoordinates( const CPoint2D& t_Point ) const;

    double dotProduct( const CPoint2D& t_Point ) const;

    bool operator==( CPoint2D const & rhs) const;
    bool operator!=( CPoint2D const & rhs) const;

    // True if current point is left from passed point (t_Point)
    bool isLeft(const CPoint2D& t_Point ) const;

    // Translates point for given coordinates
    std::shared_ptr< CPoint2D > translate( const double t_x, const double t_y );

  protected:
    double m_x;
    double m_y;

  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // PointsProfile2DCompare
  ////////////////////////////////////////////////////////////////////////////////////////

  // Compare position of the points relative to profile angle. Used in conjuction with stl algorithms for
  // point positioning.
  class PointsProfile2DCompare {
  public:
    explicit PointsProfile2DCompare( const double t_ProfileAngle );

    bool operator() ( const std::shared_ptr< const CPoint2D >& t_Point1, 
      const std::shared_ptr< const CPoint2D >& t_Point2 );

  private:
    double m_ProfileAngle;
  };
}

#endif