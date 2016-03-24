#ifndef TARCOGBASESHADE_H
#define TARCOGBASESHADE_H

#include <memory>

#include "TarIGUSolidLayer.hpp"

namespace Gasses{
  class CGas;
}

namespace Tarcog {

  class CTarSurface;
  class CTarIGUGapLayer;
  class CTarEnvironment;

  class CShadeOpenings {
  public:
    CShadeOpenings( double const t_Atop, double const t_Abot, double const t_Aleft, 
      double const t_Aright, double const t_Afront );

    CShadeOpenings();

    double Aeq_bot();
    double Aeq_top();

  private:
    void initialize();
    double openingMultiplier();

    double m_Atop;
    double m_Abot;
    double m_Aleft;
    double m_Aright;
    double m_Afront;
  };

  class CTarIGUShadeLayer : public CTarIGUSolidLayer {
  public:
    CTarIGUShadeLayer( double t_Thickness, double t_Conductivity, std::shared_ptr< CShadeOpenings > t_ShadeOpenings,
      std::shared_ptr< CTarSurface > t_FrontSurface = nullptr, std::shared_ptr< CTarSurface > t_BackSurface = nullptr);

    CTarIGUShadeLayer( double t_Thickness, double t_Conductivity );

  private:
    void calculateConvectionConductionState();

    void calcInBetweenShadeFlow( std::shared_ptr< CTarIGUGapLayer > t_Gap1, std::shared_ptr< CTarIGUGapLayer > t_Gap2 );
    void calcEdgeShadeFlow( std::shared_ptr< CTarEnvironment > t_Environment, std::shared_ptr< CTarIGUGapLayer > t_Gap );

    std::shared_ptr< CShadeOpenings > m_ShadeOpenings;
  };

}


#endif