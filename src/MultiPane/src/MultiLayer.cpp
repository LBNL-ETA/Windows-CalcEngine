#include"MultiLayer.hpp"
#include "MultiLayerInterreflectances.hpp"
#include "EquivalentLayerSingleComponent.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace MultiPane {
  
  CMultiLayer::CMultiLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
    const double t_Tb_dif_dif, const double t_Rb_dif_dif ) {
    m_Interreflectances[ ScatteringSimple::Direct ] = make_shared< CInterreflectancesSingleComponent >
      ( t_Tf_dir_dir, t_Rf_dir_dir, t_Tb_dir_dir, t_Rb_dir_dir );
    m_Interreflectances[ ScatteringSimple::Diffuse ] = make_shared< CInterreflectancesSingleComponent >
      ( t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif );
    m_EquivalentLayer[ ScatteringSimple::Direct ] = make_shared< CEquivalentLayerSingleComponent >
      ( t_Tf_dir_dir, t_Rf_dir_dir, t_Tb_dir_dir, t_Rb_dir_dir );
    m_EquivalentLayer[ ScatteringSimple::Diffuse ] = make_shared< CEquivalentLayerSingleComponent >
      ( t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif );
  }

  void CMultiLayer::addLayer( const double t_Tf_dir_dir, const double t_Rf_dir_dir, 
    const double t_Tb_dir_dir, const double t_Rb_dir_dir,
    const double t_Tf_dir_dif, const double t_Rf_dir_dif, 
    const double t_Tb_dir_dif, const double t_Rb_dir_dif,
    const double t_Tf_dif_dif, const double t_Rf_dif_dif, 
    const double t_Tb_dif_dif, const double t_Rb_dif_dif,
    const Side t_Side ) {
    m_Interreflectances[ ScatteringSimple::Direct ]->
      addLayer( t_Tf_dir_dir, t_Rf_dir_dir, t_Tb_dir_dir, t_Rb_dir_dir, t_Side );
    m_Interreflectances[ ScatteringSimple::Diffuse ]->
      addLayer( t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif, t_Side );
    m_EquivalentLayer[ ScatteringSimple::Direct ]->
      addLayer( t_Tf_dir_dir, t_Rf_dir_dir, t_Tb_dir_dir, t_Rb_dir_dir, t_Side );
    m_EquivalentLayer[ ScatteringSimple::Diffuse ]->
      addLayer( t_Tf_dif_dif, t_Rf_dif_dif, t_Tb_dif_dif, t_Rb_dif_dif, t_Side );
  }

}