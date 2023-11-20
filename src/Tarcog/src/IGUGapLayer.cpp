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

namespace Tarcog
{
    namespace ISO15099
    {
        CIGUGapLayer::CIGUGapLayer(double const t_Thickness, double const t_Pressure) :
            CBaseIGULayer(t_Thickness), m_Pressure(t_Pressure)
        {}

        CIGUGapLayer::CIGUGapLayer(double const t_Thickness,
                                   double const t_Pressure,
                                   const Gases::CGas & t_Gas) :
            CBaseIGULayer(t_Thickness), m_Pressure(t_Pressure), m_Gas(t_Gas)
        {}

        void CIGUGapLayer::connectToBackSide(std::shared_ptr<CBaseLayer> const & t_Layer)
        {
            CBaseLayer::connectToBackSide(t_Layer);
            m_Surface[Side::Back] = t_Layer->getSurface(Side::Front);
        }

        void CIGUGapLayer::calculateConvectionOrConductionFlow()
        {
            checkNextLayer();
            if(!isCalculated())
            {
                if(FenestrationCommon::isEqual(getThickness(), 0))
                {
                    throw std::runtime_error("Layer thickness is set to zero.");
                }

                convectiveH();
            }
        }

        void CIGUGapLayer::checkNextLayer() const
        {
            if(m_NextLayer != nullptr)
            {
                m_NextLayer->getGainFlow();
            }
        }

        double CIGUGapLayer::averageSurfaceTemperature()
        {
            return averageTemperature();
        }

        double CIGUGapLayer::calculateRayleighNumber()
        {
            using ConstantsData::GRAVITYCONSTANT;

            const auto tGapTemperature = averageSurfaceTemperature();
            const auto deltaTemp = std::abs(getSurface(Side::Back)->getTemperature()
                                            - getSurface(Side::Front)->getTemperature());

            const auto aProperties = m_Gas.getGasProperties();

            double ra = 0;
            if(!FenestrationCommon::isEqual(aProperties.m_Viscosity, 0))
            {   // if viscosity is zero then it is vacuum
                ra =
                  GRAVITYCONSTANT * pow(getThickness(), 3) * deltaTemp * aProperties.m_SpecificHeat
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
            const auto tGapTemperature = averageSurfaceTemperature();
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
                m_ConductiveConvectiveCoeff = m_ConductiveConvectiveCoeff + 2 * m_AirflowProperties.m_AirSpeed;
            }

            return m_ConductiveConvectiveCoeff;
        }

        double CIGUGapLayer::getGasTemperature()
        {
            return averageSurfaceTemperature();
        }

        double CIGUGapLayer::averageTemperature() const
        {
            double aveTemp = Gases::DefaultTemperature;
            if(areSurfacesInitialized())
            {
                aveTemp = (getSurface(Side::Front)->getTemperature()
                           + getSurface(Side::Back)->getTemperature())
                          / 2;
            }

            return aveTemp;
        }

        double CIGUGapLayer::getPressure()
        {
            if(m_SealedGapProperties.has_value())
            {
                auto Vini = getArea() * m_Thickness;
                auto modThickness = getThickness();
                auto Vgap = getArea() * modThickness;
                return m_SealedGapProperties->pressure * Vini * averageSurfaceTemperature()
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

    }   // namespace ISO15099

}   // namespace Tarcog
