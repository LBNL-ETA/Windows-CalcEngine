#include <memory>
#include <stdexcept>

#include "Constants.hpp"
#include "GasItem.hpp"
#include "GasData.hpp"
#include "GasProperties.hpp"

using namespace std;

namespace Gases {
  
  CGasItem::CGasItem() : m_Fraction( 1.0 ), m_GasData( make_shared< CGasData >() ) {
    // 100% Air is default item
    initialize();
  }

  CGasItem::CGasItem( double t_Fraction, shared_ptr< CGasData const > t_GasData ) :
    m_Fraction( t_Fraction ), m_GasData( t_GasData ) {
    initialize();
  }

  void CGasItem::initialize() {
    m_Temperature = DefaultTemperature;
    m_Pressure = DefaultPressure;
    m_FractionalGasProperties = make_shared< GasProperties > ();
    m_GasProperties = make_shared< GasProperties > ();
  }

  CGasItem& CGasItem::operator=(const CGasItem& t_GasItem) {
    this->m_Fraction = t_GasItem.m_Fraction;
    this->m_Pressure = t_GasItem.m_Pressure;
    this->m_Temperature = t_GasItem.m_Temperature;
    this->m_GasData = t_GasItem.m_GasData;
    this->m_FractionalGasProperties = t_GasItem.m_FractionalGasProperties;
    this->m_GasProperties = t_GasItem.m_GasProperties;

    return *this;
  }

  double CGasItem::getFraction() {
    return m_Fraction;
  }

  void CGasItem::resetCalculatedProperties() {
    m_GasProperties->m_PropertiesCalculated = false;
    m_FractionalGasProperties->m_PropertiesCalculated = false;
  }

  void CGasItem::setTemperature( double t_Temperature ) {
    m_Temperature = t_Temperature;
    resetCalculatedProperties();
  }

  void CGasItem::setPressure( double t_Pressure ) {
    m_Pressure = t_Pressure;
    resetCalculatedProperties();
  }

  shared_ptr< GasProperties > CGasItem::getGasProperties() {
    using ConstantsData::UNIVERSALGASCONSTANT;

    if ( !m_GasProperties->m_PropertiesCalculated ) {
      m_GasProperties->m_ThermalConductivity = m_GasData->GetPropertyValue( CoeffType::cCond, m_Temperature );
      m_GasProperties->m_Viscosity = m_GasData->GetPropertyValue( CoeffType::cVisc, m_Temperature );
      m_GasProperties->m_SpecificHeat = m_GasData->GetPropertyValue( CoeffType::cCp, m_Temperature );
      m_GasProperties->m_MolecularWeight = m_GasData->GetMolecularWeight();
      m_GasProperties->m_Density = m_Pressure * m_GasProperties->m_MolecularWeight / ( UNIVERSALGASCONSTANT * m_Temperature );
      m_GasProperties->calculateAlphaAndPrandl();
      m_GasProperties->m_PropertiesCalculated = true;
    }

    return m_GasProperties;
  }

  shared_ptr< GasProperties > CGasItem::getFractionalGasProperties() {
    if ( !m_FractionalGasProperties->m_PropertiesCalculated ) {
      shared_ptr< GasProperties > itemGasProperties;

      itemGasProperties = getGasProperties();

      // update for fractional data
      m_FractionalGasProperties->m_ThermalConductivity = itemGasProperties->m_ThermalConductivity * m_Fraction;
      m_FractionalGasProperties->m_Viscosity = itemGasProperties->m_Viscosity * m_Fraction;
      m_FractionalGasProperties->m_SpecificHeat = itemGasProperties->m_SpecificHeat * m_Fraction;
      m_FractionalGasProperties->m_MolecularWeight = itemGasProperties->m_MolecularWeight * m_Fraction;
      m_FractionalGasProperties->m_Density = itemGasProperties->m_Density * m_Fraction;
      m_FractionalGasProperties->m_Alpha = itemGasProperties->m_Alpha * m_Fraction;
      m_FractionalGasProperties->m_PrandlNumber = itemGasProperties->m_PrandlNumber * m_Fraction;
    }

    return m_FractionalGasProperties;
  }

};