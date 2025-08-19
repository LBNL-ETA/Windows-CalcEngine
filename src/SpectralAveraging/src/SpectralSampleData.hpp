#pragma once

#include <vector>
#include <memory>
#include <map>
#include <WCECommon.hpp>

namespace FenestrationCommon
{
    class CSeries;
}

namespace SpectralAveraging
{
    ///////////////////////////////////////////////////////////////////////////
    /// MeasurementType
    ///////////////////////////////////////////////////////////////////////////

    enum class MeasurementType
    {
        Direct,
        Diffuse,
        Total
    };

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
        virtual FenestrationCommon::CSeries & properties(FenestrationCommon::Property prop,
                                                         FenestrationCommon::Side side,
                                                         MeasurementType type) = 0;

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

        FenestrationCommon::CSeries &
          properties(FenestrationCommon::Property prop,
                     FenestrationCommon::Side side,
                     MeasurementType type = MeasurementType::Total) override;

        [[nodiscard]] virtual std::vector<double> getWavelengths() const;
        [[nodiscard]] virtual FenestrationCommon::Limits getWavelengthLimits() const;
        void interpolate(std::vector<double> const & t_Wavelengths) override;

        void cutExtraData(double minLambda, double maxLambda) override;

    protected:
        virtual void calculateProperties();
        void reset();

        std::map<std::pair<FenestrationCommon::Property, FenestrationCommon::Side>,
                 FenestrationCommon::CSeries>
          m_Property;

        bool m_absCalculated;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// PVM
    ///////////////////////////////////////////////////////////////////////////
    enum class PVM
    {
        JSC,
        VOC,
        FF
    };

    class EnumPVM : public FenestrationCommon::Enum<PVM>
    {};

    inline EnumPVM::Iterator begin(EnumPVM)
    {
        return EnumPVM::Iterator(static_cast<int>(PVM::JSC));
    }

    inline EnumPVM::Iterator end(EnumPVM)
    {
        return EnumPVM::Iterator(static_cast<int>(PVM::FF) + 1);
    }

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
