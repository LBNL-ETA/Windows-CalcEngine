#pragma once

#include <memory>

#include "IGUSolidLayer.hpp"
#include "LayerTypes.hpp"

namespace Gases
{
    class CGas;
}

namespace Tarcog::ISO15099
{
    class Surface;

    class CIGUVentilatedGapLayer;

    class CEnvironment;

    auto constexpr OPENING_TOLERANCE = 1e-6;

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

        [[nodiscard]] double Aeq_bot() const;
        [[nodiscard]] double Aeq_top() const;

        [[nodiscard]] double frontPorosity() const;

        [[nodiscard]] bool isOpen() const;

        //! If gap next to shade is tighter than the opening, that gap will be dominant compared
        //! to the openning
        void checkAndSetDominantWidth(double gapWidth);

    private:
        [[nodiscard]] double openingMultiplier() const;

        double m_Atop{0};
        double m_Abot{0};
        double m_Aleft{0};
        double m_Aright{0};
        double m_Afront{0};
        double m_FrontPorosity{0};
    };

    class CIGUShadeLayer : public CIGUSolidLayer
    {
    public:
        CIGUShadeLayer(double t_Thickness,
                       double t_Conductivity,
                       CShadeOpenings && t_ShadeOpenings,
                       const std::shared_ptr<Tarcog::ISO15099::Surface> & t_FrontSurface = nullptr,
                       const std::shared_ptr<Tarcog::ISO15099::Surface> & t_BackSurface = nullptr);

        CIGUShadeLayer(const SolidLayer & t_Layer, const CShadeOpenings & t_ShadeOpenings);

        CIGUShadeLayer(double t_Thickness, double t_Conductivity);

        std::shared_ptr<CBaseLayer> clone() const override;

        bool isPermeable() const override;

    private:
        void setDominantAirflowWidth();

        void calculateConvectionOrConductionFlow() override;

        void calcInBetweenShadeFlow(CIGUVentilatedGapLayer & t_Gap1,
                                    CIGUVentilatedGapLayer & t_Gap2);

        void calcEdgeShadeFlow(CEnvironment & t_Environment, CIGUVentilatedGapLayer & t_Gap);

        double equivalentConductivity(double t_Conductivity, double permeabilityFactor);

        CShadeOpenings m_ShadeOpenings;

        double m_MaterialConductivity;
    };


}   // namespace Tarcog::ISO15099
