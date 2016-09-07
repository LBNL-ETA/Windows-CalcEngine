#ifndef GEOMETRY2DBEAM_H
#define GEOMETRY2DBEAM_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  enum class Side;

}

namespace Viewer {

  class CViewSegment2D;
  class CGeometry2D;
  class CSegment2D;
  class CPoint2D;

  ////////////////////////////////////////////////////////////////////////////////////////
  // BeamViewFactor
  ////////////////////////////////////////////////////////////////////////////////////////
  struct BeamViewFactor {
    BeamViewFactor( const size_t t_Geometry2DIndex, const size_t t_SegmentIndex, 
      const double t_Value, const double t_PercentHit );
    bool operator==( const BeamViewFactor& t_BVF1 );
    // static bool isEqual( const BeamViewFactor& t_VF1, const BeamViewFactor& t_VF2 );
    size_t enclosureIndex;
    size_t segmentIndex;
    double value;
    double percentHit;
  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DBeam
  ////////////////////////////////////////////////////////////////////////////////////////

  // Keeps information about single beam and segments that are intersected with it
  class CDirect2DBeam {
  public:
    explicit CDirect2DBeam( const std::shared_ptr< const CViewSegment2D >& t_Beam );

    // Checks if segments intersects with the beam
    void checkSegment( const std::shared_ptr< const CViewSegment2D >& t_Segment );

    double Side() const;

    // Check if passed segment is part of the beam
    std::shared_ptr< const CViewSegment2D > getClosestCommonSegment( 
      const std::shared_ptr< const CDirect2DBeam >& t_Beam ) const;

    double cosAngle( const std::shared_ptr< const CViewSegment2D >& t_Segment );

  private:
    // Checks if segment is aleardy part of beam hit
    bool isSegmentIn( const std::shared_ptr< const CViewSegment2D >& t_Segment ) const;

    // Direct beam
    std::shared_ptr< const CViewSegment2D > m_Beam;

    // Segments that beam is intersecting with
    std::shared_ptr< std::vector< std::shared_ptr< const CViewSegment2D > > > m_Segments;
  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DRay
  ////////////////////////////////////////////////////////////////////////////////////////

  // Keeps information about direct ray. Ray is containing of two direct beams.
  class CDirect2DRay {
  public:
    CDirect2DRay( const std::shared_ptr< CDirect2DBeam >& t_Beam1, 
      const std::shared_ptr< CDirect2DBeam >& t_Beam2 );
    CDirect2DRay( const std::shared_ptr< CViewSegment2D >& t_Ray1, 
      const std::shared_ptr< CViewSegment2D >& t_Ray2 );

    // Returns ray height. Projection of the ray to the normal plane.
    double rayNormalHeight() const;

    // Checks if segments intersects with the ray
    void checkSegment( const std::shared_ptr< const CViewSegment2D >& t_Segment );

    // Return segment hit by the ray
    std::shared_ptr< const CViewSegment2D > closestSegmentHit() const;

    double cosAngle( const std::shared_ptr< const CViewSegment2D >& t_Segment );

  private:
    std::shared_ptr< CDirect2DBeam > m_Beam1;
    std::shared_ptr< CDirect2DBeam > m_Beam2;
  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DRayResult
  ////////////////////////////////////////////////////////////////////////////////////////

  // Keeps result of beam ViewFactors. It is expensive operation to recalculate them every time
  // so this will just save results for the next call
  class CDirect2DRaysResult {
  public:
    CDirect2DRaysResult( const double t_ProfileAngle, double const t_DirectToDirect, 
      const std::shared_ptr< std::vector < BeamViewFactor > >& t_BeamViewFactors );
    
    // Beam view factors for given profile angle
    std::shared_ptr< std::vector < BeamViewFactor > > beamViewFactors() const;

    // Direct to direct transmitted beam component
    double directToDirect() const;

    double profileAngle() const;

  private:
    std::shared_ptr< std::vector < BeamViewFactor > > m_ViewFactors;
    double m_DirectToDirect;
    double m_ProfileAngle;
  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DRayResults
  ////////////////////////////////////////////////////////////////////////////////////////

  // Keeps result of beam ViewFactors. It is expensive operation to recalculate them every time
  // so this will just save results for the next call
  class CDirect2DRaysResults {
  public:
    CDirect2DRaysResults();
    
    // Beam view factors for given profile angle
    std::shared_ptr< CDirect2DRaysResult > getResult( const double t_ProfileAngle );

    // append results
    std::shared_ptr< CDirect2DRaysResult > append( const double t_ProfileAngle, const double t_DirectToDirect,
      const std::shared_ptr< std::vector < BeamViewFactor > >& t_BeamViewFactor );

    // clear all results
    void clear();

  private:
    std::shared_ptr< std::vector < std::shared_ptr< CDirect2DRaysResult > > > m_Results;
  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CDirect2DRays
  ////////////////////////////////////////////////////////////////////////////////////////

  // Keeps information about group of direct rays entering or exiting the enclosure
  class CDirect2DRays {
  public:
    explicit CDirect2DRays( const FenestrationCommon::Side t_Side );

    void appendGeometry2D( const std::shared_ptr< const CGeometry2D >& t_Geometry2D );

    // Beam view factors for given profile angle
    std::shared_ptr< std::vector < BeamViewFactor > > beamViewFactors( const double t_ProfileAngle );

    // Direct to direct transmitted beam component
    double directToDirect( const double t_ProfileAngle );

  private:
    void calculateAllProperties( const double t_ProfileAngle );

    // Finds lower and upper ray of every enclosure in the system
    void findRayBoundaries( const double t_ProfileAngle );
    
    // Finds all points that are on the path of the ray
    void findInBetweenRays( const double t_ProfileAngle );

    // Calculate beam view factors
    void calculateBeamProperties( const double t_ProfileAngle );

    // Check if given point is in possible path of the ray
    bool isInRay( const std::shared_ptr< const CPoint2D >& t_Point );

    std::shared_ptr< CViewSegment2D > createSubBeam( const std::shared_ptr< const CPoint2D >& t_Point, 
      const double t_ProfileAngle ) const;

    FenestrationCommon::Side m_Side;

    std::vector< std::shared_ptr< const CGeometry2D > > m_Geometries2D;
    std::shared_ptr< CViewSegment2D > m_LowerRay;
    std::shared_ptr< CViewSegment2D > m_UpperRay;
    std::vector< std::shared_ptr< CDirect2DRay > > m_Rays;

    CDirect2DRaysResults m_Results;
    std::shared_ptr< CDirect2DRaysResult > m_CurrentResult;

  };

  ////////////////////////////////////////////////////////////////////////////////////////
  // CGeometry2DBeam
  ////////////////////////////////////////////////////////////////////////////////////////

  // Class to handle beam view factors
  class CGeometry2DBeam {
  public:
    CGeometry2DBeam();

    void appendGeometry2D( const std::shared_ptr< const CGeometry2D >& t_Geometry2D );
    
    std::shared_ptr< std::vector < BeamViewFactor > > 
      beamViewFactors( const double t_ProfileAngle, const FenestrationCommon::Side t_Side );
    
    // Direct to direct transmitted beam component
    double directToDirect( const double t_ProfileAngle, const FenestrationCommon::Side t_Side );

  private:
    CDirect2DRays* getRay( const FenestrationCommon::Side t_Side );
    CDirect2DRays m_Incoming;
    CDirect2DRays m_Outgoing;

    
  };

}

#endif