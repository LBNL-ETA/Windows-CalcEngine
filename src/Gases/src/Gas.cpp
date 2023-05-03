#include <stdexcept>
#include <numeric>

#include "WCECommon.hpp"
#include "Gas.hpp"
#include "GasData.hpp"
#include "GasItem.hpp"
#include "GasSetting.hpp"
#include "GasExcept.hpp"


namespace Gases {
    CGas::CGas() {
        // create default gas to be Air
        auto Air = CGasItem();
        m_GasItem.push_back(Air);
    }

    CGas::CGas(const std::vector<CGasItem> &gases) {
        addGasItems(gases);
    }

    void CGas::addGasItem(const double percent, const CGasData &t_GasData) {
        CGasItem item(percent, t_GasData);
        // Need to remove default since user wants to create their own Gases
        if (m_DefaultGas) {
            m_GasItem.clear();
            m_DefaultGas = false;
        }
        m_GasItem.push_back(item);
    }

    void CGas::addGasItems(const std::vector<CGasItem> &gases) {
        if (m_DefaultGas) {
            m_GasItem.clear();
            m_DefaultGas = false;
        }
        for (const auto &gasItem: gases) {
            m_GasItem.emplace_back(gasItem.fraction(), gasItem.gasData());
        }
    }

    void CGas::addGasItem(double percent, Gases::GasDef def) {
        addGasItem(percent, getGasData(def));
    }

    double CGas::totalPercent() const {
        auto totalPercent = 0.0;

        for (auto &it: m_GasItem) {
            totalPercent += it.fraction();
        }

        return totalPercent;
    }

    void CGas::setTemperatureAndPressure(double const t_Temperature, double const t_Pressure) {
        m_Pressure = t_Pressure;
        for (auto &item: m_GasItem) {
            item.setTemperature(t_Temperature);
            item.setPressure(t_Pressure);
        }
    }

    GasProperties CGas::getSimpleGasProperties() {
        m_SimpleProperties = m_GasItem[0].getFractionalGasProperties();
        for (auto it = next(m_GasItem.begin()); it != m_GasItem.end(); ++it) {
            m_SimpleProperties += it->getFractionalGasProperties();
        }

        return m_SimpleProperties;
    }

    GasProperties CGas::getGasProperties() {
        auto aSettings = CGasSettings::instance();
        return aSettings.getVacuumPressure() < m_Pressure ? getStandardPressureGasProperties()
                                                          : getVacuumPressureGasProperties();
    }

    GasProperties CGas::getStandardPressureGasProperties() {
        auto simpleProperties = getSimpleGasProperties();

        // Call the new functions
        auto miItem = calculateMiItems();
        auto [lambdaPrimItem, lambdaSecondItem] = calculateLambdaItems();

        auto gasSize = m_GasItem.size();

        double miMix(0);
        double lambdaPrimMix(0);
        double lambdaSecondMix(0);
        double cpMix(0);

        for (size_t i = 0; i < gasSize; ++i) {
            auto itGasProperties = m_GasItem[i].getGasProperties();
            auto lambdaPr(
                    lambdaPrim(itGasProperties.m_MolecularWeight, itGasProperties.m_Viscosity));
            auto lambdaSe(lambdaSecond(itGasProperties.m_MolecularWeight,
                                       itGasProperties.m_Viscosity,
                                       itGasProperties.m_ThermalConductivity));

            auto sumMix = 1.0;
            for (size_t j = 0; j < gasSize; ++j) {
                sumMix += miItem[i][j];
            }

            miMix += itGasProperties.m_Viscosity / sumMix;

            sumMix = 1.0;
            for (size_t j = 0; j < gasSize; ++j) {
                sumMix += lambdaPrimItem[i][j];
            }

            lambdaPrimMix += lambdaPr / sumMix;

            sumMix = 1.0;
            for (size_t j = 0; j < gasSize; ++j) {
                sumMix += lambdaSecondItem[i][j];
            }

            lambdaSecondMix += lambdaSe / sumMix;

            cpMix +=
                    itGasProperties.m_SpecificHeat * m_GasItem[i].fraction() * itGasProperties.m_MolecularWeight;
        }

        m_Properties.m_ThermalConductivity = lambdaPrimMix + lambdaSecondMix;
        m_Properties.m_Viscosity = miMix;
        m_Properties.m_SpecificHeat = cpMix / simpleProperties.m_MolecularWeight;
        m_Properties.m_Density = simpleProperties.m_Density;
        m_Properties.m_MolecularWeight = simpleProperties.m_MolecularWeight;
        m_Properties.m_PrandlNumber = calculatePrandtlNumber(m_Properties.m_ThermalConductivity,
                                                             m_Properties.m_SpecificHeat,
                                                             m_Properties.m_Viscosity,
                                                             m_Properties.m_Density);

        return m_Properties;
    }


    std::vector<std::vector<double>> CGas::calculateMiItems() {
        std::vector<std::vector<double>> miItem;
        auto gasSize = m_GasItem.size();
        miItem.resize(gasSize);

        for (size_t i = 0; i < gasSize; ++i) {
            for (size_t j = 0; j < gasSize; ++j) {
                if (i != j)
                    miItem[i].push_back(viscDenomTwoGases(m_GasItem[i], m_GasItem[j]));
                else
                    miItem[i].push_back(0);
            }
        }

        return miItem;
    }

    std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>
    CGas::calculateLambdaItems() {
        std::vector<std::vector<double>> lambdaPrimItem;
        std::vector<std::vector<double>> lambdaSecondItem;
        auto gasSize = m_GasItem.size();
        lambdaPrimItem.resize(gasSize);
        lambdaSecondItem.resize(gasSize);

        for (size_t i = 0; i < gasSize; ++i) {
            for (size_t j = 0; j < gasSize; ++j) {
                if (i != j) {
                    lambdaPrimItem[i].push_back(
                            lambdaPrimDenomTwoGases(m_GasItem[i], m_GasItem[j]));
                    lambdaSecondItem[i].push_back(
                            lambdaSecondDenomTwoGases(m_GasItem[i], m_GasItem[j]));
                } else {
                    lambdaPrimItem[i].push_back(0);
                    lambdaSecondItem[i].push_back(0);
                }
            }
        }

        return {lambdaPrimItem, lambdaSecondItem};
    }

    GasProperties CGas::getVacuumPressureGasProperties() {
        return getSimpleGasProperties();
    }

    // This implements equation 63 (ISO 15099)
    double CGas::viscTwoGases(GasProperties const &t_Gas1Properties,
                              GasProperties const &t_Gas2Properties) const {
        if (t_Gas1Properties.m_Viscosity == 0 || t_Gas2Properties.m_Viscosity == 0) {
            throw ZeroViscosityError();
        }
        if ((t_Gas1Properties.m_MolecularWeight == 0) || (t_Gas2Properties.m_MolecularWeight == 0)) {
            throw ZeroMolecularWeightError();
        }

        auto uFraction = t_Gas1Properties.m_Viscosity / t_Gas2Properties.m_Viscosity;
        auto weightFraction =
                t_Gas1Properties.m_MolecularWeight / t_Gas2Properties.m_MolecularWeight;
        auto nominator = pow((1 + pow(uFraction, 0.5) * pow(1 / weightFraction, 0.25)), 2);
        auto denominator = 2 * sqrt(2.0) * pow(1 + weightFraction, 0.5);

        if (denominator == 0) {
            throw ZeroDynamicalViscosityError();
        }

        return nominator / denominator;
    }

    // Implementation of sum items in denominator of equation 62 (ISO15099)
    double CGas::viscDenomTwoGases(CGasItem &t_GasItem1, CGasItem &t_GasItem2) const {
        auto phiValue = viscTwoGases(t_GasItem1.getGasProperties(), t_GasItem2.getGasProperties());
        if ((t_GasItem1.fraction() == 0) || (t_GasItem2.fraction() == 0)) {
            throw ZeroGasFractionError();
        }

        return (t_GasItem2.fraction() / t_GasItem1.fraction()) * phiValue;
    }

    // This implements equation 66 (ISO 15099)
    double CGas::lambdaPrimTwoGases(GasProperties const &t_Gas1Properties,
                                    GasProperties const &t_Gas2Properties) const {
        if ((t_Gas1Properties.m_MolecularWeight == 0) || (t_Gas2Properties.m_MolecularWeight == 0)) {
            throw ZeroMolecularWeightError();
        }

        auto item1 = lambdaSecondTwoGases(t_Gas1Properties, t_Gas2Properties);
        auto item2 =
                1
                + 2.41
                  * ((t_Gas1Properties.m_MolecularWeight - t_Gas2Properties.m_MolecularWeight)
                     * (t_Gas1Properties.m_MolecularWeight - 0.142 * t_Gas2Properties.m_MolecularWeight)
                     / pow((t_Gas1Properties.m_MolecularWeight + t_Gas2Properties.m_MolecularWeight),
                           2));

        return item1 * item2;
    }

    // This implements equation 68 (ISO 15099)
    double CGas::lambdaSecondTwoGases(GasProperties const &t_Gas1Properties,
                                      const GasProperties &t_Gas2Properties) const {
        auto lambdaPrim1{
                lambdaPrim(t_Gas1Properties.m_MolecularWeight, t_Gas1Properties.m_Viscosity)};
        auto lambdaPrim2{
                lambdaPrim(t_Gas2Properties.m_MolecularWeight, t_Gas2Properties.m_Viscosity)};
        if ((lambdaPrim1 == 0) || (lambdaPrim2 == 0)) {
            throw ZeroPrimaryThermalConductivityCoefficientError();
        }

        if ((t_Gas1Properties.m_MolecularWeight == 0) || (t_Gas2Properties.m_MolecularWeight == 0)) {
            throw ZeroMolecularWeightError();
        }

        auto tFraction = lambdaPrim1 / lambdaPrim2;
        auto weightFraction =
                t_Gas1Properties.m_MolecularWeight / t_Gas2Properties.m_MolecularWeight;
        auto nominator = pow((1 + pow(tFraction, 0.5) * pow(weightFraction, 0.25)), 2);
        auto denominator = 2 * sqrt(2.0) * pow((1 + weightFraction), 0.5);

        if (denominator == 0) {
            throw ZeroThermalConductivityCoefficientError();
        }

        return nominator / denominator;
    }

    // Implementation of sum items in denominator of equation 65 (ISO15099)
    double CGas::lambdaPrimDenomTwoGases(CGasItem &t_GasItem1, CGasItem &t_GasItem2) const {
        auto phiValue =
                lambdaPrimTwoGases(t_GasItem1.getGasProperties(), t_GasItem2.getGasProperties());

        if ((t_GasItem1.fraction() == 0) || (t_GasItem2.fraction() == 0)) {
            throw ZeroGasFractionError();
        }

        return (t_GasItem2.fraction() / t_GasItem1.fraction()) * phiValue;
    }

    // Implementation of sum items in denominator of equation 67 (ISO15099)
    double CGas::lambdaSecondDenomTwoGases(CGasItem &t_GasItem1, CGasItem &t_GasItem2) const {
        auto phiValue =
                lambdaSecondTwoGases(t_GasItem1.getGasProperties(), t_GasItem2.getGasProperties());

        if ((t_GasItem1.fraction() == 0) || (t_GasItem2.fraction() == 0)) {
            throw ZeroGasFractionError();
        }

        return (t_GasItem2.fraction() / t_GasItem1.fraction()) * phiValue;
    }

    bool CGas::operator==(CGas const &rhs) const {
        return m_GasItem == rhs.m_GasItem && m_SimpleProperties == rhs.m_SimpleProperties
               && m_Properties == rhs.m_Properties && m_DefaultGas == rhs.m_DefaultGas
               && m_Pressure == rhs.m_Pressure;
    }

    bool CGas::operator!=(CGas const &rhs) const {
        return !(rhs == *this);
    }

    std::vector<CGasItem> CGas::gasItems() const {
        return m_GasItem;
    }

}   // namespace Gases
