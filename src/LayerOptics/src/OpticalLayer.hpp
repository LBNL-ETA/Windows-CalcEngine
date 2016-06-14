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

  // Handles general case layer when properties can be direct, diffuse or combination between these two.
  class CLayer {
  public:
    CLayer( std::shared_ptr< CScatteringSurface > t_Front, std::shared_ptr< CScatteringSurface > t_Back );

    std::shared_ptr< CScatteringSurface > getSurface( const FenestrationCommon::Side t_Side ) const;

    double getProperty( const FenestrationCommon::PropertySimple t_Property, 
      const FenestrationCommon::Side t_Side, const FenestrationCommon::Scattering t_Scattering );

  private:
    std::map< FenestrationCommon::Side, std::shared_ptr< CScatteringSurface > > m_Surface;

  };

}

#endif