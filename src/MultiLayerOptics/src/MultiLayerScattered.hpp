#ifndef MULTILAYER_H
#define MULTILAYER_H

#include<memory>
#include<map>

#include"WCECommon.hpp"

namespace SingleLayerOptics {

  class CLayerSingleComponent;
  class CScatteringLayer;

}

namespace MultiLayerOptics {

  class CInterRef;
  class CEquivalentScatteringLayer;
  
  // Handles equivalent layer properties of multilayer IGU that is made of
  // any type of layer (specular or diffuse)
  class CMultiLayerScattered {
  public:
    CMultiLayerScattered( 
      const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
      const double t_Tb_dir_dir, const double t_Rb_dir_dir, 
      const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
      const double t_Tb_dir_dif, const double t_Rb_dir_dif, 
      const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
      const double t_Tb_dif_dif, const double t_Rb_dif_dif );

    CMultiLayerScattered( const std::shared_ptr< const SingleLayerOptics::CScatteringLayer >& t_Layer );

    void addLayer( 
      const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
      const double t_Tb_dir_dir, const double t_Rb_dir_dir, 
      const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
      const double t_Tb_dir_dif, const double t_Rb_dir_dif, 
      const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
      const double t_Tb_dif_dif, const double t_Rb_dif_dif,
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    void addLayer( const std::shared_ptr< SingleLayerOptics::CScatteringLayer >& t_Layer, 
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    double getPropertySimple( const FenestrationCommon::PropertySimple t_Property,
      const FenestrationCommon::Side t_Side,
      const FenestrationCommon::Scattering t_Scattering ) const;

    double getAbsorptanceLayer( const size_t Index, FenestrationCommon::Side t_Side,
      FenestrationCommon::ScatteringSimple t_Scattering );

    double getAbsorptance( FenestrationCommon::Side t_Side, FenestrationCommon::ScatteringSimple t_Scattering );

  private:
    void initialize( const std::shared_ptr< const SingleLayerOptics::CScatteringLayer >& t_Layer );

    std::shared_ptr< CInterRef > m_InterRef;
    std::shared_ptr< CEquivalentScatteringLayer > m_Layer;
  };
}

#endif