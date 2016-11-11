#ifndef TARIGUSOLIDLAYERDEFLECTION_H
#define TARIGUSOLIDLAYERDEFLECTION_H

#include <memory>

#include "IGUSolidLayer.hpp"

namespace Tarcog {

  ////////////////////////////////////////////////////////////////////////////
  ////    CIGUSolidLayerDeflection
  ////////////////////////////////////////////////////////////////////////////
  class CIGUSolidLayerDeflection : public CIGUSolidLayer {
  public:
    CIGUSolidLayerDeflection( const CIGUSolidLayer& t_SolidLayer );
    CIGUSolidLayerDeflection( const CIGUSolidLayer& t_SolidLayer,
      const double t_YoungsModulus, const double t_PoisonRatio );

    CIGUSolidLayerDeflection( const CIGUSolidLayerDeflection& t_Layer );

    double flexuralRigidity() const;

    virtual std::shared_ptr< CBaseLayer > clone() const;

  protected:
    virtual void calculateConvectionOrConductionFlow();
    double pressureDifference() const;

  private:
    double m_YoungsModulus;
    double m_PoisonRatio;
  };

  ////////////////////////////////////////////////////////////////////////////
  ////    CIGUDeflectionTempAndPressure
  ////////////////////////////////////////////////////////////////////////////
  class CIGUDeflectionTempAndPressure : public CIGUSolidLayerDeflection {
  public:
    CIGUDeflectionTempAndPressure( std::shared_ptr< CIGUSolidLayerDeflection >& t_SolidLayer,
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
  ////    CIGUDeflectionMeasuread
  ////////////////////////////////////////////////////////////////////////////
  class CIGUDeflectionMeasuread : public CIGUSolidLayerDeflection {
  public:
    CIGUDeflectionMeasuread( std::shared_ptr< CIGUSolidLayerDeflection >& t_Layer, 
      const double t_MeanDeflection, const double t_MaxDeflection );
  };
}

#endif