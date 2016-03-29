#ifndef MULTIPANESPECULAR_H
#define MULTIPANESPECULAR_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  enum class Side;
  enum class Property;

}

namespace SpectralAveraging {

  class CSpectralProperties;

}

namespace LayerOptics {

  class CSpecularCell;

}

namespace MultiPane {

  class CEquivalentLayer;
  class CAbsorptancesMultiPane;

  class CEquivalentLayerAngle {
  public:
    CEquivalentLayerAngle( std::shared_ptr< CEquivalentLayer > t_Layer,
      std::shared_ptr< CAbsorptancesMultiPane > t_Abs, const double t_Angle );
    double angle() const;
    std::shared_ptr< CEquivalentLayer > layer() const;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > getProperties( 
      const FenestrationCommon::Side t_Side, const FenestrationCommon::Property t_Property );
    std::shared_ptr< SpectralAveraging::CSpectralProperties > Abs( size_t const Index );
  private:
    std::shared_ptr< CEquivalentLayer > m_Layer;
    std::shared_ptr< CAbsorptancesMultiPane > m_Abs;
    double m_Angle;
  };

  // Handles equivalent properties of multipane glass consists only of specular layers
  class CMultiPaneSpecular {
  public:
    CMultiPaneSpecular( std::shared_ptr< std::vector< double > > t_CommonWavelength,
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_SolarRadiation,
      std::shared_ptr< LayerOptics::CSpecularCell > t_Layer );

    void addLayer( std::shared_ptr< LayerOptics::CSpecularCell > t_Layer );

    double getProperty( const FenestrationCommon::Side t_Side, const FenestrationCommon::Property t_Property, 
      const double t_Angle, const double minLambda, const double maxLambda );

    double Abs( size_t const Index, const double t_Angle, const double minLambda, const double maxLambda );

  private:
    // Get correct angular object out of array and if object does not exists, then it just
    // creates new one and stores it into array
    std::shared_ptr< CEquivalentLayerAngle > getAngular( const double t_Angle );
    // creates equivalent layer properties for certain angle
    std::shared_ptr< CEquivalentLayerAngle > createNewAngular( const double t_Angle );

    // Contains all specular layers (cells) that are added to the model. This way program will
    // be able to recalculate equivalent properties for any angle
    std::vector< std::shared_ptr< LayerOptics::CSpecularCell > > m_Layers;

    std::shared_ptr< std::vector< double > > m_CommonWavelengths;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > m_SolarRadiation;

    // Results for angle-properties pair. If same angle is required twice, then model will not
    // calculate it twice. First it will search for results here and if results are not
    // available, then it will perform calculation for given angle
    std::vector< std::shared_ptr< CEquivalentLayerAngle > > m_EquivalentAngle;
  };

}


#endif