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
    CGasItem::CGasItem(CGasItem const & t_GasItem) :
        m_Temperature(t_GasItem.m_Temperature),
        m_Pressure(t_GasItem.m_Pressure),
        m_Fraction(t_GasItem.m_Fraction),
        m_GasProperties(t_GasItem.m_GasProperties),
        m_FractionalGasProperties(t_GasItem.m_FractionalGasProperties),
        m_GasData(t_GasItem.m_GasData)
    {}

    CGasItem::CGasItem(double const t_Fraction, CGasData const & t_GasData) :
        m_Fraction(t_Fraction), m_GasData(t_GasData)
    {}

    CGasItem::CGasItem(double aFraction, Gases::GasDef def) :
        CGasItem(aFraction, getGasData(def))
    {}

    void CGasItem::fillStandardPressureProperites()
    {
        using ConstantsData::UNIVERSALGASCONSTANT;
        m_GasProperties.m_ThermalConductivity =
          m_GasData.getPropertyValue(CoeffType::cCond, m_Temperature);
        m_GasProperties.m_Viscosity = m_GasData.getPropertyValue(CoeffType::cVisc, m_Temperature);
        m_GasProperties.m_SpecificHeat = m_GasData.getPropertyValue(CoeffType::cCp, m_Temperature);
        m_GasProperties.m_MolecularWeight = m_GasData.getMolecularWeight();
        m_GasProperties.m_Density =
          m_Pressure * m_GasProperties.m_MolecularWeight / (UNIVERSALGASCONSTANT * m_Temperature);
        m_GasProperties.m_PrandlNumber =
          calculatePrandtlNumber(m_GasProperties.m_ThermalConductivity,
                                 m_GasProperties.m_SpecificHeat,
                                 m_GasProperties.m_Viscosity,
                                 m_GasProperties.m_Density);
    }

    void CGasItem::fillVacuumPressureProperties()
    {
        using ConstantsData::UNIVERSALGASCONSTANT;
        using ConstantsData::WCE_PI;
        auto const alpha1 = 0.79;
        auto const alpha2 = 0.79;
        auto const alpha = alpha1 * alpha2 / (alpha2 + alpha1 * (1 - alpha2));
        auto const specificHeatRatio = m_GasData.getSpecificHeatRatio();
        if(specificHeatRatio == 1)
        {
            throw std::runtime_error("Specific heat ratio of a gas cannot be equal to one.");
        }
        auto B = alpha * (specificHeatRatio + 1) / (specificHeatRatio - 1);
        B *= sqrt(UNIVERSALGASCONSTANT / (8 * WCE_PI * m_GasData.getMolecularWeight() * m_Temperature));
        m_GasProperties.m_ThermalConductivity = B * m_Pressure;
        m_GasProperties.m_Viscosity = 0;
        m_GasProperties.m_SpecificHeat = 0;
        m_GasProperties.m_MolecularWeight = m_GasData.getMolecularWeight();
        m_GasProperties.m_Density = 0;
        m_GasProperties.m_PrandlNumber = 0;
    }

    double CGasItem::fraction() const
    {
        return m_Fraction;
    }

    void CGasItem::resetCalculatedProperties()
    {
        m_GasProperties.m_PropertiesCalculated = false;
        m_FractionalGasProperties.m_PropertiesCalculated = false;
    }

    void CGasItem::setTemperature(double const t_Temperature)
    {
        m_Temperature = t_Temperature;
        resetCalculatedProperties();
    }

    void CGasItem::setPressure(double const t_Pressure)
    {
        m_Pressure = t_Pressure;
        resetCalculatedProperties();
    }

    GasProperties CGasItem::getGasProperties()
    {
        if(!m_GasProperties.m_PropertiesCalculated)
        {
            if(m_Pressure > CGasSettings::instance().getVacuumPressure())
            {
                fillStandardPressureProperites();
            }
            else
            {
                fillVacuumPressureProperties();
            }
            m_GasProperties.m_PropertiesCalculated = true;
        }

        return m_GasProperties;
    }

    GasProperties CGasItem::getFractionalGasProperties()
    {
        if(!m_FractionalGasProperties.m_PropertiesCalculated)
        {
            auto itemGasProperties = getGasProperties();

            // update for fractional data
            m_FractionalGasProperties.m_ThermalConductivity =
              itemGasProperties.m_ThermalConductivity * m_Fraction;
            m_FractionalGasProperties.m_Viscosity = itemGasProperties.m_Viscosity * m_Fraction;
            m_FractionalGasProperties.m_SpecificHeat =
              itemGasProperties.m_SpecificHeat * m_Fraction;
            m_FractionalGasProperties.m_MolecularWeight =
              itemGasProperties.m_MolecularWeight * m_Fraction;
            m_FractionalGasProperties.m_Density = itemGasProperties.m_Density * m_Fraction;
            m_FractionalGasProperties.m_PrandlNumber =
              itemGasProperties.m_PrandlNumber * m_Fraction;
        }

        return m_FractionalGasProperties;
    }

    bool CGasItem::operator==(CGasItem const & rhs) const
    {
        return m_Temperature == rhs.m_Temperature && m_Pressure == rhs.m_Pressure
               && m_Fraction == rhs.m_Fraction && m_GasProperties == rhs.m_GasProperties
               && m_FractionalGasProperties == rhs.m_FractionalGasProperties
               && m_GasData == rhs.m_GasData;
    }

    bool CGasItem::operator!=(CGasItem const & rhs) const
    {
        return !(rhs == *this);
    }

    std::string CGasItem::name() const
    {
        return m_GasData.name();
    }

    CGasData CGasItem::gasData() const
    {
        return m_GasData;
    }

}   // namespace Gases
