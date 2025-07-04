#include <cmath>
#include <cassert>
#include <stdexcept>

#include "WCEGases.hpp"
#include "IGUVentilatedGapLayer.hpp"
#include "TarcogConstants.hpp"


namespace Tarcog::ISO15099
{
    Gases::GasProperties getGasPropertiesAtReferenceTemperatureAndPressure(Gases::CGas & gas,
                                                                           const double temperature,
                                                                           const double pressure)
    {
        auto refGas{gas};
        refGas.setTemperatureAndPressure(temperature, pressure);
        return refGas.getGasProperties();
    }

    CIGUVentilatedGapLayer::CIGUVentilatedGapLayer(GapLayer const & t_Layer) :
        CIGUGapLayer(*t_Layer),
        m_Layer(t_Layer),
        m_State{Gases::DefaultTemperature, Gases::DefaultTemperature},
        m_ReferenceGasProperties(getGasPropertiesAtReferenceTemperatureAndPressure(
          gasSpecification.gas, ReferenceTemperature, gasSpecification.pressure))
    {}

    CIGUVentilatedGapLayer::CIGUVentilatedGapLayer(const GapLayer & t_Layer,
                                                   double forcedVentilationInletTemperature,
                                                   double forcedVentilationInletSpeed) :
        CIGUGapLayer(*t_Layer),
        m_Layer(t_Layer),
        m_State{.inletTemperature = Gases::DefaultTemperature,  .outletTemperature = Gases::DefaultTemperature},
        m_ReferenceGasProperties(getGasPropertiesAtReferenceTemperatureAndPressure(
          gasSpecification.gas, ReferenceTemperature, gasSpecification.pressure)),
        m_ForcedVentilation(
          ForcedVentilation{forcedVentilationInletSpeed, forcedVentilationInletTemperature})
    {}

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
        gasSpecification.airflowProperties.airSpeed = t_speed;
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    double CIGUVentilatedGapLayer::getDrivingPressure()
    {
        using ConstantsData::GRAVITYCONSTANT;

        const auto gapTemperature = averageLayerTemperature();

        return m_ReferenceGasProperties.m_Density * ReferenceTemperature * GRAVITYCONSTANT
               * m_Height * std::abs(cos(FenestrationCommon::radians(m_Tilt - 90)))
               * std::abs(gapTemperature - m_State.inletTemperature)
               / (gapTemperature * m_State.inletTemperature);
    }

    double CIGUVentilatedGapLayer::bernoullyPressureTerm()
    {
        return 0.5 * gasSpecification.gas.getGasProperties().m_Density;
    }

    double CIGUVentilatedGapLayer::hagenPressureTerm()
    {
        return 12 * gasSpecification.gas.getGasProperties().m_Viscosity * m_Height
               / pow(getThickness(), 2);
    }

    double CIGUVentilatedGapLayer::pressureLossTerm()
    {
        return 0.5 * gasSpecification.gas.getGasProperties().m_Density * (m_Zin + m_Zout);
    }

    double CIGUVentilatedGapLayer::betaCoeff()
    {
        return exp(-m_Height / characteristicHeight());
    }

    void CIGUVentilatedGapLayer::smoothEnergyGain(double const qv1, double const qv2)
    {
        m_LayerGainFlow = (std::abs(qv1) + std::abs(qv2)) / 2;
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
            m_LayerGainFlow = ventilatedHeatGain();
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
        // Characteristic height can only be calculated after initialization is performed
        if(!FenestrationCommon::isEqual(m_ConductiveConvectiveCoeff, 0.0))
        {
            const auto aProperties = gasSpecification.gas.getGasProperties();
            return aProperties.m_Density * aProperties.m_SpecificHeat * getThickness()
                   * gasSpecification.airflowProperties.airSpeed
                   / (4 * m_ConductiveConvectiveCoeff);
        }
        return 0.0;
    }

    double CIGUVentilatedGapLayer::calcImpedance(double const t_A) const
    {
        if(!FenestrationCommon::isEqual(t_A, 0.0))
        {
            return pow(m_Width * getThickness() / (0.6 * t_A) - 1, 2);
        }

        return 0.0;
    }

    double CIGUVentilatedGapLayer::ventilatedHeatGain()
    {
        const auto aProperties = gasSpecification.gas.getGasProperties();
        return aProperties.m_Density * aProperties.m_SpecificHeat
               * gasSpecification.airflowProperties.airSpeed * getThickness()
               * (m_State.inletTemperature - m_State.outletTemperature) / m_Height;
    }

    double CIGUVentilatedGapLayer::calculateThermallyDrivenSpeed()
    {
        const double pressureLossTermValue{bernoullyPressureTerm() + pressureLossTerm()};
        const double hagenPressureTermValue{hagenPressureTerm()};
        return (sqrt(std::abs(pow(hagenPressureTermValue, 2)
                              + 4 * pressureLossTermValue * getDrivingPressure()))
                - hagenPressureTermValue)
               / (2 * pressureLossTermValue);
    }

    void CIGUVentilatedGapLayer::calculateOutletTemperatureFromAirFlow()
    {
        // Always use forced ventilation if exists.
        gasSpecification.airflowProperties.airSpeed = m_ForcedVentilation.has_value()
                                                          ? m_ForcedVentilation->speed
                                                          : calculateThermallyDrivenSpeed();
        const double beta{betaCoeff()};
        const double alpha{1 - beta};

        m_State.outletTemperature =
          alpha * averageSurfaceTemperature() + beta * m_State.inletTemperature;
    }

    double CIGUVentilatedGapLayer::calculateThermallyDrivenSpeedOfAdjacentGap(
      CIGUVentilatedGapLayer & adjacentGap)
    {
        const double ratio = getThickness() / adjacentGap.getThickness();
        const double A =
          bernoullyPressureTerm() + adjacentGap.pressureLossTerm()
          + pow(ratio, 2) * (adjacentGap.bernoullyPressureTerm() + adjacentGap.pressureLossTerm());
        const double B = hagenPressureTerm() + ratio * adjacentGap.hagenPressureTerm();
        gasSpecification.airflowProperties.airSpeed =
          (sqrt(std::abs(pow(B, 2.0) + 4 * A * getDrivingPressure())) - B) / (2.0 * A);
        return gasSpecification.airflowProperties.airSpeed / ratio;
    }

    VentilatedGapTemperatures
      CIGUVentilatedGapLayer::calculateInletAndOutletTemperaturesWithTheAdjacentGap(
        CIGUVentilatedGapLayer & adjacentGap,
        VentilatedGapTemperatures current,
        VentilatedGapTemperatures previous,
        const double relaxationParameter)
    {
        VentilatedGapTemperatures result;

        const double Tav1{averageSurfaceTemperature()};
        const double Tav2{adjacentGap.averageSurfaceTemperature()};

        adjacentGap.setFlowSpeed(calculateThermallyDrivenSpeedOfAdjacentGap(adjacentGap));

        const double beta1 = betaCoeff();
        const double beta2 = adjacentGap.betaCoeff();

        // Calculate common expressions
        const double commonOutletTemp{((1 - beta1) * Tav1 + beta1 * (1 - beta2) * Tav2)
                                      / (1 - beta1 * beta2)};
        const double commonInletTemp{(1 - beta2) * Tav2};

        // clang-format off
        const bool isTav1Greater = averageLayerTemperature() > adjacentGap.averageLayerTemperature();
        result.outletTemperature =
          isTav1Greater ?
          commonOutletTemp :
          commonInletTemp + beta2 * current.inletTemperature;

        result.inletTemperature =
          isTav1Greater ?
          commonInletTemp + beta2 * current.outletTemperature :
          commonOutletTemp;
        // clang-format on

        const auto Tup = relaxationParameter * result.outletTemperature
                         + (1 - relaxationParameter) * previous.outletTemperature;
        const auto Tdown = relaxationParameter * result.inletTemperature
                           + (1 - relaxationParameter) * previous.inletTemperature;

        setFlowTemperatures(Tup, Tdown);
        adjacentGap.setFlowTemperatures(Tdown, Tup);

        return result;
    }

    namespace Helper
    {
        bool isConverged(double TgapOut, double TgapOutOld, double tolerance)
        {
            return std::abs(TgapOut - TgapOutOld) < tolerance;
        }

        void adjustRelaxationParameter(double & relaxationParameter, size_t & iterationStep)
        {
            relaxationParameter -= IterationConstants::RELAXATION_PARAMETER_AIRFLOW_STEP;
            iterationStep = 0;
            if(relaxationParameter <= IterationConstants::RELAXATION_PARAMETER_AIRFLOW_MIN)
            {
                throw std::runtime_error("Airflow iterations fail to converge. "
                                         "Maximum number of iteration steps reached.");
            }
        }

    }   // namespace Helper

    double CIGUVentilatedGapLayer::performIterationStep(double relaxationParameter,
                                                        double & TgapOut)
    {
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
        double TgapOutOld = TgapOut;
        calculateOutletTemperatureFromAirFlow();
        const auto tempGap = averageLayerTemperature();
        TgapOut = relaxationParameter * tempGap + (1 - relaxationParameter) * TgapOutOld;
        return TgapOutOld;
    }

    void CIGUVentilatedGapLayer::calculateVentilatedAirflow(double inletTemperature)
    {
        setInletTemperature(inletTemperature);

        double relaxationParameter = IterationConstants::RELAXATION_PARAMETER_AIRFLOW;
        bool converged = false;
        size_t iterationStep = 0;
        double TgapOut = averageLayerTemperature();

        while(!converged)
        {
            const double TgapOutOld{performIterationStep(relaxationParameter, TgapOut)};

            converged = Helper::isConverged(
              TgapOut, TgapOutOld, IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW);

            ++iterationStep;
            if(iterationStep > IterationConstants::NUMBER_OF_STEPS)
            {
                Helper::adjustRelaxationParameter(relaxationParameter, iterationStep);
            }
        }
    }


    bool CIGUVentilatedGapLayer::isConverged(const VentilatedGapTemperatures & current,
                                             const VentilatedGapTemperatures & previous)
    {
        bool outletTempConverged = std::abs(current.outletTemperature - previous.outletTemperature)
                                   < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;
        bool inletTempConverged = std::abs(current.inletTemperature - previous.inletTemperature)
                                  < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;
        return outletTempConverged && inletTempConverged;
    }

    void CIGUVentilatedGapLayer::adjustTemperatures(CIGUVentilatedGapLayer & adjacentGap)
    {
        setInletTemperature(adjacentGap.averageLayerTemperature());
        adjacentGap.setInletTemperature(averageLayerTemperature());
    }

    void CIGUVentilatedGapLayer::performIterationStep(CIGUVentilatedGapLayer & adjacentGap,
                                                      VentilatedGapTemperatures & current,
                                                      double RelaxationParameter)
    {
        VentilatedGapTemperatures previous{current};
        current = calculateInletAndOutletTemperaturesWithTheAdjacentGap(
          adjacentGap, current, previous, RelaxationParameter);
    }

    void CIGUVentilatedGapLayer::calculateThermallyDrivenAirflowWithAdjacentGap(
      CIGUVentilatedGapLayer & adjacentGap)
    {
        double Tup = averageLayerTemperature();
        double Tdown = adjacentGap.averageLayerTemperature();
        VentilatedGapTemperatures current{Tdown, Tup};
        auto previous = current;
        double RelaxationParameter = IterationConstants::RELAXATION_PARAMETER_AIRFLOW;
        bool converged = false;
        size_t iterationStep = 0;

        while(!converged)
        {
            adjustTemperatures(adjacentGap);
            performIterationStep(adjacentGap, current, RelaxationParameter);
            converged = isConverged(current, previous);

            ++iterationStep;
            if(iterationStep > IterationConstants::NUMBER_OF_STEPS)
            {
                throw std::runtime_error("Airflow iterations fail to converge. Maximum number "
                                         "of iteration steps reached.");
            }

            const double qv1 = getGainFlow();
            const double qv2 = adjacentGap.getGainFlow();
            smoothEnergyGain(qv1, qv2);
            adjacentGap.smoothEnergyGain(qv1, qv2);

            previous = current;
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
