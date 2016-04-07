#ifndef MATERIALDESCRIPTION_H
#define MATERIALDESCRIPTION_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  enum class Side;
  enum class Property;
  enum class SpecularMaterialType;
  class CSeries;
  // enum class WavelengthRange;

}

namespace SpectralAveraging {

  // enum class SampleProperty;
  class CSpectralSample;
  class CAngularSpectralSample;

}

namespace LayerOptics {

  struct RMaterialProperties {
  public:
    RMaterialProperties( const double aTf, const double aTb, const double aRf, const double aRb );
    double Tf;
    double Tb;
    double Rf;
    double Rb;
  };

  // Base class for any material definition. It reprsents material properties over the certain range.
  // It supports also angular dependent materials
  class CMaterial {
  public:
    CMaterial( const double minLambda, const double maxLambda );

    // Get certain material property over the entire range
    virtual double getProperty( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side ) const = 0;

    virtual double getPropertyAtAngle( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side, const double t_Angle ) const;

    // Get properties for every band defined in the material
    virtual std::shared_ptr< std::vector< double > > 
      getBandProperties( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side ) const = 0;

    std::shared_ptr< std::vector< RMaterialProperties > > getBandProperties();

    virtual std::shared_ptr< std::vector< double > > 
      getBandPropertiesAtAngle( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side, const double t_Angle ) const;

    std::shared_ptr< std::vector< double > > getBandWavelengths();
    size_t getBandSize();
    // Return index of wavelength range for passed value. Returns -1 if index is out of range
    int getBandIndex( const double t_Wavelength );

    double getMinLambda() const;
    double getMaxLambda() const;

  protected:
    double m_MinLambda;
    double m_MaxLambda;

    // Set state in order not to calculate wavelengths every time
    virtual std::shared_ptr< std::vector< double > > calculateBandWavelengths() = 0;
    bool m_WavelengthsCalculated;
    std::shared_ptr< std::vector< double > > m_Wavelengths;

  };

  // Simple material with no angular dependence on reflection or transmittance. This is mainly used 
  // for shading device materials
  class CMaterialSingleBand : public CMaterial {
  public:
    CMaterialSingleBand( const double t_Tf, const double t_Tb, const double t_Rf, const double t_Rb, 
      const double minLambda, const double maxLambda );

    double getProperty( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side ) const;

    std::shared_ptr< std::vector< double > > 
      getBandProperties( const FenestrationCommon::Property t_Property, const FenestrationCommon::Side t_Side ) const;
    
  private:
    std::shared_ptr< std::vector< double > > calculateBandWavelengths();
    double T( FenestrationCommon::Side t_Side ) const;
    double R( FenestrationCommon::Side t_Side ) const;

  protected:
    double m_Tf;
    double m_Tb;
    double m_Rf;
    double m_Rb;
  };

  // Material that for given solar and partial range (visible, uv) will calculate equivalent optical
  // properties for the entire range
  class CMaterialDualBand : public CMaterial {
  
  public:
    // ratio is calculated outside of the class and can be provided here.
    // TODO: Need to confirm with the team if we actually need this approach (ratio should be calculated and not quessed)
    CMaterialDualBand( std::shared_ptr< CMaterial > t_PartialRange,
      std::shared_ptr< CMaterial > t_SolarRange, const double t_Ratio );
    
    // ratio is calculated based on provided solar radiation values
    CMaterialDualBand( std::shared_ptr< CMaterial > t_PartialRange,
      std::shared_ptr< CMaterial > t_SolarRange, 
      std::shared_ptr< FenestrationCommon::CSeries > t_SolarRadiation );

    double getProperty( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side ) const;

    std::shared_ptr< std::vector< double > > 
      getBandProperties( const FenestrationCommon::Property t_Property, const FenestrationCommon::Side t_Side ) const;

  private:
    std::shared_ptr< std::vector< double > > calculateBandWavelengths();
    // Checks if material is within valid range. Otherwise, algorithm is not valid.
    void checkIfMaterialWithingSolarRange( std::shared_ptr< CMaterial > t_Material ) const;
    void createUVRange();

    // Creates after UV range and stores data into m_Materials
    void createNIRRange( std::shared_ptr< CMaterial > t_PartialRange,
      std::shared_ptr< CMaterial > t_SolarRange, const double t_Fraction );

    // Same equation is valid for every property
    double getModifiedProperty( const double t_Range, const double t_Solar, const double t_Fraction ) const;

    std::shared_ptr< CMaterial > m_MaterialFullRange;
    
    std::vector< std::shared_ptr< CMaterial > > m_Materials;
  
  };

  // Material that contains data measured over the range of wavelengths. It also provides material properties
  // at certain angle. Assumes that material properties at certain angle can be calculated by using coated and
  // uncoated algorithms
  class CMaterialSample : public CMaterial {
  public:
    CMaterialSample( std::shared_ptr< SpectralAveraging::CSpectralSample > t_SpectralSample, 
      const double t_Thickness, const FenestrationCommon::SpecularMaterialType t_Type,
      const double minLambda, const double maxLambda );

    // In this case sample property is taken. Standard spectral data file contains T, Rf, Rb that is 
    // measured at certain wavelengths.
    double getPropertyAtAngle( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side, const double t_Angle ) const;
    double getProperty( const FenestrationCommon::Property t_Property, const FenestrationCommon::Side t_Side ) const;

    // Get properties at each wavelength and at given incident angle
    std::shared_ptr< std::vector< double > > 
      getBandPropertiesAtAngle( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side, const double t_Angle ) const;

    std::shared_ptr< std::vector< double > > 
      getBandProperties( const FenestrationCommon::Property t_Property, const FenestrationCommon::Side t_Side ) const;

  private:
    std::shared_ptr< std::vector< double > > calculateBandWavelengths();
    std::shared_ptr< SpectralAveraging::CAngularSpectralSample > m_AngularSample;

  };

}

#endif