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
    constexpr double DefaultSurfaceAccommodation = 0.89;
    enum class GasDef;

    struct GasItemProperties
    {
        double temperature{DefaultTemperature};   // unit in Kelvins
        double pressure{DefaultPressure};         // unit in Pa
        double fraction{DefaultFraction};         // value between 0 and 1
        GasProperties properties;
        GasProperties fractionalProperties;
        CGasData gasData;
    };

    bool operator==(GasItemProperties const & lhs, GasItemProperties const & rhs);

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
        [[nodiscard]] GasProperties getFractionalGasProperties(double alpha1, double alpha2);
        [[nodiscard]] GasProperties getGasProperties(double alpha1 = DefaultSurfaceAccommodation,
                                                     double alpha2 = DefaultSurfaceAccommodation);
        bool operator==(CGasItem const & rhs) const;
        bool operator!=(CGasItem const & rhs) const;

        [[nodiscard]] std::string name() const;

    private:
        [[nodiscard]] GasProperties fillStandardPressureProperties() const;
        [[nodiscard]] GasProperties fillVacuumPressureProperties(double alpha1,
                                                                 double alpha2) const;
        void resetCalculatedProperties();
        GasItemProperties m_Properties;
    };

}   // namespace Gases
