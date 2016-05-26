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

}

namespace MultiPane {

  class CEquivalentBSDFLayerSingleBand;

  // Calculates equivalent BSDF matrices for transmittances and reflectances
  class CEquivalentBSDFLayer {
  public:
    CEquivalentBSDFLayer( std::shared_ptr< std::vector< double > > t_CommonWavelengths, 
      std::shared_ptr< FenestrationCommon::CSeries > t_SolarRadiation, 
      std::shared_ptr< LayerOptics::CBSDFLayer > t_Layer );

    void addLayer( std::shared_ptr< LayerOptics::CBSDFLayer > t_Layer );

    std::shared_ptr< FenestrationCommon::CSquareMatrix > Tau( const double minLambda, const double maxLambda, 
      FenestrationCommon::Side t_Side );
    std::shared_ptr< FenestrationCommon::CSquareMatrix > Rho( const double minLambda, const double maxLambda, 
      FenestrationCommon::Side t_Side );
    std::shared_ptr< std::vector< double > > Abs( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const size_t Index );

  private:

    void calculate( const double minLambda, const double maxLambda );

    // Vector of layer results over each wavelength
    std::shared_ptr< std::vector< std::shared_ptr< CEquivalentBSDFLayerSingleBand > > > m_LayersWL;

    // Matrices for the entire system
    std::map< FenestrationCommon::Side, std::shared_ptr< FenestrationCommon::CSquareMatrix > > m_Tau;
    std::map< FenestrationCommon::Side, std::shared_ptr< FenestrationCommon::CSquareMatrix > > m_Rho;

    std::map< FenestrationCommon::Side, std::shared_ptr< std::vector< std::shared_ptr< std::vector< double > > > > > m_Abs;

    std::shared_ptr< const FenestrationCommon::CSquareMatrix > m_Lambda;
    std::shared_ptr< FenestrationCommon::CSeries > m_SolarRadiation;

    std::shared_ptr< std::vector< double > > m_CombinedLayerWavelengths;
    bool m_Calculated;

  };

}

#endif