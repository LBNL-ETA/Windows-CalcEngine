#pragma once

#include <memory>
#include <string>

#include "GasData.hpp"
#include "GasProperties.hpp"

namespace Gases
{

    double const DefaultPressure = 101325.0;
    double const DefaultTemperature = 273.15;
    double const DefaultFraction = 1.0;
    enum class GasDef;

    class CGasItem
    {
    public:
        CGasItem() = default;
        CGasItem(CGasItem const & t_GasItem);
        CGasItem(double aFraction, CGasData const & t_GasData);
        CGasItem(double aFraction, Gases::GasDef def);
        CGasItem & operator=(CGasItem const & t_GasItem) = default;
        void setTemperature(double t_Temperature);
        void setPressure(double t_Pressure);
        [[nodiscard]] double fraction() const;
        [[nodiscard]] CGasData gasData() const;
        [[nodiscard]] GasProperties getFractionalGasProperties();
        [[nodiscard]] GasProperties getGasProperties();
        bool operator==(CGasItem const & rhs) const;
        bool operator!=(CGasItem const & rhs) const;

        [[nodiscard]] std::string name() const;

    private:
        [[nodiscard]] GasProperties fillStandardPressureProperties() const;
        [[nodiscard]] GasProperties fillVacuumPressureProperties() const;
        void resetCalculatedProperties();
        double m_Temperature{DefaultTemperature};   // unit in Kelvins
        double m_Pressure{DefaultPressure};         // unit in Pa
        double m_Fraction{DefaultFraction};         // value between 0 and 1
        GasProperties m_GasProperties;
        GasProperties m_FractionalGasProperties;
        CGasData m_GasData;
    };

}   // namespace Gases
