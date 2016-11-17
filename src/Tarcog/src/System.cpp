#include "System.hpp"
#include "IGU.hpp"
#include "Environment.hpp"
#include "SingleSystem.hpp"

using namespace std;

namespace Tarcog {

  CSystem::CSystem( shared_ptr< CIGU > t_IGU,
    shared_ptr< CEnvironment > t_Indoor,
    shared_ptr< CEnvironment > t_Outdoor ) {
    m_System[ System::SHGC ] = make_shared< CSingleSystem >( t_IGU, t_Indoor, t_Outdoor );
    m_System[ System::Uvalue ] = make_shared< CSingleSystem >( *m_System.at( System::SHGC ) );
    m_System.at( System::Uvalue )->setSolarRadiation( 0 ); 

    for( auto& aSystem : m_System ) {
      aSystem.second->solve();
    }

  }

  shared_ptr< vector< double > > CSystem::getTemperatures( System const t_System ) const {
    return m_System.at( t_System )->getTemperatures();
  }

  shared_ptr< vector< double > > CSystem::getRadiosities( System const t_System ) const {
    return m_System.at( t_System )->getRadiosities();
  }

  shared_ptr< vector< double > > CSystem::getMaxDeflections( System const t_System ) const {
    return m_System.at( t_System )->getMaxDeflections();
  }

  shared_ptr< vector< double > > CSystem::getMeanDeflections( System const t_System ) const {
    return m_System.at( t_System )->getMeanDeflections();
  }

  vector< shared_ptr< CIGUSolidLayer > > CSystem::getSolidLayers( System const t_System ) const {
    return m_System.at( t_System )->getSolidLayers();
  }

  double CSystem::getHeatFlow( System const t_System, Environment const t_Environment ) const {
    return m_System.at( t_System )->getHeatFlow( t_Environment );
  }

  double CSystem::getUValue() const {
    return m_System.at( System::Uvalue )->getUValue();
  }

  double CSystem::getSHGC( double const t_TotSol ) const {
    return t_TotSol - ( m_System.at( System::SHGC )->getHeatFlow( Environment::Indoor ) -
      m_System.at( System::Uvalue )->getHeatFlow( Environment::Indoor ) ) /
      m_System.at( System::SHGC )->getSolarRadiation();
  }

  size_t CSystem::getNumberOfIterations( System const t_System ) const {
    return m_System.at( t_System )->getNumberOfIterations();
  }

}
