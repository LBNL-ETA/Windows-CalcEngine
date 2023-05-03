#include <memory>
#include <gtest/gtest.h>

#include "WCEGases.hpp"

class TestGasExceptions : public testing::Test {
};

TEST_F(TestGasExceptions, ZeroGasFractionException) {
    SCOPED_TRACE("Test Zero Gas Fraction Exception");

    Gases::CGas gas{{{0, Gases::GasDef::Air}, {1, Gases::GasDef::Argon}}};
    EXPECT_THROW(gas.getGasProperties(), Gases::ZeroGasFractionError);
}

TEST_F(TestGasExceptions, ZeroMolecularWeight) {
    SCOPED_TRACE("Test zero molecular weight exception");

    const Gases::CGasData Broken{"Air",
                                 0,                                 // Molecular weight
                                 1.4,                               // Specific heat ratio
                                 {1.002737e+03, 1.2324e-02, 0.0},   // Specific heat coefficients
                                 {2.8733e-03, 7.76e-05, 0.0},       // Conductivity coefficients
                                 {3.7233e-06, 4.94e-08, 0.0}};      // Viscosity coefficients

    const Gases::CGasData Argon{"Argon",
                                39.948,                           // Molecular weight
                                1.67,                             // Specific heat ratio
                                {5.21929e+02, 0.0, 0.0},          // Specific heat coefficients
                                {2.2848e-03, 5.1486e-05, 0.0},    // Conductivity coefficients
                                {3.3786e-06, 6.4514e-08, 0.0}};   // Viscosity coefficients

    Gases::CGas gas{{{0.1, Broken}, {0.9, Argon}}};
    EXPECT_THROW(gas.getGasProperties(), Gases::ZeroMolecularWeightError);
}

TEST_F(TestGasExceptions, ZeroViscosityError) {
    SCOPED_TRACE("Test zero viscosity exception");

    const Gases::CGasData Broken{"Air",
                                 28.97,                             // Molecular weight
                                 1.4,                               // Specific heat ratio
                                 {1.002737e+03, 1.2324e-02, 0.0},   // Specific heat coefficients
                                 {2.8733e-03, 7.76e-05, 0.0},       // Conductivity coefficients
                                 {0, 0.0, 0.0}};                    // Viscosity coefficients

    const Gases::CGasData Argon{"Argon",
                                39.948,                           // Molecular weight
                                1.67,                             // Specific heat ratio
                                {5.21929e+02, 0.0, 0.0},          // Specific heat coefficients
                                {2.2848e-03, 5.1486e-05, 0.0},    // Conductivity coefficients
                                {3.3786e-06, 6.4514e-08, 0.0}};   // Viscosity coefficients

    Gases::CGas gas{{{0.1, Broken}, {0.9, Argon}}};
    EXPECT_THROW(gas.getGasProperties(), Gases::ZeroViscosityError);
}