#include <cmath>
#include <stdexcept>
#include <cassert>
#include <iostream>

#include <WCEGases.hpp>
#include <WCECommon.hpp>

#include "BaseShade.hpp"
#include "IGUGapLayer.hpp"
#include "Surface.hpp"
#include "NusseltNumber.hpp"


using FenestrationCommon::Side;


namespace Tarcog::ISO15099
{

    bool isStillAir(double airSpeed)
    {
        return FenestrationCommon::isEqual(airSpeed, 0);
    }

    CIGUGapLayer::CIGUGapLayer(double const t_Thickness, double const t_Pressure) :
        CBaseLayer(t_Thickness)
    {
        gasSpecification.pressure = t_Pressure;
    }

    CIGUGapLayer::CIGUGapLayer(double const t_Thickness,
                               double const t_Pressure,
                               const Gases::CGas & t_Gas) :
        CBaseLayer(t_Thickness)
    {
        gasSpecification.pressure = t_Pressure;
        gasSpecification.gas = t_Gas;
    }

    void CIGUGapLayer::connectToBackSide(std::shared_ptr<CBaseLayer> const & t_Layer)
    {
        CBaseLayer::connectToBackSide(t_Layer);
        m_Surface[Side::Back] = t_Layer->getSurface(Side::Front);
    }

    void CIGUGapLayer::calculateConvectionOrConductionFlow()
    {
        if(!isCalculated())
        {
            if(FenestrationCommon::isEqual(getThickness(), 0))
            {
                throw std::runtime_error("Layer thickness is set to zero.");
            }

            m_ConductiveConvectiveCoeff = convectiveH();
        }
    }

    double CIGUGapLayer::calculateRayleighNumber()
    {
        using ConstantsData::GRAVITYCONSTANT;

        const auto tGapTemperature = averageLayerTemperature();
        const auto deltaTemp =
          std::abs(getSurfaceTemperature(Side::Back) - getSurfaceTemperature(Side::Front));

        const auto aProperties = gasSpecification.gas.getGasProperties();

        double ra = 0;
        if(!FenestrationCommon::isEqual(aProperties.m_Viscosity, 0))
        {   // if viscosity is zero then it is vacuum
            ra = GRAVITYCONSTANT * pow(getThickness(), 3) * deltaTemp * aProperties.m_SpecificHeat
                 * pow(aProperties.m_Density, 2)
                 / (tGapTemperature * aProperties.m_Viscosity * aProperties.m_ThermalConductivity);
        }

        return ra;
    }

    double CIGUGapLayer::aspectRatio() const
    {
        if(FenestrationCommon::isEqual(getThickness(), 0))
        {
            throw std::runtime_error("Gap thickness is set to zero.");
        }
        return m_Height / getThickness();
    }

    double CIGUGapLayer::getPressure()
    {
        if(m_SealedGapProperties.has_value())
        {
            auto Vini = getSurfaceArea() * m_Thickness;
            auto modThickness = getThickness();
            auto Vgap = getSurfaceArea() * modThickness;
            return m_SealedGapProperties->pressure * Vini * averageLayerTemperature()
                   / (m_SealedGapProperties->temperature * Vgap);
        }
        return gasSpecification.pressure;
    }

    double CIGUGapLayer::getMaxDeflection() const
    {
        return m_Thickness + getSurface(Side::Front)->getMaxDeflection()
               - getSurface(Side::Back)->getMaxDeflection();
    }

    double CIGUGapLayer::getMeanDeflection() const
    {
        return getThickness();
    }

    std::shared_ptr<CBaseLayer> CIGUGapLayer::clone() const
    {
        return std::make_shared<CIGUGapLayer>(*this);
    }

    void CIGUGapLayer::setSealedGapProperties(double t_Temperature, double t_Pressure)
    {
        m_SealedGapProperties = SealedGapProperties(t_Temperature, t_Pressure);
    }

    void CIGUGapLayer::updateGasSpecifications()
    {
        gasSpecification.pressure = getPressure();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    double CIGUGapLayer::calculateConvectiveConductiveCoefficient()
    {
        auto gasProperties = gasSpecification.gas.getGasProperties();
        if(!FenestrationCommon::isVacuum(gasSpecification.pressure))
        {
            CNusseltNumber nusseltNumber{};
            return nusseltNumber.calculate(m_Tilt, calculateRayleighNumber(), aspectRatio())
                   * gasProperties.m_ThermalConductivity / getThickness();
        }
        else
        {
            // Handle vacuum state
            return gasProperties.m_ThermalConductivity;
        }
    }

    double CIGUGapLayer::addAirflowEffect(const double convectiveCoefficient)
    {
        if(!isStillAir(gasSpecification.airflowProperties.m_AirSpeed))
        {
            return convectiveCoefficient + 2 * gasSpecification.airflowProperties.m_AirSpeed;
        }
        return convectiveCoefficient;
    }

    double CIGUGapLayer::convectiveH()
    {
        updateGasSpecifications();

        auto coeff{calculateConvectiveConductiveCoefficient()};
        coeff = addAirflowEffect(coeff);

        return coeff;
    }

}   // namespace Tarcog::ISO15099
