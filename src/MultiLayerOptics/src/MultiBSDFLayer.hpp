#ifndef MULTIBSDFLAYER_H
#define MULTIBSDFLAYER_H

#include <memory>
#include <vector>
#include <map>

namespace FenestrationCommon {

  class CSeries;
  class CSquareMatrix;
  enum class Side;
  enum class PropertySimple;

}

namespace SingleLayerOptics {

  class CBSDFIntegrator;

}

namespace MultiLayerOptics {

  class CEquivalentBSDFLayer;
  
  class CMultiBSDFLayer {
  public:
    CMultiBSDFLayer( const std::shared_ptr< CEquivalentBSDFLayer >& t_Layer, 
      const std::shared_ptr< FenestrationCommon::CSeries >& t_SolarRadiation );

    // Whole matrix results
    std::shared_ptr< FenestrationCommon::CSquareMatrix > getMatrix( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property );

    double DirDir( const double minLambda, const double maxLambda, const FenestrationCommon::Side t_Side,
      const FenestrationCommon::PropertySimple t_Property, const double t_Theta, const double t_Phi );

    // Vector of layer by layer absorptances for each incoming direction
    std::shared_ptr< std::vector< double > > Abs( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const size_t Index );

    // Hemispherical results for every direction
    std::shared_ptr< std::vector< double > > DirHem( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property );

    // Directional hemispherical results for given Theta and Phi direction
    double DirHem( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property,
      const double t_Theta, const double t_Phi );

    double Abs( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const size_t Index, const double t_Theta, const double t_Phi );

    // Diffuse to diffuse properties
    double DiffDiff( const double minLambda, const double maxLambda,
      const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property );

    double AbsDiff( const double minLambda, const double maxLambda, const FenestrationCommon::Side t_Side,
      const size_t t_LayerIndex );

  private:
    void calculate( const double minLambda, const double maxLambda );

    void calcHemisphericalAbs( const FenestrationCommon::Side t_Side );

    std::shared_ptr< CEquivalentBSDFLayer > m_Layer;
    std::shared_ptr< FenestrationCommon::CSeries > m_SolarRadiation;

    std::shared_ptr< SingleLayerOptics::CBSDFIntegrator > m_Results;

    std::map< FenestrationCommon::Side,
      std::shared_ptr< std::vector< std::shared_ptr< std::vector< double > > > > > m_Abs;
    
    // Hemispherical absorptances for every layer
    std::map < FenestrationCommon::Side, std::shared_ptr< std::vector < double > > > m_AbsHem;

    bool m_Calculated;

  };
  
}

#endif