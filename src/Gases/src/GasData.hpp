#ifndef GASDATA_H
#define GASDATA_H

#include <memory>
#include <string>

#include "GasProperties.hpp"

namespace Gases
{

  class CIntCoeff;

  class CGasData
  {
    public:
      CGasData();
      CGasData(
        std::string const &t_Name,
        double const t_Wght,
        double const t_SpecHeatRatio,
        std::shared_ptr< CIntCoeff const > t_Cp,
        std::shared_ptr< CIntCoeff const > t_Con,
        std::shared_ptr< CIntCoeff const > t_Visc );

      CGasData& operator=(const CGasData& t_GasData);
      double GetMolecularWeight() const;
      double GetPropertyValue( CoeffType t_Type, double t_Temperature ) const;
    private:
      std::string m_gasName;
      double m_molWeight;
      double m_specificHeatRatio;
      std::shared_ptr< CIntCoeff const > m_coeffCp;
      std::shared_ptr< CIntCoeff const > m_coeffCon;
      std::shared_ptr< CIntCoeff const > m_coeffVisc;
  };

};

#endif