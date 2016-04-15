#include <stdexcept>

#include "GasProperties.hpp"
#include "GasData.hpp"

using namespace std;

namespace Gases
{
  CGasData::CGasData() : m_gasName ( "Air" ), m_molWeight( 28.97 ), m_specificHeatRatio( 1.4 ),
                         m_coeffCp( make_shared< CIntCoeff >( 1.002737e+03, 1.2324e-02, 0.0 ) ),
                         m_coeffCon( make_shared< CIntCoeff >( 2.8733e-03, 7.76e-05, 0.0 ) ),
                         m_coeffVisc( make_shared< CIntCoeff >( 3.7233e-06, 4.94e-08, 0.0 ) ) {
    // Create default Air
    // m_gasName = "Air";
    // m_molWeight = 28.97;
    // m_specificHeatRatio = 1.4;
    // m_coeffCon = make_shared< CIntCoeff >( 2.8733e-03, 7.76e-05, 0.0 );
    // m_coeffCp = make_shared< CIntCoeff >( 1.002737e+03, 1.2324e-02, 0.0 );
    // m_coeffVisc = make_shared< CIntCoeff >( 3.7233e-06, 4.94e-08, 0.0 );

  }

  CGasData::CGasData(
    string const &t_Name, 
    double const t_Wght, 
    double const t_SpecHeatRatio, 
    shared_ptr< CIntCoeff const > t_Cp, 
    shared_ptr< CIntCoeff const > t_Con, 
    shared_ptr< CIntCoeff const > t_Visc ) : 
    m_gasName( t_Name ), m_molWeight( t_Wght ), m_specificHeatRatio( t_SpecHeatRatio ),
    m_coeffCp( t_Cp ), m_coeffCon( t_Con ), m_coeffVisc( t_Visc )
  {

  }

  CGasData& CGasData::operator=( const CGasData& t_GasData ) {
    this->m_gasName = t_GasData.m_gasName;
    this->m_molWeight = t_GasData.m_molWeight;
    this->m_specificHeatRatio = t_GasData.m_specificHeatRatio;
    this->m_coeffCon = t_GasData.m_coeffCon;
    this->m_coeffCp = t_GasData.m_coeffCp;
    this->m_coeffVisc = t_GasData.m_coeffVisc;

    return *this;
  }

  double CGasData::GetPropertyValue( CoeffType t_Type, double t_Temperature ) const
  {
    double value = 0;

    switch ( t_Type )
    {
    case CoeffType::cCond:
        value = m_coeffCon->interpolationValue( t_Temperature );
        break;
    case CoeffType::cCp:
        value = m_coeffCp->interpolationValue( t_Temperature );
        break;
    case CoeffType::cVisc:
        value = m_coeffVisc->interpolationValue( t_Temperature );
        break;
      default:
        value = 0;
        throw runtime_error( "Requested gas property do not exist" );
    }

    return value;
  }

  double CGasData::GetMolecularWeight() const
  {
    return m_molWeight;
  }

};