//
// Created by Dragan Vidanovic on 7/2/17.
//

#include <vector>
#include "Polynom.hpp"

using namespace std;

namespace FenestrationCommon {

  Polynom::Polynom( std::vector<double> const &t_Coeffs ) : m_Coeffs( t_Coeffs ) {

  }

  double Polynom::value( double const t_X ) {
    double result = 0;
    double curX = 1;
    for ( auto val : m_Coeffs ) {
      result += val * curX;
      curX *= t_X;
    }

    return result;
  }

}
