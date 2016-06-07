#ifndef MULTILAYER_H
#define MULTILAYER_H

#include<memory>

#include"FenestrationCommon.hpp"

namespace MultiPane {

  class CEquivalentLayerSingleComponent;
  class CInterreflectancesSingleComponent;
  
  // Handles equivalent layer properties of multilayer IGU that is made of
  // any type of layer (specular or diffuse)
  class CMultiLayer {
  public:
    CMultiLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir, const double t_Tb_dir_dir, const double t_Rb_dir_dir, 
      const double t_Tf_dir_dif, const double t_Rf_dir_dif, const double t_Tb_dir_dif, const double t_Rb_dir_dif, 
      const double t_Tf_dif_dif, const double t_Rf_dif_dif, const double t_Tb_dif_dif, const double t_Rb_dif_dif );

    void addLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir, const double t_Tb_dir_dir, const double t_Rb_dir_dir, 
      const double t_Tf_dir_dif, const double t_Rf_dir_dif, const double t_Tb_dir_dif, const double t_Rb_dir_dif, 
      const double t_Tf_dif_dif, const double t_Rf_dif_dif, const double t_Tb_dif_dif, const double t_Rb_dif_dif,
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

  private:
    // Interreflectances from direct component only
    std::shared_ptr< CInterreflectancesSingleComponent > m_InterDirect;

    // Interreflectances from diffuse component only
    std::shared_ptr< CInterreflectancesSingleComponent > m_InterDiffuse;

    // Equivalent layer from direct component only
    std::shared_ptr< CEquivalentLayerSingleComponent > m_EquivalentDirect;

    // Equivalent layer from diffuse component only
    std::shared_ptr< CEquivalentLayerSingleComponent > m_EquivalentDiffuse;
  };
}

#endif