#pragma once

#include <vector>
#include <memory>
#include <map>
#include <array>

#include <WCECommon.hpp>

#include "SpectralSampleDataDefinitions.hpp"

namespace FenestrationCommon
{
    class CSeries;
}

namespace SpectralAveraging
{
    ///////////////////////////////////////////////////////////////////////////
    /// MeasuredRow
    ///////////////////////////////////////////////////////////////////////////

    struct OpticalProperties
    {
        double Tf{0};
        double Tb{0};
        double Rf{0};
        double Rb{0};
    };

    struct MeasuredRow
    {
        double wavelength{0};
        OpticalProperties direct;
        OpticalProperties diffuse;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// SampleData
    ///////////////////////////////////////////////////////////////////////////
    class SampleData
    {
    public:
        SampleData() = default;
        virtual ~SampleData() = default;

        virtual void interpolate(const std::vector<double> & t_Wavelengths) = 0;
        virtual FenestrationCommon::CSeries properties(FenestrationCommon::Property prop,
                                                       FenestrationCommon::Side side,
                                                       ScatteringType type) = 0;

        FenestrationCommon::CSeries properties(FenestrationCommon::Property prop,
                                               FenestrationCommon::Side side);

        virtual void cutExtraData(double minLambda, double maxLambda) = 0;

        [[nodiscard]] bool Flipped() const;
        void Flipped(bool t_Flipped);

    private:
        bool m_Flipped{false};
    };

    ///////////////////////////////////////////////////////////////////////////
    /// CSpectralSampleData
    ///////////////////////////////////////////////////////////////////////////
    // Measured sample data for given wavelengths.
    class CSpectralSampleData : public SampleData
    {
    public:
        ~CSpectralSampleData() override = default;
        CSpectralSampleData();
        explicit CSpectralSampleData(const std::vector<MeasuredRow> & tValues);

        static std::shared_ptr<CSpectralSampleData>
          create(const std::vector<MeasuredRow> & tValues);

        static std::shared_ptr<CSpectralSampleData> create();

        void addRecord(double t_Wavelength,
                       double t_TransmittanceFront,
                       double t_TransmittanceBack,
                       double t_ReflectanceFront,
                       double t_ReflectanceBack);

        void addRecord(double t_Wavelength,
                       const OpticalProperties & direct,
                       const OpticalProperties & diffuse = OpticalProperties());

        using SampleData::properties;   // To unhide properties(prop, side) from base class

        FenestrationCommon::CSeries properties(FenestrationCommon::Property prop,
                                               FenestrationCommon::Side side,
                                               ScatteringType type) override;

        [[nodiscard]] virtual std::vector<double> getWavelengths() const;
        [[nodiscard]] virtual FenestrationCommon::Limits getWavelengthLimits() const;
        void interpolate(std::vector<double> const & t_Wavelengths) override;

        void cutExtraData(double minLambda, double maxLambda) override;

    protected:
        std::map<MeasurementKey, FenestrationCommon::CSeries> m_Property;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// PhotovoltaicSampleData
    ///////////////////////////////////////////////////////////////////////////
    class PhotovoltaicSampleData : public CSpectralSampleData
    {
    public:
        explicit PhotovoltaicSampleData(const CSpectralSampleData & spectralSampleData);
        PhotovoltaicSampleData(const CSpectralSampleData & spectralSampleData,
                               const FenestrationCommon::CSeries & eqeValuesFront,
                               const FenestrationCommon::CSeries & eqeValuesBack);

        void cutExtraData(double minLambda, double maxLambda) override;

        [[nodiscard]] FenestrationCommon::CSeries eqe(const FenestrationCommon::Side side) const;

    private:
        std::map<FenestrationCommon::Side, FenestrationCommon::CSeries> m_EQE;
    };

}   // namespace SpectralAveraging
