#pragma once

#include <map>
#include <string>

#include "GasProperties.hpp"

namespace Gases
{
    enum class CoeffType;
    class CIntCoeff;

    class CGasData
    {
    public:
        CGasData();
        CGasData(CGasData const & t_GasData);
        CGasData(std::string_view t_Name,
                 double t_Wght,
                 double t_SpecHeatRatio,
                 CIntCoeff const & t_Cp,
                 CIntCoeff const & t_Con,
                 CIntCoeff const & t_Visc);

        [[nodiscard]] double getMolecularWeight() const;
        [[nodiscard]] double getPropertyValue(CoeffType t_Type, double t_Temperature) const;
        [[nodiscard]] double getSpecificHeatRatio() const;
        [[nodiscard]] std::string name() const;

        bool operator==(const CGasData & rhs) const;
        bool operator!=(const CGasData & rhs) const;

    private:
        std::string m_gasName;
        double m_molWeight;
        double m_specificHeatRatio;
        std::map<CoeffType, CIntCoeff> m_Coefficients;
    };

}   // namespace Gases
