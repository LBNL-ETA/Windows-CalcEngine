#include <limits>
#include <WCECommon.hpp>

#include "CMASettings.hpp"

#include <cassert>

namespace CMA
{
    ///////////////////////////////////////////////////
    //  CMABestWorstUFactors
    ///////////////////////////////////////////////////

    CMABestWorstUFactors::CMABestWorstUFactors(double hci, double hco, double gapConductance) :
        m_convective(hci, hco), m_GapConductance(gapConductance)
    {}

    CMABestWorstUFactors::CMABestWorstUFactors(double hci,
                                               double hco,
                                               double gapConductance,
                                               double interiorGlassThickness,
                                               double interiorGlassConductivity,
                                               double interiorGlassSurfaceEmissivity,
                                               double exteriorGlassThickness,
                                               double exteriorGlassConductivity,
                                               double exteriorGlassSurfaceEmissivity,
                                               double insideAirTemperature,
                                               double outsideAirTemperature) :
        m_convective(hci, hco),
        m_GapConductance(gapConductance),
        m_InteriorGlassThickness(interiorGlassThickness),
        m_InteriorGlassConductivity(interiorGlassConductivity),
        m_InteriorGlassSurfaceEmissivity(interiorGlassSurfaceEmissivity),
        m_ExteriorGlassThickness(exteriorGlassThickness),
        m_ExteriorGlassConductivity(exteriorGlassConductivity),
        m_ExteriorGlassSurfaceEmissivity(exteriorGlassSurfaceEmissivity),
        m_InsideAirTemperature(insideAirTemperature),
        m_OutsideAirTemperature(outsideAirTemperature)
    {}

    double CMABestWorstUFactors::uValue()
    {
        assert(m_InsideAirTemperature != m_OutsideAirTemperature);
        calculate();
        return heatFlow() / (m_InsideAirTemperature - m_OutsideAirTemperature);
    }

    double CMABestWorstUFactors::hcout()
    {
        calculate();
        return m_convective.outside;
    }

    double CMABestWorstUFactors::heatFlow() const
    {
        const double deltaTemp = m_InsideAirTemperature - m_OutsideAirTemperature;
        const double interiorGlassCond = m_InteriorGlassConductivity / m_InteriorGlassThickness;
        const double exteriorGlassCond = m_ExteriorGlassConductivity / m_ExteriorGlassThickness;

        return deltaTemp
               / (1.0 / interiorGlassCond + 1.0 / exteriorGlassCond + 1.0 / m_GapConductance
                  + 1.0 / hi() + 1.0 / ho());
    }

    double CMABestWorstUFactors::hrout(double surfaceTemperature) const
    {
        return m_ExteriorGlassSurfaceEmissivity * ConstantsData::STEFANBOLTZMANN
               * (std::pow(surfaceTemperature + ConstantsData::KELVINCONV, 4)
                  - std::pow(m_OutsideAirTemperature + ConstantsData::KELVINCONV, 4))
               / (surfaceTemperature - m_OutsideAirTemperature);
    }

    double CMABestWorstUFactors::hrin(double surfaceTemperature) const
    {
        return m_InteriorGlassSurfaceEmissivity * ConstantsData::STEFANBOLTZMANN
               * (std::pow(m_InsideAirTemperature + ConstantsData::KELVINCONV, 4)
                  - std::pow(surfaceTemperature + ConstantsData::KELVINCONV, 4))
               / (m_InsideAirTemperature - surfaceTemperature);
    }

    double CMABestWorstUFactors::insideSurfaceTemperature() const
    {
        return m_InsideAirTemperature - heatFlow() / hi();
    }

    double CMABestWorstUFactors::outsideSurfaceTemperature() const
    {
        return m_OutsideAirTemperature + heatFlow() / ho();
    }

    void CMABestWorstUFactors::calculate()
    {
        if(!m_radiative)
        {
            double insideTemperature{0.25 * (m_InsideAirTemperature - m_OutsideAirTemperature)
                                     + m_OutsideAirTemperature};
            double outsideTemperature{m_InsideAirTemperature
                                      - 0.25 * (m_InsideAirTemperature - m_OutsideAirTemperature)};
            double hri{hrin(insideTemperature)};
            double hro{hrout(outsideTemperature)};
            double error{std::numeric_limits<double>::max()};
            constexpr double errorTolerance{1e-4};
            while(error > errorTolerance)
            {
                const double previousInside{insideTemperature};
                insideTemperature = insideSurfaceTemperature();
                const double previousOutside{outsideTemperature};
                outsideTemperature = outsideSurfaceTemperature();

                m_radiative = Film{hri, hro};

                hri = hrin(insideTemperature);
                hro = hrout(outsideTemperature);
                error = std::max(std::abs(previousInside - insideTemperature),
                                 std::abs(previousOutside - outsideTemperature));
            }
        }
    }

    ///////////////////////////////////////////////////
    //  CMABestUFactor
    ///////////////////////////////////////////////////

    void CMABestWorstUFactors::invalidate() noexcept
    {
        m_radiative.reset();
    }

    CMABestWorstUFactors CreateBestWorstUFactorOption(Option option)
    {
        static const double defaultInsideFilmCofficintBest = 1.85425;
        static const double defaultOutsideFilmCoefficientBest = 26;
        static const double defaultGapConductanceBest = 0.498817;

        static const double defaultInsideFilmCofficintWorst = 2.86612;
        static const double defaultOutsideFilmCoefficientWorst = 26;
        static const double defaultGapConductanceWorst = 5.880546;

        static const std::map<Option, CMABestWorstUFactors> object{
          {Option::Best,
           CMABestWorstUFactors(defaultInsideFilmCofficintBest,
                                defaultOutsideFilmCoefficientBest,
                                defaultGapConductanceBest)},
          {Option::Worst,
           CMABestWorstUFactors(defaultInsideFilmCofficintWorst,
                                defaultOutsideFilmCoefficientWorst,
                                defaultGapConductanceWorst)}};

        return object.at(option);
    }

}   // namespace CMA
