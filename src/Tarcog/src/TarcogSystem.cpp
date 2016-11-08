#include "TarcogSystem.hpp"
#include "TarIGU.hpp"
#include "TarcogSingleSystem.hpp"

using namespace std;

namespace Tarcog {

  CTarcogSystem::CTarcogSystem( shared_ptr< CTarIGU > t_IGU, 
                                shared_ptr< CTarEnvironment > t_Indoor, 
                                shared_ptr< CTarEnvironment > t_Outdoor ) {
    m_IGUSolar = make_shared< CTarcogSingleSystem >( t_IGU, t_Indoor, t_Outdoor );
    m_IGU = make_shared< CTarcogSingleSystem >( *m_IGUSolar );
    m_IGU->setSolarRadiation( 0 );
  }

}
