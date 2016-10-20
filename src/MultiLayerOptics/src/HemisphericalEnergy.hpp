#ifndef HEMISPHERICALENERGY_H
#define HEMISPHERICALENERGY_H

#include <memory>
#include <vector>
#include <map>

namespace FenestrationCommon {

  enum class Side;
  enum class PropertySimple;
  class CSeries;

}

namespace SingleLayerOptics {
  
  class CBSDFLayer;

}

namespace MultiLayerOptics {

  class CEquivalentBSDFLayer;

  enum class Energy { Solar, Sky };

  // Class to handle hemispherical energy distribution from sky and from sun. It will calculate
  // optical properties for both sources and then calculate total energy that has been transmitted
  // reflected or absorbed for given state
  class CHemisphericalEnergy {
  public:
    CHemisphericalEnergy( const std::shared_ptr< std::vector< double > >& t_CommonWavelengths,
      const std::shared_ptr< FenestrationCommon::CSeries >& t_SolarRadiation,
      const std::shared_ptr< std::vector< double > >& t_SolarMatrix,
      const std::shared_ptr< FenestrationCommon::CSeries >& t_SkyRadiation,
      const std::shared_ptr< std::vector< double > >& t_SkyMatrix,
      const std::shared_ptr< SingleLayerOptics::CBSDFLayer >& t_Layer );

    void addLayer( const std::shared_ptr< SingleLayerOptics::CBSDFLayer >& t_Layer );

    std::shared_ptr< std::vector< double > > get( const FenestrationCommon::PropertySimple t_Property,
      const Energy t_Energy );

  private:
    class CSingleEnergy {
    public:
      CSingleEnergy();
      CSingleEnergy( const std::shared_ptr< std::vector< double > >& t_CommonWavelengths,
        const std::shared_ptr< FenestrationCommon::CSeries >& t_SolarRadiation, 
        const std::shared_ptr< std::vector< double > >& t_SolarMatrix,
        const std::shared_ptr< SingleLayerOptics::CBSDFLayer >& t_Layer );

      void addLayer( const std::shared_ptr< SingleLayerOptics::CBSDFLayer >& t_Layer );

      std::shared_ptr< std::vector< double > > getVector( 
        const FenestrationCommon::PropertySimple t_Property );

    private:
      std::shared_ptr< CEquivalentBSDFLayer > m_Layer;
      std::shared_ptr< std::vector< double > > m_Distribution;
    };
    std::map< Energy, CSingleEnergy > m_Energy;
  };

}

#endif