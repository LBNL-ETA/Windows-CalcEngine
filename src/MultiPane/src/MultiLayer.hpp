#ifndef MULTILAYER_H
#define MULTILAYER_H

#include<memory>
#include<map>

#include"FenestrationCommon.hpp"

namespace LayerOptics {

  class CLayerSingleComponent;
  class CLayer;

}

namespace MultiPane {

  class CInterRef;
  class CEquivalentLayer;
  
  // Handles equivalent layer properties of multilayer IGU that is made of
  // any type of layer (specular or diffuse)
  class CMultiLayer {
  public:
    CMultiLayer( 
      const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
      const double t_Tb_dir_dir, const double t_Rb_dir_dir, 
      const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
      const double t_Tb_dir_dif, const double t_Rb_dir_dif, 
      const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
      const double t_Tb_dif_dif, const double t_Rb_dif_dif );

    CMultiLayer( std::shared_ptr< const LayerOptics::CLayer > t_Layer );

    void addLayer( 
      const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
      const double t_Tb_dir_dir, const double t_Rb_dir_dir, 
      const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
      const double t_Tb_dir_dif, const double t_Rb_dir_dif, 
      const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
      const double t_Tb_dif_dif, const double t_Rb_dif_dif,
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    void addLayer( std::shared_ptr< const LayerOptics::CLayer > t_Layer, 
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    double getPropertySimple( const FenestrationCommon::PropertySimple t_Property,
      const FenestrationCommon::Side t_Side,
      const FenestrationCommon::Scattering t_Scattering ) const;

    double getAbsorptanceLayer( const size_t Index, FenestrationCommon::Side t_Side,
      FenestrationCommon::ScatteringSimple t_Scattering );

    double getAbsorptance( FenestrationCommon::Side t_Side, FenestrationCommon::ScatteringSimple t_Scattering );

  private:
    void initialize( std::shared_ptr< const LayerOptics::CLayer > t_Layer );

    std::shared_ptr< CInterRef > m_InterRef;
    std::shared_ptr< CEquivalentLayer > m_Layer;
  };
}

#endif