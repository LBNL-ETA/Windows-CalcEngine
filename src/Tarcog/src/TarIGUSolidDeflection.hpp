#ifndef TARIGUSOLIDLAYERDEFLECTION_H
#define TARIGUSOLIDLAYERDEFLECTION_H

#include <memory>

#include "TarIGUSolidLayer.hpp"

namespace Tarcog {

  ////////////////////////////////////////////////////////////////////////////
  ////    CTarIGUSolidLayerDeflection
  ////////////////////////////////////////////////////////////////////////////
  class CTarIGUSolidLayerDeflection : public CTarIGUSolidLayer {
  public:
    CTarIGUSolidLayerDeflection( const CTarIGUSolidLayer& t_SolidLayer );
    CTarIGUSolidLayerDeflection( const CTarIGUSolidLayer& t_SolidLayer,
      const double t_YoungsModulus, const double t_PoisonRatio );

    CTarIGUSolidLayerDeflection( const CTarIGUSolidLayerDeflection& t_Layer );

    double flexuralRigidity() const;

    virtual std::shared_ptr< CBaseTarcogLayer > clone() const;

  protected:
    virtual void calculateConvectionOrConductionFlow();
    double pressureDifference() const;

  private:
    double m_YoungsModulus;
    double m_PoisonRatio;
  };

  ////////////////////////////////////////////////////////////////////////////
  ////    CTarIGUDeflectionTempAndPressure
  ////////////////////////////////////////////////////////////////////////////
  class CTarIGUDeflectionTempAndPressure : public CTarIGUSolidLayerDeflection {
  public:
    CTarIGUDeflectionTempAndPressure( std::shared_ptr< CTarIGUSolidLayerDeflection >& t_SolidLayer,
      const double t_MaxDeflectionCoeff, const double t_MinDeflectionCoeff );

  protected:
    virtual void calculateConvectionOrConductionFlow();

  private:
    double LdMean( const double t_P, const double t_D ) const;
    double LdMax( const double t_P, const double t_D ) const;

    double m_MaxCoeff;
    double m_MeanCoeff;
  };

  ////////////////////////////////////////////////////////////////////////////
  ////    CTarIGUDeflectionMeasuread
  ////////////////////////////////////////////////////////////////////////////
  class CTarIGUDeflectionMeasuread : public CTarIGUSolidLayerDeflection {
  public:
    CTarIGUDeflectionMeasuread( std::shared_ptr< CTarIGUSolidLayerDeflection >& t_Layer, 
      const double t_MeanDeflection, const double t_MaxDeflection );
  };
}

#endif