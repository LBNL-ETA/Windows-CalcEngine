#include <memory>
#include <algorithm>
#include <gtest\gtest.h>

#include "SpectralProperties.hpp"
#include "IntegratorStrategy.hpp"

using namespace std;
using namespace SpectralAveraging;

class TestSpectralInterpolation : public testing::Test
{

private:
  shared_ptr< CSpectralProperties > m_SpectralProperty;

protected:
  virtual void SetUp()
  {
    m_SpectralProperty = make_shared< CSpectralProperties >();

    // part of ASTM E891-87 Table 1
    m_SpectralProperty->addProperty( 0.40, 556 );
    m_SpectralProperty->addProperty( 0.41, 656.3 );
    m_SpectralProperty->addProperty( 0.42, 690.8 );
    m_SpectralProperty->addProperty( 0.43, 641.9 );
    m_SpectralProperty->addProperty( 0.44, 798.5 );
    m_SpectralProperty->addProperty( 0.45, 956.6 );
    m_SpectralProperty->addProperty( 0.46, 990 );
    m_SpectralProperty->addProperty( 0.47, 998 );
    m_SpectralProperty->addProperty( 0.48, 1046.1 );
    m_SpectralProperty->addProperty( 0.49, 1005.1 );
    m_SpectralProperty->addProperty( 0.50, 1026.7 );

  };

public:
  shared_ptr< CSpectralProperties > getProperty() { return m_SpectralProperty; };

};

TEST_F( TestSpectralInterpolation, TestInterpolation )
{
  SCOPED_TRACE( "Begin Test: Test interpolation over the range of data." );
  
  shared_ptr< CSpectralProperties > aInterpolatedProperties = nullptr;
  
  shared_ptr< CSpectralProperties > aSpectralProperties = getProperty();

  shared_ptr< vector< double > > wavelengths = make_shared< vector< double > >();
  wavelengths->push_back( 0.400 );
  wavelengths->push_back( 0.405 );
  wavelengths->push_back( 0.410 );
  wavelengths->push_back( 0.415 );
  wavelengths->push_back( 0.420 );
  wavelengths->push_back( 0.425 );
  wavelengths->push_back( 0.430 );
  wavelengths->push_back( 0.435 );
  wavelengths->push_back( 0.440 );
  wavelengths->push_back( 0.445 );
  wavelengths->push_back( 0.450 );
  wavelengths->push_back( 0.455 );
  wavelengths->push_back( 0.460 );
  wavelengths->push_back( 0.465 );
  wavelengths->push_back( 0.470 );
  wavelengths->push_back( 0.475 );
  wavelengths->push_back( 0.480 );
  wavelengths->push_back( 0.485 );
  wavelengths->push_back( 0.490 );
  wavelengths->push_back( 0.495 );
  //wavelengths->push_back( 0.500 );



  aInterpolatedProperties = aSpectralProperties->interpolate( wavelengths );

  vector< double > correctResults;
  correctResults.push_back( 556.000 );
  correctResults.push_back( 606.150 );
  correctResults.push_back( 656.300 );
  correctResults.push_back( 673.550 );
  correctResults.push_back( 690.800 );
  correctResults.push_back( 666.350 );
  correctResults.push_back( 641.900 );
  correctResults.push_back( 720.200 );
  correctResults.push_back( 798.500 );
  correctResults.push_back( 877.550 );
  correctResults.push_back( 956.600 );
  correctResults.push_back( 973.300 );
  correctResults.push_back( 990.000 );
  correctResults.push_back( 994.000 );
  correctResults.push_back( 998.000 );
  correctResults.push_back( 1022.050 );
  correctResults.push_back( 1046.100 );
  correctResults.push_back( 1025.600 );
  correctResults.push_back( 1005.100 );
  correctResults.push_back( 1015.900 );
  // correctResults.push_back( 1026.700 );

  vector< double > calculatedResults;
  vector< shared_ptr < CSpectralProperty > >::const_iterator it;
  for( it = aInterpolatedProperties->begin(); it != aInterpolatedProperties->end(); ++it )
  {
    calculatedResults.push_back( (*it)->value() );
  }

  EXPECT_EQ( calculatedResults.size(), correctResults.size() );

  for( int i = 0; i < calculatedResults.size(); ++i ) {
    EXPECT_NEAR( correctResults[ i ], calculatedResults[ i ], 1e-6 );
  }

}