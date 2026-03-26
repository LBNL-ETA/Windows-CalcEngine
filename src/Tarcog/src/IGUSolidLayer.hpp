#pragma once

#include <memory>
#include <optional>

#include "BaseLayer.hpp"
#include "TarcogConstants.hpp"
#include "EffectiveMultipliers.hpp"

namespace FenestrationCommon
{
    enum class Side;
}

namespace Tarcog::ISO15099
{
    class CIGUGapLayer;
    class CIGUGapLayer;
    class CEnvironment;
    class Surface;

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
        [[nodiscard]] double effectiveFrontThermalOpennessArea() const;

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

    CShadeOpenings getShadeOpenings(double width, double height,
                                    const EffectiveLayers::EffectiveMultipliers & effectiveMultipliers);

    struct DeflectionMaterial
    {
        double youngsModulus{DeflectionConstants::YOUNGSMODULUS};
        double poissonRatio{DeflectionConstants::POISONRATIO};
        double density{MaterialConstants::GLASSDENSITY};
    };

    class CIGUSolidLayer : public CBaseLayer
    {
    public:
        CIGUSolidLayer(double t_Thickness,
                       double t_Conductivity,
                       const std::shared_ptr<Tarcog::ISO15099::Surface> & t_FrontSurface,
                       const std::shared_ptr<Tarcog::ISO15099::Surface> & t_BackSurface);

        CIGUSolidLayer(double t_Thickness,
                       double t_Conductivity,
                       double t_FrontEmissivity,
                       double t_FrontIRTransmittance,
                       double t_BackEmissivity,
                       double t_BackIRTransmittance);

        void connectToBackSide(const std::shared_ptr<CBaseLayer> & t_Layer) override;

        double getConductance() const;
        double getSolarAbsorptance() const;

        void setSolarAbsorptance(double const t_SolarAbsorptance);

        void setLayerState(double t_Tf, double t_Tb, double t_Jf, double t_Jb);
        void setSolarRadiation(double t_SolarRadiation);
        void setSolarHeatGain(double t_SolarAbsorptance, double t_SolarRadiation);

        double getRadiationFlow() override;

        [[nodiscard]] double youngsModulus() const;
        [[nodiscard]] double density() const;
        [[nodiscard]] double flexuralRigidity() const;

        double getMaxDeflection() const override;
        double getMeanDeflection() const override;

        bool isPermeable() const override;
        [[nodiscard]] bool isShadeLayer() const;

        std::shared_ptr<CBaseLayer> clone() const override;

        void applyDeflection(double meanDeflection, double maxDeflection);
        [[nodiscard]] bool hasMeasuredDeflection() const;
        void setMeasuredDeflection();

        void setConductivity(double t_Conductivity);
        void setDeflectionMaterial(const DeflectionMaterial & mat);

        void assignEffectiveMultipliers(
          const EffectiveLayers::EffectiveMultipliers & effectiveMultipliers);
        void markAsShadeLayer();

    protected:
        void calculateConvectionOrConductionFlow() override;

        double m_Conductivity;

    private:
        void calculateSolidConvectionOrConductionFlow();
        void calculateShadeConvectionOrConductionFlow();

        [[nodiscard]] CShadeOpenings getEffectiveOpenings() const;
        void calcInBetweenShadeFlow(CIGUGapLayer & gap1,
                                    CIGUGapLayer & gap2);
        void calcEdgeShadeFlow(CEnvironment & environment, CIGUGapLayer & gap);
        [[nodiscard]] double equivalentConductivity(double conductivity,
                                                    double permeabilityFactor);

        double m_SolarAbsorptance;
        std::optional<DeflectionMaterial> m_DeflectionMaterial;
        bool m_HasMeasuredDeflection{false};

        // Shade data (populated only for shade layers)
        bool m_IsShadeLayer{false};
        std::optional<EffectiveLayers::EffectiveMultipliers> m_ShadeMultipliers;
        double m_MaterialConductivity{0};
    };

}   // namespace Tarcog::ISO15099
