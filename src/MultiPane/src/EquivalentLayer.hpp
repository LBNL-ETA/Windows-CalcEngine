#ifndef EQUIVALENTLAYER_H
#define EQUIVALENTLAYER_H

#include <memory>
#include <vector>

namespace SpectralAveraging {

  class CSpectralProperties;
}

namespace MultiPane {

  // Spectral properties of glazing system made up of layers with defined or measured spectral properties.
  class CEquivalentLayer {
  public:
    CEquivalentLayer( std::shared_ptr< SpectralAveraging::CSpectralProperties > t_T, 
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Rf, 
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Rb );

    void addLayer( std::shared_ptr< SpectralAveraging::CSpectralProperties > t_T, 
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Rf, 
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Rb );

    std::shared_ptr< SpectralAveraging::CSpectralProperties > T() const;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > Rf() const;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > Rb() const;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > AbsF();
    std::shared_ptr< SpectralAveraging::CSpectralProperties > AbsB();

  private:
    std::shared_ptr< SpectralAveraging::CSpectralProperties > transmittanceTot( 
      SpectralAveraging::CSpectralProperties& t_T1, 
      SpectralAveraging::CSpectralProperties& t_T2, 
      SpectralAveraging::CSpectralProperties& t_Rb1, 
      SpectralAveraging::CSpectralProperties& t_Rf2 );

    std::shared_ptr< SpectralAveraging::CSpectralProperties > ReflectanceFrontTot( 
      SpectralAveraging::CSpectralProperties& t_T1, 
      SpectralAveraging::CSpectralProperties& t_Rf1, 
      SpectralAveraging::CSpectralProperties& t_Rb1, 
      SpectralAveraging::CSpectralProperties& t_Rf2 );

    std::shared_ptr< SpectralAveraging::CSpectralProperties > ReflectanceBackTot( 
      SpectralAveraging::CSpectralProperties& t_T2, 
      SpectralAveraging::CSpectralProperties& t_Rb2, 
      SpectralAveraging::CSpectralProperties& t_Rb1, 
      SpectralAveraging::CSpectralProperties& t_Rf2 );

    std::shared_ptr< SpectralAveraging::CSpectralProperties > m_T;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > m_Rf;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > m_Rb;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > m_AbsF;
    std::shared_ptr< SpectralAveraging::CSpectralProperties > m_AbsB;
  
  };

}

#endif