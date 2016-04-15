#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "Geometry2D.hpp"
#include "ViewSegment2D.hpp"
#include "Point2D.hpp"
#include "SquareMatrix.hpp"

using namespace std;
using namespace Viewer;
using namespace FenestrationCommon;

class TestEnclosure2DViewFactors2 : public testing::Test {

private:
  shared_ptr< CGeometry2D > m_Enclosure2D;

protected:
  virtual void SetUp() {
    m_Enclosure2D = make_shared< CGeometry2D >();

    // Segment 1
    shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 0, 0 );
    shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0.5, 3 );
  
    shared_ptr< CViewSegment2D > aSegment1 = make_shared< CViewSegment2D >( aStartPoint1, aEndPoint1 );
    m_Enclosure2D->appendSegment( aSegment1 );

    // Segment 2
    shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 0.5, 3 );
    shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 1.5, 2 );
  
    shared_ptr< CViewSegment2D > aSegment2 = make_shared< CViewSegment2D >( aStartPoint2, aEndPoint2 );
    m_Enclosure2D->appendSegment( aSegment2 );

    // Segment 3
    shared_ptr< CPoint2D > aStartPoint3 = make_shared< CPoint2D >( 1.5, 2 );
    shared_ptr< CPoint2D > aEndPoint3 = make_shared< CPoint2D >( 2, 0.5 );
  
    shared_ptr< CViewSegment2D > aSegment3 = make_shared< CViewSegment2D >( aStartPoint3, aEndPoint3 );
    m_Enclosure2D->appendSegment( aSegment3 );

    // Segment 4
    shared_ptr< CPoint2D > aStartPoint4 = make_shared< CPoint2D >( 2, 0.5 );
    shared_ptr< CPoint2D > aEndPoint4 = make_shared< CPoint2D >( 0, 0 );
  
    shared_ptr< CViewSegment2D > aSegment4 = make_shared< CViewSegment2D >( aStartPoint4, aEndPoint4 );
    m_Enclosure2D->appendSegment( aSegment4 );

  }

public:
  shared_ptr< CGeometry2D > getEnclosure() { return m_Enclosure2D; };

};

TEST_F( TestEnclosure2DViewFactors2, Enclosure2DViewFactors ) {
  SCOPED_TRACE( "Begin Test: 2D Enclosure - View Factors (no blocking)." );

  shared_ptr< CGeometry2D > aEnclosure = getEnclosure();
  
  shared_ptr< CSquareMatrix > viewFactors = aEnclosure->viewFactors();

  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 0 ][ 0 ], 1e-6 );
  EXPECT_NEAR( 0.321497809, ( *viewFactors )[ 0 ][ 1 ], 1e-6 );
  EXPECT_NEAR( 0.318886289, ( *viewFactors )[ 0 ][ 2 ], 1e-6 );
  EXPECT_NEAR( 0.359615901, ( *viewFactors )[ 0 ][ 3 ], 1e-6 );

  EXPECT_NEAR( 0.691407182, ( *viewFactors )[ 1 ][ 0 ], 1e-6 );
  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 1 ][ 1 ], 1e-6 );
  EXPECT_NEAR( 0.028240588, ( *viewFactors )[ 1 ][ 2 ], 1e-6 );
  EXPECT_NEAR( 0.280352230, ( *viewFactors )[ 1 ][ 3 ], 1e-6 );

  EXPECT_NEAR( 0.613390025, ( *viewFactors )[ 2 ][ 0 ], 1e-6 );
  EXPECT_NEAR( 0.025259150, ( *viewFactors )[ 2 ][ 1 ], 1e-6 );
  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 2 ][ 2 ], 1e-6 );
  EXPECT_NEAR( 0.361350825, ( *viewFactors )[ 2 ][ 3 ], 1e-6 );

  EXPECT_NEAR( 0.530536525, ( *viewFactors )[ 3 ][ 0 ], 1e-6 );
  EXPECT_NEAR( 0.192320043, ( *viewFactors )[ 3 ][ 1 ], 1e-6 );
  EXPECT_NEAR( 0.277143432, ( *viewFactors )[ 3 ][ 2 ], 1e-6 );
  EXPECT_NEAR( 0.000000000, ( *viewFactors )[ 3 ][ 3 ], 1e-6 );

}