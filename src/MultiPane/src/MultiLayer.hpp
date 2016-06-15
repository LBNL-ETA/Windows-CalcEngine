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

  class CEquivalentLayer;
  class CInterRefSingleComponent;
  
  // Handles equivalent layer properties of multilayer IGU that is made of
  // any type of layer (specular or diffuse)
  class CMultiLayer {
  public:
    CMultiLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
      const double t_Tb_dir_dir, const double t_Rb_dir_dir, 
      const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
      const double t_Tb_dir_dif, const double t_Rb_dir_dif, 
      const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
      const double t_Tb_dif_dif, const double t_Rb_dif_dif );

    CMultiLayer( std::shared_ptr< const LayerOptics::CLayer > t_Layer );

    void addLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
      const double t_Tb_dir_dir, const double t_Rb_dir_dir, 
      const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
      const double t_Tb_dir_dif, const double t_Rb_dir_dif, 
      const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
      const double t_Tb_dif_dif, const double t_Rb_dif_dif,
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    void addLayer( std::shared_ptr< const LayerOptics::CLayer > t_Layer, 
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

  private:
    std::shared_ptr< LayerOptics::CLayer > getLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir,
      const double t_Tb_dir_dir, const double t_Rb_dir_dir,
      const double t_Tf_dir_dif, const double t_Rf_dir_dif,
      const double t_Tb_dir_dif, const double t_Rb_dir_dif,
      const double t_Tf_dif_dif, const double t_Rf_dif_dif,
      const double t_Tb_dif_dif, const double t_Rb_dif_dif );

    void createInterreflectances( std::shared_ptr< const LayerOptics::CLayer > t_Layer );

    std::map< FenestrationCommon::ScatteringSimple, 
      std::shared_ptr< CInterRefSingleComponent > > m_Interref;
    std::map< FenestrationCommon::Side, std::shared_ptr< CEquivalentLayer > > m_EquivalentLayer;

    std::shared_ptr< LayerOptics::CLayer > m_Layer;
  };
}

#endif