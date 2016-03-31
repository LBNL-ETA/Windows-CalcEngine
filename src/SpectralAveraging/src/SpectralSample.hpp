#ifndef SPECTRALSAMPLE_H
#define SPECTRALSAMPLE_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  enum class Property;
  enum class Side;
  enum class IntegrationType;
  class CSeries;

}

namespace SpectralAveraging {

  class CSpectralSampleData;
  class CAngularSpectralProperties;

  enum class WavelengthSet { Custom, Source, Data };

  // enum class SampleProperty { T, Rf, Rb, Abs };

  // Base class for spectral sample data. Its base setup are spectral properties over certain range. It handles detector and source data.
  // Concrete sample data are handled in inherited classes and tha will depend on type of the sample data
  class CSample {
  public:
    CSample( std::shared_ptr< FenestrationCommon::CSeries > t_SourceData );

    // Assigns detector and wavelengths from other sample. 
    void assignDetectorAndWavelengths( std::shared_ptr< CSample > t_Sample );

    // Gets source data. In case wavelengths are referenced to detector or custom wavelength set, it will perform interpolation
    // according to desired settings.
    std::shared_ptr< FenestrationCommon::CSeries > getSourceData();

    // Setting detector spectral properties for the sample
    void setDetectorData( const std::shared_ptr< FenestrationCommon::CSeries > t_DetectorData );

    // Integrate sample property over the certain spectrum range
    double getProperty( const double minLambda, const double maxLambda, 
      const FenestrationCommon::Property t_Property, const FenestrationCommon::Side t_Side );

    // Spectral properties over the wavelength range
    std::shared_ptr< FenestrationCommon::CSeries > getEnergyProperties( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side );

    // Defining the source of wavelengths to be used with the sample. Wavelengths can be used from measured sample,
    // detector data or can be custom provided.
    void setWavelengths( WavelengthSet t_WavelengthSet, 
      std::shared_ptr< std::vector< double > > t_Wavelenghts = nullptr );

    // For given incoming source and detector data, calculates energy (Transmitted, Reflected or Absorbed) for given
    // spectrum range.
    double getEnergy( const double minLambda, const double maxLambda, const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side );

    size_t getBandSize() const;

  protected:
    virtual void reset();
    void calculateState();
    virtual void calculateProperties() = 0;

    // It can be single or multiple samples. In any case this should be seen as single set of wavelengts
    virtual std::shared_ptr< std::vector< double > > getWavelengthsFromSample() = 0;

    std::shared_ptr< FenestrationCommon::CSeries > m_SourceData;
    std::shared_ptr< FenestrationCommon::CSeries > m_DetectorData;

    std::shared_ptr< std::vector< double > > m_Wavelengths;
    WavelengthSet m_WavelengthSet;

    // Keep energy for current state of the sample. Energy is calculated for each wavelength.
    std::shared_ptr< FenestrationCommon::CSeries > m_IncomingSource;
    std::shared_ptr< FenestrationCommon::CSeries > m_TransmittedSource;
    std::shared_ptr< FenestrationCommon::CSeries > m_ReflectedFrontSource;
    std::shared_ptr< FenestrationCommon::CSeries > m_ReflectedBackSource;
    std::shared_ptr< FenestrationCommon::CSeries > m_AbsorbedFrontSource;
    std::shared_ptr< FenestrationCommon::CSeries > m_AbsorbedBackSource;
    
    FenestrationCommon::IntegrationType m_IntegrationType;

    bool m_StateCalculated;
  };

  class CSpectralSample : public CSample {
  public:
    CSpectralSample( std::shared_ptr< CSpectralSampleData > t_SampleData, 
      std::shared_ptr< FenestrationCommon::CSeries > t_SourceData );

    // Before retreiving measured data from sample, function will do all necessary interpolations to desired wavelengths.
    std::shared_ptr< CSpectralSampleData > getMeasuredData();

    // Returns property at each wavelength
    std::shared_ptr< FenestrationCommon::CSeries > getWavelengthsProperty( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side );

    std::shared_ptr< std::vector< double > > getWavelengthsFromSample();

  protected:
    virtual void calculateProperties();
    
    std::shared_ptr< CSpectralSampleData > m_SampleData;

    std::shared_ptr< FenestrationCommon::CSeries > m_Transmittance;
    std::shared_ptr< FenestrationCommon::CSeries > m_RefFront;
    std::shared_ptr< FenestrationCommon::CSeries > m_RefBack;
    std::shared_ptr< FenestrationCommon::CSeries > m_AbsFront;
    std::shared_ptr< FenestrationCommon::CSeries > m_AbsBack;
  };

}

#endif