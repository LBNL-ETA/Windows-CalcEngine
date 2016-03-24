#include <memory>
#include <algorithm>
#include <gtest\gtest.h>

#include "SpectralProperties.hpp"
#include "IntegratorStrategy.hpp"

using namespace std;
using namespace SpectralAveraging;

class TestSpectralIntegration : public testing::Test
{

private:
  shared_ptr< CSpectralProperties > m_SpectralProperty;

protected:
  virtual void SetUp()
  {
    m_SpectralProperty = make_shared< CSpectralProperties >();

    m_SpectralProperty->addProperty( 0.500,  0.5511 );
    m_SpectralProperty->addProperty( 0.505,  0.5519 );
    m_SpectralProperty->addProperty( 0.510,  0.5523 );
    m_SpectralProperty->addProperty( 0.515,  0.5529 );
    m_SpectralProperty->addProperty( 0.520,  0.5543 );
    m_SpectralProperty->addProperty( 0.525,  0.5552 );
    m_SpectralProperty->addProperty( 0.530,  0.5579 );
    m_SpectralProperty->addProperty( 0.535,  0.5626 );
    m_SpectralProperty->addProperty( 0.540,  0.5699 );
    m_SpectralProperty->addProperty( 0.545,  0.5789 );
    m_SpectralProperty->addProperty( 0.550,  0.5884 );
    m_SpectralProperty->addProperty( 0.555,  0.5949 );
    m_SpectralProperty->addProperty( 0.560,  0.5971 );
    m_SpectralProperty->addProperty( 0.565,  0.5946 );
    m_SpectralProperty->addProperty( 0.570,  0.5885 );
    m_SpectralProperty->addProperty( 0.575,  0.5784 );
    m_SpectralProperty->addProperty( 0.580,  0.5666 );
    m_SpectralProperty->addProperty( 0.585,  0.5547 );
    m_SpectralProperty->addProperty( 0.590,  0.5457 );
    m_SpectralProperty->addProperty( 0.595,  0.5425 );
    m_SpectralProperty->addProperty( 0.600,  0.5435 );

  };

public:
  shared_ptr< CSpectralProperties > getProperty() { return m_SpectralProperty; };

};

TEST_F( TestSpectralIntegration, TestRectangular )
{
  SCOPED_TRACE( "Begin Test: Test rectangular integration over the specturm of data." );
  
  shared_ptr< CSpectralProperties > aIntegratedProperties = nullptr;
  
  shared_ptr< CSpectralProperties > aSpectralProperties = getProperty();

  aIntegratedProperties = aSpectralProperties->integrate( IntegrationType::Rectangular );

  vector< double > correctResults;
  correctResults.push_back( 0.0027555 );
  correctResults.push_back( 0.0027595 );
  correctResults.push_back( 0.0027615 );
  correctResults.push_back( 0.0027645 );
  correctResults.push_back( 0.0027715 );
  correctResults.push_back( 0.0027760 );
  correctResults.push_back( 0.0027895 );
  correctResults.push_back( 0.0028130 );
  correctResults.push_back( 0.0028495 );
  correctResults.push_back( 0.0028945 );
  correctResults.push_back( 0.0029420 );
  correctResults.push_back( 0.0029745 );
  correctResults.push_back( 0.0029855 );
  correctResults.push_back( 0.0029730 );
  correctResults.push_back( 0.0029425 );
  correctResults.push_back( 0.0028920 );
  correctResults.push_back( 0.0028330 );
  correctResults.push_back( 0.0027735 );
  correctResults.push_back( 0.0027285 );
  correctResults.push_back( 0.0027125 );

  vector< double > calculatedResults;
  vector< shared_ptr < CSpectralProperty > >::const_iterator it;
  for( it = aIntegratedProperties->begin(); it != aIntegratedProperties->end(); ++it )
  {
    calculatedResults.push_back( (*it)->value() );
  }

  EXPECT_EQ( calculatedResults.size(), correctResults.size() );

  for( int i = 0; i < calculatedResults.size(); ++i ) {
    EXPECT_NEAR( correctResults[ i ], calculatedResults[ i ], 1e-6 );
  }

}

TEST_F( TestSpectralIntegration, TestTrapezoidal )
{
  SCOPED_TRACE( "Begin Test: Test trapezoidal integration over the specturm of data." );
  
  shared_ptr< CSpectralProperties > aIntegratedProperties = nullptr;
  
  shared_ptr< CSpectralProperties > aSpectralProperties = getProperty();

  aIntegratedProperties = aSpectralProperties->integrate( IntegrationType::Trapezoidal );

  vector< double > correctResults;
  correctResults.push_back( 0.00275750 );
  correctResults.push_back( 0.00276050 );
  correctResults.push_back( 0.00276300 );
  correctResults.push_back( 0.00276800 );
  correctResults.push_back( 0.00277375 );
  correctResults.push_back( 0.00278275 );
  correctResults.push_back( 0.00280125 );
  correctResults.push_back( 0.00283125 );
  correctResults.push_back( 0.00287200 );
  correctResults.push_back( 0.00291825 );
  correctResults.push_back( 0.00295825 );
  correctResults.push_back( 0.00298000 );
  correctResults.push_back( 0.00297925 );
  correctResults.push_back( 0.00295775 );
  correctResults.push_back( 0.00291725 );
  correctResults.push_back( 0.00286250 );
  correctResults.push_back( 0.00280325 );
  correctResults.push_back( 0.00275100 );
  correctResults.push_back( 0.00272050 );
  correctResults.push_back( 0.00271500 );

  vector< double > calculatedResults;
  vector< shared_ptr < CSpectralProperty > >::const_iterator it;
  for( it = aIntegratedProperties->begin(); it != aIntegratedProperties->end(); ++it )
  {
    calculatedResults.push_back( (*it)->value() );
  }

  EXPECT_EQ( calculatedResults.size(), correctResults.size() );

  for( int i = 0; i < calculatedResults.size(); ++i ) {
    EXPECT_NEAR( correctResults[ i ], calculatedResults[ i ], 1e-6 );
  }

}