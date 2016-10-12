#ifndef SUPPORTPILLAR_H
#define SUPPORTPILLAR_H

#include "TarIGUGapLayer.hpp"

namespace Tarcog {

  class CSupportPillar : public CTarIGUGapLayer {
  public:
    CSupportPillar( const CTarIGUGapLayer& t_Layer, const double t_Conductivity );
    CSupportPillar( const CSupportPillar& t_Pillar );

  protected:
    virtual void calculateConvectionOrConductionFlow();
    virtual double conductivityOfPillarArray() = 0;
    double m_Conductivity;
  };

  class CCircularPillar : public CSupportPillar {
  public:
    CCircularPillar( const CTarIGUGapLayer& t_Gap, const double t_Conductivity,
      const double t_Spacing, const double t_Radius );
    CCircularPillar( const CCircularPillar& t_Pillar );

  private:
    double conductivityOfPillarArray();
    double m_Spacing;
    double m_Radius;

  };

}

#endif