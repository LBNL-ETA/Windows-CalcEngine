#ifndef TARIGU_H
#define TARIGU_H

#include <memory>
#include <vector>

namespace Tarcog {

  class CBaseTarcogLayer;
  class CBaseIGUTarcogLayer;
  class CTarIGUSolidLayer;
  class CTarIGUGapLayer;
  class CTarSurface;

  class CTarIGU {
  public:
    CTarIGU( double t_Width, double t_Height, double t_Tilt = 90 );
    ~CTarIGU();

    void addLayer( std::shared_ptr< CBaseIGUTarcogLayer > t_Layer );

    std::vector< std::shared_ptr< CTarIGUSolidLayer > > getSolidLayers();
    std::vector< std::shared_ptr< CTarIGUGapLayer > > getGapLayers();
    std::vector< std::shared_ptr< CBaseIGUTarcogLayer > > getLayers() const;

    void setTilt( double const t_Tilt );
    void setWidth( double const t_Width );
    void setHeight( double const t_Height );

    void setSolarRadiation( double const t_SolarRadiation );

    std::shared_ptr< CBaseTarcogLayer > getFirstLayer() const;
    std::shared_ptr< CBaseTarcogLayer > getLastLayer() const;

    std::shared_ptr< std::vector< double > > getState();
    void setState( std::shared_ptr< std::vector< double > > t_State );

    double getTilt() const;
    double getWidth( ) const;
    double getHeight( ) const;
    double getThickness() const;

    size_t getNumOfLayers() const;

    double getInteriorVentilationFlow() const;

    void setInitialGuess( const std::shared_ptr< std::vector< double > >& t_Temperatures );

    void setDeflectionProperties( const double t_Tini, const double t_Pini );
    void setDeflectionProperties( const std::vector< double >& t_MeasuredDeflections );

  private:
    // Replces layer in existing construction and keeps correct connections in linked list
    void replaceLayer( std::shared_ptr< CBaseIGUTarcogLayer > t_Original, 
      std::shared_ptr< CBaseIGUTarcogLayer > t_Replacement );

    std::vector< std::shared_ptr< CBaseIGUTarcogLayer > > m_Layers;

    double m_Width; // meters
    double m_Height; // meters
    double m_Tilt; // degrees

    // Routines to calculate deflection coefficients
    double Ldmean() const;
    double Ldmax() const;

  };

}


#endif