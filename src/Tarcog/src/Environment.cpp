#include "Environment.hpp"
#include "TarcogConstants.hpp"


namespace Tarcog::ISO15099
{
    CEnvironment::CEnvironment(double t_Pressure,
                               double t_AirSpeed,
                               AirHorizontalDirection t_AirDirection) :
        m_DirectSolarRadiation(0),
        m_Emissivity(TarcogConstants::DEFAULT_ENV_EMISSIVITY),
        m_HInput(0),
        m_HCoefficientModel(BoundaryConditionsCoeffModel::CalculateH),
        m_IRCalculatedOutside(false)
    {
        gasSpecification.pressure = t_Pressure;
        gasSpecification.airflowProperties =
          AirflowProperties{t_AirSpeed, AirVerticalDirection::None, t_AirDirection, false};
    }

    CEnvironment::~CEnvironment()
    {
        tearDownConnections();
    }

    void CEnvironment::setHCoeffModel(BoundaryConditionsCoeffModel const t_BCModel,
                                      double const t_HCoeff)
    {
        m_HCoefficientModel = t_BCModel;
        m_HInput = t_HCoeff;
        resetCalculated();
        gasSpecification.setTemperature(getAirTemperature());
    }

    void CEnvironment::setEnvironmentIR(double const t_InfraRed)
    {
        setIRFromEnvironment(t_InfraRed);
        m_IRCalculatedOutside = true;
        resetCalculated();
        gasSpecification.setTemperature(getAirTemperature());
    }

    void CEnvironment::setEmissivity(double const t_Emissivity)
    {
        m_Emissivity = t_Emissivity;
        resetCalculated();
        gasSpecification.setTemperature(getAirTemperature());
    }

    double CEnvironment::getEnvironmentIR()
    {
        return getIRFromEnvironment();
    }

    double CEnvironment::getHc()
    {
        return getConductionConvectionCoefficient();
    }

    double CEnvironment::getAmbientTemperature()
    {
        double hc = getHc();
        double hr = getHr();
        return (hc * getAirTemperature() + hr * getRadiationTemperature()) / (hc + hr);
    }

    double CEnvironment::getDirectSolarRadiation() const
    {
        return m_DirectSolarRadiation;
    }

    void CEnvironment::connectToIGULayer(std::shared_ptr<CBaseLayer> const &)
    {
        //
    }

    void CEnvironment::calculateRadiationFlow()
    {
        // In case of environments, there is no need to calculate radiation
        // if radiation is provided from outside calculations
        if(!m_IRCalculatedOutside)
        {
            setIRFromEnvironment(calculateIRFromVariables());
        }
    }

    void CEnvironment::calculateConvectionOrConductionFlow()
    {
        switch(m_HCoefficientModel)
        {
            case BoundaryConditionsCoeffModel::CalculateH: {
                m_ConductiveConvectiveCoeff = calculateHc();
                break;
            }
            case BoundaryConditionsCoeffModel::HPrescribed: {
                const auto hr = getHr();
                m_ConductiveConvectiveCoeff = m_HInput - hr;
                break;
            }
            case BoundaryConditionsCoeffModel::HcPrescribed: {
                m_ConductiveConvectiveCoeff = m_HInput;
                break;
            }
            default: {
                throw std::runtime_error(
                  "Incorrect definition for convection model (Indoor/Outdoor environment).");
            }
        }
    }

    double CEnvironment::getPressure() const
    {
        return gasSpecification.pressure;
    }

}   // namespace Tarcog::ISO15099
