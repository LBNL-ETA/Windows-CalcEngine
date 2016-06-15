#ifndef MULTILAYERINTERREF_H
#define MULTILAYERINTERREF_H

#include <memory>
#include <vector>

#include "FenestrationCommon.hpp"

namespace LayerOptics {

  class CLayer;

}

namespace MultiPane {
  
  class CInterRef {
  public:
    CInterRef( std::shared_ptr< LayerOptics::CLayer > t_Layer );

    void addLayer( std::shared_ptr< LayerOptics::CLayer > t_Layer, 
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    double getEnergyToSurface( const size_t Index, const FenestrationCommon::Side t_SurfaceSide,
      const FenestrationCommon::Side t_EnergySide, const FenestrationCommon::Scattering t_Scattering );

  private:
    std::vector< std::shared_ptr< LayerOptics::CLayer > > m_Layers;
    bool m_StateCalculated;
  };

}

#endif