#include <stdexcept>
#include <math.h>
#include "Gas.hpp"
#include "GasData.hpp"
#include "GasItem.hpp"
#include "GasProperties.hpp"

using namespace std;

namespace Gases
{

  CGas::CGas() : m_SimpleProperties( make_shared< GasProperties > () ), m_Properties( make_shared< GasProperties > () ) {
    // create default gas to be Air
    shared_ptr< CGasItem > Air = make_shared< CGasItem >();
    m_GasItem.push_back( Air );
    m_DefaultGas = true;
  }

  CGas::CGas( const CGas& t_Gas ) {
    m_GasItem.clear();
    for( shared_ptr< CGasItem > item : t_Gas.m_GasItem ) {
      shared_ptr< CGasItem > aItem = make_shared< CGasItem >( *item );
      m_GasItem.push_back( aItem );
    }
    m_GasItem = t_Gas.m_GasItem;
    m_SimpleProperties = t_Gas.m_SimpleProperties;
    m_Properties = t_Gas.m_Properties;
    m_DefaultGas = t_Gas.m_DefaultGas;
  }

  void CGas::addGasItem( const shared_ptr< CGasItem >& t_GasItem ) {
    // Need to remove default since user wants to create their own Gases
    if ( m_DefaultGas ) {
      m_GasItem.clear();
      m_DefaultGas = false;
    }
    m_GasItem.push_back( t_GasItem );
  }

  double CGas::totalPercent()
  {
    double totalPercent = 0;

    vector< shared_ptr< CGasItem > >::const_iterator it;
    for( it = m_GasItem.begin(); it<m_GasItem.end(); ++it ) {
      totalPercent += (*it)->getFraction();
    }

    return totalPercent;
  }

  void CGas::setTemperatureAndPressure( double t_Temperature, double t_Pressure ) {
    vector< shared_ptr< CGasItem > >::const_iterator it;
    for( it = m_GasItem.begin(); it<m_GasItem.end(); ++it ) {
      ( *it )->setTemperature( t_Temperature );
      ( *it )->setPressure( t_Pressure );
    }
  }

  shared_ptr< GasProperties > CGas::getSimpleGasProperties()
  {
    vector< shared_ptr< CGasItem > >::iterator it;

    for ( it = m_GasItem.begin(); it != m_GasItem.end(); ++it ) {
      if ( it == m_GasItem.begin() ) {
        *m_SimpleProperties = *(( *it )->getFractionalGasProperties());
      } else {
        *m_SimpleProperties += *(( *it )->getFractionalGasProperties());
      }
    }

    return m_SimpleProperties;
  }

  shared_ptr< GasProperties > CGas::getGasProperties()
  {
    shared_ptr< GasProperties > simpleProperties = getSimpleGasProperties();

    // coefficients for intermediate calculations
    vector< vector< double > > miItem;
    vector< vector< double > > lambdaPrimItem;
    vector< vector< double > > lambdaSecondItem;

    size_t gasSize = m_GasItem.size();
    int counter = 0;

    miItem.resize( gasSize );
    lambdaPrimItem.resize( gasSize );
    lambdaSecondItem.resize( gasSize );

    // first to calculate denominator properties for gas mixtures
    vector< shared_ptr< CGasItem > >::iterator primaryIt;
    vector< shared_ptr< CGasItem > >::iterator secondaryIt;
    
    for ( primaryIt = m_GasItem.begin(); primaryIt != m_GasItem.end(); ++primaryIt ) {
      for ( secondaryIt = m_GasItem.begin(); secondaryIt != m_GasItem.end(); ++secondaryIt ) {
        if ( *primaryIt != *secondaryIt ) {
          miItem[counter].push_back( viscDenomTwoGases( *( *primaryIt ), *( *secondaryIt ) ) );
          lambdaPrimItem[counter].push_back( lambdaPrimDenomTwoGases( *( *primaryIt ), *( *secondaryIt ) ) );
          lambdaSecondItem[counter].push_back( lambdaSecondDenomTwoGases( *( *primaryIt ), *( *secondaryIt ) ) );
        }
        else {
          miItem[counter].push_back( 0 );
          lambdaPrimItem[counter].push_back( 0 );
          lambdaSecondItem[counter].push_back( 0 );
        }
      }
      counter++;
    }

    double miMix( 0 );
    double lambdaPrimMix( 0 );
    double lambdaSecondMix( 0 );
    double cpMix( 0 );

    counter = 0;
    vector< shared_ptr< CGasItem > >::iterator it;
    for ( it = m_GasItem.begin(); it != m_GasItem.end(); ++it ) {
      double sumMix = 1;
      shared_ptr< GasProperties > itGasProperties = (*it)->getGasProperties();

      double lambdaPrim( itGasProperties->getLambdaPrim() );
      double lambdaSecond( itGasProperties->getLambdaSecond() );

      for( size_t i = 0; i < gasSize; ++i ) {
        sumMix += miItem[counter][i];
      }

      miMix += itGasProperties->m_Viscosity / sumMix;

      sumMix = 1;
      for( size_t i = 0; i < gasSize; ++i ) {
        sumMix += lambdaPrimItem[counter][i];
      }

      lambdaPrimMix += lambdaPrim / sumMix;

      sumMix = 1;
      for( size_t i = 0; i < gasSize; ++i ) {
        sumMix += lambdaSecondItem[counter][i];
      }

      lambdaSecondMix += lambdaSecond / sumMix;

      cpMix += itGasProperties->m_SpecificHeat * (*it)->getFraction() * itGasProperties->m_MolecularWeight;
      ++counter;
    }

    m_Properties->m_ThermalConductivity = lambdaPrimMix + lambdaSecondMix;
    m_Properties->m_Viscosity = miMix;
    m_Properties->m_SpecificHeat = cpMix / simpleProperties->m_MolecularWeight;
    m_Properties->m_Density = simpleProperties->m_Density;
    m_Properties->m_MolecularWeight = simpleProperties->m_MolecularWeight;
    m_Properties->calculateAlphaAndPrandl();


    return m_Properties;

  }

  // This implements equation 63 (ISO 15099)
  double CGas::viscTwoGases( const GasProperties& t_Gas1Properties,
    const GasProperties& t_Gas2Properties ) const {
    
    if ( ( t_Gas1Properties.m_Viscosity == 0 ) || ( t_Gas2Properties.m_Viscosity == 0 ) ) {
      throw runtime_error( "Viscosity of the gas component in Gases is equal to zero." );
    }
    if ( ( t_Gas1Properties.m_MolecularWeight == 0 ) || ( t_Gas2Properties.m_MolecularWeight == 0 ) ) {
      throw runtime_error( "Molecular weight of the gas component in Gases is equal to zero." );
    }

    double uFraction = t_Gas1Properties.m_Viscosity / t_Gas2Properties.m_Viscosity;
    double weightFraction = t_Gas1Properties.m_MolecularWeight / t_Gas2Properties.m_MolecularWeight;
    double nominator = pow( (1 + pow( uFraction, 0.5 ) * pow( 1 / weightFraction, 0.25 ) ), 2 );
    double denominator = 2 * sqrt( 2.0 ) * pow( 1 + weightFraction, 0.5 );

    if( denominator == 0 ) {
      throw runtime_error( "Dynamic viscosity coefficient is gas mixture is calculated to be zero." );
    }

    return nominator / denominator;

  }

  // Implementation of sum items in denominator of equation 62 (ISO15099)
  double CGas::viscDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2 ) {

    double phiValue = viscTwoGases( *t_GasItem1.getGasProperties(), *t_GasItem2.getGasProperties() );
    if ( ( t_GasItem1.getFraction() == 0 ) || ( t_GasItem2.getFraction() == 0 ) ) {
      throw runtime_error( "Fraction of gas component in gas mixture is set to be equal to zero." );
    }

    return ( t_GasItem2.getFraction() / t_GasItem1.getFraction() ) * phiValue;

  }

  // This implements equation 66 (ISO 15099)
  double CGas::lambdaPrimTwoGases( const GasProperties& t_Gas1Properties, 
    const GasProperties& t_Gas2Properties ) {

    if ( ( t_Gas1Properties.m_MolecularWeight == 0 ) || ( t_Gas2Properties.m_MolecularWeight == 0 ) ) {
      throw runtime_error( "Molecular weight of the gas component in Gases is equal to zero." );
    }

    double item1 = lambdaSecondTwoGases( t_Gas1Properties, t_Gas2Properties );
    double item2 = 1 + 2.41 * ( ( t_Gas1Properties.m_MolecularWeight - t_Gas2Properties.m_MolecularWeight ) *
      ( t_Gas1Properties.m_MolecularWeight - 0.142 * t_Gas2Properties.m_MolecularWeight ) / 
      pow(( t_Gas1Properties.m_MolecularWeight + t_Gas2Properties.m_MolecularWeight ), 2) );

    return item1 * item2;

  }

  // This implements equation 68 (ISO 15099)
  double CGas::lambdaSecondTwoGases( const GasProperties& t_Gas1Properties, 
    const GasProperties& t_Gas2Properties ) {

    if ( ( t_Gas1Properties.getLambdaPrim() == 0 ) || ( t_Gas2Properties.getLambdaPrim() == 0 ) ) {
      throw runtime_error( "Primary thermal conductivity (lambda prim) of the gas component in Gases is equal to zero." );
    }

    if ( ( t_Gas1Properties.m_MolecularWeight == 0 ) || ( t_Gas2Properties.m_MolecularWeight == 0 ) ) {
      throw runtime_error( "Molecular weight of the gas component in Gases is equal to zero." );
    }

    double tFraction = t_Gas1Properties.getLambdaPrim() / t_Gas2Properties.getLambdaPrim();
    double weightFraction = t_Gas1Properties.m_MolecularWeight / t_Gas2Properties.m_MolecularWeight;
    double nominator = pow( ( 1 + pow( tFraction, 0.5 ) * pow( weightFraction, 0.25 ) ), 2 );
    double denominator = 2 * sqrt( 2.0 ) * pow( (1 + weightFraction ), 0.5 );

    if ( denominator == 0 ) {
      throw runtime_error( "Thermal conductivity coefficient in gas mixture is calculated to be zero." );
    }

    return nominator / denominator;

  }

  // Implementation of sum items in denominator of equation 65 (ISO15099)
  double CGas::lambdaPrimDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2 ) {

    double phiValue = lambdaPrimTwoGases( *t_GasItem1.getGasProperties(), *t_GasItem2.getGasProperties() );

    if ( ( t_GasItem1.getFraction() == 0 ) || ( t_GasItem2.getFraction() == 0 ) ) {
      throw runtime_error( "Fraction of gas component in gas mixture is set to be equal to zero." );
    }

    return (t_GasItem2.getFraction() / t_GasItem1.getFraction()) * phiValue;

  }

  // Implementation of sum items in denominator of equation 67 (ISO15099)
  double CGas::lambdaSecondDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2 ) {

    double phiValue = lambdaSecondTwoGases( *t_GasItem1.getGasProperties(), *t_GasItem2.getGasProperties() );

    if ( ( t_GasItem1.getFraction() == 0 ) || ( t_GasItem2.getFraction() == 0 ) ) {
      throw runtime_error( "Fraction of gas component in gas mixture is set to be equal to zero." );
    }

    return ( t_GasItem2.getFraction() / t_GasItem1.getFraction() ) * phiValue;

  }

  CGas& CGas::operator=( const CGas& t_Gas ) {
    m_GasItem.clear();
    for( shared_ptr< CGasItem > item : t_Gas.m_GasItem ) {
      shared_ptr< CGasItem > aItem = make_shared< CGasItem >( *item );
      m_GasItem.push_back( aItem );
    }
    m_SimpleProperties = t_Gas.m_SimpleProperties;
    m_Properties = t_Gas.m_Properties;
    m_DefaultGas = t_Gas.m_DefaultGas;

    return *this;
  }

}
