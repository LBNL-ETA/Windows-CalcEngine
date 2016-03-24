#ifndef ABSORPTANCESMULTIPANE_H
#define ABSORPTANCESMULTIPANE_H

#include <memory>
#include <vector>

namespace SpectralAveraging {

  class CSpectralProperties;

}

namespace MultiPane {

  // Calculate absorptances of multiplane layers for simple case (single incident angle)
  class CAbsorptancesMultiPane {
  public:
    CAbsorptancesMultiPane( std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_T, 
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_Rf, 
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_Rb );

    void addLayer( std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_T, 
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_Rf, 
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_Rb );

    std::shared_ptr< SpectralAveraging::CSpectralProperties > Abs( size_t const Index );
    size_t numOfLayers();

  private:
    void calculateState();

    std::shared_ptr< SpectralAveraging::CSpectralProperties > rCoeffs( 
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_T, 
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_Rf,
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_Rb,
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_RCoeffs );

    std::shared_ptr< SpectralAveraging::CSpectralProperties > tCoeffs( 
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_T,
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_Rb,
      std::shared_ptr< const SpectralAveraging::CSpectralProperties > t_RCoeffs );

    std::vector < std::shared_ptr< const SpectralAveraging::CSpectralProperties > > m_T;
    std::vector < std::shared_ptr< const SpectralAveraging::CSpectralProperties > > m_Rf;
    std::vector < std::shared_ptr< const SpectralAveraging::CSpectralProperties > > m_Rb;
    std::vector < std::shared_ptr< SpectralAveraging::CSpectralProperties > > m_Abs;

    std::vector < std::shared_ptr< SpectralAveraging::CSpectralProperties > > m_rCoeffs;
    std::vector < std::shared_ptr< SpectralAveraging::CSpectralProperties > > m_tCoeffs;

    bool m_StateCalculated;
  };
}

#endif