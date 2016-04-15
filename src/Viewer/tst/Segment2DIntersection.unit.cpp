#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "Segment2D.hpp"
#include "Point2D.hpp"

using namespace std;
using namespace Viewer;

class TestSegment2DIntersection : public testing::Test {

protected:
  virtual void SetUp() {
    
  }

};

TEST_F( TestSegment2DIntersection, Segment2DTest1 ) {
  SCOPED_TRACE( "Begin Test: Segment 2D - intersection point (1)." );
  
  shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 0, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 10, 10 );
  
  shared_ptr< CSegment2D > aSegment1 = make_shared< CSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 1, 0 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 10, 10 );
  
  shared_ptr< CSegment2D > aSegment2 = make_shared< CSegment2D >( aStartPoint2, aEndPoint2 );

  bool isInt = aSegment1->intersectionWithSegment( aSegment2 );

  EXPECT_EQ( false, isInt );

}

TEST_F( TestSegment2DIntersection, Segment2DTest2 ) {
  SCOPED_TRACE( "Begin Test: Segment 2D - intersection point (2)." );
  
  shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 4, 2 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 8, 1 );
  
  shared_ptr< CSegment2D > aSegment1 = make_shared< CSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 1, 3 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 5, 7 );
  
  shared_ptr< CSegment2D > aSegment2 = make_shared< CSegment2D >( aStartPoint2, aEndPoint2 );

  bool isInt = aSegment1->intersectionWithSegment( aSegment2 );

  EXPECT_EQ( false, isInt );

}

TEST_F( TestSegment2DIntersection, Segment2DTest3 ) {
  SCOPED_TRACE( "Begin Test: Segment 2D - parallel lines (no intersection)." );
  
  shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 0, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 1 );
  
  shared_ptr< CSegment2D > aSegment1 = make_shared< CSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 1, 0 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 1, 1 );
  
  shared_ptr< CSegment2D > aSegment2 = make_shared< CSegment2D >( aStartPoint2, aEndPoint2 );

  bool isInt = aSegment1->intersectionWithSegment( aSegment2 );

  EXPECT_EQ( false, isInt );

}

TEST_F( TestSegment2DIntersection, Segment2DTest4 ) {
  SCOPED_TRACE( "Begin Test: Segment 2D - parallel lines (Total overlap)." );
  
  shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 0, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 1 );
  
  shared_ptr< CSegment2D > aSegment1 = make_shared< CSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 0, 0 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 0, 2 );
  
  shared_ptr< CSegment2D > aSegment2 = make_shared< CSegment2D >( aStartPoint2, aEndPoint2 );

  bool isInt = aSegment1->intersectionWithSegment( aSegment2 );

  EXPECT_EQ( false, isInt );

}

TEST_F( TestSegment2DIntersection, Segment2DTest5 ) {
  SCOPED_TRACE( "Begin Test: Segment 2D - parallel lines (Total overlap - different directions)." );
  
  shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 0, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 1 );
  
  shared_ptr< CSegment2D > aSegment1 = make_shared< CSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 0, 2 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 0, 0 );
  
  shared_ptr< CSegment2D > aSegment2 = make_shared< CSegment2D >( aStartPoint2, aEndPoint2 );

  bool isInt = aSegment1->intersectionWithSegment( aSegment2 );

  EXPECT_EQ( false, isInt );

}

TEST_F( TestSegment2DIntersection, Segment2DTest6 ) {
  SCOPED_TRACE( "Begin Test: Segment 2D - parallel lines (Total overlap - same directions, different lengths)." );
  
  shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 0, 10 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 10, 0 );
  
  shared_ptr< CSegment2D > aSegment1 = make_shared< CSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 0, 10 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 5, 5 );
  
  shared_ptr< CSegment2D > aSegment2 = make_shared< CSegment2D >( aStartPoint2, aEndPoint2 );

  bool isInt = aSegment1->intersectionWithSegment( aSegment2 );

  EXPECT_EQ( false, isInt );

}