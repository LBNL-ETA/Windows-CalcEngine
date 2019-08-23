#ifndef MATERIALDESCRIPTION_H
#define MATERIALDESCRIPTION_H

#include <memory>
#include <vector>
#include <map>
#include <WCESpectralAveraging.hpp>

// Lixing
namespace FenestrationCommon
{
    enum class Side;
    enum class Property;
    enum class MaterialType;
    enum class WavelengthRange;
    class CSeries;

}   // namespace FenestrationCommon

namespace SpectralAveraging
{
    // enum class SampleProperty;
    class CSpectralSample;
    class CPhotovoltaicSample;
    class CAngularSpectralSample;
    class CSingleAngularMeasurement;
    class CAngularMeasurements;

}   // namespace SpectralAveraging

namespace SingleLayerOptics
{
    class CSurface;

    struct RMaterialProperties
    {
    public:
        RMaterialProperties(double aTf, double aTb, double aRf, double aRb);
        double getProperty(FenestrationCommon::Property t_Property,
                           FenestrationCommon::Side t_Side) const;

    private:
        std::map<FenestrationCommon::Side, std::shared_ptr<CSurface>> m_Surface;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterial
    //////////////////////////////////////////////////////////////////////////////////////////

    //! \breif Base virtual class for any material definition.
    //!
    //! It represents material properties over the certain wavelength range. It also defines
    //! interface for angular dependency of material properties.
    class CMaterial
    {
    public:
        CMaterial(double minLambda, double maxLambda);
        explicit CMaterial(FenestrationCommon::WavelengthRange t_Range);

        virtual void setSourceData(FenestrationCommon::CSeries &);
        virtual void setDetectorData(FenestrationCommon::CSeries &t_DetectorData);

        // Get certain material property over the entire range
        virtual double getProperty(FenestrationCommon::Property t_Property,
                                   FenestrationCommon::Side t_Side) const = 0;

        virtual double getPropertyAtAngle(FenestrationCommon::Property t_Property,
                                          FenestrationCommon::Side t_Side,
                                          double t_Angle) const;

        // Get properties for every band defined in the material
        virtual std::vector<double> getBandProperties(FenestrationCommon::Property t_Property,
                                                      FenestrationCommon::Side t_Side) const = 0;

        std::vector<RMaterialProperties> getBandProperties();

        std::shared_ptr<SpectralAveraging::CSpectralSample> getSpectralSample();

        virtual std::vector<double>
          getBandPropertiesAtAngle(FenestrationCommon::Property t_Property,
                                   FenestrationCommon::Side t_Side,
                                   double t_Angle) const;

        std::vector<double> getBandWavelengths();
        virtual void setBandWavelengths(const std::vector<double> & wavelengths);

        size_t getBandSize();
        // Return index of wavelength range for passed value. Returns -1 if index is out of range
        int getBandIndex(double t_Wavelength);

        double getMinLambda() const;
        double getMaxLambda() const;

    protected:
        double m_MinLambda;
        double m_MaxLambda;

        std::vector<double> trimWavelengthToRange(const std::vector<double> & wavelengths) const;
        // Set state in order not to calculate wavelengths every time
        virtual std::vector<double> calculateBandWavelengths() = 0;
        bool m_WavelengthsCalculated;
        std::vector<double> m_Wavelengths;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialSingleBand
    //////////////////////////////////////////////////////////////////////////////////////////

    //! \brief Simple material with no angular dependence on reflection or transmittance.
    //!
    //! This is mainly used for shading device materials
    class CMaterialSingleBand : public CMaterial
    {
    public:
        CMaterialSingleBand(
          double t_Tf, double t_Tb, double t_Rf, double t_Rb, double minLambda, double maxLambda);
        CMaterialSingleBand(double t_Tf,
                            double t_Tb,
                            double t_Rf,
                            double t_Rb,
                            FenestrationCommon::WavelengthRange t_Range);

        double getProperty(FenestrationCommon::Property t_Property,
                           FenestrationCommon::Side t_Side) const override;

        std::vector<double> getBandProperties(FenestrationCommon::Property t_Property,
                                              FenestrationCommon::Side t_Side) const override;

    private:
        std::vector<double> calculateBandWavelengths() override;

    protected:
        std::map<FenestrationCommon::Side, std::shared_ptr<CSurface>> m_Property;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialDualBand
    //////////////////////////////////////////////////////////////////////////////////////////

    //! \brief Material that for given solar and partial range (visible, uv) will calculate
    //! equivalent optical properties for the entire range
    class CMaterialDualBand : public CMaterial
    {
    public:
        // ratio is calculated outside of the class and can be provided here.
        // TODO: Need to confirm with the team if we actually need this approach
        // (ratio should be calculated and not quessed)
        CMaterialDualBand(const std::shared_ptr<CMaterial> & t_PartialRange,
                          const std::shared_ptr<CMaterial> & t_SolarRange,
                          double t_Ratio);

        // ratio is calculated based on provided solar radiation values
        CMaterialDualBand(const std::shared_ptr<CMaterial> & t_PartialRange,
                          const std::shared_ptr<CMaterial> & t_SolarRange,
                          const FenestrationCommon::CSeries &t_SolarRadiation);

        CMaterialDualBand(const std::shared_ptr<CMaterial> & t_PartialRange,
                          const std::shared_ptr<CMaterial> & t_SolarRange);

        void setSourceData(FenestrationCommon::CSeries &t_SourceData) override;
        void
          setDetectorData(FenestrationCommon::CSeries &t_DetectorData) override;

        double getProperty(FenestrationCommon::Property t_Property,
                           FenestrationCommon::Side t_Side) const override;

        std::vector<double> getBandProperties(FenestrationCommon::Property t_Property,
                                              FenestrationCommon::Side t_Side) const override;

    private:
        std::vector<double> calculateBandWavelengths() override;
        // Checks if material is within valid range. Otherwise, algorithm is not valid.
        void checkIfMaterialWithingSolarRange(const CMaterial & t_Material) const;
        void createUVRange();

        // Creates after UV range and stores data into m_Materials
        void createNIRRange(const std::shared_ptr<CMaterial> & t_PartialRange,
                            const CMaterial & t_SolarRange,
                            double t_Fraction);

        // Properties over the rest of range will depend on partial range as well.
        // We do want to keep correct properties of partial range, but will want to update
        // properties for other partial ranges that are not provided by the user.
        double getModifiedProperty(double t_Range, double t_Solar, double t_Fraction) const;

        std::shared_ptr<CMaterial> m_MaterialFullRange;
        std::shared_ptr<CMaterial> m_MaterialPartialRange;

        std::vector<std::shared_ptr<CMaterial>> m_Materials;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialSample
    //////////////////////////////////////////////////////////////////////////////////////////

    //! /brief Material that contains data measured over the range of wavelengths.
    //!
    //! It also provides material properties at certain angle. Assumes that material properties
    //! at certain angle can be calculated by using coated and uncoated algorithms
    class CMaterialSample : public CMaterial
    {
    public:
        CMaterialSample(
          const std::shared_ptr<SpectralAveraging::CSpectralSample> & t_SpectralSample,
          double t_Thickness,
          FenestrationCommon::MaterialType t_Type,
          double minLambda,
          double maxLambda);

        CMaterialSample(
          const std::shared_ptr<SpectralAveraging::CSpectralSample> & t_SpectralSample,
          double t_Thickness,
          FenestrationCommon::MaterialType t_Type,
          FenestrationCommon::WavelengthRange t_Range);

        void setSourceData(FenestrationCommon::CSeries &t_SourceData) override;
        void
          setDetectorData(FenestrationCommon::CSeries &t_DetectorData) override;

        // In this case sample property is taken. Standard spectral data file contains T, Rf, Rb
        // that is measured at certain wavelengths.
        double getPropertyAtAngle(FenestrationCommon::Property t_Property,
                                  FenestrationCommon::Side t_Side,
                                  double t_Angle) const override;
        double getProperty(FenestrationCommon::Property t_Property,
                           FenestrationCommon::Side t_Side) const override;

        // Get properties at each wavelength and at given incident angle
        std::vector<double> getBandPropertiesAtAngle(FenestrationCommon::Property t_Property,
                                                     FenestrationCommon::Side t_Side,
                                                     double t_Angle) const override;

        std::vector<double> getBandProperties(FenestrationCommon::Property t_Property,
                                              FenestrationCommon::Side t_Side) const override;

        void setBandWavelengths(const std::vector<double> & wavelengths) override;

    protected:
        std::vector<double> calculateBandWavelengths() override;
        std::shared_ptr<SpectralAveraging::CAngularSpectralSample> m_AngularSample;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialPhotovoltaic
    //////////////////////////////////////////////////////////////////////////////////////////
    class CMaterialPhotovoltaic : public CMaterialSample
    {
    public:
        CMaterialPhotovoltaic(
                const std::shared_ptr<SpectralAveraging::CPhotovoltaicSample> & t_SpectralSample,
                double t_Thickness,
                FenestrationCommon::MaterialType t_Type,
                double minLambda,
                double maxLambda);

        CMaterialPhotovoltaic(
                const std::shared_ptr<SpectralAveraging::CPhotovoltaicSample> & t_SpectralSample,
                double t_Thickness,
                FenestrationCommon::MaterialType t_Type,
                FenestrationCommon::WavelengthRange t_Range);

        FenestrationCommon::CSeries PCE(FenestrationCommon::Side t_Side) const;
        FenestrationCommon::CSeries W(FenestrationCommon::Side t_Side) const;

    private:
        std::shared_ptr<SpectralAveraging::CPhotovoltaicSample> m_PVSample;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialMeasured
    //////////////////////////////////////////////////////////////////////////////////////////

    // Material that contains data measured over the range of wavelengths. It also provides material
    // properties at certain angle. Assumes that material properties at certain angle can be
    // calculated by using coated and uncoated algorithms
    class CMaterialMeasured : public CMaterial
    {
    public:
        CMaterialMeasured(
          const std::shared_ptr<SpectralAveraging::CAngularMeasurements> & t_Measurements,
          double minLambda,
          double maxLambda);

        CMaterialMeasured(
          const std::shared_ptr<SpectralAveraging::CAngularMeasurements> & t_Measurements,
          FenestrationCommon::WavelengthRange t_Range);

        void setSourceData(FenestrationCommon::CSeries &t_SourceData) override;

        // In this case sample property is taken. Standard spectral data file contains T, Rf, Rb
        // that is measured at certain wavelengths.
        double getPropertyAtAngle(FenestrationCommon::Property t_Property,
                                  FenestrationCommon::Side t_Side,
                                  double t_Angle) const override;
        double getProperty(FenestrationCommon::Property t_Property,
                           FenestrationCommon::Side t_Side) const override;

        // Get properties at each wavelength and at given incident angle
        std::vector<double> getBandPropertiesAtAngle(FenestrationCommon::Property t_Property,
                                                     FenestrationCommon::Side t_Side,
                                                     double t_Angle) const override;

        std::vector<double> getBandProperties(FenestrationCommon::Property t_Property,
                                              FenestrationCommon::Side t_Side) const override;

    private:
        std::vector<double> calculateBandWavelengths() override;
        std::shared_ptr<SpectralAveraging::CAngularMeasurements> m_AngularMeasurements;
    };
}   // namespace SingleLayerOptics

#endif
