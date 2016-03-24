#include <memory>
#include <gtest\gtest.h>

#include <memory>

#include "ViewSegment2D.hpp"
#include "Point2D.hpp"

using namespace std;
using namespace Viewer;

class TestSegment2DSelfShadowing : public testing::Test {

protected:
	virtual void SetUp() {
		
	};

};

TEST_F( TestSegment2DSelfShadowing, Segment2DNoShadowing ) {
	SCOPED_TRACE( "Begin Test: Segments self shadowing - No shadowing case." );
	
	shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 10, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 0 );
	
	shared_ptr< CViewSegment2D > aSegment1 = make_shared< CViewSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 0, 1 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 10, 1 );
	
	shared_ptr< CViewSegment2D > aSegment2 = make_shared< CViewSegment2D >( aStartPoint2, aEndPoint2 );

  Shadowing aShadowing = aSegment1->selfShadowing( aSegment2 );

  EXPECT_EQ( Shadowing::No, aShadowing );

}

TEST_F( TestSegment2DSelfShadowing, Segment2DTotalShadowing ) {
	SCOPED_TRACE( "Begin Test: Segments self shadowing - Total shadowing case." );
	
	shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 10, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 0 );
	
	shared_ptr< CViewSegment2D > aSegment1 = make_shared< CViewSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 10, 1 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 0, 1 );
	
	shared_ptr< CViewSegment2D > aSegment2 = make_shared< CViewSegment2D >( aStartPoint2, aEndPoint2 );

  Shadowing aShadowing = aSegment1->selfShadowing( aSegment2 );

  EXPECT_EQ( Shadowing::Total, aShadowing );

}

TEST_F( TestSegment2DSelfShadowing, Segment2DNoShadowingSamePoint1 ) {
	SCOPED_TRACE( "Begin Test: Segments self shadowing - No shadowing case (share same point angle < 180)." );
	
	shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 10, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 0 );
	
	shared_ptr< CViewSegment2D > aSegment1 = make_shared< CViewSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 0, 1 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 10, 0 );
	
	shared_ptr< CViewSegment2D > aSegment2 = make_shared< CViewSegment2D >( aStartPoint2, aEndPoint2 );

  Shadowing aShadowing = aSegment1->selfShadowing( aSegment2 );

  EXPECT_EQ( Shadowing::No, aShadowing );

}

TEST_F( TestSegment2DSelfShadowing, Segment2DNoShadowingSamePoint2 ) {
	SCOPED_TRACE( "Begin Test: Segments self shadowing - No shadowing case (share same point, angle > 180)." );
	
	shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 10, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 0 );
	
	shared_ptr< CViewSegment2D > aSegment1 = make_shared< CViewSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 0, 0 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 0, -2 );
	
	shared_ptr< CViewSegment2D > aSegment2 = make_shared< CViewSegment2D >( aStartPoint2, aEndPoint2 );

  Shadowing aShadowing = aSegment1->selfShadowing( aSegment2 );

  EXPECT_EQ( Shadowing::Total, aShadowing );

}

TEST_F( TestSegment2DSelfShadowing, Segment2DPartialShadowingThis ) {
	SCOPED_TRACE( "Begin Test: Segments self shadowing - Partial shadowing case (view blocked by itself)." );
	
	shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 10, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 0 );
	
	shared_ptr< CViewSegment2D > aSegment1 = make_shared< CViewSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( -5, -1 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( -5, 1 );
	
	shared_ptr< CViewSegment2D > aSegment2 = make_shared< CViewSegment2D >( aStartPoint2, aEndPoint2 );

  Shadowing aShadowing = aSegment1->selfShadowing( aSegment2 );

  EXPECT_EQ( Shadowing::Partial, aShadowing );

}

TEST_F( TestSegment2DSelfShadowing, Segment2DPartialShadowingOther ) {
	SCOPED_TRACE( "Begin Test: Segments self shadowing - Partial shadowing case (view blocked by viewed surface)." );
	
	shared_ptr< CPoint2D > aStartPoint1 = make_shared< CPoint2D >( 10, 0 );
  shared_ptr< CPoint2D > aEndPoint1 = make_shared< CPoint2D >( 0, 0 );
	
	shared_ptr< CViewSegment2D > aSegment1 = make_shared< CViewSegment2D >( aStartPoint1, aEndPoint1 );

  shared_ptr< CPoint2D > aStartPoint2 = make_shared< CPoint2D >( 5, 5 );
  shared_ptr< CPoint2D > aEndPoint2 = make_shared< CPoint2D >( 5, 10 );
	
	shared_ptr< CViewSegment2D > aSegment2 = make_shared< CViewSegment2D >( aStartPoint2, aEndPoint2 );

  Shadowing aShadowing = aSegment1->selfShadowing( aSegment2 );

  EXPECT_EQ( Shadowing::Partial, aShadowing );

}