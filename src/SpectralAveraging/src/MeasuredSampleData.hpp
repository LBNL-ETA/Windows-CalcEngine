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
    struct MeasuredRow
    {
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

        static std::shared_ptr<CSpectralSampleData>
          create(const std::vector<MeasuredRow> & tValues);

        static std::shared_ptr<CSpectralSampleData> create();

        void addRecord(double t_Wavelength,
                       double t_Transmittance,
                       double t_ReflectanceFront,
                       double t_ReflectanceBack);
        FenestrationCommon::CSeries & properties(FenestrationCommon::Property prop,
                             FenestrationCommon::Side side);
        virtual std::vector<double> getWavelengths() const;
        virtual void interpolate(std::vector<double> const & t_Wavelengths);

        bool Flipped() const;
        virtual void Filpped(bool t_Flipped);

        void cutExtraData(double minLambda, double maxLambda);

    protected:
        CSpectralSampleData(const std::vector<MeasuredRow> & tValues);

        virtual void calculateProperties();
        void reset();

        std::map<std::pair<FenestrationCommon::Property, FenestrationCommon::Side>,
                 FenestrationCommon::CSeries>
          m_Property;

        bool m_Flipped;
        bool m_absCalculated;
    };

}   // namespace SpectralAveraging

#endif
