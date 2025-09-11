#pragma once

namespace Gases
{
    enum class CoeffType
    {
        cCond,
        cVisc,
        cCp
    };

    class CIntCoeff
    {
    public:
        CIntCoeff() = default;
        CIntCoeff(double t_A, double t_B, double t_C);
        [[nodiscard]] double interpolationValue(double t_Temperature) const;

        bool operator==(const CIntCoeff & rhs) const;
        bool operator!=(const CIntCoeff & rhs) const;

    private:
        double m_A{0};
        double m_B{0};
        double m_C{0};
    };

    [[nodiscard]] double calculatePrandtlNumber(double thermalConductivity,
                                                double specificHeat,
                                                double viscosity,
                                                double density);

    [[nodiscard]] double lambdaPrim(double molecularWeight, double viscosity);
    [[nodiscard]] double
      lambdaSecond(double molecularWeight, double viscosity, double thermalConductivity);

    struct GasProperties
    {
        GasProperties() = default;
        GasProperties(GasProperties const & t_GasProperties);
        GasProperties(double t_ThermalConductivity,
                      double t_Viscosity,
                      double t_SpecificHeat,
                      double t_Density,
                      double t_MolecularWeight,
                      double t_PrandlNumber,
                      bool t_PropertiesCalculated);

        GasProperties & operator+(const GasProperties & t_A);
        GasProperties & operator+=(const GasProperties & t_A);
        GasProperties & operator=(const GasProperties & t_A);
        bool operator==(const GasProperties & t_A) const;

        double m_ThermalConductivity{0.0};
        double m_Viscosity{0.0};
        double m_SpecificHeat{0.0};
        double m_Density{0.0};
        double m_MolecularWeight{0.0};
        double m_PrandlNumber{0.0};

        bool m_PropertiesCalculated{false};
    };
}   // namespace Gases