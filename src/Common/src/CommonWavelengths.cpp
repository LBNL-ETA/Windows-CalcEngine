#include <algorithm>
#include <iterator>
#include <assert.h>

#include "CommonWavelengths.hpp"

using namespace std;

namespace FenestrationCommon {

  CCommonWavelengths::CCommonWavelengths() {
  }

  void CCommonWavelengths::addWavelength( std::shared_ptr< std::vector< double > > const & t_wv ) {
    m_Wavelengths.push_back( t_wv );
  }

  shared_ptr< vector< double > > CCommonWavelengths::getCombinedWavelengths( const Combine t_Combination ) {
    shared_ptr< vector< double > > aCombined = nullptr;
    for( auto it = m_Wavelengths.begin(); it < m_Wavelengths.end(); ++it ) {
      if( it == m_Wavelengths.begin() ) {
        aCombined = *it;
      } else {
        aCombined = combineWavelegths( *aCombined, *( *it ), t_Combination );
      }
    }

    assert( aCombined != nullptr );

    return aCombined;
  }

  shared_ptr< vector< double > > CCommonWavelengths::combineWavelegths( vector< double > const & t_wv1,
    vector< double > const & t_wv2, Combine const t_Combination ) const {

    // Set union of two wavelengths without repeating common data
    auto unionWavelengths = make_shared< vector< double > >();
    auto combinedWavelengths = make_shared< vector< double > >();

    set_union( t_wv1.begin(), t_wv1.end(), t_wv2.begin(), t_wv2.end(), 
      back_inserter( *unionWavelengths ) );

    if( t_Combination == Combine::Interpolate ) {
      // Remove extrapolated data. It is incorrect to have extrapolated wavelengths from one sample
      auto min1 = *min_element( t_wv1.begin(), t_wv1.end() );
      auto min2 = *min_element( t_wv2.begin(), t_wv2.end() );
      auto minWV = max( min1, min2 );

      auto max2 = *max_element( t_wv2.begin(), t_wv2.end() );
      auto max1 = *max_element( t_wv1.begin(), t_wv1.end() );
      auto maxWV = min( max1, max2 );

      for( auto val : *unionWavelengths ) {
        if( ( val >= minWV ) && ( val <= maxWV ) ) {
          combinedWavelengths->push_back( val );
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