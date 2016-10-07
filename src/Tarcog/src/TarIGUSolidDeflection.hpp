#ifndef TARIGUSOLIDLAYERDEFLECTION_H
#define TARIGUSOLIDLAYERDEFLECTION_H

#include <memory>

#include "TarIGUSolidLayer.hpp"

namespace Tarcog {

  class CTarIGUSolidLayerDeflection : public CTarIGUSolidLayer {
  public:
    CTarIGUSolidLayerDeflection( std::shared_ptr< CTarIGUSolidLayer >& t_SolidLayer );
    CTarIGUSolidLayerDeflection( std::shared_ptr< CTarIGUSolidLayer >& t_SolidLayer,
      const double t_YoungsModulus, const double t_PoisonRatio );

  protected:
    virtual void calculateConvectionConductionState();

  private:
    // std::shared_ptr< CTarIGUSolidLayer > m_SolidLayer;

    double LdMean( const double t_P, const double t_D ) const;
    double LdMax( const double t_P, const double t_D ) const;
    double flexuralRigidity() const;
    double pressureDifference() const;

    double m_YoungsModulus;
    double m_PoisonRatio;

    // deflection coefficients that will be calculated once per layer.
    // No need to calculate them mulitple times.
    void calcCoeffs();
    double m_MaxCoeff;
    double m_MeanCoeff;
  };

}

#endif