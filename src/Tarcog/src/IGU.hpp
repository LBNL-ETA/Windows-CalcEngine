#ifndef TARIGU_H
#define TARIGU_H

#include <memory>
#include <vector>

namespace Tarcog {

  enum class Environment;
  class CBaseLayer;
  class CBaseIGULayer;
  class CIGUSolidLayer;
  class CIGUGapLayer;
  class CSurface;

  class CIGU {
  public:
    CIGU( const double t_Width, const double t_Height, const double t_Tilt = 90 );
    CIGU( const CIGU& t_IGU );
    ~CIGU();

    void addLayer( const std::shared_ptr< CBaseIGULayer >& t_Layer );

    std::vector< std::shared_ptr< CIGUSolidLayer > > getSolidLayers() const;
    std::vector< std::shared_ptr< CIGUGapLayer > > getGapLayers() const;
    std::vector< std::shared_ptr< CBaseIGULayer > > getLayers() const;

    void setTilt( const double t_Tilt );
    void setWidth( const double t_Width );
    void setHeight( const double t_Height );

    void setSolarRadiation( const double t_SolarRadiation );

    std::shared_ptr< CBaseLayer > getLayer( const Environment t_Environment ) const;

    std::shared_ptr< std::vector< double > > getState();
    void setState( std::shared_ptr< std::vector< double > > t_State );

    std::shared_ptr< std::vector< double > > getTemperatures() const;
    std::shared_ptr< std::vector< double > > getRadiosities() const;
    std::shared_ptr< std::vector< double > > getMaxDeflections() const;
    std::shared_ptr< std::vector< double > > getMeanDeflections() const;

    double getTilt() const;
    double getWidth( ) const;
    double getHeight( ) const;
    double getThickness() const;

    size_t getNumOfLayers() const;

    double getVentilationFlow( const Environment t_Environment ) const;

    void setInitialGuess( const std::shared_ptr< std::vector< double > >& t_Temperatures );

    void setDeflectionProperties( const double t_Tini, const double t_Pini );
    void setDeflectionProperties( const std::vector< double >& t_MeasuredDeflections );

  private:
    // Replces layer in existing construction and keeps correct connections in linked list
    void replaceLayer( const std::shared_ptr< CBaseIGULayer >& t_Original, 
      const std::shared_ptr< CBaseIGULayer >& t_Replacement );

    // Check if layer needs to be decorated with another object
    void checkForLayerUpgrades( const std::shared_ptr< CBaseIGULayer > t_Layer );

    std::vector< std::shared_ptr< CBaseIGULayer > > m_Layers;

    double m_Width; // meters
    double m_Height; // meters
    double m_Tilt; // degrees

    // Routines to calculate deflection coefficients
    double Ldmean() const;
    double Ldmax() const;

  };

}


#endif
