#ifndef OPTICALLAYER_H
#define OPTICALLAYER_H

#include <memory>
#include <map>

namespace FenestrationCommon {

  enum class Side;

}

namespace LayerOptics {

  class CScatteringSurface;

  // Handles general case layer when properties can be direct, diffuse or combination between these two.
  class CLayer {
  public:
    CLayer( std::shared_ptr< CScatteringSurface > t_Front, std::shared_ptr< CScatteringSurface > t_Back );

    std::shared_ptr< CScatteringSurface > getSurface( const FenestrationCommon::Side t_Side ) const;

  private:
    std::map< FenestrationCommon::Side, std::shared_ptr< CScatteringSurface > > m_Surface;

  };

}

#endif