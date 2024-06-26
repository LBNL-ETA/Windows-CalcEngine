#include <memory>
#include <cmath>
#include <stdexcept>

#include "WCECommon.hpp"
#include "GasItem.hpp"
#include "GasData.hpp"
#include "GasSetting.hpp"
#include "GasCreator.hpp"

namespace Gases
{
    bool operator==(const GasItemProperties & lhs, const GasItemProperties & rhs)
    {
        return lhs.temperature == rhs.temperature && lhs.pressure == rhs.pressure
               && lhs.fraction == rhs.fraction && lhs.properties == rhs.properties
               && lhs.fractionalProperties == rhs.fractionalProperties
               && lhs.gasData == rhs.gasData;
    }

    CGasItem::CGasItem(CGasItem const & t_GasItem) : m_Properties(t_GasItem.m_Properties)
    {}

    CGasItem::CGasItem(double const t_Fraction, CGasData const & t_GasData)
    {
        m_Properties.fraction = t_Fraction;
        m_Properties.gasData = t_GasData;
    }

    CGasItem::CGasItem(double aFraction, Gases::GasDef def) : CGasItem(aFraction, getGasData(def))
    {}

    GasProperties CGasItem::fillStandardPressureProperties() const
    {
        using ConstantsData::UNIVERSALGASCONSTANT;

        return {
          m_Properties.gasData.getPropertyValue(
            CoeffType::cCond, m_Properties.temperature),   // m_ThermalConductivity
          m_Properties.gasData.getPropertyValue(CoeffType::cVisc,
                                                m_Properties.temperature),   // m_Viscosity
          m_Properties.gasData.getPropertyValue(CoeffType::cCp,
                                                m_Properties.temperature),   // m_SpecificHeat
          m_Properties.pressure * m_Properties.gasData.getMolecularWeight()
            / (UNIVERSALGASCONSTANT * m_Properties.temperature),   // m_Density
          m_Properties.gasData.getMolecularWeight(),               // m_MolecularWeight
          calculatePrandtlNumber(                                  // m_PrandlNumber
            m_Properties.gasData.getPropertyValue(CoeffType::cCond, m_Properties.temperature),
            m_Properties.gasData.getPropertyValue(CoeffType::cCp, m_Properties.temperature),
            m_Properties.gasData.getPropertyValue(CoeffType::cVisc, m_Properties.temperature),
            m_Properties.pressure * m_Properties.gasData.getMolecularWeight()
              / (UNIVERSALGASCONSTANT * m_Properties.temperature)),
          true   // m_PropertiesCalculated
        };
    }

    GasProperties CGasItem::fillVacuumPressureProperties(const double alpha1,
                                                         const double alpha2) const
    {
        using ConstantsData::UNIVERSALGASCONSTANT;
        using ConstantsData::WCE_PI;

        const auto alpha = alpha1 * alpha2 / (alpha2 + alpha1 * (1 - alpha2));
        const auto specificHeatRatio = m_Properties.gasData.getSpecificHeatRatio();
        if(specificHeatRatio == 1)
        {
            throw std::runtime_error("Specific heat ratio of a gas cannot be equal to one.");
        }
        auto B = alpha * (specificHeatRatio + 1) / (specificHeatRatio - 1);
        B *= sqrt(
          UNIVERSALGASCONSTANT
          / (8 * WCE_PI * m_Properties.gasData.getMolecularWeight() * m_Properties.temperature));

        return {
          B * m_Properties.pressure,                   // m_ThermalConductivity
          0.0,                                         // m_Viscosity
          0.0,                                         // m_SpecificHeat
          0.0,                                         // m_Density
          m_Properties.gasData.getMolecularWeight(),   // m_MolecularWeight
          0.0,                                         // m_PrandlNumber
          true                                         // m_PropertiesCalculated
        };
    }


    double CGasItem::fraction() const
    {
        return m_Properties.fraction;
    }

    void CGasItem::resetCalculatedProperties()
    {
        m_Properties.properties.m_PropertiesCalculated = false;
        m_Properties.fractionalProperties.m_PropertiesCalculated = false;
    }

    void CGasItem::setTemperature(double const t_Temperature)
    {
        m_Properties.temperature = t_Temperature;
        resetCalculatedProperties();
    }

    void CGasItem::setPressure(double const t_Pressure)
    {
        m_Properties.pressure = t_Pressure;
        resetCalculatedProperties();
    }

    GasProperties CGasItem::getGasProperties(const double alpha1, const double alpha2)
    {
        if(!m_Properties.properties.m_PropertiesCalculated)
        {
            m_Properties.properties =
              (m_Properties.pressure > CGasSettings::instance().getVacuumPressure())
                ? fillStandardPressureProperties()
                : fillVacuumPressureProperties(alpha1, alpha2);
        }

        return m_Properties.properties;
    }

    GasProperties CGasItem::getFractionalGasProperties(const double alpha1, const double alpha2)
    {
        if(!m_Properties.fractionalProperties.m_PropertiesCalculated)
        {
            auto itemGasProperties = getGasProperties(alpha1, alpha2);

            // update for fractional data
            m_Properties.fractionalProperties.m_ThermalConductivity =
              itemGasProperties.m_ThermalConductivity * m_Properties.fraction;
            m_Properties.fractionalProperties.m_Viscosity =
              itemGasProperties.m_Viscosity * m_Properties.fraction;
            m_Properties.fractionalProperties.m_SpecificHeat =
              itemGasProperties.m_SpecificHeat * m_Properties.fraction;
            m_Properties.fractionalProperties.m_MolecularWeight =
              itemGasProperties.m_MolecularWeight * m_Properties.fraction;
            m_Properties.fractionalProperties.m_Density =
              itemGasProperties.m_Density * m_Properties.fraction;
            m_Properties.fractionalProperties.m_PrandlNumber =
              itemGasProperties.m_PrandlNumber * m_Properties.fraction;
        }

        return m_Properties.fractionalProperties;
    }

    bool CGasItem::operator==(CGasItem const & rhs) const
    {
        return m_Properties == rhs.m_Properties;
    }

    bool CGasItem::operator!=(CGasItem const & rhs) const
    {
        return !(rhs == *this);
    }

    std::string CGasItem::name() const
    {
        return m_Properties.gasData.name();
    }

    CGasData CGasItem::gasData() const
    {
        return m_Properties.gasData;
    }
}   // namespace Gases
