#pragma once

#include <memory>

#include "BaseLayer.hpp"
#include "DeflectionInterface.hpp"

namespace FenestrationCommon
{
    enum class Side;
}

namespace Tarcog::ISO15099
{
    class CIGUSolidLayer : public CBaseLayer, public Tarcog::Deflectable
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

        void setLayerState(double t_Tf, double t_Tb, double t_Jf, double t_Jb);
        void setSolarRadiation(double t_SolarRadiation);
        void setSolarHeatGain(double t_SolarAbsorptance, double t_SolarRadiation);

        double getRadiationFlow() override;

        virtual double youngsModulus() const;

        double getMaxDeflection() const override;
        double getMeanDeflection() const override;

        virtual double density() const;

        std::shared_ptr<CBaseLayer> clone() const override;

        void applyDeflection(double meanDeflection, double maxDeflection);

        void setConductivity(double t_Conductivity);

    protected:
        void calculateConvectionOrConductionFlow() override;

        double m_Conductivity;
    private:

        double m_SolarAbsorptance;
    };

}   // namespace Tarcog::ISO15099
