#include <cmath>

#include "IndoorEnvironment.hpp"
#include "Surface.hpp"

using FenestrationCommon::Side;


namespace Tarcog::ISO15099
{
    // Keep airspeed to zero and airdirection to default windward
    CIndoorEnvironment::CIndoorEnvironment(double const t_AirTemperature, double const t_Pressure) :
        CEnvironment(t_Pressure, 0, AirHorizontalDirection::Windward)
    {
        m_RoomRadiationTemperature = t_AirTemperature;   // Radiation temperature is by default air
        m_Surface[Side::Back] = std::make_shared<Surface>(m_Emissivity, 0);
        getSurface(Side::Back)->setTemperature(t_AirTemperature);
    }

    CIndoorEnvironment::CIndoorEnvironment(CIndoorEnvironment const & t_Indoor) :
        CEnvironment(t_Indoor)
    {
        operator=(t_Indoor);
    }

    CIndoorEnvironment & CIndoorEnvironment::operator=(CIndoorEnvironment const & t_Environment)
    {
        this->CEnvironment::operator=(t_Environment);
        m_RoomRadiationTemperature = t_Environment.m_RoomRadiationTemperature;

        return *this;
    }

    void CIndoorEnvironment::connectToIGULayer(std::shared_ptr<CBaseLayer> const & t_IGULayer)
    {
        t_IGULayer->connectToBackSide(shared_from_this());
    }

    void CIndoorEnvironment::setRoomRadiationTemperature(double const t_RadiationTemperature)
    {
        m_RoomRadiationTemperature = t_RadiationTemperature;
        resetCalculated();
    }

    std::shared_ptr<CBaseLayer> CIndoorEnvironment::clone() const
    {
        return cloneEnvironment();
    }

    std::shared_ptr<CEnvironment> CIndoorEnvironment::cloneEnvironment() const
    {
        return std::make_shared<CIndoorEnvironment>(*this);
    }

    double CIndoorEnvironment::getGasTemperature()
    {
        return surfaceTemperature(Side::Back);
    }

    double CIndoorEnvironment::calculateIRFromVariables()
    {
        using ConstantsData::STEFANBOLTZMANN;
        return STEFANBOLTZMANN * m_Emissivity * pow(m_RoomRadiationTemperature, 4);
    }

    double CIndoorEnvironment::hcFromAirSpeed()
    {
        return 4 + 4 * gasSpecification.airflowProperties.m_AirSpeed;
    }

    double CIndoorEnvironment::hcThermallyDriven()
    {
        using ConstantsData::GRAVITYCONSTANT;

        const auto tiltRadians{FenestrationCommon::radians(m_Tilt)};
        auto tMean =
          getGasTemperature() + 0.25 * (surfaceTemperature(Side::Front) - getGasTemperature());
        if(tMean < 0)
            tMean = 0.1;
        const auto deltaTemp = std::abs(surfaceTemperature(Side::Front) - getGasTemperature());
        gasSpecification.setTemperature(tMean);
        const auto aProperties = gasSpecification.gas.getGasProperties();
        const auto gr = GRAVITYCONSTANT * pow(m_Height, 3) * deltaTemp
                        * pow(aProperties.m_Density, 2) / (tMean * pow(aProperties.m_Viscosity, 2));
        const auto RaCrit = 2.5e5 * pow(exp(0.72 * m_Tilt) / sin(tiltRadians), 0.2);
        const auto RaL = gr * aProperties.m_PrandlNumber;
        auto Gnui = 0.0;
        if((0.0 <= m_Tilt) && (m_Tilt < 15.0))
        {
            Gnui = 0.13 * pow(RaL, 1 / 3.0);
        }
        else if((15.0 <= m_Tilt) && (m_Tilt <= 90.0))
        {
            if(RaL <= RaCrit)
            {
                Gnui = 0.56 * pow(RaL * sin(tiltRadians), 0.25);
            }
            else
            {
                Gnui = 0.13 * (pow(RaL, 1 / 3.0) - pow(RaCrit, 1 / 3.0))
                       + 0.56 * pow(RaCrit * sin(tiltRadians), 0.25);
            }
        }
        else if((90.0 < m_Tilt) && (m_Tilt <= 179.0))
        {
            Gnui = 0.56 * pow(RaL * sin(tiltRadians), 0.25);
        }
        else if((179.0 < m_Tilt) && (m_Tilt <= 180.0))
        {
            Gnui = 0.58 * pow(RaL, 1 / 3.0);
        }
        return Gnui * aProperties.m_ThermalConductivity / m_Height;
    }

    double CIndoorEnvironment::calculateHc()
    {
        return (gasSpecification.airflowProperties.m_AirSpeed > 0) ? hcFromAirSpeed()
                                                                   : hcThermallyDriven();
    }

    double CIndoorEnvironment::getHr()
    {
        return getRadiationFlow() / (getRadiationTemperature() - surfaceTemperature(Side::Front));
    }

    void CIndoorEnvironment::setIRFromEnvironment(double const t_IR)
    {
        m_Surface.at(Side::Back)->setJ(t_IR);
    }

    double CIndoorEnvironment::getIRFromEnvironment() const
    {
        return J(Side::Back);
    }

    double CIndoorEnvironment::getRadiationTemperature() const
    {
        return m_RoomRadiationTemperature;
    }
}   // namespace Tarcog::ISO15099
