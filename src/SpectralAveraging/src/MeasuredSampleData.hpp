#ifndef SPECTRALSAMPLEDATA_H
#define SPECTRALSAMPLEDATA_H

#include <vector>
#include <memory>

namespace FenestrationCommon
{
    class CSeries;
}

namespace SpectralAveraging
{
    enum class SampleData
    {
        T,
        Rf,
        Rb,
        AbsF,
        AbsB
    };

    struct MeasuredRow {
    	MeasuredRow(double wl, double t, double rf, double rb);
    	double wavelength;
    	double T;
    	double Rf;
    	double Rb;
    };

    // Measured sample data for given wavelengths.
    class CSpectralSampleData
    {
    public:
        virtual ~CSpectralSampleData() = default;
        CSpectralSampleData();

        static std::shared_ptr<CSpectralSampleData> create(const std::initializer_list<MeasuredRow> & tValues);

        void addRecord(double t_Wavelength,
                       double t_Transmittance,
                       double t_ReflectanceFront,
                       double t_ReflectanceBack);
        std::shared_ptr<FenestrationCommon::CSeries> properties(SampleData t_Property);
        virtual std::vector<double> getWavelengths() const;
        virtual void interpolate(std::vector<double> const & t_Wavelengths);

        bool Flipped() const;
        virtual void Filpped(bool const t_Flipped);

    protected:
		CSpectralSampleData( const std::initializer_list<MeasuredRow> & tValues);

        virtual void calculateProperties();
        void reset();

        std::shared_ptr<FenestrationCommon::CSeries> m_Transmittances;
        std::shared_ptr<FenestrationCommon::CSeries> m_ReflectancesFront;
        std::shared_ptr<FenestrationCommon::CSeries> m_ReflectancesBack;

        // Calculated from sample measurements
        std::shared_ptr<FenestrationCommon::CSeries> m_AbsorptancesFront;
        std::shared_ptr<FenestrationCommon::CSeries> m_AbsorptancesBack;

        bool m_Flipped;
        bool m_absCalculated;
    };

}   // namespace SpectralAveraging

#endif
