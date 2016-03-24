#include <memory>
#include <gtest\gtest.h>

#include <memory>

#include "Segment2D.hpp"
#include "Point2D.hpp"

using namespace std;
using namespace Viewer;

class TestSegment2D : public testing::Test {

protected:
  virtual void SetUp() {
    
  };

};

TEST_F( TestSegment2D, Segment2DTest1 ) {
  SCOPED_TRACE( "Begin Test: Segment 2D - length and normal (1)." );
  
  shared_ptr< CPoint2D > aStartPoint = make_shared< CPoint2D >( 0, 0 );
  shared_ptr< CPoint2D > aEndPoint = make_shared< CPoint2D >( 10, 0 );
  
  CSegment2D aSegment = CSegment2D( aStartPoint, aEndPoint );

  double length = aSegment.length();

  EXPECT_NEAR( 10, length, 1e-6 );

  // shared_ptr< CSegment2D > aNormal = aSegment.getNormal();
  // shared_ptr< const CPoint2D > aNormalPoint = aNormal->endPoint();
  // double x = aNormalPoint->x();
  // double y = aNormalPoint->y();
  // 
  // EXPECT_NEAR( 0, x, 1e-6 );
  // EXPECT_NEAR( -1, y, 1e-6 );

}

TEST_F( TestSegment2D, Segment2DTest2 ) {
  SCOPED_TRACE( "Begin Test: Segment 2D - length and normal (2)." );
  
  shared_ptr< CPoint2D > aStartPoint = make_shared< CPoint2D >( 0, 0 );
  shared_ptr< CPoint2D > aEndPoint = make_shared< CPoint2D >( 10, 10 );
  
  CSegment2D aSegment = CSegment2D( aStartPoint, aEndPoint );

  double length = aSegment.length();

  EXPECT_NEAR( 14.14213562, length, 1e-6 );

  // shared_ptr< CSegment2D > aNormal = aSegment.getNormal();
  // shared_ptr< const CPoint2D > aNormalPoint = aNormal->endPoint();
  // double x = aNormalPoint->x();
  // double y = aNormalPoint->y();
  // 
  // EXPECT_NEAR( 0.707106781, x, 1e-6 );
  // EXPECT_NEAR( -0.707106781, y, 1e-6 );

}