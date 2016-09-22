#ifndef EQUIVALENTBSDFLAYERMULTIWL_H
#define EQUIVALENTBSDFLAYERMULTIWL_H

#include <memory>
#include <vector>
#include <map>

#include "AbsorptancesMultiPaneBSDF.hpp"

namespace FenestrationCommon {

  class CSquareMatrix;
  class CSeries;
  enum class Side;

}

namespace LayerOptics {

  class CBSDFLayer;
  class CBSDFResults;

}

namespace MultiPane {

  class CEquivalentBSDFLayerSingleBand;

  // Calculates equivalent BSDF matrices for transmittances and reflectances
  class CEquivalentBSDFLayer {
  public:
    CEquivalentBSDFLayer( const std::shared_ptr< std::vector< double > >& t_CommonWavelengths, 
      const std::shared_ptr< FenestrationCommon::CSeries >& t_SolarRadiation, 
      const std::shared_ptr< LayerOptics::CBSDFLayer >& t_Layer );

    void addLayer( const std::shared_ptr< LayerOptics::CBSDFLayer >& t_Layer );

    // Whole matrix results
    std::shared_ptr< FenestrationCommon::CSquareMatrix > Tau( const double minLambda, const double maxLambda, 
      FenestrationCommon::Side t_Side );
    std::shared_ptr< FenestrationCommon::CSquareMatrix > Rho( const double minLambda, const double maxLambda, 
      FenestrationCommon::Side t_Side );

    // Vector of layer by layer absorptances for each incoming direction
    std::shared_ptr< std::vector< double > > Abs( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const size_t Index );

    // Hemispherical results
    std::shared_ptr< std::vector< double > > TauHem( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side );
    std::shared_ptr< std::vector< double > > RhoHem( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side );

    // Directional hemispherical results for given Theta and Phi direction
    double TauHem( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const double t_Theta, const double t_Phi );
    double RhoHem( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const double t_Theta, const double t_Phi );
    double Abs( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const size_t Index, const double t_Theta, const double t_Phi );

  private:

    void calculate( const double minLambda, const double maxLambda );

    // Vector of layer results over each wavelength
    std::shared_ptr< std::vector< std::shared_ptr< CEquivalentBSDFLayerSingleBand > > > m_LayersWL;


    std::map< FenestrationCommon::Side, std::shared_ptr< std::vector< std::shared_ptr< std::vector< double > > > > > m_Abs;

    std::shared_ptr< LayerOptics::CBSDFResults > m_Results;

    std::shared_ptr< const FenestrationCommon::CSquareMatrix > m_Lambda;
    std::shared_ptr< FenestrationCommon::CSeries > m_SolarRadiation;

    std::shared_ptr< std::vector< double > > m_CombinedLayerWavelengths;
    bool m_Calculated;

  };

}

#endif