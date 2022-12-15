
#include <cmath>
#include <cassert>
#include <stdexcept>

#include "IGUVentilatedGapLayer.hpp"
#include "WCEGases.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        CIGUVentilatedGapLayer::CIGUVentilatedGapLayer(
          std::shared_ptr<CIGUGapLayer> const & t_Layer) :
            CIGUGapLayer(*t_Layer),
            m_Layer(t_Layer),
            m_inTemperature(Gases::DefaultTemperature),
            m_outTemperature(Gases::DefaultTemperature),
            m_Zin(0),
            m_Zout(0)
        {
            m_ReferenceGas = m_Gas;
            m_ReferenceGas.setTemperatureAndPressure(ReferenceTemperature, m_Pressure);
        }

        double CIGUVentilatedGapLayer::layerTemperature()
        {
            assert(m_Height != 0);
            const auto cHeight = characteristicHeight();
            const auto avTemp = averageTemperature();
            return avTemp - (cHeight / m_Height) * (m_outTemperature - m_inTemperature);
        }

        void CIGUVentilatedGapLayer::setFlowGeometry(double const t_Ain, double const t_Aout)
        {
            m_Ain = t_Aout;
            m_Aout = t_Ain;
        }

        void CIGUVentilatedGapLayer::setInletTemperature(double inletTemperature)
        {
            m_InletTemperature = inletTemperature;
            calculateImpedances(inletTemperature);
        }

        void CIGUVentilatedGapLayer::calculateImpedances(double inletTemperature)
        {
            m_AirVerticalDirection = layerTemperature() > inletTemperature
                                       ? AirVerticalDirection::Up
                                       : AirVerticalDirection::Down;

            m_Zin = calcImpedance(m_Ain);
            m_Zout = calcImpedance(m_Aout);

            resetCalculated();
        }

        void CIGUVentilatedGapLayer::setFlowTemperatures(double t_inTemperature,
                                                         double t_outTemperature)
        {
            m_inTemperature = t_inTemperature;
            m_outTemperature = t_outTemperature;
            resetCalculated();
        }

        void CIGUVentilatedGapLayer::setFlowSpeed(double const t_speed)
        {
            m_AirSpeed = t_speed;
            resetCalculated();
        }

        double CIGUVentilatedGapLayer::getDrivingPressure()
        {
            using ConstantsData::GRAVITYCONSTANT;
            using ConstantsData::WCE_PI;

            const auto tiltAngle = WCE_PI / 180 * (m_Tilt - 90);
            const auto gapTemperature = layerTemperature();
            const auto aProperties = m_ReferenceGas.getGasProperties();
            const auto temperatureMultiplier =
              std::abs(gapTemperature - m_InletTemperature) / (gapTemperature * m_InletTemperature);
            return aProperties.m_Density * ReferenceTemperature * GRAVITYCONSTANT * m_Height
                   * std::abs(cos(tiltAngle)) * temperatureMultiplier;
        }

        double CIGUVentilatedGapLayer::bernoullyPressureTerm()
        {
            const auto aGasProperties = m_Gas.getGasProperties();
            return 0.5 * aGasProperties.m_Density;
        }

        double CIGUVentilatedGapLayer::hagenPressureTerm()
        {
            const auto aGasProperties = m_Gas.getGasProperties();
            return 12 * aGasProperties.m_Viscosity * m_Height / pow(getThickness(), 2);
        }

        double CIGUVentilatedGapLayer::pressureLossTerm()
        {
            calculateImpedances(m_InletTemperature);
            const auto aGasProperties = m_Gas.getGasProperties();
            return 0.5 * aGasProperties.m_Density * (m_Zin + m_Zout);
        }

        double CIGUVentilatedGapLayer::betaCoeff()
        {
            calculateLayerHeatFlow();
            return exp(-m_Height / characteristicHeight());
        }

        void CIGUVentilatedGapLayer::smoothEnergyGain(double const qv1, double const qv2)
        {
            const auto smooth = (std::abs(qv1) + std::abs(qv2)) / 2;
            m_LayerGainFlow = smooth;
            if(m_inTemperature < m_outTemperature)
            {
                m_LayerGainFlow = -m_LayerGainFlow;
            }
        }

        void CIGUVentilatedGapLayer::calculateConvectionOrConductionFlow()
        {
            CIGUGapLayer::calculateConvectionOrConductionFlow();
            if(!isCalculated())
            {
                ventilatedFlow();
            }
        }

        double CIGUVentilatedGapLayer::characteristicHeight()
        {
            const auto aProperties = m_Gas.getGasProperties();
            double cHeight = 0;
            // Characteristic height can only be calculated after initialization is performed
            if(!FenestrationCommon::isEqual(m_ConductiveConvectiveCoeff, 0))
            {
                cHeight = aProperties.m_Density * aProperties.m_SpecificHeat * getThickness()
                          * m_AirSpeed / (4 * m_ConductiveConvectiveCoeff);
            }
            return cHeight;
        }

        double CIGUVentilatedGapLayer::calcImpedance(double const t_A) const
        {
            auto impedance = 0.0;

            if(!FenestrationCommon::isEqual(t_A, 0))
            {
                impedance = pow(m_Width * getThickness() / (0.6 * t_A) - 1, 2);
            }

            return impedance;
        }

        void CIGUVentilatedGapLayer::ventilatedFlow()
        {
            const auto aProperties = m_Gas.getGasProperties();
            m_LayerGainFlow = aProperties.m_Density * aProperties.m_SpecificHeat * m_AirSpeed
                              * getThickness() * (m_inTemperature - m_outTemperature) / m_Height;
        }

        double CIGUVentilatedGapLayer::calculateThermallyDrivenSpeed()
        {
            double drivingPressure = getDrivingPressure();
            double A = bernoullyPressureTerm() + pressureLossTerm();
            double B = hagenPressureTerm();
            return (sqrt(std::abs(pow(B, 2) + 4 * A * drivingPressure)) - B) / (2 * A);
        }

        void CIGUVentilatedGapLayer::calculateOutletTemperatureFromAirFlow()
        {
            if(!isVentilationForced())
            {
                m_AirSpeed = calculateThermallyDrivenSpeed();
            }
            double beta = betaCoeff();
            double alpha = 1 - beta;

            m_outTemperature = alpha * averageTemperature() + beta * m_InletTemperature;
            m_inTemperature = m_InletTemperature;
        }

        double CIGUVentilatedGapLayer::calculateThermallyDrivenSpeedOfAdjacentGap(
          CIGUVentilatedGapLayer & adjacentGap)
        {
            double ratio = getThickness() / adjacentGap.getThickness();
            double A1 = bernoullyPressureTerm() + adjacentGap.pressureLossTerm();
            double A2 = adjacentGap.bernoullyPressureTerm() + adjacentGap.pressureLossTerm();
            double B1 = hagenPressureTerm();
            double B2 = adjacentGap.hagenPressureTerm();
            double A = A1 + pow(ratio, 2) * A2;
            double B = B1 + ratio * B2;
            m_AirSpeed =
              (sqrt(std::abs(pow(B, 2.0) + 4 * A * getDrivingPressure())) - B) / (2.0 * A);
            return m_AirSpeed / ratio;
        }

        VentilatedTemperature
          CIGUVentilatedGapLayer::calculateInletAndOutletTemperaturesWithTheAdjecentGap(
            CIGUVentilatedGapLayer & adjacentGap,
            VentilatedTemperature current,
            VentilatedTemperature previous,
            double relaxationParameter)
        {
            VentilatedTemperature result;

            double tempGap1 = layerTemperature();
            double tempGap2 = adjacentGap.layerTemperature();
            double Tav1 = averageTemperature();
            double Tav2 = adjacentGap.averageTemperature();

            adjacentGap.setFlowSpeed(calculateThermallyDrivenSpeedOfAdjacentGap(adjacentGap));

            double beta1 = betaCoeff();
            double beta2 = adjacentGap.betaCoeff();
            double alpha1 = 1 - beta1;
            double alpha2 = 1 - beta2;

            if(tempGap1 > tempGap2)
            {
                result.outlet = (alpha1 * Tav1 + beta1 * alpha2 * Tav2) / (1 - beta1 * beta2);
                result.inlet = alpha2 * Tav2 + beta2 * current.outlet;
            }
            else
            {
                result.inlet = (alpha1 * Tav1 + beta1 * alpha2 * Tav2) / (1 - beta1 * beta2);
                result.outlet = alpha2 * Tav2 + beta2 * current.inlet;
            }

            const auto Tup = relaxationParameter * result.outlet + (1 - relaxationParameter) * previous.outlet;
            const auto Tdown = relaxationParameter * result.inlet + (1 - relaxationParameter) * previous.inlet;

            setFlowTemperatures(Tup, Tdown);
            adjacentGap.setFlowTemperatures(Tdown, Tup);

            return result;
        }

    }   // namespace ISO15099

}   // namespace Tarcog
