#ifndef OPTICALLAYER_H
#define OPTICALLAYER_H

#include <memory>
#include <map>

namespace FenestrationCommon {

  enum class Side;
  enum class PropertySimple;
  enum class Scattering;

}

namespace LayerOptics {

  class CScatteringSurface;
  class CLayerSingleComponent;

  // Handles general case layer when properties can be direct, diffuse or combination between these two.
  class CLayer {
  public:
    CLayer( std::shared_ptr< CScatteringSurface > t_Front, std::shared_ptr< CScatteringSurface > t_Back );
    CLayer( std::shared_ptr< const CLayer > t_Layer );

    std::shared_ptr< CScatteringSurface > getSurface( const FenestrationCommon::Side t_Side ) const;

    double getProperty( const FenestrationCommon::PropertySimple t_Property, 
      const FenestrationCommon::Side t_Side, const FenestrationCommon::Scattering t_Scattering ) const;

    std::shared_ptr< CLayerSingleComponent > getLayer( const FenestrationCommon::Scattering t_Scattering ) const;

  private:
    std::map< FenestrationCommon::Side, std::shared_ptr< CScatteringSurface > > m_Surface;

  };

}

#endif