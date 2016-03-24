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
    const double x() const;
    const double y() const;
    bool sameCoordinates( const CPoint2D& t_Point ) const;

    double dotProduct( std::shared_ptr< const CPoint2D > t_Point ) const;

    // True if current point is left from passed point (t_Point)
    bool isLeft( std::shared_ptr< const CPoint2D > t_Point ) const;

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
    PointsProfile2DCompare( const double t_ProfileAngle );

    bool operator() ( std::shared_ptr< const CPoint2D > t_Point1, 
      std::shared_ptr< const CPoint2D > t_Point2 );

  private:
    double m_ProfileAngle;
  };
}

#endif