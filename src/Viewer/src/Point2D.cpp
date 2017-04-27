#include "Point2D.hpp"
#include "ViewerConstants.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Viewer {

  ////////////////////////////////////////////////////////////////////////////////////////
  // CPoint2D
  ////////////////////////////////////////////////////////////////////////////////////////

  CPoint2D::CPoint2D( const double x, const double y ) : m_x( x ), m_y( y ) {
  
  }

  double CPoint2D::x() const {
    return m_x;
  }

  double CPoint2D::y() const {
    return m_y;
  }

  bool CPoint2D::sameCoordinates( const CPoint2D& t_Point ) const {
    return ( fabs( t_Point.x() - m_x ) < ViewerConstants::DISTANCE_TOLERANCE ) && 
      ( fabs( t_Point.y() - m_y ) < ViewerConstants::DISTANCE_TOLERANCE );
  }

  double CPoint2D::dotProduct( const CPoint2D& t_Point ) const {
    return m_x * t_Point.x() + m_y * t_Point.y(); 
  }

  bool CPoint2D::isLeft( const CPoint2D& t_Point ) const {
    return m_x < t_Point.x();
  }

  shared_ptr< CPoint2D > CPoint2D::translate( const double t_x, const double t_y ) {
    shared_ptr< CPoint2D > aPoint = make_shared< CPoint2D >( m_x + t_x, m_y + t_y );
    return aPoint;
  }

  ////////////////////////////////////////////////////////////////////////////////////////
  // PointsCompare
  ////////////////////////////////////////////////////////////////////////////////////////

  PointsProfile2DCompare::PointsProfile2DCompare( const double t_ProfileAngle ) : m_ProfileAngle( t_ProfileAngle ) {
    
  }

  bool PointsProfile2DCompare::operator() ( const shared_ptr< const CPoint2D >& t_Point1, 
    const shared_ptr< const CPoint2D >& t_Point2 ) {
    bool isHigher = false;
    if( m_ProfileAngle != 0 ) {
      const double tanPhi = tan( radians( m_ProfileAngle ) );
      if( tanPhi > 0 ) {
        isHigher = ( t_Point1->x() - t_Point1->y() / tanPhi ) < ( t_Point2->x() - t_Point2->y() / tanPhi );
      } else {
        isHigher = ( t_Point1->x() - t_Point1->y() / tanPhi ) > ( t_Point2->x() - t_Point2->y() / tanPhi );
      }
    } else {
      isHigher = t_Point1->y() > t_Point2->y();
    }
    return isHigher;
    
  }

}