#include <cmath>
#include <cassert>
#include <stdexcept>

#include <WCEGases.hpp>
#include <WCECommon.hpp>

#include "IGUGapLayer.hpp"
#include "IGUSolidLayer.hpp"
#include "Surface.hpp"
#include "NusseltNumber.hpp"
#include "TarcogConstants.hpp"


using FenestrationCommon::Side;


namespace Helper
{
    bool isConverged(double TgapOut, double TgapOutOld, double tolerance)
    {
        return std::abs(TgapOut - TgapOutOld) < tolerance;
    }

    struct RelaxationState
    {
        double relaxationParameter;
        size_t iterationStep;
    };

    RelaxationState adjustRelaxationParameter(RelaxationState state)
    {
        state.relaxationParameter -= Tarcog::IterationConstants::RELAXATION_PARAMETER_AIRFLOW_STEP;
        state.iterationStep = 0;
        if(state.relaxationParameter
           <= Tarcog::IterationConstants::RELAXATION_PARAMETER_AIRFLOW_MIN)
        {
            throw std::runtime_error("Airflow iterations fail to converge. "
                                     "Maximum number of iteration steps reached.");
        }
        return state;
    }

    template<typename State, typename StepFunction>
    void iterateUntilConverged(State & state, double & lastDelta, StepFunction && stepFn)
    {
        const double tinyDeltaThreshold = 0.01;
        const size_t maxTinyDeltas = 10;
        const size_t maxIterations = 100;

        size_t tinyDeltaCount = 0;
        bool converged = false;

        while(!converged)
        {
            auto [delta, justConverged] = stepFn(state);
            converged = justConverged;
            ++state.iterationStep;

            if(delta < tinyDeltaThreshold)
            {
                ++tinyDeltaCount;
            }
            else
            {
                tinyDeltaCount = 0;
            }

            if(state.iterationStep % Tarcog::IterationConstants::NUMBER_OF_STEPS == 0)
            {
                state = adjustRelaxationParameter(state);
            }

            if(state.iterationStep > maxIterations || tinyDeltaCount > maxTinyDeltas)
            {
                break;
            }

            lastDelta = delta;
        }
    }
}   // namespace Helper


namespace Tarcog::ISO15099
{

    Gases::GasProperties getGasPropertiesAtReferenceTemperatureAndPressure(
      Gases::CGas & gas,
      const double temperature,
      const double pressure)
    {
        auto refGas{gas};
        refGas.setTemperatureAndPressure(temperature, pressure);
        return refGas.getGasProperties();
    }

    bool isStillAir(double airSpeed)
    {
        return FenestrationCommon::isEqual(airSpeed, 0.0);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CIGUGapLayer -- constructors
    ////////////////////////////////////////////////////////////////////////////

    CIGUGapLayer::CIGUGapLayer(double const t_Thickness,
                               double const t_Pressure,
                               bool t_isDCenterMeasured) :
        CBaseLayer(t_Thickness), m_isDCenterMeasured(t_isDCenterMeasured)
    {
        gasSpecification.pressure = t_Pressure;
    }

    CIGUGapLayer::CIGUGapLayer(double const t_Thickness,
                               double const t_Pressure,
                               const Gases::CGas & t_Gas,
                               bool t_isDCenterMeasured) :
        CBaseLayer(t_Thickness), m_isDCenterMeasured(t_isDCenterMeasured)
    {
        gasSpecification.pressure = t_Pressure;
        gasSpecification.gas = t_Gas;
    }

    CIGUGapLayer::CIGUGapLayer(double t_Thickness,
                               double t_Pressure,
                               const Gases::CGas & t_Gas,
                               double t_AccommodationCoefficient1,
                               double t_AccommodationCoefficient2,
                               bool t_isDCenterMeasured) :
        CBaseLayer(t_Thickness),
        m_AccommodationCoefficient1(t_AccommodationCoefficient1),
        m_AccommodationCoefficient2(t_AccommodationCoefficient2),
        m_isDCenterMeasured(t_isDCenterMeasured)
    {
        gasSpecification.pressure = t_Pressure;
        gasSpecification.gas = t_Gas;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CIGUGapLayer -- base gap methods
    ////////////////////////////////////////////////////////////////////////////

    void CIGUGapLayer::connectToBackSide(std::shared_ptr<CBaseLayer> const & t_Layer)
    {
        CBaseLayer::connectToBackSide(t_Layer);
        m_Surface[Side::Back] = t_Layer->getSurface(Side::Front);
    }

    void CIGUGapLayer::calculateConvectionOrConductionFlow()
    {
        if(m_PillarMeasuredConductance.has_value())
        {
            if(!isCalculated())
            {
                m_ConductiveConvectiveCoeff = m_PillarMeasuredConductance.value();
                setCalculated();
            }
            return;
        }

        if(m_IsVentilated)
        {
            calculateHeatFlowNextLayer();
        }

        if(!isCalculated())
        {
            if(FenestrationCommon::isEqual(getThickness(), 0.0))
            {
                throw std::runtime_error("Layer thickness is set to zero.");
            }

            m_ConductiveConvectiveCoeff = convectiveH();
        }

        if(m_PillarAreaOfContactFn && !isCalculated())
        {
            m_ConductiveConvectiveCoeff += conductivityOfPillarArray();
        }

        if(m_IsVentilated && !isCalculated())
        {
            m_LayerGainFlow = ventilatedHeatGain();
        }
    }

    double CIGUGapLayer::calculateRayleighNumber()
    {
        if(!FenestrationCommon::isVacuum(gasSpecification.pressure))
        {
            using ConstantsData::GRAVITYCONSTANT;
            const auto aProperties = gasSpecification.gas.getGasProperties();
            const auto deltaTemp =
              std::abs(surfaceTemperature(Side::Back) - surfaceTemperature(Side::Front));

            return GRAVITYCONSTANT * pow(getThickness(), 3) * deltaTemp * aProperties.m_SpecificHeat
                   * pow(aProperties.m_Density, 2)
                   / (averageLayerTemperature() * aProperties.m_Viscosity
                      * aProperties.m_ThermalConductivity);
        }

        return 0;
    }

    double CIGUGapLayer::aspectRatio() const
    {
        if(FenestrationCommon::isEqual(getThickness(), 0.0))
        {
            throw std::runtime_error("Gap thickness is set to zero.");
        }
        return m_Height / getThickness();
    }

    double CIGUGapLayer::getPressure()
    {
        if(m_ManufacturingConditions.has_value())
        {
            auto Vini = getSurfaceArea() * m_Thickness;
            auto modThickness = getThickness();
            auto Vgap = getSurfaceArea() * modThickness;
            return m_ManufacturingConditions->pressure * Vini * averageLayerTemperature()
                   / (m_ManufacturingConditions->temperature * Vgap);
        }
        return gasSpecification.pressure;
    }

    double CIGUGapLayer::getThickness() const
    {
        auto thickness = CBaseLayer::getThickness();
        if(m_isDCenterMeasured)
        {
            auto prevSolid = std::dynamic_pointer_cast<CIGUSolidLayer>(getPreviousLayer());
            if(prevSolid != nullptr && prevSolid->isShadeLayer())
            {
                thickness -= prevSolid->getThickness() / 2;
            }
            auto nxtSolid = std::dynamic_pointer_cast<CIGUSolidLayer>(getNextLayer());
            if(nxtSolid != nullptr && nxtSolid->isShadeLayer())
            {
                thickness -= nxtSolid->getThickness() / 2;
            }
        }

        return thickness;
    }

    double CIGUGapLayer::getMaxDeflection() const
    {
        return m_Thickness + surfaceDeflectionMax(Side::Front) - surfaceDeflectionMax(Side::Back);
    }

    double CIGUGapLayer::getMeanDeflection() const
    {
        return getThickness();
    }

    std::shared_ptr<CBaseLayer> CIGUGapLayer::clone() const
    {
        return std::make_shared<CIGUGapLayer>(*this);
    }

    void CIGUGapLayer::setManufacturingConditions(double t_Temperature, double t_Pressure)
    {
        m_ManufacturingConditions = ManufacturingConditions(t_Temperature, t_Pressure);
    }

    void CIGUGapLayer::updateGasSpecifications()
    {
        gasSpecification.pressure = getPressure();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    double CIGUGapLayer::calculateConvectiveConductiveCoefficient()
    {
        auto gasProperties = gasSpecification.gas.getGasProperties(m_AccommodationCoefficient1,
                                                                   m_AccommodationCoefficient2);
        if(!FenestrationCommon::isVacuum(gasSpecification.pressure))
        {
            CNusseltNumber nusseltNumber{};
            return nusseltNumber.calculate(m_Tilt, calculateRayleighNumber(), aspectRatio())
                   * gasProperties.m_ThermalConductivity / getThickness();
        }
        return gasProperties.m_ThermalConductivity;
    }

    double CIGUGapLayer::addAirflowEffect(const double convectiveCoefficient)
    {
        if(!isStillAir(gasSpecification.airflowProperties.airSpeed))
        {
            return convectiveCoefficient + 2 * gasSpecification.airflowProperties.airSpeed;
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

    GasSpecification CIGUGapLayer::getGasSpecification() const
    {
        return gasSpecification;
    }

    double CIGUGapLayer::averageLayerTemperature()
    {
        if(!m_IsVentilated)
        {
            return SurfacesManager::averageLayerTemperature();
        }

        assert(m_Height != 0);
        const auto cHeight = characteristicHeight();
        const auto avTemp = averageSurfaceTemperature();
        return avTemp
               - (cHeight / m_Height)
                   * (m_VentState.outletTemperature - m_VentState.inletTemperature);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CIGUGapLayer -- pillar activation
    ////////////////////////////////////////////////////////////////////////////

    void CIGUGapLayer::activatePillar(double materialConductivity,
                                      double cellArea,
                                      PillarAreaFn areaOfContactFn,
                                      PillarResistanceFn thermalResistanceFn)
    {
        m_PillarMaterialConductivity = materialConductivity;
        m_PillarCellArea = cellArea;
        m_PillarAreaOfContactFn = std::move(areaOfContactFn);
        m_PillarResistanceFn = std::move(thermalResistanceFn);
    }

    void CIGUGapLayer::activatePillar(double measuredConductance)
    {
        m_PillarMeasuredConductance = measuredConductance;
    }

    double CIGUGapLayer::conductivityOfPillarArray()
    {
        const auto area = m_PillarAreaOfContactFn();
        const auto resistance = m_PillarResistanceFn(
          getPreviousLayer()->getConductivity(),
          getNextLayer()->getConductivity(),
          m_PillarMaterialConductivity,
          m_Thickness,
          area);
        return 1 / (m_PillarCellArea * resistance);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CIGUGapLayer -- ventilation activation
    ////////////////////////////////////////////////////////////////////////////

    void CIGUGapLayer::activateVentilation()
    {
        m_IsVentilated = true;
        m_VentState = {Gases::DefaultTemperature, Gases::DefaultTemperature};
        m_ReferenceGasProperties = getGasPropertiesAtReferenceTemperatureAndPressure(
          gasSpecification.gas, ReferenceTemperature, gasSpecification.pressure);
    }

    void CIGUGapLayer::activateVentilation(double forcedVentilationInletTemperature,
                                           double forcedVentilationInletSpeed)
    {
        activateVentilation();
        m_ForcedVentilation = ForcedVentilation{forcedVentilationInletSpeed,
                                                forcedVentilationInletTemperature};
    }

    bool CIGUGapLayer::isVentilated() const
    {
        return m_IsVentilated;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CIGUGapLayer -- ventilation flow methods
    ////////////////////////////////////////////////////////////////////////////

    void CIGUGapLayer::setFlowGeometry(double const t_Ain, double const t_Aout)
    {
        m_Zin = calcImpedance(t_Aout);
        m_Zout = calcImpedance(t_Ain);
    }

    void CIGUGapLayer::setInletTemperature(double inletTemperature)
    {
        m_VentState.inletTemperature = inletTemperature;
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    void CIGUGapLayer::setFlowTemperatures(double t_inTemperature, double t_outTemperature)
    {
        m_VentState.inletTemperature = t_inTemperature;
        m_VentState.outletTemperature = t_outTemperature;
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    void CIGUGapLayer::setFlowSpeed(double const t_speed)
    {
        gasSpecification.airflowProperties.airSpeed = t_speed;
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
    }

    double CIGUGapLayer::getDrivingPressure()
    {
        using ConstantsData::GRAVITYCONSTANT;

        const auto gapTemperature = averageLayerTemperature();

        return m_ReferenceGasProperties.m_Density * ReferenceTemperature * GRAVITYCONSTANT
               * m_Height * std::abs(cos(FenestrationCommon::radians(m_Tilt - 90)))
               * std::abs(gapTemperature - m_VentState.inletTemperature)
               / (gapTemperature * m_VentState.inletTemperature);
    }

    double CIGUGapLayer::bernoullyPressureTerm()
    {
        return 0.5 * gasSpecification.gas.getGasProperties().m_Density;
    }

    double CIGUGapLayer::hagenPressureTerm()
    {
        return 12 * gasSpecification.gas.getGasProperties().m_Viscosity * m_Height
               / pow(getThickness(), 2);
    }

    double CIGUGapLayer::pressureLossTerm()
    {
        return 0.5 * gasSpecification.gas.getGasProperties().m_Density * (m_Zin + m_Zout);
    }

    double CIGUGapLayer::betaCoeff()
    {
        return exp(-m_Height / characteristicHeight());
    }

    void CIGUGapLayer::smoothEnergyGain(double const qv1, double const qv2)
    {
        m_LayerGainFlow = (std::abs(qv1) + std::abs(qv2)) / 2;
        if(m_VentState.inletTemperature < m_VentState.outletTemperature)
        {
            m_LayerGainFlow = -m_LayerGainFlow;
        }
    }

    void CIGUGapLayer::calculateHeatFlowNextLayer() const
    {
        if(getNextLayer() != nullptr)
        {
            getNextLayer()->calculateLayerHeatFlow();
        }
    }

    double CIGUGapLayer::characteristicHeight()
    {
        if(!FenestrationCommon::isEqual(m_ConductiveConvectiveCoeff, 0.0))
        {
            const auto aProperties = gasSpecification.gas.getGasProperties();
            return aProperties.m_Density * aProperties.m_SpecificHeat * getThickness()
                   * gasSpecification.airflowProperties.airSpeed
                   / (4 * m_ConductiveConvectiveCoeff);
        }
        return 0.0;
    }

    double CIGUGapLayer::calcImpedance(double const t_A) const
    {
        if(!FenestrationCommon::isEqual(t_A, 0.0))
        {
            return pow(m_Width * getThickness() / (0.6 * t_A) - 1, 2);
        }
        return 0.0;
    }

    double CIGUGapLayer::ventilatedHeatGain()
    {
        const auto aProperties = gasSpecification.gas.getGasProperties();
        return aProperties.m_Density * aProperties.m_SpecificHeat
               * gasSpecification.airflowProperties.airSpeed * getThickness()
               * (m_VentState.inletTemperature - m_VentState.outletTemperature) / m_Height;
    }

    double CIGUGapLayer::calculateThermallyDrivenSpeed()
    {
        const double pressureLossTermValue{bernoullyPressureTerm() + pressureLossTerm()};
        const double hagenPressureTermValue{hagenPressureTerm()};
        return (sqrt(std::abs(pow(hagenPressureTermValue, 2)
                              + 4 * pressureLossTermValue * getDrivingPressure()))
                - hagenPressureTermValue)
               / (2 * pressureLossTermValue);
    }

    void CIGUGapLayer::calculateOutletTemperatureFromAirFlow()
    {
        gasSpecification.airflowProperties.airSpeed = m_ForcedVentilation.has_value()
                                                        ? m_ForcedVentilation->speed
                                                        : calculateThermallyDrivenSpeed();
        const double beta{betaCoeff()};
        const double alpha{1 - beta};

        m_VentState.outletTemperature =
          alpha * averageSurfaceTemperature() + beta * m_VentState.inletTemperature;
    }

    double CIGUGapLayer::calculateThermallyDrivenSpeedOfAdjacentGap(CIGUGapLayer & adjacentGap)
    {
        const double ratio = getThickness() / adjacentGap.getThickness();
        const double aaa =
          bernoullyPressureTerm() + adjacentGap.pressureLossTerm()
          + pow(ratio, 2)
              * (adjacentGap.bernoullyPressureTerm() + adjacentGap.pressureLossTerm());
        const double bbb = hagenPressureTerm() + ratio * adjacentGap.hagenPressureTerm();
        gasSpecification.airflowProperties.airSpeed =
          (sqrt(std::abs(pow(bbb, 2.0) + 4 * aaa * getDrivingPressure())) - bbb) / (2.0 * aaa);
        return gasSpecification.airflowProperties.airSpeed / ratio;
    }

    VentilatedGapTemperatures
      CIGUGapLayer::calculateInletAndOutletTemperaturesWithTheAdjacentGap(
        CIGUGapLayer & adjacentGap,
        VentilatedGapTemperatures current,
        VentilatedGapTemperatures previous,
        const double relaxationParameter)
    {
        VentilatedGapTemperatures result;

        const double Tav1 = averageSurfaceTemperature();
        const double Tav2 = adjacentGap.averageSurfaceTemperature();

        adjacentGap.setFlowSpeed(calculateThermallyDrivenSpeedOfAdjacentGap(adjacentGap));

        const double beta1 = betaCoeff();
        const double beta2 = adjacentGap.betaCoeff();

        const double denom = 1.0 - beta1 * beta2;
        const double safeDenom =
          std::abs(denom) < 1e-4 ? (denom < 0.0 ? -1e-4 : 1e-4) : denom;

        const double commonOutletTemp =
          ((1 - beta1) * Tav1 + beta1 * (1 - beta2) * Tav2) / safeDenom;
        const double commonInletTemp = (1 - beta2) * Tav2;

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

        double Tup = relaxationParameter * result.outletTemperature
                     + (1.0 - relaxationParameter) * previous.outletTemperature;

        double Tdown = relaxationParameter * result.inletTemperature
                       + (1.0 - relaxationParameter) * previous.inletTemperature;

        auto clampVal = [](double val, double minVal, double maxVal) {
            return std::max(minVal, std::min(maxVal, val));
        };

        constexpr double Tmin = 200.0;
        constexpr double Tmax = 400.0;
        Tup = clampVal(Tup, Tmin, Tmax);
        Tdown = clampVal(Tdown, Tmin, Tmax);

        setFlowTemperatures(Tup, Tdown);
        adjacentGap.setFlowTemperatures(Tdown, Tup);

        return result;
    }

    double CIGUGapLayer::performIterationStep(double relaxationParameter, double & TgapOut)
    {
        resetCalculated();
        gasSpecification.setTemperature(averageLayerTemperature());
        double TgapOutOld = TgapOut;
        calculateOutletTemperatureFromAirFlow();
        const auto tempGap = averageLayerTemperature();
        TgapOut = relaxationParameter * tempGap + (1 - relaxationParameter) * TgapOutOld;
        return TgapOutOld;
    }

    void CIGUGapLayer::calculateVentilatedAirflow(double inletTemperature)
    {
        setInletTemperature(inletTemperature);
        Helper::RelaxationState state{.relaxationParameter =
                                        IterationConstants::RELAXATION_PARAMETER_AIRFLOW,
                                      .iterationStep = 0};

        double TgapOut = averageLayerTemperature();
        double lastDelta = std::numeric_limits<double>::max();

        auto step = [&](Helper::RelaxationState & stateRef) -> std::pair<double, bool> {
            const double TgapOutOld = performIterationStep(stateRef.relaxationParameter, TgapOut);

            double delta = std::abs(TgapOut - TgapOutOld);
            bool converged = Helper::isConverged(
              TgapOut, TgapOutOld, IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW);

            return {delta, converged};
        };

        Helper::iterateUntilConverged(state, lastDelta, step);
    }

    bool CIGUGapLayer::isConverged(const VentilatedGapTemperatures & current,
                                   const VentilatedGapTemperatures & previous)
    {
        bool outletTempConverged = std::abs(current.outletTemperature - previous.outletTemperature)
                                   < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;
        bool inletTempConverged = std::abs(current.inletTemperature - previous.inletTemperature)
                                  < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;
        return outletTempConverged && inletTempConverged;
    }

    void CIGUGapLayer::adjustTemperatures(CIGUGapLayer & adjacentGap)
    {
        setInletTemperature(adjacentGap.averageLayerTemperature());
        adjacentGap.setInletTemperature(averageLayerTemperature());
    }

    void CIGUGapLayer::calculateThermallyDrivenAirflowWithAdjacentGap(
      CIGUGapLayer & adjacentGap)
    {
        VentilatedGapTemperatures current{
          .inletTemperature = adjacentGap.averageLayerTemperature(),
          .outletTemperature = averageLayerTemperature()};

        Helper::RelaxationState state{.relaxationParameter =
                                        IterationConstants::RELAXATION_PARAMETER_AIRFLOW,
                                      .iterationStep = 0};

        double lastDelta = std::numeric_limits<double>::max();

        auto step = [&](Helper::RelaxationState & stateRef) -> std::pair<double, bool> {
            adjustTemperatures(adjacentGap);
            auto previous = current;
            current = calculateInletAndOutletTemperaturesWithTheAdjacentGap(
              adjacentGap, current, previous, stateRef.relaxationParameter);

            double delta = std::abs(current.outletTemperature - previous.outletTemperature)
                           + std::abs(current.inletTemperature - previous.inletTemperature);

            const double qv1 = getGainFlow();
            const double qv2 = adjacentGap.getGainFlow();
            smoothEnergyGain(qv1, qv2);
            adjacentGap.smoothEnergyGain(qv1, qv2);

            const bool converged = isConverged(current, previous);
            return {delta, converged};
        };

        Helper::iterateUntilConverged(state, lastDelta, step);
    }

    void CIGUGapLayer::precalculateState()
    {
        if(m_IsVentilated && m_ForcedVentilation.has_value())
        {
            calculateVentilatedAirflow(m_ForcedVentilation->temperature);
        }
    }

}   // namespace Tarcog::ISO15099
