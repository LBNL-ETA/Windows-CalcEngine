#ifndef ABSORPTANCESMULTIPANE_H
#define ABSORPTANCESMULTIPANE_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CSeries;

}

namespace MultiPane {

  // Calculate absorptances of multiplane layers for simple case (single incident angle)
  class CAbsorptancesMultiPane {
  public:
    CAbsorptancesMultiPane( std::shared_ptr< const FenestrationCommon::CSeries > t_T, 
      std::shared_ptr< const FenestrationCommon::CSeries > t_Rf, 
      std::shared_ptr< const FenestrationCommon::CSeries > t_Rb );

    void addLayer( std::shared_ptr< const FenestrationCommon::CSeries > t_T, 
      std::shared_ptr< const FenestrationCommon::CSeries > t_Rf, 
      std::shared_ptr< const FenestrationCommon::CSeries > t_Rb );

    std::shared_ptr< FenestrationCommon::CSeries > Abs( size_t const Index );
    size_t numOfLayers();

  private:
    void calculateState();

    std::shared_ptr< FenestrationCommon::CSeries > rCoeffs( 
      std::shared_ptr< const FenestrationCommon::CSeries > t_T, 
      std::shared_ptr< const FenestrationCommon::CSeries > t_Rf,
      std::shared_ptr< const FenestrationCommon::CSeries > t_Rb,
      std::shared_ptr< const FenestrationCommon::CSeries > t_RCoeffs );

    std::shared_ptr< FenestrationCommon::CSeries > tCoeffs( 
      std::shared_ptr< const FenestrationCommon::CSeries > t_T,
      std::shared_ptr< const FenestrationCommon::CSeries > t_Rb,
      std::shared_ptr< const FenestrationCommon::CSeries > t_RCoeffs );

    std::vector < std::shared_ptr< const FenestrationCommon::CSeries > > m_T;
    std::vector < std::shared_ptr< const FenestrationCommon::CSeries > > m_Rf;
    std::vector < std::shared_ptr< const FenestrationCommon::CSeries > > m_Rb;
    std::vector < std::shared_ptr< FenestrationCommon::CSeries > > m_Abs;

    std::vector < std::shared_ptr< FenestrationCommon::CSeries > > m_rCoeffs;
    std::vector < std::shared_ptr< FenestrationCommon::CSeries > > m_tCoeffs;

    bool m_StateCalculated;
  };
}

#endif