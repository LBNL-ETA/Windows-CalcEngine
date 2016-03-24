#ifndef WAVELENGTHRANGE_H
#define WAVELENGTHRANGE_H

#include <memory>

namespace FenestrationCommon {

  enum class WavelengthRange;

  class CWavelengthRange {
  public:
    CWavelengthRange( const double minLambda, const double maxLambda );

    double minLambda() const;
    double maxLambda() const;

  private:
    double m_MinLambda;
    double m_MaxLambda;
  };

  // To create wavelength range for certain pre-defined ranges
  class CWavelengthRangeFactory {
  public:
    CWavelengthRangeFactory();
    std::shared_ptr< const CWavelengthRange > getWavelengthRange( const WavelengthRange t_Range );
  };

}

#endif