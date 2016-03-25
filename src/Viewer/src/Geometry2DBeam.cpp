#include <assert.h>
#include <algorithm>

#include "Geometry2DBeam.hpp"
#include "Geometry2D.hpp"
#include "ViewSegment2D.hpp"
#include "Segment2D.hpp"
#include "Point2D.hpp"
#include "ViewerConstants.hpp"
#include "MathFunctions.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Viewer {

  ////////////////////////////////////////////////////////////////////////////////////////
  // BeamViewFactor
  ////////////////////////////////////////////////////////////////////////////////////////
  BeamViewFactor::BeamViewFactor( const size_t t_Geometry2DIndex, 
    const size_t t_SegmentIndex, const double t_Value, const double t_PercentHit ) : enclosureIndex( t_Geometry2DIndex ), 
    segmentIndex( t_SegmentIndex ), value( t_Value ), percentHit( t_PercentHit ) {
    
  };

  bool BeamViewFactor::operator==( const BeamViewFactor& t_BVF ) {
    return ( t_BVF.enclosureIndex == this->enclosureIndex ) && 
           ( t_BVF.segmentIndex == this->segmentIndex );  
  };


  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DBeam
  ////////////////////////////////////////////////////////////////////////////////////////
  CDirect2DBeam::CDirect2DBeam( shared_ptr< const CViewSegment2D > t_Beam ) : m_Beam( t_Beam ) {
    if( t_Beam == nullptr ) {
      throw runtime_error("Direct beam must have correct beam assigned.");
    }
    m_Segments = make_shared< vector< shared_ptr< const CViewSegment2D > > >();
  };

  // Check if segment intersects with the beam
  void CDirect2DBeam::checkSegment( shared_ptr< const CViewSegment2D > t_Segment ) {
    IntersectionStatus aStatus = m_Beam->intersectionWithLine( t_Segment );
    if( aStatus != IntersectionStatus::No ) {
      m_Segments->push_back( t_Segment );
    }
  };

  double CDirect2DBeam::Side() const {
    assert( m_Beam != nullptr );
    return m_Beam->startPoint()->y();
  }

  shared_ptr< const CViewSegment2D > CDirect2DBeam::getClosestCommonSegment( 
    shared_ptr< const CDirect2DBeam > t_Beam ) const {
    shared_ptr< const CViewSegment2D > aSegment = nullptr;
    for( shared_ptr< const CViewSegment2D > thisSegment : *m_Segments ) {
      if( t_Beam->isSegmentIn( thisSegment ) ) {
        if( aSegment == nullptr ) {
          aSegment = thisSegment;
        } else {
          if( aSegment->centerPoint()->x() > thisSegment->centerPoint()->x() ) {
            aSegment = thisSegment;
          }
        }
      }
    }

    return aSegment;
  };

  double CDirect2DBeam::cosAngle( shared_ptr< const CViewSegment2D > t_Segment ) {
    assert( m_Beam != nullptr );
    return m_Beam->dotProduct( t_Segment ) / m_Beam->length();
  };

  bool CDirect2DBeam::isSegmentIn( shared_ptr< const CViewSegment2D > t_Segment ) const {
    bool isIn = false;
    for( shared_ptr< const CViewSegment2D > thisSegment : *m_Segments ) {
      if( thisSegment == t_Segment ) {
        isIn = true;
        break;
      }
    }
    return isIn;
  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DRay
  ////////////////////////////////////////////////////////////////////////////////////////

  CDirect2DRay::CDirect2DRay( shared_ptr< CDirect2DBeam > t_Beam1, 
    shared_ptr< CDirect2DBeam > t_Beam2 ) : m_Beam1( t_Beam1 ), m_Beam2( t_Beam2 ) {
    if( t_Beam1 == nullptr ) {
      throw runtime_error("Beam number one of the ray is not correctly created.");
    }
    if( t_Beam2 == nullptr ) {
      throw runtime_error("Beam number two of the ray is not correctly created.");
    }
  };

  CDirect2DRay::CDirect2DRay( shared_ptr< CViewSegment2D > t_Ray1, 
    shared_ptr< CViewSegment2D > t_Ray2 ) {
    if( t_Ray1 == nullptr ) {
      throw runtime_error("Ray number one of the ray is not correctly created.");
    }
    if( t_Ray2 == nullptr ) {
      throw runtime_error("Ray number two of the ray is not correctly created.");
    }
    m_Beam1 = make_shared< CDirect2DBeam >( t_Ray1 );
    m_Beam2 = make_shared< CDirect2DBeam >( t_Ray2 );
  };

  double CDirect2DRay::rayNormalHeight() const {
    assert( m_Beam1 != nullptr );
    assert( m_Beam2 != nullptr );
    return m_Beam1->Side() - m_Beam2->Side();
  };

  void CDirect2DRay::checkSegment( shared_ptr< const CViewSegment2D > t_Segment ) {
    assert( m_Beam1 != nullptr );
    assert( m_Beam2 != nullptr );
    m_Beam1->checkSegment( t_Segment );
    m_Beam2->checkSegment( t_Segment );
  };

  // Return segment hit by the ray
  shared_ptr< const CViewSegment2D > CDirect2DRay::closestSegmentHit() const {
    return m_Beam1->getClosestCommonSegment( m_Beam2 );
  };

  double CDirect2DRay::cosAngle( shared_ptr< const CViewSegment2D > t_Segment ) {
    assert( m_Beam1 != nullptr );
    return m_Beam1->cosAngle( t_Segment );
  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DRayResult
  ////////////////////////////////////////////////////////////////////////////////////////

  CDirect2DRaysResult::CDirect2DRaysResult( const double t_ProfileAngle, double const t_DirectToDirect, 
      shared_ptr< vector < BeamViewFactor > > t_BeamViewFactors ) : m_ViewFactors( t_BeamViewFactors ),
      m_DirectToDirect( t_DirectToDirect), m_ProfileAngle( t_ProfileAngle ) {
     
  };

  shared_ptr< vector < BeamViewFactor > > CDirect2DRaysResult::beamViewFactors() const {
    return m_ViewFactors; 
  };

   double CDirect2DRaysResult::directToDirect() const {
    return m_DirectToDirect; 
   };

   double CDirect2DRaysResult::profileAngle() const {
    return m_ProfileAngle; 
   };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DRayResults
  ////////////////////////////////////////////////////////////////////////////////////////

  CDirect2DRaysResults::CDirect2DRaysResults() {
    m_Results = make_shared< vector < shared_ptr< CDirect2DRaysResult > > >();  
  };

  shared_ptr< CDirect2DRaysResult > CDirect2DRaysResults::getResult( const double t_ProfileAngle ) {
    shared_ptr< CDirect2DRaysResult > Result = nullptr;

    vector < shared_ptr< CDirect2DRaysResult > >::iterator it;
    it = find_if( m_Results->begin(), m_Results->end(), 
      [ &t_ProfileAngle ]( shared_ptr< CDirect2DRaysResult > obj ) { return fabs( obj->profileAngle() - t_ProfileAngle ) < 1e-6; } );

    if( it != m_Results->end() ) {
      Result = *it;
    }

    return Result;
  };

  std::shared_ptr< CDirect2DRaysResult > CDirect2DRaysResults::append( const double t_ProfileAngle, const double t_DirectToDirect,
    std::shared_ptr< std::vector < BeamViewFactor > > t_BeamViewFactor ) {
    shared_ptr< CDirect2DRaysResult > aResult = make_shared< CDirect2DRaysResult >( t_ProfileAngle, t_DirectToDirect, t_BeamViewFactor );
    m_Results->push_back( aResult );
    return aResult;
  };

  void CDirect2DRaysResults::clear() {
    m_Results->clear();
  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DRays
  ////////////////////////////////////////////////////////////////////////////////////////

  CDirect2DRays::CDirect2DRays( const Side t_Side ) : m_Side( t_Side ) {
    m_LowerRay = nullptr;
    m_UpperRay = nullptr;
    m_CurrentResult = nullptr;
  };

  void CDirect2DRays::appendGeometry2D( shared_ptr< const CGeometry2D > t_Geometry2D ) {
    m_Geometries2D.push_back( t_Geometry2D );
    m_Results.clear();
  };

  shared_ptr< vector < BeamViewFactor > > CDirect2DRays::beamViewFactors( const double t_ProfileAngle ) {
    calculateAllProperties( t_ProfileAngle );
    assert( m_CurrentResult != nullptr );
    return m_CurrentResult->beamViewFactors();
  };

  double CDirect2DRays::directToDirect( const double t_ProfileAngle ) {
    calculateAllProperties( t_ProfileAngle );
    assert( m_CurrentResult != nullptr );
    return m_CurrentResult->directToDirect();
  };

  void CDirect2DRays::calculateAllProperties( const double t_ProfileAngle ) {
    if( m_CurrentResult != nullptr && m_CurrentResult->profileAngle() != t_ProfileAngle ) {
      m_CurrentResult = m_Results.getResult( t_ProfileAngle );
    }
    if( m_CurrentResult == nullptr ) {
      findRayBoundaries( t_ProfileAngle );
      findInBetweenRays( t_ProfileAngle );
      calculateBeamProperties( t_ProfileAngle );
    }
  };

  void CDirect2DRays::findRayBoundaries( const double t_ProfileAngle ) {
    shared_ptr< CViewSegment2D > entryRay = nullptr;
    for( shared_ptr< const CGeometry2D > aGeometry : m_Geometries2D ) {
      // TODO: Geometry depends on entry or exit points
      shared_ptr< const CPoint2D > aPoint = nullptr;
      switch( m_Side ) {
      case Side::Front:
        aPoint = aGeometry->entryPoint();
        break;
      case Side::Back:
        aPoint = aGeometry->exitPoint();
        break;
      default:
        assert("Incorrect assignement of ray position.");
        break;
      }
      entryRay = createSubBeam( aPoint, t_ProfileAngle );
      if( aGeometry == *m_Geometries2D.begin() ) {
        m_LowerRay = entryRay;
        m_UpperRay = entryRay;
      } else {
        // This sets profile angle for point comparison that follows in next lines
        PointsProfile2DCompare aProfilePoint = PointsProfile2DCompare( t_ProfileAngle );
        if( aProfilePoint( m_LowerRay->startPoint(), entryRay->startPoint() ) ) {
          m_LowerRay = entryRay;
        }
        if( !aProfilePoint( m_UpperRay->startPoint(), entryRay->startPoint() ) ) {
          m_UpperRay = entryRay;
        }
      }
    }
  };

  void CDirect2DRays::findInBetweenRays( const double t_ProfileAngle ) {
    
    vector< shared_ptr< const CPoint2D > > inBetweenPoints;
    
    // m_Beams.push_back( m_UpperRay );
    for( shared_ptr< const CGeometry2D > aEnclosure : m_Geometries2D ) {
      shared_ptr< vector< shared_ptr< CViewSegment2D > > > aSegments = aEnclosure->segments();
      if( isInRay( ( *aSegments )[0]->startPoint() ) ) {
        inBetweenPoints.push_back( ( *aSegments )[0]->startPoint() );
      }
      for( shared_ptr< CViewSegment2D > aSegment : *aSegments ) {
        shared_ptr< const CPoint2D > endPoint = aSegment->endPoint();
        // Ray is alway going from left to right. For point to be in between beam, it must be visible 
        // for upper ray and invisible for lower ray
        if( m_UpperRay->position( endPoint ) == PointPosition::Visible && 
          m_LowerRay->position( endPoint ) == PointPosition::Invisible ) {
          inBetweenPoints.push_back( endPoint );
        }
      }
    }

    m_Rays.clear();

    sort( inBetweenPoints.begin(), inBetweenPoints.end(), PointsProfile2DCompare( t_ProfileAngle ) );

    // Creating incoming rays
    shared_ptr< CViewSegment2D > firstBeam = m_UpperRay;
    shared_ptr< CViewSegment2D > secondBeam = nullptr;
    for( shared_ptr< const CPoint2D > aPoint : inBetweenPoints ) {
      secondBeam = createSubBeam( aPoint, t_ProfileAngle );
      shared_ptr< CDirect2DRay > aRay = make_shared< CDirect2DRay >( firstBeam, secondBeam );

      // Dont save rays that are smaller than distance tolerance
      if( aRay->rayNormalHeight() > ViewerConstants::DISTANCE_TOLERANCE ) {
        m_Rays.push_back( aRay );
      }
      firstBeam = secondBeam;
    }
   shared_ptr< CDirect2DRay > aRay = make_shared< CDirect2DRay >( firstBeam, m_LowerRay );
   m_Rays.push_back( aRay );
  }

  void CDirect2DRays::calculateBeamProperties( const double t_ProfileAngle ) {
    // First check all segments and calculte total ray height
    double totalHeight = 0;
    for( shared_ptr< CDirect2DRay > beamRay : m_Rays ) {
      totalHeight += beamRay->rayNormalHeight();
      for( shared_ptr< const CGeometry2D > aEnclosure : m_Geometries2D )  {
        for( shared_ptr< CViewSegment2D > aSegment : ( *aEnclosure->segments() ) ) {
          beamRay->checkSegment( aSegment );
        }
      }
    }

    // Now calculate beam view factors
    shared_ptr< vector < BeamViewFactor > > aViewFactors = make_shared< vector< BeamViewFactor > >();
    double aDirectToDirect = 0;
    // Create beam direction parallel to x-axe
    shared_ptr< CPoint2D > sPoint = make_shared< CPoint2D >( 0, 0 );
    shared_ptr< CPoint2D > ePoint = make_shared< CPoint2D >( 1, 0 );
    shared_ptr< CViewSegment2D > aNormalBeamDirection = make_shared< CViewSegment2D >( sPoint, ePoint );
    for( shared_ptr< CDirect2DRay > beamRay : m_Rays ) {
      double currentHeight = beamRay->rayNormalHeight();
      double projectedBeamHeight = beamRay->cosAngle( aNormalBeamDirection );
      double viewFactor = 0;
      double percentHit = 0;
      shared_ptr< const CViewSegment2D > closestSegment = beamRay->closestSegmentHit();
      for( size_t e = 0; e < m_Geometries2D.size(); ++e ) {
        for( size_t s = 0; s < m_Geometries2D[ e ]->segments()->size(); ++s ) {
          shared_ptr< CViewSegment2D > currentSegment = ( *m_Geometries2D[ e ]->segments() )[ s ];
          if( currentSegment == beamRay->closestSegmentHit() ) {
            viewFactor = currentHeight / totalHeight;
            projectedBeamHeight = projectedBeamHeight * currentHeight;
            double segmentHitLength = projectedBeamHeight / fabs( beamRay->cosAngle( currentSegment->getNormal() ) );
            percentHit = segmentHitLength / currentSegment->length();
            vector< BeamViewFactor >::iterator aTest = find( aViewFactors->begin(), 
              aViewFactors->end(), BeamViewFactor( e, s, 0, 0 ) );
            if( aTest != aViewFactors->end() ) {
              BeamViewFactor& aVF = *aTest;
              aVF.value += viewFactor;
              aVF.percentHit += percentHit;
            } else {
              BeamViewFactor aVF = BeamViewFactor( e, s, viewFactor, percentHit );
              aViewFactors->push_back( aVF );
            }
          }
        }
      }

      // No segment is being hit. That means ray goes through. Add this to total view factor
      if( viewFactor == 0 ) {
        aDirectToDirect += currentHeight / totalHeight;
      }
    }
    m_CurrentResult = m_Results.append( t_ProfileAngle, aDirectToDirect, aViewFactors );
  }

  bool CDirect2DRays::isInRay( shared_ptr< const CPoint2D > t_Point ) {
    assert( m_UpperRay != nullptr );
    assert( m_LowerRay != nullptr );
    return m_UpperRay->position( t_Point ) == PointPosition::Visible && 
      m_LowerRay->position( t_Point ) == PointPosition::Invisible;
  }

  shared_ptr< CViewSegment2D > CDirect2DRays::createSubBeam( shared_ptr< const CPoint2D > t_Point, 
    const double t_ProfileAngle ) const {
    shared_ptr< CViewSegment2D > subSegment = nullptr;
    const double deltaX = 10;
    const double tanPhi = tan( radians ( t_ProfileAngle ) );
    double yStart = t_Point->y() - t_Point->x() * tanPhi;
    double yEnd = yStart + deltaX * tanPhi;

    shared_ptr< CPoint2D > startPoint = make_shared< CPoint2D >( 0, yStart );
    shared_ptr< CPoint2D > endPoint = make_shared< CPoint2D >( deltaX, yEnd );
    return make_shared< CViewSegment2D >( startPoint, endPoint );
  }

  ////////////////////////////////////////////////////////////////////////////////////////
  // CGeometry2DBeam
  ////////////////////////////////////////////////////////////////////////////////////////

  CGeometry2DBeam::CGeometry2DBeam() : m_Incoming( Side::Front ), m_Outgoing( Side::Back ) {

  }

  void CGeometry2DBeam::appendGeometry2D( shared_ptr< const CGeometry2D > t_Geometry2D ) {
    m_Incoming.appendGeometry2D( t_Geometry2D );
    m_Outgoing.appendGeometry2D( t_Geometry2D );
  }

  // Returns non zero view factors. It also calculates direct to direct component for the beam
  shared_ptr< vector < BeamViewFactor > > CGeometry2DBeam::beamViewFactors( const double t_ProfileAngle, const Side t_Side ) {
    CDirect2DRays* aRay = getRay( t_Side );
    return aRay->beamViewFactors( t_ProfileAngle );
  }

  double CGeometry2DBeam::directToDirect( const double t_ProfileAngle, const Side t_Side ) {
    CDirect2DRays* aRay = getRay( t_Side );
    return aRay->directToDirect( t_ProfileAngle );
  };

  CDirect2DRays* CGeometry2DBeam::getRay( const Side t_Side ) {
    CDirect2DRays* aRay = nullptr;
    switch( t_Side ) {
    case Side::Front:
      return aRay = &m_Incoming;
      break;
    case Side::Back:
      return aRay = &m_Outgoing;
      break;
    default:
      assert("Incorrect assignement of ray position.");
      break;
    }
    return aRay;
  };

}