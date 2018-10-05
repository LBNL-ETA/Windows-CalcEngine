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
        class ISurface;

        class CIGUVentilatedGapLayer;

        class CEnvironment;

        class CShadeOpenings
        {
        public:
            CShadeOpenings(
              double t_Atop, double t_Abot, double t_Aleft, double t_Aright, double t_Afront);

            CShadeOpenings();

            double Aeq_bot();
            double Aeq_top();

        private:
            void initialize();
            double openingMultiplier();

            double m_Atop;
            double m_Abot;
            double m_Aleft;
            double m_Aright;
            double m_Afront;
        };

        class CIGUShadeLayer : public CIGUSolidLayer
        {
        public:
            CIGUShadeLayer(double t_Thickness,
                           double t_Conductivity,
                           std::shared_ptr<CShadeOpenings> const & t_ShadeOpenings,
                           std::shared_ptr<Tarcog::ISurface> const & t_FrontSurface = nullptr,
                           std::shared_ptr<Tarcog::ISurface> const & t_BackSurface = nullptr);

            CIGUShadeLayer(std::shared_ptr<CIGUSolidLayer> & t_Layer,
                           std::shared_ptr<CShadeOpenings> & t_ShadeOpenings);

            CIGUShadeLayer(double t_Thickness, double t_Conductivity);

            std::shared_ptr<CBaseLayer> clone() const override;

        private:
            void calculateConvectionOrConductionFlow() override;

            void calcInBetweenShadeFlow(std::shared_ptr<CIGUVentilatedGapLayer> t_Gap1,
                                        std::shared_ptr<CIGUVentilatedGapLayer> t_Gap2);

            void calcEdgeShadeFlow(std::shared_ptr<CEnvironment> t_Environment,
                                   std::shared_ptr<CIGUVentilatedGapLayer> t_Gap);

            std::shared_ptr<CShadeOpenings> m_ShadeOpenings;
        };

    }   // namespace ISO15099

}   // namespace Tarcog


#endif
