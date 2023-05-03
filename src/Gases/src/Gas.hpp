#pragma once

#include <memory>
#include <vector>
#include "GasProperties.hpp"
#include "GasCreator.hpp"
#include "GasItem.hpp"

namespace Gases
{
    class CGasData;

    class CGas
    {
    public:
        CGas();
        explicit CGas(const std::vector<CGasItem> & gases);
        CGas(const CGas & t_Gas);
        void addGasItem(double percent, const CGasData & t_GasData);
        void addGasItem(double percent, Gases::GasDef def);
        void addGasItems(const std::vector<CGasItem> & gases);
        double totalPercent();
        GasProperties getSimpleGasProperties();
        GasProperties getGasProperties();
        void setTemperatureAndPressure(double t_Temperature, double t_Pressure);

        [[nodiscard]] std::vector<CGasItem> gasItems() const;

        CGas & operator=(CGas const & t_Gas) = default;
        bool operator==(CGas const & t_Gas) const;
        bool operator!=(CGas const & t_Gas) const;

    private:
        GasProperties getStandardPressureGasProperties();
        GasProperties getVacuumPressureGasProperties();

        [[nodiscard]] double viscTwoGases(GasProperties const & t_Gas1Properties,
                                          GasProperties const & t_Gas2Properties) const;
        double viscDenomTwoGases(CGasItem & t_GasItem1, CGasItem & t_GasItem2) const;

        [[nodiscard]] double lambdaPrimTwoGases(GasProperties const & t_Gas1Properties,
                                                GasProperties const & t_Gas2Properties) const;
        [[nodiscard]] double lambdaSecondTwoGases(GasProperties const & t_Gas1Properties,
                                                  GasProperties const & t_Gas2Properties) const;

        double lambdaPrimDenomTwoGases(CGasItem & t_GasItem1, CGasItem & t_GasItem2) const;
        double lambdaSecondDenomTwoGases(CGasItem & t_GasItem1, CGasItem & t_GasItem2) const;

        std::vector<CGasItem> m_GasItem;
        GasProperties m_SimpleProperties;
        GasProperties m_Properties;

        bool m_DefaultGas{true};
        double m_Pressure{DefaultPressure};
    };

}   // namespace Gases
