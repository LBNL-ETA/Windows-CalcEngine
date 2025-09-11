#pragma once

#include "SpectralSampleData.hpp"


#include <memory>
#include <vector>

#include <WCECommon.hpp>

namespace SpectralAveraging
{
    class CSpectralSampleData;
    class PhotovoltaicSampleData;

    enum class WavelengthSet
    {
        Custom,
        Source,
        Data
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /// CSample
    /////////////////////////////////////////////////////////////////////////////////////

    //! \brief Base class for spectral sample data.
    //!
    //! Its base setup are spectral properties over certain range. It handles detector and source
    //! data. Concrete sample data are handled in inherited classes and tha will depend on type of
    //! the sample data
    class CSample
    {
    public:
        virtual ~CSample() = default;
        explicit CSample(const FenestrationCommon::CSeries & t_SourceData);
        CSample();
        CSample(const CSample & t_Sample) = default;
        CSample & operator=(CSample const & t_Sample) = default;

        //! \brief Assigns detector and wavelengths from other sample.
        void assignDetectorAndWavelengths(const CSample & t_Sample);

        // Gets source data. In case wavelengths are referenced to detector or custom
        // wavelength set, it will perform interpolation according to desired settings.
        FenestrationCommon::CSeries & getSourceData();
        void setSourceData(const FenestrationCommon::CSeries & t_SourceData);

        // Setting detector spectral properties for the sample
        void setDetectorData(const FenestrationCommon::CSeries & t_DetectorData);

        // Integrate sample property over the certain spectrum range
        double getProperty(double minLambda,
                           double maxLambda,
                           FenestrationCommon::Property t_Property,
                           FenestrationCommon::Side t_Side,
                           ScatteringType t_ScatteringType = ScatteringType::Total);

        // Spectral properties over the wavelength range
        FenestrationCommon::CSeries & getEnergyProperties(FenestrationCommon::Property t_Property,
                                                          FenestrationCommon::Side t_Side,
                                                          ScatteringType t_ScatteringType);

        // Defining the source of wavelengths to be used with the sample. Wavelengths can be used
        // from measured sample, detector data or can be custom provided.
        void setWavelengths(WavelengthSet t_WavelengthSet,
                            const std::vector<double> & t_Wavelenghts = {});

        // For given incoming source and detector data, calculates energy (Transmitted, Reflected or
        // Absorbed) for given spectrum range.
        double getEnergy(double minLambda,
                         double maxLambda,
                         FenestrationCommon::Property t_Property,
                         FenestrationCommon::Side t_Side,
                         ScatteringType t_ScatteringType);

        [[nodiscard]] std::vector<double> getWavelengths() const;

        [[nodiscard]] size_t getBandSize() const;

    protected:
        virtual void reset();
        virtual void calculateState(FenestrationCommon::IntegrationType integrator,
                                    double normalizationCoefficient);
        virtual void calculateProperties(FenestrationCommon::IntegrationType integrator,
                                         double normalizationCoefficient) = 0;

        // It can be single or multiple samples. In any case this should be seen as single set of
        // wavelengths
        [[nodiscard]] virtual std::vector<double> getWavelengthsFromSample() const = 0;

        FenestrationCommon::CSeries m_SourceData;
        FenestrationCommon::CSeries m_DetectorData;

        std::vector<double> m_Wavelengths;
        WavelengthSet m_WavelengthSet;

        // Keep energy for current state of the sample. Energy is calculated for each wavelength.
        FenestrationCommon::CSeries m_IncomingSource;
        std::map<ScatteringKey, FenestrationCommon::CSeries> m_EnergySource;

        bool m_StateCalculated;

        FenestrationCommon::IntegrationType m_IntegrationType{
          FenestrationCommon::IntegrationType::Trapezoidal};
        double m_NormalizationCoefficient{1.0};
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /// CSpectralSample
    /////////////////////////////////////////////////////////////////////////////////////

    class CSpectralSample : public CSample
    {
    public:
        CSpectralSample(std::shared_ptr<CSpectralSampleData> const & t_SampleData,
                        const FenestrationCommon::CSeries & t_SourceData);

        explicit CSpectralSample(std::shared_ptr<CSpectralSampleData> const & t_SampleData);

        // Before retrieving measured data from sample, function will do all necessary
        // interpolations to desired wavelengths.
        std::shared_ptr<CSpectralSampleData> getMeasuredData();

        // Returns property at each wavelength
        FenestrationCommon::CSeries getWavelengthsProperty(FenestrationCommon::Property t_Property,
                                                           FenestrationCommon::Side t_Side,
                                                           ScatteringType t_Scatter);

        [[nodiscard]] std::vector<double> getWavelengthsFromSample() const override;

        void cutExtraData(double minLambda, double maxLambda);

        void Flipped(bool flipped);

        [[nodiscard]] FenestrationCommon::Limits getWavelengthLimits() const;

    protected:
        void calculateProperties(FenestrationCommon::IntegrationType integrator,
                                 double normalizationCoefficient) override;
        void calculateState(FenestrationCommon::IntegrationType integrator,
                            double normalizationCoefficient) override;

        std::shared_ptr<CSpectralSampleData> m_SampleData;

        std::map<ScatteringKey, FenestrationCommon::CSeries> m_Property;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /// CPhotovoltaicSample
    /////////////////////////////////////////////////////////////////////////////////////

    class CPhotovoltaicSample : public CSpectralSample
    {
    public:
        CPhotovoltaicSample(const std::shared_ptr<PhotovoltaicSampleData> & t_PhotovoltaicData,
                            const FenestrationCommon::CSeries & t_SourceData);

        FenestrationCommon::CSeries jscPrime(const FenestrationCommon::Side side);

    protected:
        void calculateState(FenestrationCommon::IntegrationType integrator,
                            double normalizationCoefficient) override;
        [[nodiscard]] PhotovoltaicSampleData * getSample() const;

        static double jscPrimeCalc(double wavelength, double eqe);

        // This values need to be multiplied with incoming radiation in order to give current per
        // wavelength
        std::map<FenestrationCommon::Side, FenestrationCommon::CSeries> m_JcsPrime;
    };

}   // namespace SpectralAveraging
