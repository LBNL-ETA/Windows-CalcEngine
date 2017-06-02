#ifndef GASDATA_H
#define GASDATA_H

#include <unordered_map>
#include <memory>
#include <string>

#include "GasProperties.hpp"

namespace Gases
{

  enum class CoeffType;
  class CIntCoeff;

  class CGasData {
    public:
      CGasData();
      CGasData(
        std::string const &t_Name,
        double const t_Wght,
        double const t_SpecHeatRatio,
        CIntCoeff const & t_Cp,
        CIntCoeff const & t_Con,
        CIntCoeff const & t_Visc );

      CGasData& operator=( const CGasData& t_GasData );
      double GetMolecularWeight() const;
      double GetPropertyValue( CoeffType const t_Type, double const t_Temperature ) const;
      double getSpecificHeatRatio() const;

    private:
      std::string m_gasName;
      double m_molWeight;
      double m_specificHeatRatio;
      std::unordered_map< CoeffType, CIntCoeff > m_Coefficients;
  };

}

#endif
