#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "BSDFDirections.hpp"
#include "BSDFPatch.hpp"
#include "BeamDirection.hpp"

using namespace std;
using namespace LayerOptics;

class TestBSDFDirectionsClosestIndex : public testing::Test
{

private:
  shared_ptr< CBSDFHemisphere > m_BSDFHemisphere;

protected:
  virtual void SetUp() {
    m_BSDFHemisphere = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );
  }

public:
  shared_ptr< CBSDFDirections > GetDirections( const BSDFHemisphere t_Side ) 
    { return m_BSDFHemisphere->getDirections( t_Side ); };

};

TEST_F( TestBSDFDirectionsClosestIndex, TestClosestIndex1 ) {
  SCOPED_TRACE( "Begin Test: Find closest index 1." );
  
  CBSDFDirections aDirections = *GetDirections( BSDFHemisphere::Incoming );

  double theta = 15;
  double phi = 270;

	size_t beamIndex = aDirections.getNearestBeamIndex( theta, phi );
	
  EXPECT_EQ( 7, beamIndex );

}

TEST_F( TestBSDFDirectionsClosestIndex, TestClosestIndex2 ) {
  SCOPED_TRACE( "Begin Test: Find closest index 2." );

  CBSDFDirections aDirections = *GetDirections( BSDFHemisphere::Incoming );

  double theta = 70;
  double phi = 175;

  size_t beamIndex = aDirections.getNearestBeamIndex( theta, phi );

  EXPECT_EQ( 37, beamIndex );

}

TEST_F( TestBSDFDirectionsClosestIndex, TestClosestIndex3 ) {
  SCOPED_TRACE( "Begin Test: Find closest index 3." );

  CBSDFDirections aDirections = *GetDirections( BSDFHemisphere::Incoming );

  double theta = 55;
  double phi = 60;

  size_t beamIndex = aDirections.getNearestBeamIndex( theta, phi );

  EXPECT_EQ( 23, beamIndex );

}