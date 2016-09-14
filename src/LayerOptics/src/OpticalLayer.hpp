#ifndef OPTICALLAYER_H
#define OPTICALLAYER_H

#include <memory>
#include <map>

namespace FenestrationCommon {

  enum class Side;
  enum class PropertySimple;
  enum class Scattering;
  enum class ScatteringSimple;

}

namespace LayerOptics {

  class CScatteringSurface;
  class CLayerSingleComponent;

  // Handles general case layer when properties can be direct, diffuse or combination between these two.
  class CLayer {
  public:
    CLayer( const std::shared_ptr< CScatteringSurface >& t_Front, 
      const std::shared_ptr< CScatteringSurface >& t_Back );
    CLayer( const std::shared_ptr< const CLayer >& t_Layer );
    CLayer(
      const double Tf_dir_dir, const double Rf_dir_dir,
      const double Tb_dir_dir, const double Rb_dir_dir,
      const double Tf_dir_dif, const double Rf_dir_dif,
      const double Tb_dir_dif, const double Rb_dir_dif,
      const double Tf_dif_dif, const double Rf_dif_dif,
      const double Tb_dif_dif, const double Rb_dif_dif );

    std::shared_ptr< CScatteringSurface > getSurface( const FenestrationCommon::Side t_Side ) const;

    double getPropertySimple( const FenestrationCommon::PropertySimple t_Property, 
      const FenestrationCommon::Side t_Side, const FenestrationCommon::Scattering t_Scattering ) const;

    void setPropertySimple( const FenestrationCommon::PropertySimple t_Property,
      const FenestrationCommon::Side t_Side, const FenestrationCommon::Scattering t_Scattering,
      const double value ) const;

    double getAbsorptance( const FenestrationCommon::Side t_Side, 
      const FenestrationCommon::ScatteringSimple t_Scattering ) const;

    std::shared_ptr< CLayerSingleComponent > getLayer( const FenestrationCommon::Scattering t_Scattering ) const;

  private:
    std::map< FenestrationCommon::Side, std::shared_ptr< CScatteringSurface > > m_Surface;

  };

}

#endif