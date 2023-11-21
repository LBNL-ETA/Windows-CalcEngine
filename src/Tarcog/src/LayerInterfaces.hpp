#pragma once

#include <memory>
#include <map>

#include "WCECommon.hpp"
#include "WCEGases.hpp"

#include "AirFlow.hpp"

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
        CLayerHeatFlow();

        CLayerHeatFlow(const CLayerHeatFlow & t_Layer);
        CLayerHeatFlow & operator=(const CLayerHeatFlow & t_Layer);
        virtual double getHeatFlow() final;
        virtual double getGainFlow() final;
        virtual double getConductionConvectionCoefficient() final;
        virtual double getRadiationFlow();
        virtual double getConvectionConductionFlow() final;

        virtual std::shared_ptr<Surface>
          getSurface(FenestrationCommon::Side t_Position) const final;
        virtual void setSurface(std::shared_ptr<Surface> t_Surface,
                                FenestrationCommon::Side t_Position) final;

        [[nodiscard]] double getSurfaceTemperature(FenestrationCommon::Side t_Position) const;
        [[nodiscard]] double J(FenestrationCommon::Side t_Position) const;

        virtual double averageLayerTemperature();
        [[nodiscard]] double averageSurfaceTemperature() const;

        virtual void resetCalculated() final;
        virtual void setCalculated() final;
        virtual bool isCalculated() final;

    protected:
        virtual void calculateLayerHeatFlow() final;
        virtual void calculateRadiationFlow() = 0;
        virtual void calculateConvectionOrConductionFlow() = 0;

        std::map<FenestrationCommon::Side, std::shared_ptr<Surface>> m_Surface;
        double m_ConductiveConvectiveCoeff;
        double m_LayerGainFlow;

        bool m_IsCalculated{false};
    };

}   // namespace Tarcog::ISO15099
