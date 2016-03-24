#include <assert.h>
#include <stdexcept>

#include "ViewSegment2D.hpp"
#include "Point2D.hpp"
#include "ViewerConstants.hpp"

using namespace std;

namespace Viewer {

  CViewSegment2D::CViewSegment2D( std::shared_ptr< const CPoint2D > t_StartPoint, 
      std::shared_ptr< const CPoint2D > t_EndPoint ) : CSegment2D( t_StartPoint, t_EndPoint ),
      m_NormalCalculated( false ) {
   m_Normal = nullptr;
  };

  shared_ptr< const CViewSegment2D > CViewSegment2D::getNormal() {
    if( !m_NormalCalculated ) {
      calculateNormal();
      m_NormalCalculated = true;
    }
    return m_Normal;
  }

  double CViewSegment2D::viewFactorCoefficient( std::shared_ptr< const CSegment2D > t_Segment ) const {
    if( t_Segment == nullptr ) {
      throw runtime_error("Segment for view factor calculation must be provided.");
    }

    double r11 = CSegment2D( m_StartPoint, t_Segment->endPoint() ).length();
    double r22 = CSegment2D( m_EndPoint, t_Segment->startPoint() ).length();
    double r12 = CSegment2D( m_StartPoint, t_Segment->startPoint() ).length();
    double r21 = CSegment2D( m_EndPoint, t_Segment->endPoint() ).length();

    double vFCoeff = r12 + r21 - r22 - r11;

    if( vFCoeff < ViewerConstants::MIN_VIEW_COEFF ) {
      vFCoeff = 0;
    }

    return vFCoeff;
  };

  Shadowing CViewSegment2D::selfShadowing( std::shared_ptr< const CViewSegment2D > t_Segment ) const {
    Shadowing totalShadowing = Shadowing::Partial;
    Shadowing vThis = isInSelfShadow( t_Segment );
    Shadowing vOther = t_Segment->isInSelfShadow( shared_from_this() );
    
    if( vThis == Shadowing::Total || vOther == Shadowing::Total ) {
      totalShadowing = Shadowing::Total;
    } else if( vThis == Shadowing::No && vOther == Shadowing::No ) {
      totalShadowing = Shadowing::No;
    }

    return totalShadowing;

  };

  Shadowing CViewSegment2D::isInSelfShadow( shared_ptr< const CViewSegment2D > t_Segment ) const {
    int numOfInvisibles = 0;

    PointPosition visibilityStart = position( t_Segment->startPoint() );
    PointPosition visibilityEnd = position( t_Segment->endPoint() );
    
    if( visibilityStart == PointPosition::Invisible ) {
      ++numOfInvisibles;
    }

    if( visibilityEnd == PointPosition::Invisible ) {
      ++numOfInvisibles;
    }

    if( numOfInvisibles == 1 ) {
      if( visibilityStart == PointPosition::OnLine || visibilityEnd == PointPosition::OnLine ) {
        ++numOfInvisibles;
      }
    }

    return Shadowing( numOfInvisibles );
  };

  shared_ptr< vector < shared_ptr< CViewSegment2D > > > CViewSegment2D::subSegments( const size_t numSegments ) const {
    if( numSegments == 0 ) {
      throw runtime_error("Number of subsegments must be greater than zero.");
    }
    shared_ptr< vector < shared_ptr< CViewSegment2D > > > subSegments = 
      make_shared< vector < shared_ptr< CViewSegment2D > > >();
    double dX = ( m_EndPoint->x() - m_StartPoint->x() ) / numSegments;
    double dY = ( m_EndPoint->y() - m_StartPoint->y() ) / numSegments;
    double startX = m_StartPoint->x();
    double startY = m_StartPoint->y();
    shared_ptr< CPoint2D > sPoint = make_shared< CPoint2D >( startX, startY );
    for( size_t i = 1; i <= numSegments; ++i ) {
      shared_ptr< CPoint2D > ePoint = make_shared< CPoint2D >( startX + i * dX , startY + i * dY );
      shared_ptr< CViewSegment2D > aSegment = make_shared< CViewSegment2D >( sPoint, ePoint );
      subSegments->push_back( aSegment );
      sPoint = ePoint;
    }

    return subSegments;
  };

  shared_ptr< CViewSegment2D > CViewSegment2D::translate( const double t_x, const double t_y ){
    shared_ptr<CSegment2D > aSegment = CSegment2D::translate( t_x, t_y );
    return make_shared< CViewSegment2D >( aSegment->startPoint(), aSegment->endPoint() );
  }

  PointPosition CViewSegment2D::position( std::shared_ptr< const CPoint2D > t_Point ) const {
    PointPosition aPosition = PointPosition::OnLine;

    if( !( t_Point->sameCoordinates( *m_StartPoint ) || t_Point->sameCoordinates( *m_EndPoint ) ) ) {
      double dx = m_EndPoint->x() - m_StartPoint->x();
      double dy = m_EndPoint->y() - m_StartPoint->y();
      double position = dx * ( t_Point->y() - m_StartPoint->y() ) - dy * ( t_Point->x() - m_StartPoint->x() );
      if( position > ViewerConstants::DISTANCE_TOLERANCE ) {
        aPosition = PointPosition::Invisible;
      } else if( position < -ViewerConstants::DISTANCE_TOLERANCE ) {
        aPosition = PointPosition::Visible;
      }
    }

    return aPosition;
  };

  void CViewSegment2D::calculateNormal() {
    assert( length() > 0 );
    double xn = ( m_EndPoint->y() - m_StartPoint->y() ) / length();
    double yn = ( m_StartPoint->x() - m_EndPoint->x() ) / length();
    shared_ptr< CPoint2D > startPoint = make_shared< CPoint2D >( 0, 0 ); // normal always starts from (0, 0)
    shared_ptr< CPoint2D > endPoint = make_shared< CPoint2D >( xn, yn );
    m_Normal = make_shared< CViewSegment2D >( startPoint, endPoint );
  }

}