#ifndef LAYERINTERFACES_H
#define LAYERINTERFACES_H

#include <memory>
#include <map>

#include "WCECommon.hpp"
#include "WCEGases.hpp"

namespace FenestrationCommon
{
    enum class Side;
}


namespace Tarcog::ISO15099
{
    class Surface;

    class CLayerHeatFlow : public virtual FenestrationCommon::CState
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

    protected:
        virtual void calculateLayerHeatFlow() final;
        virtual void calculateRadiationFlow() = 0;
        virtual void calculateConvectionOrConductionFlow() = 0;
        [[nodiscard]] bool areSurfacesInitialized() const;

        std::map<FenestrationCommon::Side, std::shared_ptr<Surface>> m_Surface;
        double m_ConductiveConvectiveCoeff;
        double m_LayerGainFlow;
    };

    enum class AirVerticalDirection
    {
        None,
        Up,
        Down
    };

    enum class AirHorizontalDirection
    {
        None,
        Leeward,
        Windward
    };

    struct SealedGapProperties
    {
        SealedGapProperties(double t_Temperature, double t_Pressure);
        double temperature;
        double pressure;
    };

    class CGasLayer : public virtual FenestrationCommon::CState
    {
    public:
        CGasLayer();
        explicit CGasLayer(double t_Pressure);
        CGasLayer(double t_Pressure,
                  double t_AirSpeed,
                  AirVerticalDirection t_AirVerticalDirection);
        CGasLayer(double t_Pressure,
                  double t_AirSpeed,
                  AirHorizontalDirection t_AirHorizontalDirection);
        CGasLayer(double t_Pressure, const Gases::CGas & t_Gas);

        virtual double getPressure();

        void setSealedGapProperties(double t_Temperature, double t_Pressure);

        [[nodiscard]] bool isVentilationForced() const;

        virtual double getGasTemperature() = 0;

    protected:
        void initializeStateVariables() override;

        double m_Pressure;
        double m_AirSpeed;
        AirVerticalDirection m_AirVerticalDirection;
        AirHorizontalDirection m_AirHorizontalDirection;
        bool m_IsVentilationForced;

        // Gap by default will not be considered to be sealed. If not sealed then
        // pressure will be considered to be m_Pressure;
        std::optional<SealedGapProperties> m_SealedGapProperties{std::nullopt};

        Gases::CGas m_Gas;
    };

}   // namespace Tarcog::ISO15099


#endif
