#ifndef MULTIPANESPECULAR_H
#define MULTIPANESPECULAR_H

#include <memory>
#include <vector>

#include "IntegratorStrategy.hpp"

namespace FenestrationCommon {

  enum class Side;
  enum class Property;
  class CSeries;

}

namespace LayerOptics {

  class CSpecularCell;

}

namespace MultiPane {

  class CEquivalentLayerSingleComponentMW;
  class CAbsorptancesMultiPane;

  class CEquivalentLayerSingleComponentMWAngle {
  public:
    CEquivalentLayerSingleComponentMWAngle( const std::shared_ptr< CEquivalentLayerSingleComponentMW >& t_Layer,
      const std::shared_ptr< CAbsorptancesMultiPane >& t_Abs, const double t_Angle );
    double angle() const;
    std::shared_ptr< CEquivalentLayerSingleComponentMW > layer() const;
    std::shared_ptr< FenestrationCommon::CSeries > getProperties( 
      const FenestrationCommon::Side t_Side, const FenestrationCommon::Property t_Property );
    std::shared_ptr< FenestrationCommon::CSeries > Abs( size_t const Index );
  private:
    std::shared_ptr< CEquivalentLayerSingleComponentMW > m_Layer;
    std::shared_ptr< CAbsorptancesMultiPane > m_Abs;
    double m_Angle;
  };

  // Handles equivalent properties of multipane glass consists only of specular layers
  class CMultiPaneSpecular {
  public:
    CMultiPaneSpecular( const std::shared_ptr< std::vector< double > >& t_CommonWavelength,
      const std::shared_ptr< FenestrationCommon::CSeries >& t_SolarRadiation,
      const std::shared_ptr< LayerOptics::CSpecularCell >& t_Layer );

    void addLayer( const std::shared_ptr< LayerOptics::CSpecularCell >& t_Layer );

    double getProperty( const FenestrationCommon::Side t_Side, const FenestrationCommon::Property t_Property, 
      const double t_Angle, const double minLambda, const double maxLambda, 
      const FenestrationCommon::IntegrationType t_IntegrationType = FenestrationCommon::IntegrationType::Trapezoidal );

    double getHemisphericalProperty( const FenestrationCommon::Side t_Side, 
      const FenestrationCommon::Property t_Property, 
      const std::shared_ptr< const std::vector< double > >& t_Angles,
      const double minLambda, const double maxLambda, 
      const FenestrationCommon::IntegrationType t_IntegrationType = FenestrationCommon::IntegrationType::Trapezoidal );

    // Absorptances of each layer based on angle of incidence
    double Abs( size_t const Index, const double t_Angle, const double minLambda, const double maxLambda,
      const FenestrationCommon::IntegrationType t_IntegrationType = FenestrationCommon::IntegrationType::Trapezoidal );

    // Hemispherical absorptances of each layer. Integration is performed over t_Angles.
    double AbsHemispherical( size_t const Index, const std::shared_ptr< const std::vector< double > >& t_Angles, 
      const double minLambda, const double maxLambda,
      const FenestrationCommon::IntegrationType t_IntegrationType = FenestrationCommon::IntegrationType::Trapezoidal );

  private:
    // Get correct angular object out of array and if object does not exists, then it just
    // creates new one and stores it into array
    std::shared_ptr< CEquivalentLayerSingleComponentMWAngle > getAngular( const double t_Angle );
    // creates equivalent layer properties for certain angle
    std::shared_ptr< CEquivalentLayerSingleComponentMWAngle > createNewAngular( const double t_Angle );

    // Contains all specular layers (cells) that are added to the model. This way program will
    // be able to recalculate equivalent properties for any angle
    std::vector< std::shared_ptr< LayerOptics::CSpecularCell > > m_Layers;

    std::shared_ptr< std::vector< double > > m_CommonWavelengths;
    std::shared_ptr< FenestrationCommon::CSeries > m_SolarRadiation;

    // Results for angle-properties pair. If same angle is required twice, then model will not
    // calculate it twice. First it will search for results here and if results are not
    // available, then it will perform calculation for given angle
    std::vector< std::shared_ptr< CEquivalentLayerSingleComponentMWAngle > > m_EquivalentAngle;
  };

}


#endif