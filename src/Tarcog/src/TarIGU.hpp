#ifndef TARIGU_H
#define TARIGU_H

#include <memory>
#include <vector>

namespace Tarcog {

  class CBaseTarcogLayer;
  class CBaseIGUTarcogLayer;
  class CTarIGUSolidLayer;
  class CTarSurface;

  class CTarIGU {
  public:
    CTarIGU( double t_Width, double t_Height, double t_Tilt = 90);
    ~CTarIGU();

    void addLayer( std::shared_ptr< CBaseIGUTarcogLayer > t_Layer );

    std::vector< std::shared_ptr< CTarIGUSolidLayer > > getSolidLayers() const;
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

    int getNumOfLayers() const;

    // Returns interior ventilation flow
    double getInteriorVentilationFlow() const;

    void setInitialGuess( const std::shared_ptr< std::vector< double > >& t_Temperatures );

  private:
    std::vector< std::shared_ptr< CTarIGUSolidLayer > > m_SolidLayers;
    std::vector< std::shared_ptr< CBaseIGUTarcogLayer > > m_Layers;

    double m_Width; // meters
    double m_Height; // meters
    double m_Tilt; // degrees
  };

}


#endif