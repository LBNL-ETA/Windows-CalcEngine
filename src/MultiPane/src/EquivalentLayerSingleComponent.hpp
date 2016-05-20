#ifndef EQUIVALENTLAYERSINGLECOMPONENT_H
#define EQUIVALENTLAYERSINGLECOMPONENT_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CSeries;
}

namespace MultiPane {

  // Spectral properties of glazing system made up of layers with defined or measured spectral properties.
  // Single component means that ray will propagate through IGU in single state (as perfect beam or prefectly diffuse)
  // Scattering from beam to diffuse component will be implemented in separate class
  class CEquivalentLayerSingleComponent {
  public:
    CEquivalentLayerSingleComponent( std::shared_ptr< FenestrationCommon::CSeries > t_T, 
      std::shared_ptr< FenestrationCommon::CSeries > t_Rf, 
      std::shared_ptr< FenestrationCommon::CSeries > t_Rb );

    void addLayer( std::shared_ptr< FenestrationCommon::CSeries > t_T, 
      std::shared_ptr< FenestrationCommon::CSeries > t_Rf, 
      std::shared_ptr< FenestrationCommon::CSeries > t_Rb );

    std::shared_ptr< FenestrationCommon::CSeries > T() const;
    std::shared_ptr< FenestrationCommon::CSeries > Rf() const;
    std::shared_ptr< FenestrationCommon::CSeries > Rb() const;
    std::shared_ptr< FenestrationCommon::CSeries > AbsF();
    std::shared_ptr< FenestrationCommon::CSeries > AbsB();

  private:
    std::shared_ptr< FenestrationCommon::CSeries > transmittanceTot( 
      FenestrationCommon::CSeries& t_T1, 
      FenestrationCommon::CSeries& t_T2, 
      FenestrationCommon::CSeries& t_Rb1, 
      FenestrationCommon::CSeries& t_Rf2 );

    std::shared_ptr< FenestrationCommon::CSeries > ReflectanceFrontTot( 
      FenestrationCommon::CSeries& t_T1, 
      FenestrationCommon::CSeries& t_Rf1, 
      FenestrationCommon::CSeries& t_Rb1, 
      FenestrationCommon::CSeries& t_Rf2 );

    std::shared_ptr< FenestrationCommon::CSeries > ReflectanceBackTot( 
      FenestrationCommon::CSeries& t_T2, 
      FenestrationCommon::CSeries& t_Rb2, 
      FenestrationCommon::CSeries& t_Rb1, 
      FenestrationCommon::CSeries& t_Rf2 );

    std::shared_ptr< FenestrationCommon::CSeries > m_T;
    std::shared_ptr< FenestrationCommon::CSeries > m_Rf;
    std::shared_ptr< FenestrationCommon::CSeries > m_Rb;
    std::shared_ptr< FenestrationCommon::CSeries > m_AbsF;
    std::shared_ptr< FenestrationCommon::CSeries > m_AbsB;
  
  };

}

#endif