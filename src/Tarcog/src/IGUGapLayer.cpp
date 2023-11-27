#include <cmath>
#include <stdexcept>
#include <cassert>
#include <iostream>

#include "BaseShade.hpp"
#include "IGUGapLayer.hpp"
#include "Surface.hpp"
#include "NusseltNumber.hpp"
#include "WCEGases.hpp"
#include "WCECommon.hpp"


using FenestrationCommon::Side;


namespace Tarcog::ISO15099
{
    CIGUGapLayer::CIGUGapLayer(double const t_Thickness, double const t_Pressure) :
        CBaseLayer(t_Thickness), m_Pressure(t_Pressure)
    {}

    CIGUGapLayer::CIGUGapLayer(double const t_Thickness,
                               double const t_Pressure,
                               const Gases::CGas & t_Gas) :
        CBaseLayer(t_Thickness), m_Pressure(t_Pressure), m_Gas(t_Gas)
    {}

    void CIGUGapLayer::connectToBackSide(std::shared_ptr<CBaseLayer> const & t_Layer)
    {
        CBaseLayer::connectToBackSide(t_Layer);
        m_Surface[Side::Back] = t_Layer->getSurface(Side::Front);
    }

    void CIGUGapLayer::calculateConvectionOrConductionFlow()
    {
        calculateHeatFlowNextLayer();
        if(!isCalculated())
        {
            if(FenestrationCommon::isEqual(getThickness(), 0))
            {
                throw std::runtime_error("Layer thickness is set to zero.");
            }

            convectiveH();
        }
    }

    void CIGUGapLayer::calculateHeatFlowNextLayer() const
    {
        if(getNextLayer() != nullptr)
        {
            getNextLayer()->calculateLayerHeatFlow();
        }
    }

    double CIGUGapLayer::calculateRayleighNumber()
    {
        using ConstantsData::GRAVITYCONSTANT;

        const auto tGapTemperature = averageLayerTemperature();
        const auto deltaTemp =
          std::abs(getSurfaceTemperature(Side::Back) - getSurfaceTemperature(Side::Front));

        const auto aProperties = m_Gas.getGasProperties();

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

    double CIGUGapLayer::convectiveH()
    {
        const auto tGapTemperature = averageLayerTemperature();
        m_Gas.setTemperatureAndPressure(tGapTemperature, getPressure());
        const auto Ra = calculateRayleighNumber();
        const auto Asp = aspectRatio();
        CNusseltNumber nusseltNumber{};
        const auto aProperties = m_Gas.getGasProperties();
        if(!FenestrationCommon::isEqual(aProperties.m_Viscosity, 0))
        {
            m_ConductiveConvectiveCoeff = nusseltNumber.calculate(m_Tilt, Ra, Asp)
                                          * aProperties.m_ThermalConductivity / getThickness();
        }
        else
        {   // vacuum state
            m_ConductiveConvectiveCoeff = aProperties.m_ThermalConductivity;
        }
        if(!FenestrationCommon::isEqual(m_AirflowProperties.m_AirSpeed, 0))
        {
            m_ConductiveConvectiveCoeff =
              m_ConductiveConvectiveCoeff + 2 * m_AirflowProperties.m_AirSpeed;
        }

        return m_ConductiveConvectiveCoeff;
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
        return m_Pressure;
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

}   // namespace Tarcog::ISO15099
