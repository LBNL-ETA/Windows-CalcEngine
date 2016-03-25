#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "FenestrationCommon.hpp"
#include "WavelengthRange.hpp"

using namespace std;
using namespace FenestrationCommon;

class WavelengthRangeTest : public testing::Test {

protected:
  virtual void SetUp() {
  };


};

TEST_F( WavelengthRangeTest, Test1 ) {
  SCOPED_TRACE( "Begin Test: Creation of IR range." );

  CWavelengthRangeFactory aFactory = CWavelengthRangeFactory();
  shared_ptr< const CWavelengthRange > aRange = aFactory.getWavelengthRange( WavelengthRange::IR );

  EXPECT_NEAR( 5.0, aRange->minLambda(), 1e-6 );
  EXPECT_NEAR( 100.0, aRange->maxLambda(), 1e-6 );

}

TEST_F( WavelengthRangeTest, Test2 ) {
  SCOPED_TRACE( "Begin Test: Creation of Solar range." );

  CWavelengthRangeFactory aFactory = CWavelengthRangeFactory();
  shared_ptr< const CWavelengthRange > aRange = aFactory.getWavelengthRange( WavelengthRange::Solar );

  EXPECT_NEAR( 0.3, aRange->minLambda(), 1e-6 );
  EXPECT_NEAR( 2.5, aRange->maxLambda(), 1e-6 );

}

TEST_F( WavelengthRangeTest, Test3 ) {
  SCOPED_TRACE( "Begin Test: Creation of Visible range." );

  CWavelengthRangeFactory aFactory = CWavelengthRangeFactory();
  shared_ptr< const CWavelengthRange > aRange = aFactory.getWavelengthRange( WavelengthRange::Visible );

  EXPECT_NEAR( 0.38, aRange->minLambda(), 1e-6 );
  EXPECT_NEAR( 0.78, aRange->maxLambda(), 1e-6 );

}