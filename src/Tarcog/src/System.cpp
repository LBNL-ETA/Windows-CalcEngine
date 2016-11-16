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

  shared_ptr< vector< double > > CSystem::getTemperatures( const System t_System ) const {
    return m_System.at( t_System )->getTemperatures();
  }

  shared_ptr< vector< double > > CSystem::getRadiosities( const System t_System ) const {
    return m_System.at( t_System )->getRadiosities();
  }

  shared_ptr< vector< double > > CSystem::getMaxDeflections( const System t_System ) const {
    return m_System.at( t_System )->getMaxDeflections();
  }

  shared_ptr< vector< double > > CSystem::getMeanDeflections( const System t_System ) const {
    return m_System.at( t_System )->getMeanDeflections();
  }

  double CSystem::getHeatFlow( const System t_System, const Environment t_Environment ) const {
    return m_System.at( t_System )->getHeatFlow( t_Environment );
  }

  double CSystem::getUValue() const {
    return m_System.at( System::Uvalue )->getUValue();
  }

  double CSystem::getSHGC( const double t_TotSol ) const {
    return t_TotSol - ( m_System.at( System::SHGC )->getHeatFlow( Environment::Indoor ) -
      m_System.at( System::Uvalue )->getHeatFlow( Environment::Indoor ) ) /
      m_System.at( System::SHGC )->getSolarRadiation();
  }

  size_t CSystem::getNumberOfIterations( const System t_System ) const {
    return m_System.at( t_System )->getNumberOfIterations();
  }

}