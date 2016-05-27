#ifndef MULTILAYERINTERREFLECTANCES_H
#define MULTILAYERINTERREFLECTANCES_H

#include <vector>
#include <memory>
#include <map>

#include "FenestrationCommon.hpp"

namespace LayerOptics {

  class CLayerSingleComponent;

}

namespace MultiPane {

  enum class EnergyFlow { Forward, Backward };

  // Calculates incoming energies to every surface of the layers in IGU.
  class CInterreflectances {
  public:
    CInterreflectances( const double t_Tf, const double t_Rf, const double t_Tb, const double t_Rb );

    // Adding layer to the back or front side of the IGU composition
    void addLayer( const double t_Tf, const double t_Rf, const double t_Tb, const double t_Rb, 
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    // gets value of energy to surface of certain layer
    double getEnergyToSurface( const size_t Index, const FenestrationCommon::Side t_SurfaceSide, 
      const FenestrationCommon::Side t_EnergySide );

    double getLayerAbsorptance( const size_t Index, const FenestrationCommon::Side t_Side );

  private:
    void calculateEnergies();
    void calculateForwardLayers();
    void calculateBackwardLayers();

    std::shared_ptr< std::vector< double > > IEnergy( const FenestrationCommon::Side t_Side,
      const FenestrationCommon::Side t_EnergyFlow );

    std::vector< std::shared_ptr< LayerOptics::CLayerSingleComponent > > m_Layers;

    // Keeping equivalent optical properties while adding layers. This is necessary
    // for calculation of interreflectances
    std::vector< std::shared_ptr< LayerOptics::CLayerSingleComponent > > m_ForwardLayers;
    std::vector< std::shared_ptr< LayerOptics::CLayerSingleComponent > > m_BackwardLayers;

    // Results of interreflectances are calculated for beam incoming from inside and outside
    std::map< std::pair< FenestrationCommon::Side, FenestrationCommon::Side >, 
      std::shared_ptr< std::vector< double > > > m_IEnergy;

    bool m_StateCalculated;

  };

}

#endif