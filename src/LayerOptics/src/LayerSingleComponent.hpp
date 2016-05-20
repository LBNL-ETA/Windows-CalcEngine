#ifndef LAYERSINGLECOMPONENT_H
#define LAYERSINGLECOMPONENT_H

#include <memory>

namespace FenestrationCommon {

  enum class Side;
  enum class Property;

}

namespace LayerOptics {

  struct Surface {
    Surface( const double t_T, const double t_R );
    double getProperty( const FenestrationCommon::Property t_Property );
    const double T;
    const double R;
    const double A;
  };

  // class to be used for description of single component of the light. By single component it is assumed that light 
  // will not change state (from beam to diffuse) during propagation through the layer
  class CLayerSingleComponent {
  public:
    CLayerSingleComponent( const double t_Tf, const double t_Tb, const double t_Rf, const double t_Rb );

    double getProperty( const FenestrationCommon::Property t_Property, const FenestrationCommon::Side t_Side ) const;

  private:
    std::shared_ptr< Surface > m_Front;
    std::shared_ptr< Surface > m_Back;
  };

}

#endif