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

namespace Tarcog
{
    namespace ISO15099
    {
        class ISurface;

        struct ForcedVentilation
        {
            ForcedVentilation() : Speed(0){};

            ForcedVentilation(const double t_Speed) : Speed(t_Speed){};

            double Speed;
        };

        class CLayerGeometry : public virtual FenestrationCommon::CState
        {
        public:
            CLayerGeometry();

            virtual void setWidth(double t_Width) final;
            virtual void setHeight(double t_Height) final;
            virtual void setTilt(double t_Tilt) final;

        protected:
            double m_Width;
            double m_Height;
            double m_Tilt;
        };

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
            virtual std::shared_ptr<ISurface>
              getSurface(FenestrationCommon::Side t_Position) const final;
            virtual void setSurface(std::shared_ptr<ISurface> t_Surface,
                                    FenestrationCommon::Side t_Position) final;

        protected:
            virtual void calculateLayerHeatFlow() final;
            virtual void calculateRadiationFlow() = 0;
            virtual void calculateConvectionOrConductionFlow() = 0;

        protected:
            bool areSurfacesInitalized() const;

            std::map<FenestrationCommon::Side, std::shared_ptr<ISurface>> m_Surface;
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

        class CGasLayer : public virtual FenestrationCommon::CState
        {
        public:
            CGasLayer();
            explicit CGasLayer(double t_Pressure);
            CGasLayer(double const t_Pressure,
                      double const t_AirSpeed,
                      AirVerticalDirection const t_AirVerticalDirection);
            CGasLayer(double const t_Pressure,
                      double const t_AirSpeed,
                      AirHorizontalDirection const t_AirHorizontalDirection);
            CGasLayer(double t_Pressure, const ForcedVentilation & t_ForcedVentilation);
            CGasLayer(double t_Pressure, const Gases::CGas & t_Gas);

            virtual double getPressure();

            ForcedVentilation getForcedVentilation() const;

            bool isVentilationForced() const;

            virtual double getGasTemperature() = 0;

        protected:
            void initializeStateVariables() override;

            double m_Pressure;
            double m_AirSpeed;
            AirVerticalDirection m_AirVerticalDirection;
            AirHorizontalDirection m_AirHorizontalDirection;
            ForcedVentilation m_ForcedVentilation;
            bool m_IsVentilationForced;

            Gases::CGas m_Gas;
        };

    }   // namespace ISO15099

}   // namespace Tarcog

#endif
