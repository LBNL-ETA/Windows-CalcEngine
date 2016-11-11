#ifndef TARCOGSINGLESYSTEM_H
#define TARCOGSINGLESYSTEM_H

#include <memory>
#include <map>
#include <vector>

namespace Tarcog {

  enum class Environment;
  class CTarIGU;
  class CBaseIGUTarcogLayer;
  class CTarIGUSolidLayer;
  class CTarIGUGapLayer;
  class CTarEnvironment;
  class CTarNonLinearSolver;
  
  class CTarcogSingleSystem {
  public:
    CTarcogSingleSystem( std::shared_ptr< CTarIGU > t_IGU,
                   std::shared_ptr< CTarEnvironment > t_Indoor,
                   std::shared_ptr< CTarEnvironment > t_Outdoor );

    CTarcogSingleSystem( const CTarcogSingleSystem& t_SingleSystem );

    std::vector< std::shared_ptr< CTarIGUSolidLayer > > getSolidLayers() const;
    std::vector< std::shared_ptr< CTarIGUGapLayer > > getGapLayers();

    std::shared_ptr< std::vector< double > > getTemperatures() const;
    std::shared_ptr< std::vector< double > > getRadiosities() const;

    std::shared_ptr< std::vector< double > > getMaxDeflections() const;
    std::shared_ptr< std::vector< double > > getMeanDeflections() const;

    std::shared_ptr< CTarcogSingleSystem > clone() const;

    double getHeatFlow( const Environment t_Environment ) const;
    double getConvectiveHeatFlow( const Environment t_Environment ) const;
    double getRadiationHeatFlow( const Environment t_Environment ) const;

    // If interior layer have openings, this will return heat flow from airflow
    double getVentilationFlow( const Environment t_Environment ) const;
    double getUValue() const;
    size_t getNumberOfIterations() const;

    // Set solution tolerance
    void setTolerance( const double t_Tolerance );
    // Set intial guess for solution.
    void setInitialGuess( const std::shared_ptr< std::vector< double > >& t_Temperatures );

    void setSolarRadiation( double const t_SolarRadiation );
    double getSolarRadiation() const;

    void solve();

  private:
    std::shared_ptr< CTarIGU > m_IGU;
    std::map< Environment, std::shared_ptr< CTarEnvironment > > m_Environment;
    std::shared_ptr< CTarNonLinearSolver > m_NonLinearSolver;
    void initializeStartValues();
  };

}

#endif