#include <cmath>

#include "WCECommon.hpp"
#include "GasProperties.hpp"

namespace Gases
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///  IntCoeff
    ///////////////////////////////////////////////////////////////////////////////////////////
    CIntCoeff::CIntCoeff(double const t_A, double const t_B, double const t_C) :
        m_A(t_A), m_B(t_B), m_C(t_C)
    {}

    double CIntCoeff::interpolationValue(double const t_Temperature) const
    {
        return m_A + m_B * t_Temperature + m_C * pow(t_Temperature, 2);
    }

    bool CIntCoeff::operator==(const CIntCoeff & rhs) const
    {
        using FenestrationCommon::isEqual;
        return isEqual(m_A, rhs.m_A) && isEqual(m_B, rhs.m_B) && isEqual(m_C, rhs.m_C);
    }

    bool CIntCoeff::operator!=(const CIntCoeff & rhs) const
    {
        return !(rhs == *this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    //   GasProperties
    ////////////////////////////////////////////////////////////////////////////////////////////
    GasProperties & GasProperties::operator+(GasProperties const & t_A)
    {
        m_ThermalConductivity += t_A.m_ThermalConductivity;
        m_Viscosity += t_A.m_Viscosity;
        m_SpecificHeat += t_A.m_SpecificHeat;
        m_Density += t_A.m_Density;
        m_MolecularWeight += t_A.m_MolecularWeight;
        m_PrandlNumber =
          calculatePrandtlNumber(m_ThermalConductivity, m_SpecificHeat, m_Viscosity, m_Density);

        return *this;
    }

    GasProperties::GasProperties(GasProperties const & t_GasProperties)
    {
        operator=(t_GasProperties);
    }

    GasProperties & GasProperties::operator+=(GasProperties const & t_A)
    {
        *this = *this + t_A;
        return *this;
    }

    GasProperties & GasProperties::operator=(GasProperties const & t_A)
    {
        m_ThermalConductivity = t_A.m_ThermalConductivity;
        m_Viscosity = t_A.m_Viscosity;
        m_SpecificHeat = t_A.m_SpecificHeat;
        m_Density = t_A.m_Density;
        m_MolecularWeight = t_A.m_MolecularWeight;
        m_PrandlNumber = t_A.m_PrandlNumber;
        m_PropertiesCalculated = t_A.m_PropertiesCalculated;

        return *this;
    }

    bool GasProperties::operator==(const GasProperties & t_A) const
    {
        bool equal = true;
        equal = equal && m_ThermalConductivity == t_A.m_ThermalConductivity;
        equal = equal && m_Viscosity == t_A.m_Viscosity;
        equal = equal && m_SpecificHeat == t_A.m_SpecificHeat;
        equal = equal && m_Density == t_A.m_Density;
        equal = equal && m_MolecularWeight == t_A.m_MolecularWeight;
        equal = equal && m_PrandlNumber == t_A.m_PrandlNumber;
        equal = equal && m_PropertiesCalculated == t_A.m_PropertiesCalculated;
        return equal;
    }
    GasProperties::GasProperties(double t_ThermalConductivity,
                                 double t_Viscosity,
                                 double t_SpecificHeat,
                                 double t_Density,
                                 double t_MolecularWeight,
                                 double t_PrandlNumber,
                                 bool t_PropertiesCalculated) :
        m_ThermalConductivity(t_ThermalConductivity),
        m_Viscosity(t_Viscosity),
        m_SpecificHeat(t_SpecificHeat),
        m_Density(t_Density),
        m_MolecularWeight(t_MolecularWeight),
        m_PrandlNumber(t_PrandlNumber),
        m_PropertiesCalculated(t_PropertiesCalculated)
    {}

    double calculatePrandtlNumber(double thermalConductivity,
                                  double specificHeat,
                                  double viscosity,
                                  double density)
    {
        return viscosity / density / (thermalConductivity / (specificHeat * density));
    }

    double lambdaPrim(double molecularWeight, double viscosity)
    {
        using ConstantsData::UNIVERSALGASCONSTANT;

        return 15.0 / 4.0 * UNIVERSALGASCONSTANT / molecularWeight * viscosity;
    }

    double lambdaSecond(double molecularWeight, double viscosity, double thermalConductivity)
    {
        return thermalConductivity - lambdaPrim(molecularWeight, viscosity);
    }
}   // namespace Gases
