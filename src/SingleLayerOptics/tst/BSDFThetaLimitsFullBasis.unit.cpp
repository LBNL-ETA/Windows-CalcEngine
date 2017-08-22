#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCESingleLayerOptics.hpp"

using namespace std;
using namespace SingleLayerOptics;

class TestBSDFThetaLimtisFullBasis : public testing::Test {

private:
	std::shared_ptr< CThetaLimits > m_Thetas;

protected:
	virtual void SetUp() {
		vector< double > thetaAngles = { 0, 10, 20, 30, 40, 50, 60, 70, 82.5 };
		m_Thetas = make_shared< CThetaLimits >( thetaAngles );
	}

public:
	std::shared_ptr< CThetaLimits > GetLimits() {
		return m_Thetas;
	};

};

TEST_F( TestBSDFThetaLimtisFullBasis, TestFullBasis ) {
	SCOPED_TRACE( "Begin Test: Theta limits - full basis." );

	std::shared_ptr< CThetaLimits > aLimits = GetLimits();

	vector< double > results = *( aLimits->getThetaLimits() );

	vector< double > correctResults = { 0, 5, 15, 25, 35, 45, 55, 65, 75, 90 };
	EXPECT_EQ( results.size(), correctResults.size() );

	for ( size_t i = 0; i < results.size(); ++i ) {
		EXPECT_NEAR( results[i], correctResults[i], 1e-6 );
	}

}
