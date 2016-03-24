#include <memory>
#include <gtest\gtest.h>

#include <memory>

#include "BSDFThetaLimits.hpp"

using namespace std;
using namespace LayerOptics;

class TestBSDFThetaLimtisQuarterBasis : public testing::Test
{

private:
	shared_ptr< CThetaLimits > m_Thetas;

protected:
	virtual void SetUp() {
    vector< double > thetaAngles = { 0, 18, 36, 54, 76.5 };
		m_Thetas = make_shared< CThetaLimits >( thetaAngles );
	};

public:
	shared_ptr< CThetaLimits > GetLimits() { return m_Thetas; };

};

TEST_F( TestBSDFThetaLimtisQuarterBasis, TestQuarterBasis )
{
	SCOPED_TRACE( "Begin Test: Theta limits - quarter basis." );
	
	shared_ptr< CThetaLimits > aLimits = GetLimits();
	
	vector< double > results = *( aLimits->getThetaLimits() );

  vector< double > correctResults = { 0, 9, 27, 45, 63, 90 };
  EXPECT_EQ( results.size(), correctResults.size() );

  for( size_t i = 0; i < results.size(); ++i ) {
    EXPECT_NEAR( results[i], correctResults[i], 1e-6 );
  }

}