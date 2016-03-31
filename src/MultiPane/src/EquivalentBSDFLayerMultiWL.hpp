#ifndef EQUIVALENTBSDFLAYERMULTIWL_H
#define EQUIVALENTBSDFLAYERMULTIWL_H

#include <memory>
#include <vector>

#include "AbsorptancesMultiPaneBSDF.hpp"

namespace FenestrationCommon {

  class CSquareMatrix;
  class CSeries;
  enum class Side;

}

namespace LayerOptics {

  class CBaseBSDFLayerMultiWL;

}

namespace MultiPane {

  class CEquivalentBSDFLayer;

  // Calculates equivalent BSDF matrices for transmittances and reflectances
  class CEquivalentBSDFLayerMultiWL {
  public:
    CEquivalentBSDFLayerMultiWL( std::shared_ptr< std::vector< double > > t_CommonWavelengths, 
      std::shared_ptr< FenestrationCommon::CSeries > t_SolarRadiation, 
      std::shared_ptr< LayerOptics::CBaseBSDFLayerMultiWL > t_Layer );

    void addLayer( std::shared_ptr< LayerOptics::CBaseBSDFLayerMultiWL > t_Layer );

    std::shared_ptr< FenestrationCommon::CSquareMatrix > Tau( const double minLambda, const double maxLambda, 
      FenestrationCommon::Side t_Side );
    std::shared_ptr< FenestrationCommon::CSquareMatrix > Rho( const double minLambda, const double maxLambda, 
      FenestrationCommon::Side t_Side );
    std::shared_ptr< std::vector< double > > Abs( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Side t_Side, const size_t Index );

  private:

    void calculate( const double minLambda, const double maxLambda );

    // Vector of layer results over each wavelength
    std::shared_ptr< std::vector< std::shared_ptr< CEquivalentBSDFLayer > > > m_LayersWL;

    // Matrices for the entire system
    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_TauF;
    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_TauB;
    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_RhoF;
    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_RhoB;

    std::shared_ptr< std::vector< std::shared_ptr< std::vector< double > > > > m_AbsF;
    std::shared_ptr< std::vector< std::shared_ptr< std::vector< double > > > > m_AbsB;

    std::shared_ptr< const FenestrationCommon::CSquareMatrix > m_Lambda;
    std::shared_ptr< FenestrationCommon::CSeries > m_SolarRadiation;

    std::shared_ptr< std::vector< double > > m_CombinedLayerWavelengths;
    bool m_Calculated;

  };

}

#endif