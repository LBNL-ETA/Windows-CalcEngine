#ifndef TARCOGSYSTEM_H
#define TARCOGSYSTEM_H

#include <memory>
#include <vector>
#include <map>

namespace Tarcog {

  class CIGU;
  class CEnvironment;
  class CSingleSystem;
  enum class Environment;

  enum class System { Uvalue, SHGC };
  
  class CSystem {
  public:
    CSystem( std::shared_ptr< CIGU > t_IGU,
      std::shared_ptr< CEnvironment > t_Indoor,
      std::shared_ptr< CEnvironment > t_Outdoor );

    std::shared_ptr< std::vector< double > > getTemperatures( const System t_System ) const;
    std::shared_ptr< std::vector< double > > getRadiosities( const System t_System ) const;

    std::shared_ptr< std::vector< double > > getMaxDeflections( const System t_System ) const;
    std::shared_ptr< std::vector< double > > getMeanDeflections( const System t_System ) const;

    double getHeatFlow( const System t_System, const Environment t_Environment ) const;
    double getUValue() const;
    double getSHGC( const double t_TotSol ) const;
    size_t getNumberOfIterations( const System t_System ) const;

  private:
    std::map< System, std::shared_ptr< CSingleSystem > > m_System;
  };

}

#endif