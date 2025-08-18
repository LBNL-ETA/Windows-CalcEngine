#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <mutex>

#include <WCESpectralAveraging.hpp>
#include "BeamDirection.hpp"   //  Need to include rather than forward declare to default incoming and outgoing directions to CBeamDirection()
#include "BSDFDirections.hpp"   //  Needed to have BSDFHemisphere as a member of the BSDF materials.  Could forward declare if BSDF material was changed to hide members using the pimpl ideom.

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
        virtual ~CMaterial() = default;
        CMaterial(double minLambda, double maxLambda);
        explicit CMaterial(FenestrationCommon::Limits wavelengthRange);

        virtual void setSourceData(const FenestrationCommon::CSeries &);
        virtual void setDetectorData(const FenestrationCommon::CSeries & t_DetectorData);

        // Get certain material property over the entire range
        virtual double
          getProperty(FenestrationCommon::Property t_Property,
                      FenestrationCommon::Side t_Side,
                      const CBeamDirection & t_IncomingDirection = CBeamDirection(),
                      const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const = 0;

        // Get properties for every band defined in the material
        virtual std::vector<double> getBandProperties(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const = 0;

        virtual double
          getBandProperty(FenestrationCommon::Property t_Property,
                          FenestrationCommon::Side t_Side,
                          size_t wavelengthIndex,
                          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
                          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const = 0;

        std::vector<RMaterialProperties> getBandProperties();

        std::shared_ptr<SpectralAveraging::CSpectralSample> getSpectralSample();


        const std::vector<double> & getBandWavelengths();
        virtual void setBandWavelengths(const std::vector<double> & wavelengths);

        size_t getBandSize();
        // Return index of wavelength range for passed value. Returns -1 if index is out of range
        int getBandIndex(double t_Wavelength);

        double getMinLambda() const;
        double getMaxLambda() const;

        virtual void Flipped(bool flipped);

        [[nodiscard]] bool isWavelengthInRange(double wavelength) const;

        [[nodiscard]] virtual FenestrationCommon::CSeries
          jscPrime(FenestrationCommon::Side t_Side) const;

    protected:
        double m_MinLambda;
        double m_MaxLambda;

        // Set state in order not to calculate wavelengths every time
        virtual std::vector<double> calculateBandWavelengths() = 0;
        bool m_WavelengthsCalculated{false};
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
        virtual ~CMaterialSingleBand() = default;
        CMaterialSingleBand(double t_Tf, double t_Tb, double t_Rf, double t_Rb);

        double
          getProperty(FenestrationCommon::Property t_Property,
                      FenestrationCommon::Side t_Side,
                      const CBeamDirection & t_IncomingDirection = CBeamDirection(),
                      const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        [[nodiscard]] std::vector<double> getBandProperties(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        [[nodiscard]] double getBandProperty(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          size_t wavelengthIndex,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

    private:
        std::vector<double> calculateBandWavelengths() override;

    protected:
        std::map<FenestrationCommon::Side, std::shared_ptr<CSurface>> m_Property;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialSingleBandBSDF
    //////////////////////////////////////////////////////////////////////////////////////////

    //! \brief Simple material with angular dependence on reflection or transmittance.
    //!
    //! This is mainly used for shading device materials
    class CMaterialSingleBandBSDF : public CMaterial
    {
    public:
        virtual ~CMaterialSingleBandBSDF() = default;
        CMaterialSingleBandBSDF(std::vector<std::vector<double>> const & t_Tf,
                                std::vector<std::vector<double>> const & t_Tb,
                                std::vector<std::vector<double>> const & t_Rf,
                                std::vector<std::vector<double>> const & t_Rb,
                                BSDFHemisphere const & t_Hemisphere);

        double
          getProperty(FenestrationCommon::Property t_Property,
                      FenestrationCommon::Side t_Side,
                      const CBeamDirection & t_IncomingDirection = CBeamDirection(),
                      const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        [[nodiscard]] std::vector<double> getBandProperties(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        [[nodiscard]] double getBandProperty(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          size_t wavelengthIndex,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        std::vector<std::vector<double>> const &
          getBSDFMatrix(FenestrationCommon::Property const & t_Property,
                        FenestrationCommon::Side const & t_Side) const;

        BSDFHemisphere getHemisphere() const;

    private:
        std::vector<double> calculateBandWavelengths() override;
        // Checks to make sure a matrix has the same number of values as the BSDF hemisphere
        // has directions.  Assumption:  All the inner vectors have the same number of values
        // This should probably be somewhere more general, just putting it here for now
        void validateMatrix(std::vector<std::vector<double>> const & matrix,
                            BSDFHemisphere const & m_Hemisphere) const;

    protected:
        std::map<std::pair<FenestrationCommon::Property, FenestrationCommon::Side>,
                 std::vector<std::vector<double>>>
          m_Property;
        BSDFHemisphere m_Hemisphere;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialDualBand
    //////////////////////////////////////////////////////////////////////////////////////////

    //! \brief Material that for given solar and partial range (visible, uv) will calculate
    //! equivalent optical properties for the entire range
    class IMaterialDualBand : public CMaterial
    {
    public:
        ~IMaterialDualBand() override = default;
        IMaterialDualBand(const std::shared_ptr<CMaterial> & visibleRange,
                          const std::shared_ptr<CMaterial> & solarRange);


        void setSourceData(const FenestrationCommon::CSeries & t_SourceData) override;
        void setDetectorData(const FenestrationCommon::CSeries & t_DetectorData) override;

        [[nodiscard]] double
          getProperty(FenestrationCommon::Property t_Property,
                      FenestrationCommon::Side t_Side,
                      const CBeamDirection & t_IncomingDirection = CBeamDirection(),
                      const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        [[nodiscard]] std::vector<double> getBandProperties(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        [[nodiscard]] double getBandProperty(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          size_t wavelengthIndex,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        // Creates all the required ranges in m_Materials from a ratio
        void createRangesFromRatio(double t_Ratio);

        // Creates all the required ranges in m_Materials from solar radiation
        void createRangesFromSolarRadiation(const FenestrationCommon::CSeries & t_SolarRadiation);

    protected:
        std::vector<double> calculateBandWavelengths() override;

        // Creates after UV range and stores data into m_Materials
        virtual void createNIRRange(const std::shared_ptr<CMaterial> & t_PartialRange,
                                    const std::shared_ptr<CMaterial> & t_SolarRange,
                                    double t_Fraction) = 0;

        [[nodiscard]] std::vector<double> getWavelengthsFromMaterials() const;

        // Properties over the rest of range will depend on partial range as well.
        // We do want to keep correct properties of partial range, but will want to update
        // properties for other partial ranges that are not provided by the user.
        // double getModifiedProperty(double t_Range, double t_Solar, double t_Fraction) const;

        [[nodiscard]] std::shared_ptr<CMaterial> getMaterialFromWavelength(double wavelength) const;

        std::shared_ptr<CMaterial> m_MaterialSolarRange;
        std::shared_ptr<CMaterial> m_MaterialVisibleRange;


        std::shared_ptr<CMaterial> m_MaterialScaledRange;
    };

    class CMaterialDualBand : public IMaterialDualBand
    {
    public:
        CMaterialDualBand(const std::shared_ptr<CMaterial> & t_PartialRange,
                          const std::shared_ptr<CMaterial> & t_FullRange);

    private:
        // Creates after UV range and stores data into m_Materials
        virtual void createNIRRange(const std::shared_ptr<CMaterial> & t_PartialRange,
                                    const std::shared_ptr<CMaterial> & t_FullRange,
                                    double t_Fraction) override;
    };


    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialDualBandBSDF
    //////////////////////////////////////////////////////////////////////////////////////////

    //! \brief Material that for given solar and partial range (visible, uv) will calculate
    //! equivalent optical properties for the entire range.  Uses BSDF matrices and results are
    //! therefore angular dependent.
    class CMaterialDualBandBSDF : public IMaterialDualBand
    {
    public:
        CMaterialDualBandBSDF(const std::shared_ptr<CMaterialSingleBandBSDF> & t_PartialRange,
                              const std::shared_ptr<CMaterialSingleBandBSDF> & t_FullRange);

    protected:
        // Creates after UV range and stores data into m_Materials
        void createNIRRange(const std::shared_ptr<CMaterial> & t_PartialRange,
                            const std::shared_ptr<CMaterial> & t_SolarRange,
                            double t_Fraction) override;
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
        ~CMaterialSample() override = default;
        CMaterialSample(
          const std::shared_ptr<SpectralAveraging::CSpectralSample> & t_SpectralSample,
          double t_Thickness,
          FenestrationCommon::MaterialType t_Type);

        void setSourceData(const FenestrationCommon::CSeries & t_SourceData) override;
        void setDetectorData(const FenestrationCommon::CSeries & t_DetectorData) override;

        // In this case sample property is taken. Standard spectral data file contains T, Rf, Rb
        // that is measured at certain wavelengths.
        [[nodiscard]] double
          getProperty(FenestrationCommon::Property t_Property,
                      FenestrationCommon::Side t_Side,
                      const CBeamDirection & t_IncomingDirection = CBeamDirection(),
                      const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        // Get properties at each wavelength and at given incident angle
        [[nodiscard]] std::vector<double> getBandProperties(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        [[nodiscard]] double getBandProperty(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          size_t wavelengthIndex,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        void setBandWavelengths(const std::vector<double> & wavelengths) override;

        void Flipped(bool flipped) override;

    protected:
        std::vector<double> calculateBandWavelengths() override;
        mutable SpectralAveraging::CAngularSpectralSample m_AngularSample;

    private:
        // Nested CacheKey structure
        struct CacheKey
        {
            FenestrationCommon::Property property;
            FenestrationCommon::Side side;
            double incomingTheta;
            size_t numberOfWavelengths;

            bool operator==(const CacheKey & other) const
            {
                return property == other.property && side == other.side
                       && numberOfWavelengths == other.numberOfWavelengths
                       && FenestrationCommon::isEqual(incomingTheta, other.incomingTheta);
            }
        };

        // Hash specialization for CacheKey
        struct CacheKeyHash
        {
            std::size_t operator()(const CacheKey & key) const
            {
                // Use a robust way to combine hashes
                size_t seed = 0;
                hashCombine(seed, static_cast<int>(key.property));
                hashCombine(seed, static_cast<int>(key.side));
                hashCombine(seed, key.numberOfWavelengths);
                hashCombine(seed, key.incomingTheta);
                return seed;
            }

        private:
            template<typename T>
            void hashCombine(size_t & seed, const T & value) const
            {
                seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
        };

        mutable std::mutex m_CacheMutex;
        mutable std::unordered_map<CacheKey, std::vector<double>, CacheKeyHash> m_Cache;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///   CMaterialPhotovoltaicSample
    //////////////////////////////////////////////////////////////////////////////////////////
    class CMaterialPhotovoltaicSample : public CMaterialSample
    {
    public:
        virtual ~CMaterialPhotovoltaicSample() = default;
        CMaterialPhotovoltaicSample(
          const std::shared_ptr<SpectralAveraging::CPhotovoltaicSample> & t_SpectralSample,
          double t_Thickness,
          FenestrationCommon::MaterialType t_Type);

        [[nodiscard]] FenestrationCommon::CSeries
          jscPrime(FenestrationCommon::Side t_Side) const override;

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
          const std::shared_ptr<SpectralAveraging::CAngularMeasurements> & t_Measurements);

        void setSourceData(const FenestrationCommon::CSeries & t_SourceData) override;

        // In this case sample property is taken. Standard spectral data file contains T, Rf, Rb
        // that is measured at certain wavelengths.
        double
          getProperty(FenestrationCommon::Property t_Property,
                      FenestrationCommon::Side t_Side,
                      const CBeamDirection & t_IncomingDirection = CBeamDirection(),
                      const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        // Get properties at each wavelength and at given incident angle
        [[nodiscard]] std::vector<double> getBandProperties(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

        [[nodiscard]] double getBandProperty(
          FenestrationCommon::Property t_Property,
          FenestrationCommon::Side t_Side,
          size_t wavelengthIndex,
          const CBeamDirection & t_IncomingDirection = CBeamDirection(),
          const CBeamDirection & t_OutgoingDirection = CBeamDirection()) const override;

    private:
        std::vector<double> calculateBandWavelengths() override;
        std::shared_ptr<SpectralAveraging::CAngularMeasurements> m_AngularMeasurements;
    };


}   // namespace SingleLayerOptics