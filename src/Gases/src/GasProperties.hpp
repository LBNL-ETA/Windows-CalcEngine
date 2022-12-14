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
        CIntCoeff();
        CIntCoeff(double t_A, double t_B, double t_C);
        [[nodiscard]] double interpolationValue(double t_Temperature) const;
        CIntCoeff(CIntCoeff const & t_IntCoeff);
        CIntCoeff & operator=(CIntCoeff const & t_IntCoeff);

        bool operator==(const CIntCoeff & rhs) const;
        bool operator!=(const CIntCoeff & rhs) const;

    private:
        double m_A;
        double m_B;
        double m_C;
    };

    struct GasProperties
    {
        GasProperties() :
            m_ThermalConductivity(0.0),
            m_Viscosity(0.0),
            m_SpecificHeat(0.0),
            m_Density(0.0),
            m_MolecularWeight(0.0),
            m_Alpha(0.0),
            m_PrandlNumber(0.0),
            m_PropertiesCalculated(false)
        {}

        GasProperties(GasProperties const & t_GasProperties);

        [[nodiscard]] double getLambdaPrim() const;
        [[nodiscard]] double getLambdaSecond() const;
        GasProperties & operator+(const GasProperties & t_A);
        GasProperties & operator+=(const GasProperties & t_A);
        GasProperties & operator=(const GasProperties & t_A);
        bool operator==(const GasProperties & t_A) const;

        void calculateAlphaAndPrandl();
        double m_ThermalConductivity;
        double m_Viscosity;
        double m_SpecificHeat;
        double m_Density;
        double m_MolecularWeight;
        double m_Alpha;
        double m_PrandlNumber;
        bool m_PropertiesCalculated;
    };

}   // namespace Gases
