#pragma once

#include <stdexcept>

namespace Gases {
    class ZeroGasFractionError : public std::runtime_error {
    public:
        explicit ZeroGasFractionError(
                const std::string &message = "Fraction of gas component in gas mixture is set to be equal to zero.");
    };

    class ZeroMolecularWeightError : public std::runtime_error {
    public:
        explicit ZeroMolecularWeightError(
                const std::string &message = "Molecular weight of gas component in gas mixture is set to be equal to zero.");
    };

    class ZeroViscosityError : public std::runtime_error {
    public:
        explicit ZeroViscosityError(
                const std::string &message = "Viscosity of gas component in gas mixture is set to be equal to zero.");
    };

    class ZeroDynamicalViscosityError : public std::runtime_error {
    public:
        explicit ZeroDynamicalViscosityError(
                const std::string &message = "Dynamical viscosity of gas component in gas mixture is set to be equal to zero.");
    };

    class ZeroPrimaryThermalConductivityCoefficientError : public std::runtime_error {
    public:
        explicit ZeroPrimaryThermalConductivityCoefficientError(
                const std::string &message = "Primary thermal conductivity (lambda prim) of the gas component in Gases is equal to zero.");
    };

    class ZeroThermalConductivityCoefficientError : public std::runtime_error {
    public:
        explicit ZeroThermalConductivityCoefficientError(
                const std::string &message = "Thermal conductivity coefficient in gas mixture is calculated to be zero.");
    };

}