#ifndef EQUIVALENTLAYER_H
#define EQUIVALENTLAYER_H

#include <memory>
#include <map>

#include "FenestrationCommon.hpp"

namespace SingleLayerOptics {

  class CLayer;
  class CScatteringSurface;

}

namespace MultiLayerOptics {

  class CEquivalentLayerSingleComponent;

  struct SimpleResults {
    SimpleResults() : T( 0 ), R( 0 ) { };
    double T;
    double R;
  };
  
  // Calculates Transmittance and reflectance of multi layer IGU with direct and diffuse properties
  class CEquivalentLayer {
  public:
    CEquivalentLayer( const double Tf_dir_dir, const double Rf_dir_dir, 
      const double Tb_dir_dir, const double Rb_dir_dir, 
      const double Tf_dir_dif, const double Rf_dir_dif, 
      const double Tb_dir_dif, const double Rb_dir_dif, 
      const double Tf_dif_dif, const double Rf_dif_dif, 
      const double Tb_dif_dif, const double Rb_dif_dif );

    CEquivalentLayer( const SingleLayerOptics::CLayer& t_Layer );

    void addLayer( const double Tf_dir_dir, const double Rf_dir_dir, 
      const double Tb_dir_dir, const double Rb_dir_dir, 
      const double Tf_dir_dif, const double Rf_dir_dif, 
      const double Tb_dir_dif, const double Rb_dir_dif, 
      const double Tf_dif_dif, const double Rf_dif_dif, 
      const double Tb_dif_dif, const double Rb_dif_dif,
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    void addLayer( const SingleLayerOptics::CLayer& t_Layer, 
      const FenestrationCommon::Side t_Side = FenestrationCommon::Side::Back );

    double getPropertySimple( const FenestrationCommon::PropertySimple t_Property, 
      const FenestrationCommon::Side t_Side,
      const FenestrationCommon::Scattering t_Scattering ) const;

    std::shared_ptr< SingleLayerOptics::CLayer > getLayer() const;

  private:
    void calcEquivalentProperties( const SingleLayerOptics::CLayer& t_First, 
      const SingleLayerOptics::CLayer& t_Second );

    // Find interreflectance value for given scattering
    double getInterreflectance( 
      const SingleLayerOptics::CScatteringSurface& t_First, 
      const SingleLayerOptics::CScatteringSurface& t_Second, 
      const FenestrationCommon::Scattering t_Scattering );

    // Add diffuse and direct components from scattering layer properties
    void addLayerComponents( const SingleLayerOptics::CLayer& t_Layer, const FenestrationCommon::Side t_Side );

    std::shared_ptr< SimpleResults > calcDirectDiffuseTransAndRefl( 
      const SingleLayerOptics::CScatteringSurface& f1, 
      const SingleLayerOptics::CScatteringSurface& b1, 
      const SingleLayerOptics::CScatteringSurface& f2 );

    std::shared_ptr< SingleLayerOptics::CLayer > m_Layer;
    
    // Layers for beam and diffuse components
    std::shared_ptr< CEquivalentLayerSingleComponent > m_DiffuseLayer;
    std::shared_ptr< CEquivalentLayerSingleComponent > m_BeamLayer;

  };

}

#endif