#include <memory>
#include <gtest/gtest.h>

#include "WCESpectralAveraging.hpp"

class TestSpectrumFunctions : public testing::Test {
private:
	std::vector< double > inputVector;
protected:

	void SetUp() override {
		inputVector = { 0.1, 0.2, 0.3, 0.4, 0.5 };
	}

public:
	std::vector< double > getInputVector() const {
		return inputVector;
	}

};

TEST_F( TestSpectrumFunctions, TestRatio ) {
	SCOPED_TRACE( "Begin Test: Test UV Function." );

	auto vector = getInputVector();

	auto results = SpectralAveraging::UVAction( vector );
	std::vector< std::pair< double, double > > correctResults {
			{ 0.1, 11.02317638 },
			{ 0.2, 3.320116923 },
			{ 0.3, 1 },
			{ 0.4, 0.301194212 },
			{ 0.5, 0.090717953 }
	};

	EXPECT_EQ( correctResults.size(), results.size() );

	for( auto i = 0u; i < correctResults.size(); ++i ) {
		EXPECT_NEAR( correctResults[ i ].first, results[ i ].first, 1e-6 );
		EXPECT_NEAR( correctResults[ i ].second, results[ i ].second, 1e-6 );
	}

}

TEST_F( TestSpectrumFunctions, TestUVKrochmann ) {
	SCOPED_TRACE( "Begin Test: Test UV Krochmann." );

	auto vector = getInputVector();

	auto results = SpectralAveraging::Krochmann( vector );
	std::vector< std::pair< double, double > > correctResults {
			{ 0.1, 16713.967064 },
			{ 0.2, 1297.247512 },
			{ 0.3, 100.6853190 },
			{ 0.4, 7.814648615 },
			{ 0.5, 0.606530660 }
	};

	EXPECT_EQ( correctResults.size(), results.size() );

	for( auto i = 0u; i < correctResults.size(); ++i ) {
		EXPECT_NEAR( correctResults[ i ].first, results[ i ].first, 1e-6 );
		EXPECT_NEAR( correctResults[ i ].second, results[ i ].second, 1e-6 );
	}

}

// TEST_F( TestSpectrumFunctions, TestBlackBody ) {
// 	SCOPED_TRACE( "Begin Test: Test BlackBody." );
//
// 	auto vector = getInputVector();
//
// 	const double BlackBodyTemperature = 300;
//
// 	auto results = SpectralAveraging::BlackBodySpectrum( vector, BlackBodyTemperature );
// 	std::vector< std::pair< double, double > > correctResults {
// 			{ 0.1, 16713.967064 },
// 			{ 0.2, 1297.247512 },
// 			{ 0.3, 100.6853190 },
// 			{ 0.4, 7.814648615 },
// 			{ 0.5, 0.606530660 }
// 	};
//
// 	EXPECT_EQ( correctResults.size(), results.size() );
//
// 	for( auto i = 0u; i < correctResults.size(); ++i ) {
// 		EXPECT_NEAR( correctResults[ i ].first, results[ i ].first, 1e-6 );
// 		EXPECT_NEAR( correctResults[ i ].second, results[ i ].second, 1e-6 );
// 	}
//
// }
