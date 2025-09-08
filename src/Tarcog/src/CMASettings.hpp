#pragma once

#include <optional>

#include "CMAInterface.hpp"

namespace CMA
{
    ///////////////////////////////////////////////////
    //  CMABestWorstUFactors
    ///////////////////////////////////////////////////

    //! Will be used to calculate best and worst IGU options for CMA calculations.
    //! It contains default variables that usually do not need to be changed by the user.
    class CMABestWorstUFactors
    {
    public:
        CMABestWorstUFactors() = default;
        CMABestWorstUFactors(double hci, double hco, double gapConductance);

        CMABestWorstUFactors(double hci,
                             double hco,
                             double gapConductance,
                             double interiorGlassThickness,
                             double interiorGlassConductivity,
                             double interiorGlassSurfaceEmissivity,
                             double exteriorGlassThickness,
                             double exteriorGlassConductivity,
                             double exteriorGlassSurfaceEmissivity,
                             double insideAirTemperature,
                             double outsideAirTemperature);

        [[nodiscard]] double uValue();
        [[nodiscard]] double hcout();

    private:
        struct Film
        {
            double inside{0};
            double outside{0};
        };

        [[nodiscard]] double heatFlow() const;
        [[nodiscard]] double hrout(double surfaceTemperature) const;
        [[nodiscard]] double hrin(double surfaceTemperature) const;
        [[nodiscard]] double insideSurfaceTemperature() const;
        [[nodiscard]] double outsideSurfaceTemperature() const;

        void calculate();

        Film m_convective;
        std::optional<Film> m_radiative;

        double hci() const { return m_convective.inside; }
        double hco() const { return m_convective.outside; }
        double hri() const { return m_radiative ? m_radiative->inside : 0.0; }
        double hro() const { return m_radiative ? m_radiative->outside : 0.0; }
        double ho() const {return hco() + hro();}
        double hi() const {return hci() + hri();}

        double m_GapConductance{0};
        double m_InteriorGlassThickness{0.006};
        double m_InteriorGlassConductivity{1};
        double m_InteriorGlassSurfaceEmissivity{0.84};
        double m_ExteriorGlassThickness{0.006};
        double m_ExteriorGlassConductivity{1};
        double m_ExteriorGlassSurfaceEmissivity{0.84};
        double m_InsideAirTemperature{21};
        double m_OutsideAirTemperature{-18};

        void invalidate() noexcept;
    };

    //! Creates built in values for CMA U-factors
    CMABestWorstUFactors CreateBestWorstUFactorOption(Option option);
}   // namespace CMA
