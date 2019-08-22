#ifndef SPECTRALSAMPLEDATA_H
#define SPECTRALSAMPLEDATA_H

#include <vector>
#include <memory>
#include <WCECommon.hpp>

namespace FenestrationCommon
{
    class CSeries;
}

namespace SpectralAveraging
{
    ///////////////////////////////////////////////////////////////////////////
    /// MeasuredRow
    ///////////////////////////////////////////////////////////////////////////
    struct MeasuredRow
    {
        MeasuredRow(double wl, double t, double rf, double rb);
        double wavelength;
        double T;
        double Rf;
        double Rb;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// SampleData
    ///////////////////////////////////////////////////////////////////////////
    class SampleData{
    public:
        SampleData();
        virtual ~SampleData() = default;

        virtual void interpolate(const std::vector<double> & t_Wavelengths) = 0;
        virtual FenestrationCommon::CSeries & properties(FenestrationCommon::Property prop,
                                                 FenestrationCommon::Side side) = 0;

        virtual void cutExtraData(double minLambda, double maxLambda) = 0;
        virtual void addRecord(double t_Wavelength,
                       double t_Transmittance,
                       double t_ReflectanceFront,
                       double t_ReflectanceBack) = 0;

        virtual bool Flipped() const final;
        virtual void Filpped(bool t_Flipped) final;

    protected:
        bool m_Flipped;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// CSpectralSampleData
    ///////////////////////////////////////////////////////////////////////////
    // Measured sample data for given wavelengths.
    class CSpectralSampleData : public SampleData
    {
    public:
        virtual ~CSpectralSampleData() = default;
        CSpectralSampleData();

        static std::shared_ptr<CSpectralSampleData>
          create(const std::vector<MeasuredRow> & tValues);

        static std::shared_ptr<CSpectralSampleData> create();

        virtual void addRecord(double t_Wavelength,
                       double t_Transmittance,
                       double t_ReflectanceFront,
                       double t_ReflectanceBack) override;

        virtual FenestrationCommon::CSeries & properties(FenestrationCommon::Property prop,
                             FenestrationCommon::Side side) override;

        virtual std::vector<double> getWavelengths() const;
        virtual void interpolate(std::vector<double> const & t_Wavelengths) override;

        virtual void cutExtraData(double minLambda, double maxLambda) override;

    protected:
        CSpectralSampleData(const std::vector<MeasuredRow> & tValues);

        virtual void calculateProperties();
        void reset();

        std::map<std::pair<FenestrationCommon::Property, FenestrationCommon::Side>,
                 FenestrationCommon::CSeries>
          m_Property;

        bool m_absCalculated;
    };

}   // namespace SpectralAveraging

#endif
