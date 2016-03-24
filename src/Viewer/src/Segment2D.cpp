#include <math.h>
#include <algorithm>
#include <assert.h>
#include <stdexcept>

#include "Segment2D.hpp"
#include "Point2D.hpp"
#include "SquareMatrix.hpp"
#include "LinearSolver.hpp"
#include "ViewerConstants.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Viewer {

  CSegment2D::CSegment2D( shared_ptr< const CPoint2D > t_StartPoint, shared_ptr< const CPoint2D > t_EndPoint ) : 
    m_StartPoint( t_StartPoint ), m_EndPoint( t_EndPoint ) {
    calculateLength();
    calculateCenter();
  }

  shared_ptr< const CPoint2D > CSegment2D::startPoint() const {
    return m_StartPoint;
  }

  shared_ptr< const CPoint2D > CSegment2D::endPoint() const {
    return m_EndPoint;
  }

  shared_ptr< const CPoint2D > CSegment2D::centerPoint() const {
    return m_CenterPoint;
  }

  double CSegment2D::length() const {
    return m_Length;
  }

  bool CSegment2D::intersectionWithSegment( shared_ptr< const CSegment2D > t_Segment ) const {
    bool aInt = false;
    if( length() != 0 ) {
      shared_ptr< CPoint2D > aPoint = intersection( t_Segment );

      if( aPoint != nullptr ) {
        aInt = isInRectangleRange( aPoint ) && t_Segment->isInRectangleRange( aPoint );
      }
    }

    return aInt;
  };

  IntersectionStatus CSegment2D::intersectionWithLine( std::shared_ptr< const CSegment2D > t_Segment ) const {
    IntersectionStatus status = IntersectionStatus::No;

    if( length() != 0 ) {
      shared_ptr< CPoint2D > aPoint = intersection( t_Segment );

      if( aPoint != nullptr ) {
        bool aInt = t_Segment->isInRectangleRange( aPoint );
        if( aInt ) {
          status = IntersectionStatus::Segment;
        }
        if( t_Segment->startPoint()->sameCoordinates( *aPoint ) || 
          t_Segment->endPoint()->sameCoordinates( *aPoint ) ) {
          status = IntersectionStatus::Point;
        }
      }
    }

    return status;
  };

  double CSegment2D::dotProduct( std::shared_ptr< const CSegment2D > t_Segment ) const {
    shared_ptr< CPoint2D > p1 = intensity();
    shared_ptr< CPoint2D > p2 = t_Segment->intensity();

    return p1->dotProduct(p2);
  };

  // Translates segment for given coordinates
  shared_ptr< CSegment2D > CSegment2D::translate( const double t_x, const double t_y ) {
    shared_ptr< CPoint2D > startPoint = make_shared< CPoint2D >( m_StartPoint->x() + t_x, m_StartPoint->y() + t_y );
    shared_ptr< CPoint2D > endPoint = make_shared< CPoint2D >( m_EndPoint->x() + t_x, m_EndPoint->y() + t_y );
    shared_ptr< CSegment2D > aSegment = make_shared< CSegment2D >( startPoint, endPoint );
    return aSegment;
  };

  void CSegment2D::calculateLength() {
    double deltaX = m_EndPoint->x() - m_StartPoint->x();
    double deltaY = m_EndPoint->y() - m_StartPoint->y();
    m_Length = sqrt( deltaX * deltaX + deltaY * deltaY );
  }

  void CSegment2D::calculateCenter() {
    double x = ( m_EndPoint->x() + m_StartPoint->x() ) / 2;
    double y = ( m_EndPoint->y() + m_StartPoint->y() ) / 2;

    m_CenterPoint = make_shared< CPoint2D >( x, y );
  }

  shared_ptr< CPoint2D > CSegment2D::intersection( shared_ptr< const CSegment2D > t_Segment ) const {
    if( t_Segment == nullptr ) {
      throw runtime_error("Segment for intersection must be provided. Cannot operate with null segment.");
    }

    shared_ptr< CPoint2D > intersectionPoint = nullptr;

    double A1 = coeffA();
    double A2 = t_Segment->coeffA();
    double B1 = coeffB();
    double B2 = t_Segment->coeffB();

    //// Test if lines are parallel
    //bool testLimitA = ( abs( A1 - A2 ) < ViewerConstants::DISTANCE_TOLERANCE );
    //bool testLimitB = ( abs( B1 - B2 ) < ViewerConstants::DISTANCE_TOLERANCE );
    //bool parallelLines = testLimitA && testLimitB;
    //
    //// Test if lines are identical to each other (overlapping)
    //bool testLimitAB1 = abs( A1 + B1 ) < ViewerConstants::DISTANCE_TOLERANCE; // || abs( A1 - B1 ) < ViewerConstants::DISTANCE_TOLERANCE;
    //bool testLimitAB2 = abs( A2 + B2 ) < ViewerConstants::DISTANCE_TOLERANCE; // || abs( A2 - B2 ) < ViewerConstants::DISTANCE_TOLERANCE;
    //bool testLimitBA1 = abs( A1 + A2 ) < ViewerConstants::DISTANCE_TOLERANCE;
    //bool testLimitBA2 = abs( B1 + B2 ) < ViewerConstants::DISTANCE_TOLERANCE;
    //bool overlappingLines = ( testLimitAB1 && testLimitAB2 ) || ( testLimitBA1 && testLimitBA2 );
    //
    //// Test if equations are incomplete
    //bool incompleteA = ( abs( A1 ) < ViewerConstants::DISTANCE_TOLERANCE ) && ( abs( A2 ) < ViewerConstants::DISTANCE_TOLERANCE );
    //bool incompleteB = ( abs( B1 ) < ViewerConstants::DISTANCE_TOLERANCE ) && ( abs( B2 ) < ViewerConstants::DISTANCE_TOLERANCE );
    //bool incomplete = incompleteA || incompleteB;

    //if( !parallelLines && !overlappingLines && !incomplete ) {

    double C1 = coeffC();
    double C2 = t_Segment->coeffC();
    double x = 0;
    double y = 0;
    
    if( fabs( A1 ) > ViewerConstants::DISTANCE_TOLERANCE ) {
      double t1 = C2 - C1 * A2 / A1;
      double t2 = B2 - B1 * A2 / A1;
      // assert( t2 != 0 );
      if( fabs( t2 ) > ViewerConstants::DISTANCE_TOLERANCE ) {
        y = t1 / t2;
        x = ( C1 - B1 * y ) / A1;
      } else {
        return intersectionPoint;
      }
    } else {
      y = C1 / B1;
      x = ( C2 - B2 * y ) / A2;
    }

    intersectionPoint = make_shared< CPoint2D >( x, y );
    //}

    return intersectionPoint;
  };

  bool CSegment2D::isInRectangleRange( shared_ptr< const CPoint2D > t_Point ) const {
    // Should return in range only if point is not exactly on the line's point
    bool inXRange = false;
    bool inYRange = false;

    // Check X range
    const double maxX = max( m_EndPoint->x(), m_StartPoint->x() );
    const double minX = min( m_EndPoint->x(), m_StartPoint->x() );
    if( fabs( maxX - minX ) > ViewerConstants::DISTANCE_TOLERANCE ) {
      if( t_Point->x() < ( maxX - ViewerConstants::DISTANCE_TOLERANCE ) && t_Point->x() > ( minX + ViewerConstants::DISTANCE_TOLERANCE ) ) {
        inXRange = true;
      }
    } else {
      if( fabs( t_Point->x() - maxX ) < ViewerConstants::DISTANCE_TOLERANCE ) {
        inXRange = true;
      }
    }

    // Check Y range
    const double maxY = max( m_EndPoint->y(), m_StartPoint->y() );
    const double minY = min( m_EndPoint->y(), m_StartPoint->y() );
    if( fabs( maxY - minY ) > ViewerConstants::DISTANCE_TOLERANCE ) {
      if( t_Point->y() < ( maxY - ViewerConstants::DISTANCE_TOLERANCE ) && t_Point->y() > ( minY + ViewerConstants::DISTANCE_TOLERANCE ) ) {
        inYRange = true;
      }
    } else {
      if( fabs( t_Point->y() - maxY ) < ViewerConstants::DISTANCE_TOLERANCE ) {
        inYRange = true;
      }
    }

    return inXRange && inYRange;
  };

  double CSegment2D::coeffA() const {
    return m_StartPoint->y() - m_EndPoint->y();
  }

  double CSegment2D::coeffB() const {
    return m_EndPoint->x() - m_StartPoint->x();
  }

  double CSegment2D::coeffC() const {
    return coeffB() * m_StartPoint->y() + coeffA() * m_StartPoint->x();
  }

  shared_ptr< CPoint2D > CSegment2D::intensity() const {
    double x = m_EndPoint->x() - m_StartPoint->x();
    double y = m_EndPoint->y() - m_StartPoint->y();
    shared_ptr< CPoint2D > aPoint = make_shared< CPoint2D >( x, y );
    return aPoint;
  };

}