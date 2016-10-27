#ifndef SPECTRALSAMPLEDATA_H
#define SPECTRALSAMPLEDATA_H

#include <vector>
#include <memory>

namespace FenestrationCommon {

  class CSeries;

}

namespace SpectralAveraging {

  enum class SampleData { T, Rf, Rb, AbsF, AbsB };

  // Measured sample data for given wavelengths.
  class CSpectralSampleData {
  public:
    CSpectralSampleData();

    void addRecord( double t_Wavelength, double t_Transmittance, double t_ReflectanceFront, 
      double t_ReflectanceBack );
    std::shared_ptr< FenestrationCommon::CSeries > properties( SampleData t_Property );
    virtual std::shared_ptr< std::vector< double > > getWavelengths();
    void interpolate( const std::vector< double >& t_Wavelengths );

    bool Flipped() const;
    virtual void Filpped( bool const t_Flipped );
  
  protected:
    virtual void calculateProperties();
    void reset();

    std::shared_ptr< FenestrationCommon::CSeries > m_Transmittances;
    std::shared_ptr< FenestrationCommon::CSeries > m_ReflectancesFront;
    std::shared_ptr< FenestrationCommon::CSeries > m_ReflectancesBack;

    // Calculated from sample measurements
    std::shared_ptr< FenestrationCommon::CSeries > m_AbsorptancesFront;
    std::shared_ptr< FenestrationCommon::CSeries > m_AbsorptancesBack;

    bool m_Flipped;
    bool m_absCalculated;
  };

}

#endif