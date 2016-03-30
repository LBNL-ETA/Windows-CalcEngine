#ifndef GASPROPERTIES_H
#define GASPROPERTIES_H

namespace Gases
{

  enum class CoeffType { cCond, cVisc, cCp };

  class CIntCoeff {
  public:
    CIntCoeff( double t_A, double t_B, double t_C );
    double interpolationValue( double t_Temperature ) const;
    CIntCoeff& operator=(const CIntCoeff &t_IntCoeff);
  private:
    double m_A;
    double m_B;
    double m_C;
  };

  struct GasProperties {
    GasProperties() :
      m_ThermalConductivity( 0.0 ), m_Viscosity( 0.0 ), m_SpecificHeat( 0.0 ), m_Density( 0.0 ),
      m_MolecularWeight( 0.0 ), m_Alpha( 0.0 ), m_PrandlNumber( 0.0 ), m_PropertiesCalculated( false )
    { };

    double getLambdaPrim();
    double getLambdaSecond();
    GasProperties& operator+( const GasProperties& t_A );
    GasProperties& operator+=( const GasProperties& t_A );

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

};

#endif