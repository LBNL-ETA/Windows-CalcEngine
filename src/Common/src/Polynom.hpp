#ifndef WINDOWS_CALCENGINE_POLYNOM_HPP
#define WINDOWS_CALCENGINE_POLYNOM_HPP

namespace FenestrationCommon {

  class Polynom {
  public:
    Polynom( std::vector<double> const &t_Coeffs );

    double value( double const t_X );

  private:
    std::vector<double> m_Coeffs;
  };

}


#endif //WINDOWS_CALCENGINE_POLYNOM_HPP
