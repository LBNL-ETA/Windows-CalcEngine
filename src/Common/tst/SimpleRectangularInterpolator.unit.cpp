#include <memory>
#include <gtest/gtest.h>

#include "IntegratorStrategy.hpp"

using namespace std;
using namespace FenestrationCommon;

class TestSimpleRectangularIntegration : public testing::Test
{

private:
  shared_ptr< IIntegratorStrategy > m_Integrator;

protected:
  virtual void SetUp()
  {
    CIntegratorFactory aFactory = CIntegratorFactory();
    m_Integrator = aFactory.getIntegrator( IntegrationType::Rectangular );
  };

public:
  shared_ptr< IIntegratorStrategy > getIntegrator() { return m_Integrator; };

};

TEST_F( TestSimpleRectangularIntegration, TestRectangular )
{
  SCOPED_TRACE( "Begin Test: Test rectangular integrator" );
  
  shared_ptr< IIntegratorStrategy > aIntegrator = nullptr;
  
  aIntegrator = getIntegrator();

  double value = aIntegrator->integrate( 1, 2, 10, 11 );

  EXPECT_NEAR( 10.00, value, 1e-6 );

}