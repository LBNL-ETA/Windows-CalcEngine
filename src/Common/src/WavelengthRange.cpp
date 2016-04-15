#include <assert.h>

#include "WavelengthRange.hpp"
#include "FenestrationCommon.hpp"

using namespace std;

namespace FenestrationCommon {

  /////////////////////////////////////////////////////////////////////////////////////////////
  ////   CWavelengthRange
  /////////////////////////////////////////////////////////////////////////////////////////////

  CWavelengthRange::CWavelengthRange( const double minLambda, const double maxLambda ) : 
    m_MinLambda( minLambda ), m_MaxLambda( maxLambda ) {
    
  }

  double CWavelengthRange::minLambda() const {
    return m_MinLambda;
  }
  
  double CWavelengthRange::maxLambda() const {
    return m_MaxLambda;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////
  ////   CWavelengthRangeFactory
  /////////////////////////////////////////////////////////////////////////////////////////////

  CWavelengthRangeFactory::CWavelengthRangeFactory() {
    
  }

  shared_ptr< const CWavelengthRange > CWavelengthRangeFactory::getWavelengthRange( const WavelengthRange t_Range ) {
    shared_ptr< const CWavelengthRange > aRange = nullptr;
    switch( t_Range ) {
    case WavelengthRange::IR:
      aRange = make_shared< CWavelengthRange >( 5.0, 100.0 );
      break;
    case WavelengthRange::Solar:
      aRange = make_shared< CWavelengthRange >( 0.3, 2.5 );
      break;
    case WavelengthRange::Visible:
      aRange = make_shared< CWavelengthRange >( 0.38, 0.78 );
      break;
    default:
      assert("Incorrect call for wavelength range creation.");
      break;
    }

    return aRange;
  }

}