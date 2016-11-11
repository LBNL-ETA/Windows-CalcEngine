#include "TarcogSystem.hpp"
#include "TarIGU.hpp"
#include "TarEnvironment.hpp"
#include "TarcogSingleSystem.hpp"

using namespace std;

namespace Tarcog {

  CTarcogSystem::CTarcogSystem( shared_ptr< CTarIGU > t_IGU,
    shared_ptr< CTarEnvironment > t_Indoor,
    shared_ptr< CTarEnvironment > t_Outdoor ) {
    m_System[ System::SHGC ] = make_shared< CTarcogSingleSystem >( t_IGU, t_Indoor, t_Outdoor );
    m_System[ System::Uvalue ] = make_shared< CTarcogSingleSystem >( *m_System.at( System::SHGC ) );
    m_System.at( System::Uvalue )->setSolarRadiation( 0 ); 

    for( auto& aSystem : m_System ) {
      aSystem.second->solve();
    }

  }

  shared_ptr< vector< double > > CTarcogSystem::getTemperatures( const System t_System ) const {
    return m_System.at( t_System )->getTemperatures();
  }

  shared_ptr< vector< double > > CTarcogSystem::getRadiosities( const System t_System ) const {
    return m_System.at( t_System )->getRadiosities();
  }

  shared_ptr< vector< double > > CTarcogSystem::getMaxDeflections( const System t_System ) const {
    return m_System.at( t_System )->getMaxDeflections();
  }

  shared_ptr< vector< double > > CTarcogSystem::getMeanDeflections( const System t_System ) const {
    return m_System.at( t_System )->getMeanDeflections();
  }

  double CTarcogSystem::getHeatFlow( const System t_System, const Environment t_Environment ) const {
    return m_System.at( t_System )->getHeatFlow( t_Environment );
  }

  double CTarcogSystem::getUValue() const {
    return m_System.at( System::Uvalue )->getUValue();
  }

  double CTarcogSystem::getSHGC( const double t_TotSol ) const {
    return t_TotSol - ( m_System.at( System::SHGC )->getHeatFlow( Environment::Indoor ) -
      m_System.at( System::Uvalue )->getHeatFlow( Environment::Indoor ) ) /
      m_System.at( System::SHGC )->getSolarRadiation();
  }

  size_t CTarcogSystem::getNumberOfIterations( const System t_System ) const {
    return m_System.at( t_System )->getNumberOfIterations();
  }

}
