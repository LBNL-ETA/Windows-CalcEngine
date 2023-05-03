#include "GasExcept.hpp"

namespace Gases {
    ZeroGasFractionError::ZeroGasFractionError(const std::string &message)
            : std::runtime_error(message) {}

    ZeroMolecularWeightError::ZeroMolecularWeightError(const std::string &message) : runtime_error(message) {

    }

    ZeroViscosityError::ZeroViscosityError(const std::string &message) : runtime_error(message) {

    }

    ZeroDynamicalViscosityError::ZeroDynamicalViscosityError(const std::string &message) : runtime_error(message) {

    }

    ZeroPrimaryThermalConductivityCoefficientError::ZeroPrimaryThermalConductivityCoefficientError(
            const std::string &message) : runtime_error(message) {

    }

    ZeroThermalConductivityCoefficientError::ZeroThermalConductivityCoefficientError(const std::string &message)
            : runtime_error(message) {

    }
}