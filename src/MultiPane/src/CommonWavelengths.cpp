#include <algorithm>
#include <iterator>
#include <assert.h>

#include "CommonWavelengths.hpp"

using namespace std;

namespace MultiPane {

  CCommonWavelengths::CCommonWavelengths() {
  }

  void CCommonWavelengths::addWavelength( std::shared_ptr< std::vector< double > > t_wv ) {
    m_Wavelengths.push_back( t_wv );
  }

  shared_ptr< vector< double > > CCommonWavelengths::getCombinedWavelengths( const Combine t_Combination ) {
    shared_ptr< vector< double > > aCombined = nullptr;
    vector< shared_ptr< vector< double > > >::iterator it;
    for( it = m_Wavelengths.begin(); it < m_Wavelengths.end(); ++it ) {
      if( it == m_Wavelengths.begin() ) {
        aCombined = *it;
      } else {
        aCombined = combineWavelegths( *aCombined, *( *it ), t_Combination );
      }
    }

    assert( aCombined != nullptr );

    return aCombined;
  }

  shared_ptr< vector< double > > CCommonWavelengths::combineWavelegths( const vector< double >& t_wv1,
    const vector< double >& t_wv2, const Combine t_Combination ) {
    assert( t_wv1 != nullptr );
    assert( t_wv2 != nullptr );

    // Set union of two wavelengths without repeating common data
    shared_ptr< vector< double > > unionWavelengths = make_shared< vector< double > >();
    shared_ptr< vector< double > > combinedWavelengths = make_shared< vector< double > >();

    set_union( t_wv1.begin(), t_wv1.end(), t_wv2.begin(), t_wv2.end(), 
      back_inserter( *unionWavelengths ) );

    if( t_Combination == Combine::Interpolate ) {
      // Remove extrapolated data. It is incorrect to have extrapolated wavelengths from one sample
      double min1 = *min_element( t_wv1.begin(), t_wv1.end() );
      double min2 = *min_element( t_wv2.begin(), t_wv2.end() );
      double minWV = max( min1, min2 );

      double max1 = *max_element( t_wv1.begin(), t_wv1.end() );
      double max2 = *max_element( t_wv2.begin(), t_wv2.end() );
      double maxWV = min( max1, max2 );

      for( vector< double >::iterator it = unionWavelengths->begin(); 
        it != unionWavelengths->end(); ++it ) {
        if( ( *it >= minWV ) && ( *it <= maxWV ) ) {
          combinedWavelengths->push_back( *it );
        }
      }
    } else if( t_Combination == Combine::Extrapolate ) {
      combinedWavelengths = unionWavelengths;
    } else {
      assert("Incorrect method for combining common wavelengths.");
    }

    return combinedWavelengths;
  }

}