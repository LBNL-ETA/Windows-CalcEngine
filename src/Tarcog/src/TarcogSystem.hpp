#ifndef TARCOGSYSTEM_H
#define TARCOGSYSTEM_H

#include <memory>
#include <vector>
#include <map>

namespace Tarcog {

  class CTarIGU;
  class CTarEnvironment;
  class CTarcogSingleSystem;
  enum class Environment;

  enum class System { Uvalue, SHGC };
  
  class CTarcogSystem {
  public:
    CTarcogSystem( std::shared_ptr< CTarIGU > t_IGU,
      std::shared_ptr< CTarEnvironment > t_Indoor,
      std::shared_ptr< CTarEnvironment > t_Outdoor );

    std::shared_ptr< std::vector< double > > getTemperatures( const System t_System ) const;
    std::shared_ptr< std::vector< double > > getRadiosities( const System t_System ) const;

    std::shared_ptr< std::vector< double > > getMaxDeflections( const System t_System ) const;
    std::shared_ptr< std::vector< double > > getMeanDeflections( const System t_System ) const;

    double getHeatFlow( const System t_System, const Environment t_Environment ) const;
    double getUValue() const;
    double getSHGC( const double t_TotSol ) const;
    size_t getNumberOfIterations( const System t_System ) const;

  private:
    std::map< System, std::shared_ptr< CTarcogSingleSystem > > m_System;
  };

}

#endif