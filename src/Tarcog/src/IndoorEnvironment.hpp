#pragma once

#include "Environment.hpp"


namespace Tarcog::ISO15099
{
    class CIndoorEnvironment : public CEnvironment
    {
    public:
        CIndoorEnvironment(double t_AirTemperature, double t_Pressure = 101325);
        CIndoorEnvironment(const CIndoorEnvironment & t_Indoor);
        CIndoorEnvironment & operator=(const CIndoorEnvironment & t_Environment);

        void connectToIGULayer(const std::shared_ptr<CBaseLayer> & t_IGULayer) override;

        void setRoomRadiationTemperature(double t_RadiationTemperature);

        std::shared_ptr<CBaseLayer> clone() const override;
        std::shared_ptr<CEnvironment> cloneEnvironment() const override;

    private:
        double getAirTemperature() override;
        double calculateIRFromVariables() override;

        [[nodiscard]] double hcFromAirSpeed();
        [[nodiscard]] double hcThermallyDriven();
        [[nodiscard]] double calculateHc() override;
        [[nodiscard]] double getHr() override;

        void setIRFromEnvironment(double t_IR) override;
        double getIRFromEnvironment() const override;

        double getRadiationTemperature() const override;

        double m_RoomRadiationTemperature{};
    };

}   // namespace Tarcog::ISO15099
