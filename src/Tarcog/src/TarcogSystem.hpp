#ifndef TARCOGSYSTEM_H
#define TARCOGSYSTEM_H

#include <memory>

namespace Tarcog {

  class CTarIGU;
  class CTarEnvironment;
  class CTarcogSingleSystem;
  
  class CTarcogSystem {
  public:
    CTarcogSystem( std::shared_ptr< CTarIGU > t_IGU,
      std::shared_ptr< CTarEnvironment > t_Indoor,
      std::shared_ptr< CTarEnvironment > t_Outdoor );

  private:
    std::shared_ptr< CTarcogSingleSystem > m_IGUSolar;
    std::shared_ptr< CTarcogSingleSystem > m_IGU;
  };

}

#endif