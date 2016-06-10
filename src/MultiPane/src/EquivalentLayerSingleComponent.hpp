#ifndef EQUIVALENTLAYERSINGLECOMPONENT_H
#define EQUIVALENTLAYERSINGLECOMPONENT_H

#include <memory>

#include "FenestrationCommon.hpp"

namespace LayerOptics {

  class CLayerSingleComponent;

}

namespace MultiPane {

  // Class to calculate equivalent layer transmittance and reflectances only.
  class CEquivalentLayerSingleComponent {
  public:
    CEquivalentLayerSingleComponent( const double t_Tf, const double t_Rf, const double t_Tb, const double t_Rb );
    CEquivalentLayerSingleComponent( std::shared_ptr< const LayerOptics::CLayerSingleComponent > t_Layer );

    // Adding layer to front or back side of the IGU composition
    void addLayer( const double t_Tf, const double t_Rf, const double t_Tb, const double t_Rb, 
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    void addLayer( std::shared_ptr< const LayerOptics::CLayerSingleComponent > t_Layer,
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    double getProperty( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side ) const;

    std::shared_ptr< LayerOptics::CLayerSingleComponent > getLayer() const;

  private:
    double interreflectance( std::shared_ptr< const LayerOptics::CLayerSingleComponent > t_Layer1, 
      std::shared_ptr< const LayerOptics::CLayerSingleComponent > t_Layer2 ) const;

    double T( std::shared_ptr< const LayerOptics::CLayerSingleComponent > t_Layer1, 
      std::shared_ptr< const LayerOptics::CLayerSingleComponent > t_Layer2, FenestrationCommon::Side t_Side ) const;

    double R( std::shared_ptr< const LayerOptics::CLayerSingleComponent > t_Layer1, 
      std::shared_ptr< const LayerOptics::CLayerSingleComponent > t_Layer2, FenestrationCommon::Side t_Side ) const;

    std::shared_ptr< LayerOptics::CLayerSingleComponent > m_EquivalentLayer;

  };

}

#endif