#ifndef OPTICALSURFACE_H
#define OPTICALSURFACE_H

#include <map>

namespace FenestrationCommon {

  enum class Property;
  enum class PropertySimple;

}

namespace LayerOptics {

  enum class Scattering { DirectDirect, DirectDiffuse, DiffuseDiffuse };
  enum class ScatteringSimple { Direct, Diffuse };
  
  class CSurface {
  public:
    CSurface( const double t_T, const double t_R );
    double getProperty( const FenestrationCommon::Property t_Property );

  private:
    std::map< FenestrationCommon::Property, double > m_Property;

  };

  class CScatteringSurface {
  public:
    CScatteringSurface( const double T_dir_dir, const double R_dir_dir, 
      const double T_dir_dif, const double R_dir_dif, const double T_dif_dif, const double R_dif_dif );

    // Simple property means only transmittance and reflectance
    double getPropertySimple( const FenestrationCommon::PropertySimple t_Property, const Scattering t_Scattering ) const;

    // In this case absroptance is different property from transmittance and reflectance
    double getAbsorptance( const ScatteringSimple t_Scattering ) const;

  private:
    std::map< std::pair< FenestrationCommon::PropertySimple, Scattering >, double > m_PropertySimple;
    std::map< ScatteringSimple, double > m_Absorptance;

  };

}


#endif