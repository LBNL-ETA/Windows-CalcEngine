#include <map>
#include "GasCreator.hpp"

namespace Gases
{
    CGasData getGasData(GasDef gasDef)
    {
        static const std::map<GasDef, CGasData> gasDataMap = {
          {GasDef::Air,
           {
             "Air",
             28.97,                             // Molecular weight
             1.4,                               // Specific heat ratio
             {1.002737e+03, 1.2324e-02, 0.0},   // Specific heat coefficients
             {2.8733e-03, 7.76e-05, 0.0},       // Conductivity coefficients
             {3.7233e-06, 4.94e-08, 0.0}        // Viscosity coefficients
           }},
          {GasDef::Argon,
           {
             "Argon",
             39.948,                          // Molecular weight
             1.67,                            // Specific heat ratio
             {5.21929e+02, 0.0, 0.0},         // Specific heat coefficients
             {2.2848e-03, 5.1486e-05, 0.0},   // Conductivity coefficients
             {3.3786e-06, 6.4514e-08, 0.0}    // Viscosity coefficients
           }},
          {GasDef::Krypton,
           {
             "Krypton",
             83.8,                          // Molecular weight
             1.68,                          // Specific heat ratio
             {2.4809e+02, 0.0, 0.0},        // Specific heat coefficients
             {9.443e-04, 2.8260e-5, 0.0},   // Conductivity coefficients
             {2.213e-6, 7.777e-8, 0.0}      // Viscosity coefficients
           }},
          {GasDef::Xenon,
           {
             "Xenon",
             131.3,                         // Molecular weight
             1.66,                          // Specific heat ratio
             {1.5834e+02, 0.0, 0.0},        // Specific heat coefficients
             {4.538e-04, 1.723e-05, 0.0},   // Conductivity coefficients
             {1.069e-6, 7.414e-8, 0.0}      // Viscosity coefficients
           }},
          // To be used in vacuum calculations.
          {GasDef::VacuumMixture,
           {
             "VacuumMixture",
             21.15,             // Molecular weight
             1.33,              // Specific heat ratio
             {0.0, 0.0, 0.0},   // Specific heat coefficients
             {0.0, 0.0, 0.0},   // Conductivity coefficients
             {0.0, 0.0, 0.0}    // Viscosity coefficients
           }}};
        return gasDataMap.at(gasDef);
    }
}   // namespace Gases
