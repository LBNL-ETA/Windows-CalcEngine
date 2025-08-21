#pragma once

#include <vector>
#include <memory>
#include <map>
#include <array>

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
        Diffuse
    };

    constexpr std::array<MeasurementType, 2> allMeasurements{MeasurementType::Direct,
                                                             MeasurementType::Diffuse};

    inline auto
      key(FenestrationCommon::PropertySimple p, FenestrationCommon::Side s, MeasurementType m)
    {
        return std::make_tuple(p, s, m);
    }

    inline auto
        key(FenestrationCommon::Property p, FenestrationCommon::Side s)
    {
        return std::make_pair(p, s);
    }

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

    enum class PropertyType
    {
        Direct,
        Diffuse,
        Total
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
                                                       PropertyType type) = 0;

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

        FenestrationCommon::CSeries properties(FenestrationCommon::Property prop,
                                               FenestrationCommon::Side side,
                                               PropertyType type = PropertyType::Total) override;

        [[nodiscard]] virtual std::vector<double> getWavelengths() const;
        [[nodiscard]] virtual FenestrationCommon::Limits getWavelengthLimits() const;
        void interpolate(std::vector<double> const & t_Wavelengths) override;

        void cutExtraData(double minLambda, double maxLambda) override;

    protected:
        std::map<
          std::tuple<FenestrationCommon::PropertySimple, FenestrationCommon::Side, MeasurementType>,
          FenestrationCommon::CSeries>
          m_Property;
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
