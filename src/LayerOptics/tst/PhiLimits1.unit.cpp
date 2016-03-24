#include <memory>
#include <gtest\gtest.h>

#include <memory>

#include "BSDFPhiLimits.hpp"

using namespace std;
using namespace LayerOptics;

class TestPhiLimits1 : public testing::Test
{

private:
  shared_ptr< CPhiLimits > m_PhiLimits;

protected:
  virtual void SetUp()
  {
    m_PhiLimits = make_shared< CPhiLimits >( 8 );
  };

public:
  shared_ptr< CPhiLimits > GetLimits() { return m_PhiLimits; };

};

TEST_F( TestPhiLimits1, TestBSDFRingCreation )
{
  SCOPED_TRACE( "Begin Test: BSDF Phi limits creation." );
  
  shared_ptr< CPhiLimits > aLimits = GetLimits();
  
  vector< double > results = *(aLimits->getPhiLimits());

  vector< double > correctResults = { -22.5, 22.5, 67.5, 112.5, 157.5, 202.5, 247.5, 292.5, 337.5 };
  EXPECT_EQ( results.size(), correctResults.size() );

  for( size_t i = 0; i < results.size(); ++i ) {
    EXPECT_NEAR( results[i], correctResults[i], 1e-6 );
  }

}