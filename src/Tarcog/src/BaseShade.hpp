#ifndef TARCOGBASESHADE_H
#define TARCOGBASESHADE_H

#include <memory>

#include "IGUSolidLayer.hpp"

namespace Gases
{
    class CGas;
}

namespace Tarcog
{
    namespace ISO15099
    {
        class Surface;

        class CIGUVentilatedGapLayer;

        class CEnvironment;

        auto const OPENING_TOLERANCE = 1e-6;

        class CShadeOpenings
        {
        public:
            CShadeOpenings() = default;

            CShadeOpenings(double t_Atop,
                           double t_Abot,
                           double t_Aleft,
                           double t_Aright,
                           double t_Afront,
                           double t_FrontPorosity);

            double Aeq_bot();
            double Aeq_top();

            [[nodiscard]] double frontPorositiy() const;

            [[nodiscard]] bool isOpen() const;

            //! If gap next to shade is tighter than the opening, that gap will be dominant compared
            //! to the openning
            void checkAndSetDominantWidth(double gapWidth);

        private:
            void checkForValidity();
            double openingMultiplier() const;

            double m_Atop{OPENING_TOLERANCE};
            double m_Abot{OPENING_TOLERANCE};
            double m_Aleft{0};
            double m_Aright{0};
            double m_Afront{0};
            double m_FrontPorosity{0};
        };

        class CIGUShadeLayer : public CIGUSolidLayer
        {
        public:
            CIGUShadeLayer(
              const double t_Thickness,
              const double t_Conductivity,
              CShadeOpenings && t_ShadeOpenings,
              const std::shared_ptr<Tarcog::ISO15099::Surface> & t_FrontSurface = nullptr,
              const std::shared_ptr<Tarcog::ISO15099::Surface> & t_BackSurface = nullptr);

            CIGUShadeLayer(const std::shared_ptr<CIGUSolidLayer> & t_Layer,
                           const CShadeOpenings & t_ShadeOpenings);

            CIGUShadeLayer(double t_Thickness, double t_Conductivity);

            std::shared_ptr<CBaseLayer> clone() const override;

            bool isPermeable() const override;

        private:
            void setDominanthAirflowWidth();

            void calculateConvectionOrConductionFlow() override;

            void calcInBetweenShadeFlow(std::shared_ptr<CIGUVentilatedGapLayer> t_Gap1,
                                        std::shared_ptr<CIGUVentilatedGapLayer> t_Gap2);

            void calcEdgeShadeFlow(std::shared_ptr<CEnvironment> t_Environment,
                                   std::shared_ptr<CIGUVentilatedGapLayer> t_Gap);

            double equivalentConductivity(double t_Conductivity, double permeabilityFactor);

            CShadeOpenings m_ShadeOpenings;

            double m_MaterialConductivity;
        };

    }   // namespace ISO15099

}   // namespace Tarcog


#endif
