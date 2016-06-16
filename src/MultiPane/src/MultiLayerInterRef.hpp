#ifndef MULTILAYERINTERREF_H
#define MULTILAYERINTERREF_H

#include <memory>
#include <vector>
#include <map>

#include "FenestrationCommon.hpp"

namespace FenestrationCommon {

  enum class Scattering;

}

namespace LayerOptics {

  class CLayer;

}

namespace MultiPane {

  typedef std::vector< std::shared_ptr< LayerOptics::CLayer > > CLayer_List;

  class CSurfaceEnergy;
  class CInterRefSingleComponent;
  
  class CInterRef {
  public:
    CInterRef( std::shared_ptr< LayerOptics::CLayer > t_Layer );

    void addLayer( std::shared_ptr< LayerOptics::CLayer > t_Layer, 
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    double getEnergyToSurface( const size_t Index, const FenestrationCommon::Side t_SurfaceSide,
      const FenestrationCommon::Side t_EnergySide, const FenestrationCommon::Scattering t_Scattering );

  private:
    void calculateEnergies();
    void calculateForwardLayers();
    void calculateBackwardLayers();
    
    // Function that calculate total diffuse energy that is leaving surface and that originates from direct beam
    std::shared_ptr< CSurfaceEnergy > calcDiffuseEnergy();

    // Calculate direct to diffuse component at each surface
    std::shared_ptr< CSurfaceEnergy > calcDirectToDiffuseComponent();

    std::vector< std::shared_ptr< LayerOptics::CLayer > > m_Layers;

    std::map< FenestrationCommon::Side, std::shared_ptr< CLayer_List > > m_StackedLayers;
    // std::vector< std::shared_ptr< LayerOptics::CLayer > > m_ForwardLayers;
    // std::vector< std::shared_ptr< LayerOptics::CLayer > > m_BackwardLayers;

    // for calculation of pure components (direct and diffuse)
    std::shared_ptr< CInterRefSingleComponent > m_DirectComponent;
    std::shared_ptr< CInterRefSingleComponent > m_DiffuseComponent;

    std::map< FenestrationCommon::Scattering, std::shared_ptr< CSurfaceEnergy > > m_Energy;

    bool m_StateCalculated;
  };

}

#endif