#include <cmath>
#include <cassert>
#include <stdexcept>

#include "WCEGases.hpp"
#include "IGUVentilatedGapLayer.hpp"
#include "TarcogConstants.hpp"


namespace Tarcog::ISO15099
{
    VentilatedGapState::VentilatedGapState(double inletTemperature, double outletTemperature) :
        inletTemperature(inletTemperature), outletTemperature(outletTemperature)
    {}

    ForcedVentilation::ForcedVentilation(double speed, double temperature) :
        speed(speed), temperature(temperature)
    {}

    CIGUVentilatedGapLayer::CIGUVentilatedGapLayer(std::shared_ptr<CIGUGapLayer> const & t_Layer) :
        CIGUGapLayer(*t_Layer),
        m_Layer(t_Layer),
        m_State(Gases::DefaultTemperature, Gases::DefaultTemperature),
        m_Zin(0),
        m_Zout(0)
    {
        m_ReferenceGas = gasSpecification.gas;
        m_ReferenceGas.setTemperatureAndPressure(ReferenceTemperature, gasSpecification.pressure);
    }

    CIGUVentilatedGapLayer::CIGUVentilatedGapLayer(const std::shared_ptr<CIGUGapLayer> & t_Layer,
                                                   double forcedVentilationInletTemperature,
                                                   double forcedVentilationInletSpeed) :
        CIGUGapLayer(*t_Layer),
        m_Layer(t_Layer),
        m_State(Gases::DefaultTemperature, Gases::DefaultTemperature),
        m_Zin(0),
        m_Zout(0),
        m_ForcedVentilation(
          ForcedVentilation(forcedVentilationInletSpeed, forcedVentilationInletTemperature))
    {
        m_ReferenceGas = gasSpecification.gas;
        m_ReferenceGas.setTemperatureAndPressure(ReferenceTemperature, gasSpecification.pressure);
    }

    double CIGUVentilatedGapLayer::inletTemperature() const
    {
        return m_State.inletTemperature;
    }

    double CIGUVentilatedGapLayer::outletTemperature() const
    {
        return m_State.outletTemperature;
    }

    double CIGUVentilatedGapLayer::averageLayerTemperature()
    {
        assert(m_Height != 0);
        const auto cHeight = characteristicHeight();
        const auto avTemp = averageSurfaceTemperature();
        return avTemp
               - (cHeight / m_Height) * (m_State.outletTemperature - m_State.inletTemperature);
    }

    void CIGUVentilatedGapLayer::setFlowGeometry(double const t_Ain, double const t_Aout)
    {
        m_Zin = calcImpedance(t_Aout);
        m_Zout = calcImpedance(t_Ain);
    }

    void CIGUVentilatedGapLayer::setInletTemperature(double inletTemperature)
    {
        m_State.inletTemperature = inletTemperature;
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    void CIGUVentilatedGapLayer::setFlowTemperatures(double t_inTemperature,
                                                     double t_outTemperature)
    {
        m_State.inletTemperature = t_inTemperature;
        m_State.outletTemperature = t_outTemperature;
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    void CIGUVentilatedGapLayer::setFlowSpeed(double const t_speed)
    {
        gasSpecification.airflowProperties.m_AirSpeed = t_speed;
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    double CIGUVentilatedGapLayer::getDrivingPressure()
    {
        using ConstantsData::GRAVITYCONSTANT;
        using ConstantsData::WCE_PI;

        const auto tiltAngle = WCE_PI / 180 * (m_Tilt - 90);
        const auto gapTemperature = averageLayerTemperature();
        const auto aProperties = m_ReferenceGas.getGasProperties();
        const auto temperatureMultiplier = std::abs(gapTemperature - m_State.inletTemperature)
                                           / (gapTemperature * m_State.inletTemperature);
        return aProperties.m_Density * ReferenceTemperature * GRAVITYCONSTANT * m_Height
               * std::abs(cos(tiltAngle)) * temperatureMultiplier;
    }

    double CIGUVentilatedGapLayer::bernoullyPressureTerm()
    {
        const auto aGasProperties = gasSpecification.gas.getGasProperties();
        return 0.5 * aGasProperties.m_Density;
    }

    double CIGUVentilatedGapLayer::hagenPressureTerm()
    {
        const auto aGasProperties = gasSpecification.gas.getGasProperties();
        return 12 * aGasProperties.m_Viscosity * m_Height / pow(getThickness(), 2);
    }

    double CIGUVentilatedGapLayer::pressureLossTerm()
    {
        const auto aGasProperties = gasSpecification.gas.getGasProperties();
        return 0.5 * aGasProperties.m_Density * (m_Zin + m_Zout);
    }

    double CIGUVentilatedGapLayer::betaCoeff()
    {
        return exp(-m_Height / characteristicHeight());
    }

    void CIGUVentilatedGapLayer::smoothEnergyGain(double const qv1, double const qv2)
    {
        const auto smooth = (std::abs(qv1) + std::abs(qv2)) / 2;
        m_LayerGainFlow = smooth;
        if(m_State.inletTemperature < m_State.outletTemperature)
        {
            m_LayerGainFlow = -m_LayerGainFlow;
        }
    }

    void CIGUVentilatedGapLayer::calculateConvectionOrConductionFlow()
    {
        calculateHeatFlowNextLayer();
        CIGUGapLayer::calculateConvectionOrConductionFlow();
        if(!isCalculated())
        {
            ventilatedHeatGain();
        }
    }

    void CIGUVentilatedGapLayer::calculateHeatFlowNextLayer() const
    {
        if(getNextLayer() != nullptr)
        {
            // In case of thermally driven calculations, it is necessary to initialize gaps around
            // shading layer correctly and that initialization is performed by shading layer
            getNextLayer()->calculateLayerHeatFlow();
        }
    }

    double CIGUVentilatedGapLayer::characteristicHeight()
    {
        const auto aProperties = gasSpecification.gas.getGasProperties();
        double cHeight = 0;
        // Characteristic height can only be calculated after initialization is performed
        if(!FenestrationCommon::isEqual(m_ConductiveConvectiveCoeff, 0))
        {
            cHeight = aProperties.m_Density * aProperties.m_SpecificHeat * getThickness()
                      * gasSpecification.airflowProperties.m_AirSpeed
                      / (4 * m_ConductiveConvectiveCoeff);
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

    void CIGUVentilatedGapLayer::ventilatedHeatGain()
    {
        const auto aProperties = gasSpecification.gas.getGasProperties();
        m_LayerGainFlow = aProperties.m_Density * aProperties.m_SpecificHeat
                          * gasSpecification.airflowProperties.m_AirSpeed * getThickness()
                          * (m_State.inletTemperature - m_State.outletTemperature) / m_Height;
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
        // Always use forced ventilation if exists.
        gasSpecification.airflowProperties.m_AirSpeed = m_ForcedVentilation.has_value()
                                                          ? m_ForcedVentilation->speed
                                                          : calculateThermallyDrivenSpeed();
        double beta = betaCoeff();
        double alpha = 1 - beta;

        m_State.outletTemperature =
          alpha * averageSurfaceTemperature() + beta * m_State.inletTemperature;
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
        gasSpecification.airflowProperties.m_AirSpeed =
          (sqrt(std::abs(pow(B, 2.0) + 4 * A * getDrivingPressure())) - B) / (2.0 * A);
        return gasSpecification.airflowProperties.m_AirSpeed / ratio;
    }

    VentilatedGapState
      CIGUVentilatedGapLayer::calculateInletAndOutletTemperaturesWithTheAdjecentGap(
        CIGUVentilatedGapLayer & adjacentGap,
        VentilatedGapState current,
        VentilatedGapState previous,
        double relaxationParameter)
    {
        VentilatedGapState result;

        double tempGap1 = averageLayerTemperature();
        double tempGap2 = adjacentGap.averageLayerTemperature();
        double Tav1 = averageSurfaceTemperature();
        double Tav2 = adjacentGap.averageSurfaceTemperature();

        adjacentGap.setFlowSpeed(calculateThermallyDrivenSpeedOfAdjacentGap(adjacentGap));

        double beta1 = betaCoeff();
        double beta2 = adjacentGap.betaCoeff();
        double alpha1 = 1 - beta1;
        double alpha2 = 1 - beta2;

        if(tempGap1 > tempGap2)
        {
            result.outletTemperature =
              (alpha1 * Tav1 + beta1 * alpha2 * Tav2) / (1 - beta1 * beta2);
            result.inletTemperature = alpha2 * Tav2 + beta2 * current.outletTemperature;
        }
        else
        {
            result.inletTemperature = (alpha1 * Tav1 + beta1 * alpha2 * Tav2) / (1 - beta1 * beta2);
            result.outletTemperature = alpha2 * Tav2 + beta2 * current.inletTemperature;
        }

        const auto Tup = relaxationParameter * result.outletTemperature
                         + (1 - relaxationParameter) * previous.outletTemperature;
        const auto Tdown = relaxationParameter * result.inletTemperature
                           + (1 - relaxationParameter) * previous.inletTemperature;

        setFlowTemperatures(Tup, Tdown);
        adjacentGap.setFlowTemperatures(Tdown, Tup);

        return result;
    }

    void CIGUVentilatedGapLayer::calculateVentilatedAirflow(double inletTemperature)
    {
        setInletTemperature(inletTemperature);

        double RelaxationParameter = IterationConstants::RELAXATION_PARAMETER_AIRFLOW;
        bool converged = false;
        size_t iterationStep = 0;
        double TgapOut = averageLayerTemperature();
        while(!converged)
        {
            resetCalculated();
            gasSpecification.setTemperature(averageLayerTemperature());

            double TgapOutOld = TgapOut;

            calculateOutletTemperatureFromAirFlow();

            const auto tempGap = averageLayerTemperature();

            TgapOut = RelaxationParameter * tempGap + (1 - RelaxationParameter) * TgapOutOld;

            converged =
              std::abs(TgapOut - TgapOutOld) < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;

            ++iterationStep;
            if(iterationStep > IterationConstants::NUMBER_OF_STEPS)
            {
                RelaxationParameter -= IterationConstants::RELAXATION_PARAMETER_AIRFLOW_STEP;
                iterationStep = 0;
                if(RelaxationParameter == IterationConstants::RELAXATION_PARAMETER_AIRFLOW_MIN)
                {
                    converged = true;
                    throw std::runtime_error("Airflow iterations fail to converge. "
                                             "Maximum number of iteration steps reached.");
                }
            }
        }
    }

    void CIGUVentilatedGapLayer::calculateThermallyDrivenAirflowWithAdjacentGap(
      CIGUVentilatedGapLayer & adjacentGap)
    {
        double Tup = averageLayerTemperature();
        double Tdown = adjacentGap.averageLayerTemperature();
        VentilatedGapState current{Tdown, Tup};
        double RelaxationParameter = IterationConstants::RELAXATION_PARAMETER_AIRFLOW;
        bool converged = false;
        size_t iterationStep = 0;

        while(!converged)
        {
            setInletTemperature(adjacentGap.averageLayerTemperature());
            adjacentGap.setInletTemperature(averageLayerTemperature());

            VentilatedGapState previous{current};

            current = calculateInletAndOutletTemperaturesWithTheAdjecentGap(
              adjacentGap, current, previous, RelaxationParameter);

            converged = std::abs(current.outletTemperature - previous.outletTemperature)
                        < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;
            converged = converged
                        && std::abs(current.inletTemperature - previous.inletTemperature)
                             < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;

            ++iterationStep;
            if(iterationStep > IterationConstants::NUMBER_OF_STEPS)
            {
                converged = true;
                throw std::runtime_error("Airflow iterations fail to converge. Maximum number "
                                         "of iteration steps reached.");
            }
            double qv1 = getGainFlow();
            double qv2 = adjacentGap.getGainFlow();
            smoothEnergyGain(qv1, qv2);
            adjacentGap.smoothEnergyGain(qv1, qv2);
        }
    }

    std::shared_ptr<CBaseLayer> CIGUVentilatedGapLayer::clone() const
    {
        return std::make_shared<CIGUVentilatedGapLayer>(*this);
    }

    void CIGUVentilatedGapLayer::precalculateState()
    {
        if(m_ForcedVentilation.has_value())
        {
            calculateVentilatedAirflow(m_ForcedVentilation->temperature);
        }
    }
}   // namespace Tarcog::ISO15099
