#include <assert.h>
#include <algorithm>

#include "Geometry2D.hpp"
#include "ViewSegment2D.hpp"
#include "Point2D.hpp"
#include "SquareMatrix.hpp"
#include "ViewerConstants.hpp"
#include "MathFunctions.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Viewer {

  CGeometry2D::CGeometry2D() : m_ViewFactorsCalculated( false ),
    m_Segments( make_shared< vector< shared_ptr< CViewSegment2D > > >() ), m_ViewFactors( nullptr ) {
  }

  void CGeometry2D::appendSegment( shared_ptr< CViewSegment2D > t_Segment ) {
    m_Segments->push_back( t_Segment );
    m_ViewFactorsCalculated = false;
  }

  void CGeometry2D::appendGeometry2D( std::shared_ptr< CGeometry2D > t_Geometry2D ) {
    for( shared_ptr< CViewSegment2D > aSegment : ( *t_Geometry2D->m_Segments ) ) {
      m_Segments->push_back( aSegment );
    }
    m_ViewFactorsCalculated = false;
  };

  shared_ptr< CSquareMatrix > CGeometry2D::viewFactors() {
    checkViewFactors();

    assert( m_ViewFactors != nullptr );

    return m_ViewFactors;
  };

  shared_ptr< CGeometry2D > CGeometry2D::Translate( const double t_x, const double t_y ) {
    shared_ptr< CGeometry2D > aEnclosure = make_shared< CGeometry2D >();
    for( shared_ptr< CViewSegment2D > aSegment : *m_Segments ) {
      shared_ptr< CSegment2D > newSegment = aSegment->translate( t_x, t_y );
      shared_ptr< CViewSegment2D > newEnSegment = 
        make_shared< CViewSegment2D >( newSegment->startPoint(), newSegment->endPoint() );
      aEnclosure->appendSegment( newEnSegment );
    }

    return aEnclosure;
  };

  shared_ptr< const CPoint2D > CGeometry2D::firstPoint() const {
    return m_Segments->front()->startPoint();
  };

  shared_ptr< const CPoint2D > CGeometry2D::lastPoint() const {
    return m_Segments->back()->endPoint();
  };

  shared_ptr< const CPoint2D > CGeometry2D::entryPoint() const {
    double xStart = m_Segments->front()->centerPoint()->x();
    double xEnd = m_Segments->back()->centerPoint()->x();
    shared_ptr< const CPoint2D > aPoint = nullptr;
    shared_ptr< const CPoint2D > startPoint = nullptr;
    shared_ptr< const CPoint2D > endPoint = nullptr;
    if( xStart <= xEnd ) {
      startPoint = m_Segments->front()->startPoint();
      endPoint = m_Segments->front()->endPoint();
    } else {
      startPoint = m_Segments->back()->startPoint();
      endPoint = m_Segments->back()->endPoint();
    }
    if( startPoint->x() < endPoint->x() ) {
      aPoint = startPoint;
    } else {
      aPoint = endPoint;
    }

    return aPoint;
  };

  shared_ptr< const CPoint2D > CGeometry2D::exitPoint() const {
    double xStart = m_Segments->front()->centerPoint()->x();
    double xEnd = m_Segments->back()->centerPoint()->x();
    shared_ptr< const CPoint2D > aPoint = nullptr;
    shared_ptr< const CPoint2D > startPoint = nullptr;
    shared_ptr< const CPoint2D > endPoint = nullptr;
    if( xStart >= xEnd ) {
      startPoint = m_Segments->front()->startPoint();
      endPoint = m_Segments->front()->endPoint();
    } else {
      startPoint = m_Segments->back()->startPoint();
      endPoint = m_Segments->back()->endPoint();
    }
    if( startPoint->x() > endPoint->x() ) {
      aPoint = startPoint;
    } else {
      aPoint = endPoint;
    }

    return aPoint;
  };

  shared_ptr< vector< shared_ptr< CViewSegment2D > > > CGeometry2D::segments() const {
    return m_Segments;
  };

  bool CGeometry2D::pointInSegmentsView( shared_ptr< const CViewSegment2D > t_Segment1, 
      shared_ptr< const CViewSegment2D > t_Segment2, shared_ptr< const CPoint2D > t_Point ) const {

    // Forming polygon
    vector< shared_ptr< const CViewSegment2D > > aPolygon;
    aPolygon.push_back( t_Segment1 );
    shared_ptr< const CPoint2D > point1 = t_Segment1->endPoint();
    shared_ptr< const CPoint2D > point2 = t_Segment2->startPoint();
    shared_ptr< CViewSegment2D > aSide2 = make_shared< CViewSegment2D >( point1, point2 );
    if( aSide2->length() > 0 ) {
      aPolygon.push_back( aSide2 );
    }
    aPolygon.push_back( t_Segment2 );
    shared_ptr< const CPoint2D > point3 = t_Segment2->endPoint();
    shared_ptr< const CPoint2D > point4 = t_Segment1->startPoint();
    shared_ptr< CViewSegment2D > aSide4 = make_shared< CViewSegment2D >( point3, point4 );
    if( aSide4->length() > 0 ) {
      aPolygon.push_back( aSide4 );
    }

    // now check if point is in the polygon. Note that if point is of any edge of the polygon, it will not be considered to
    // be part of blocking surface. Otherwise, program would search for blocking surfaces and perform double integration
    // over the both surfaces.
    bool inSide = true;
    for( shared_ptr< const CViewSegment2D > aSegment : aPolygon) {
      inSide = inSide && ( aSegment->position( t_Point ) == PointPosition::Visible );
      if( !inSide ) {
        break;
      }
    }

    return inSide;
  };

  bool CGeometry2D::thirdSurfaceShadowing( shared_ptr< const CViewSegment2D > t_Segment1, 
      shared_ptr< const CViewSegment2D > t_Segment2 ) const {
    bool intersection = false;
  
    // Form cross segments
    vector< shared_ptr< CViewSegment2D > > intSegments;
    shared_ptr< CViewSegment2D > r11 = 
      make_shared< CViewSegment2D >( t_Segment1->startPoint(), t_Segment2->endPoint() );
    if( r11->length() > 0 ) {
      intSegments.push_back( r11 );
    }
    shared_ptr< CViewSegment2D > r22 = 
      make_shared< CViewSegment2D >( t_Segment1->endPoint(), t_Segment2->startPoint() );
    if( r22->length() > 0 ) {
      intSegments.push_back( r22 );
    }
  
    for( shared_ptr< CViewSegment2D > aSegment : *m_Segments ) {
      for( shared_ptr< CViewSegment2D > iSegment : intSegments ) {
        if( aSegment != t_Segment1 && aSegment != t_Segment2 ) {
          intersection = intersection || iSegment->intersectionWithSegment( aSegment );
          intersection = intersection || pointInSegmentsView( t_Segment1, t_Segment2, aSegment->startPoint() );
          intersection = intersection || pointInSegmentsView( t_Segment1, t_Segment2, aSegment->endPoint() );
          if( intersection ) {
            return intersection;
          }
        }
      }
    }
    
    return intersection;
  };

  // bool CGeometry2D::thirdSurfaceShadowing( shared_ptr< const CViewSegment2D > t_Segment1, 
  //     shared_ptr< const CViewSegment2D > t_Segment2 ) const {
  //   bool intersection = false;
  //   const size_t numDiv = 5;
  //   vector< shared_ptr< CViewSegment2D > > intSegments;
  // 
  //   double dx1 = ( t_Segment1->endPoint()->x() - t_Segment1->startPoint()->x() ) / numDiv;
  //   double dy1 = ( t_Segment1->endPoint()->y() - t_Segment1->startPoint()->y() ) / numDiv;
  //   double dx2 = ( t_Segment2->startPoint()->x() - t_Segment2->endPoint()->x() ) / numDiv;
  //   double dy2 = ( t_Segment2->startPoint()->y() - t_Segment2->endPoint()->y() ) / numDiv;
  // 
  //   for( size_t i = 0; i <= numDiv; ++i ) {
  //     shared_ptr< CPoint2D > startPoint = 
  //       make_shared< CPoint2D >( t_Segment1->startPoint()->x() + dx1 * i, t_Segment1->startPoint()->y() + dy1 * i);
  //     shared_ptr< CPoint2D > endPoint = 
  //       make_shared< CPoint2D >( t_Segment2->endPoint()->x() + dx2 * i, t_Segment2->endPoint()->y() + dy2 * i);
  // 
  //     shared_ptr< CViewSegment2D > aSubSeg = 
  //       make_shared< CViewSegment2D >( startPoint, endPoint );
  // 
  //     intSegments.push_back( aSubSeg );
  //   }
  // 
  //   for( shared_ptr< CViewSegment2D > aSegment : *m_Segments ) {
  //     if( aSegment != t_Segment1 && aSegment != t_Segment2 ) {
  //       for( shared_ptr< CViewSegment2D > iSegment : intSegments ) {
  //         intersection = intersection || iSegment->intersectionWithSegment( aSegment );
  //         //intersection = intersection || pointInSegmentsView( t_Segment1, t_Segment2, aSegment->startPoint() );
  //         //intersection = intersection || pointInSegmentsView( t_Segment1, t_Segment2, aSegment->endPoint() );
  //         if( intersection ) {
  //           return intersection; // No need to waste time of further checking. Return true.
  //         }
  //       }
  //     }
  //   }
  //   
  //   return intersection;
  // };

  bool CGeometry2D::thirdSurfaceShadowingSimple( shared_ptr< const CViewSegment2D > t_Segment1, 
      shared_ptr< const CViewSegment2D > t_Segment2 ) const {
    bool intersection = false;

    shared_ptr< CViewSegment2D > centerLine = 
      make_shared< CViewSegment2D >( t_Segment1->centerPoint(), t_Segment2->centerPoint() );

    for( shared_ptr< CViewSegment2D > aSegment : *m_Segments ) {
      if( aSegment != t_Segment1 && aSegment != t_Segment2 ) {
        intersection = intersection || centerLine->intersectionWithSegment( aSegment );
        if( intersection ) {
          break;
        }
      }
    }
      
    return intersection;
  };

  double CGeometry2D::viewFactorCoeff( shared_ptr< const CViewSegment2D > t_Segment1,
      shared_ptr< const CViewSegment2D > t_Segment2 ) const {
    double subViewCoeff = 0;

    shared_ptr< vector < shared_ptr < CViewSegment2D > > > subSeg1 = 
      t_Segment1->subSegments( ViewerConstants::NUM_OF_SEGMENTS );
    shared_ptr< vector < shared_ptr < CViewSegment2D > > > subSeg2 = 
      t_Segment2->subSegments( ViewerConstants::NUM_OF_SEGMENTS );

    for( shared_ptr< const CViewSegment2D > sub1 : *subSeg1 ) {
      for( shared_ptr< const CViewSegment2D > sub2 : *subSeg2 ) {
        Shadowing selfShadowing = sub1->selfShadowing( sub2 );
        bool tSurfBlock = thirdSurfaceShadowingSimple( sub1, sub2 );
        if( !tSurfBlock && selfShadowing == Shadowing::No ) {
          double cVF = sub1->viewFactorCoefficient( sub2 );
          subViewCoeff += cVF;
        }
      }
    }

    if( subViewCoeff < ViewerConstants::MIN_VIEW_COEFF ) {
      subViewCoeff = 0;
    }

    return subViewCoeff;
  };

  double CGeometry2D::intersectionWithYAxis( const double tanPhi, std::shared_ptr< const CPoint2D > t_Point ) const {
    assert( t_Point != nullptr );
    double y = 0;
    if( tanPhi != 0 ) {
      double x1 = t_Point->y() / tanPhi;
      double x = x1 + t_Point->x();
      y = tanPhi * x;
    } else {
      y = t_Point->y();
    }

    return y;
  };

  void CGeometry2D::checkViewFactors() {
    if( !m_ViewFactorsCalculated ) {
      size_t size = m_Segments->size();

      // View factor matrix. It is already initialized to zeros
      m_ViewFactors = make_shared< CSquareMatrix >( size );
      for( size_t i = 0; i < size; ++i ) {
        for( size_t j = i; j < size; ++j ) {
          if( i != j ) {
            Shadowing selfShadowing = ( *m_Segments )[i]->selfShadowing( ( *m_Segments )[j] );
            if( selfShadowing != Shadowing::Total ) {
              bool shadowedByThirdSurface = thirdSurfaceShadowing( ( *m_Segments )[i], ( *m_Segments )[j] );
              double vfCoeff = 0;

              if( !shadowedByThirdSurface && ( selfShadowing == Shadowing::No ) ) {
                vfCoeff = ( *m_Segments )[i]->viewFactorCoefficient( ( *m_Segments )[j] );
              } else if ( shadowedByThirdSurface || selfShadowing == Shadowing::Partial ) {
                vfCoeff = viewFactorCoeff( ( *m_Segments )[i], ( *m_Segments )[j] );
              }

              ( *m_ViewFactors )[i][j] = vfCoeff / ( 2 * ( *m_Segments )[i]->length() );
              ( *m_ViewFactors )[j][i] = vfCoeff / ( 2 * ( *m_Segments )[j]->length() );
            }
          }
        }
      }

      m_ViewFactorsCalculated = true;
    }
  };

}