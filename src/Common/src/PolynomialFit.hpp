#ifndef POLYNOMIALFIT_CPOLYFIT_HPP
#define POLYNOMIALFIT_CPOLYFIT_HPP

#include <vector>

class PolynomialFit {
public:
  explicit PolynomialFit( size_t const t_Order );

  // Get polynomial fit for given coefficients
  std::vector< double > getCoefficients( std::vector< std::pair< double, double > > t_Table );

private:
  size_t m_Order;
};


#endif //POLYNOMIALFIT_CPOLYFIT_HPP
