#pragma once

#include <memory>
#include <map>

#include <WCECommon.hpp>
#include <WCEGases.hpp>

namespace FenestrationCommon
{
    enum class Side;
}

namespace Tarcog::ISO15099
{
    class Surface;

    class CLayerHeatFlow
    {
    public:
        virtual double getGainFlow() final;
        virtual double getConductionConvectionCoefficient() final;

        virtual void resetCalculated() final;
        virtual void setCalculated() final;
        virtual bool isCalculated() final;

    protected:
        virtual void calculateLayerHeatFlow() final;
        virtual void calculateRadiationFlow() = 0;
        virtual void calculateConvectionOrConductionFlow() = 0;

        double m_ConductiveConvectiveCoeff{0};
        double m_LayerGainFlow{0};

    private:
        bool m_IsCalculated{false};
    };

}   // namespace Tarcog::ISO15099
