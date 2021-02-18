#include <limits>
#include <WCECommon.hpp>

#include "CMASettings.hpp"

#include <cassert>

namespace CMA
{
    ///////////////////////////////////////////////////
    //  CMABestWorstUFactors
    ///////////////////////////////////////////////////

    CMABestWorstUFactors::CMABestWorstUFactors(double hci, double hco, double gapConductivity) :
        m_Hci(hci), m_Hco(hco), m_GapConductivity(gapConductivity)
    {}

    double CMABestWorstUFactors::uValue()
    {
        assert(m_InsideAirTemperature != m_OutsideAirTemperature);
        caluculate();
        return heatFlow(m_Hri, m_Hro) / (m_InsideAirTemperature - m_OutsideAirTemperature);
    }

    double CMABestWorstUFactors::heatFlow(const double interiorRadiationFilmCoefficient,
                                          const double exteriorRadiationFilmCoefficient) const
    {
        const double deltaTemp{m_InsideAirTemperature - m_OutsideAirTemperature};
        const double interiorGlassCond{m_InteriorGlassConductivity / m_InteriorGlassThickness};
        const double exteriorGlassCond{m_ExteriorGlassConductivity / m_ExteriorGlassThickness};
        const double gapCond{m_GapConductivity / m_GapThickness};

        return deltaTemp
               / (1 / interiorGlassCond + 1 / exteriorGlassCond + 1 / gapCond
                  + 1 / (m_Hci + interiorRadiationFilmCoefficient)
                  + 1 / (m_Hco + exteriorRadiationFilmCoefficient));
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

    double
      CMABestWorstUFactors::insideSurfaceTemperature(double interiorRadiationFilmCoefficient) const
    {
        return m_InsideAirTemperature
               - heatFlow(interiorRadiationFilmCoefficient, m_Hro)
                   / (m_Hci + interiorRadiationFilmCoefficient);
    }

    double
      CMABestWorstUFactors::outsideSurfaceTemperature(double exteriorRadiationFilmCoefficient) const
    {
        return m_OutsideAirTemperature
               + heatFlow(m_Hri, exteriorRadiationFilmCoefficient)
                   / (m_Hco + exteriorRadiationFilmCoefficient);
    }

    void CMABestWorstUFactors::caluculate()
    {
        if(!m_Calculated)
        {
            double insideTemperature{0.25 * (m_InsideAirTemperature - m_OutsideAirTemperature)
                                     + m_OutsideAirTemperature};
            double outsideTemperature{m_InsideAirTemperature
                                      - 0.25 * (m_InsideAirTemperature - m_OutsideAirTemperature)};
            double hri{hrin(insideTemperature)};
            double hro{hrout(outsideTemperature)};
            double error{std::numeric_limits<double>::max()};
            const double errorTolerance{1e-2};
            while(error > errorTolerance)
            {
                const double previousInside{insideTemperature};
                insideTemperature = insideSurfaceTemperature(hri);
                const double previousOutside{outsideTemperature};
                outsideTemperature = outsideSurfaceTemperature(hro);
                hri = hrin(insideTemperature);
                hro = hrout(outsideTemperature);
                error = std::max(std::abs(previousInside - insideTemperature),
                                 std::abs(previousOutside - outsideTemperature));
            }

            m_Hri = hri;
            m_Hro = hro;

            m_Calculated = true;
        }
    }

    ///////////////////////////////////////////////////
    //  CMABestUFactor
    ///////////////////////////////////////////////////

    const double CMABestUFactor::defaultInsideFilmCofficint = 1.85425;
    const double CMABestUFactor::defaultOutsideFilmCoefficient = 26;
    const double CMABestUFactor::defaultGapConductivity = 0.00668415;

    CMABestUFactor::CMABestUFactor() :
        CMABestWorstUFactors(
          defaultInsideFilmCofficint, defaultOutsideFilmCoefficient, defaultGapConductivity)
    {}

    ///////////////////////////////////////////////////
    //  CMAWorstUFactor
    ///////////////////////////////////////////////////

    const double CMAWorstUFactor::defaultInsideFilmCofficint = 2.86612;
    const double CMAWorstUFactor::defaultOutsideFilmCoefficient = 26;
    const double CMAWorstUFactor::defaultGapConductivity = 0.0787993;

    CMAWorstUFactor::CMAWorstUFactor() :
        CMABestWorstUFactors(
          defaultInsideFilmCofficint, defaultOutsideFilmCoefficient, defaultGapConductivity)
    {}

}   // namespace CMA
