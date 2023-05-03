#include <stdexcept>


#include "WCECommon.hpp"
#include "GasProperties.hpp"
#include "GasData.hpp"


namespace Gases
{
    CGasData::CGasData() : m_gasName("Air"), m_molWeight(28.97), m_specificHeatRatio(1.4)
    {
        m_Coefficients[CoeffType::cCp] = {1.002737e+03, 1.2324e-02, 0.0};
        m_Coefficients[CoeffType::cCond] = {2.8733e-03, 7.76e-05, 0.0};
        m_Coefficients[CoeffType::cVisc] = {3.7233e-06, 4.94e-08, 0.0};
    }

    CGasData::CGasData(std::string_view t_Name,
                       double const t_Wght,
                       double const t_SpecHeatRatio,
                       CIntCoeff const & t_Cp,
                       CIntCoeff const & t_Con,
                       CIntCoeff const & t_Visc) :
        m_gasName(t_Name), m_molWeight(t_Wght), m_specificHeatRatio(t_SpecHeatRatio)
    {
        m_Coefficients[CoeffType::cCp] = t_Cp;
        m_Coefficients[CoeffType::cCond] = t_Con;
        m_Coefficients[CoeffType::cVisc] = t_Visc;
    }

    double CGasData::getPropertyValue(CoeffType const t_Type, double const t_Temperature) const
    {
        return m_Coefficients.at(t_Type).interpolationValue(t_Temperature);
    }

    double CGasData::getSpecificHeatRatio() const
    {
        return m_specificHeatRatio;
    }

    double CGasData::getMolecularWeight() const
    {
        return m_molWeight;
    }

    std::string CGasData::name() const
    {
        return m_gasName;
    }

    bool CGasData::operator==(const CGasData & rhs) const
    {
        return m_gasName == rhs.m_gasName && m_molWeight == rhs.m_molWeight
               && m_specificHeatRatio == rhs.m_specificHeatRatio
               && m_Coefficients == rhs.m_Coefficients;
    }

    bool CGasData::operator!=(const CGasData & rhs) const
    {
        return !(rhs == *this);
    }

}   // namespace Gases
