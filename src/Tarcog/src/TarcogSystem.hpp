#ifndef TARCOGSYSTEM_H
#define TARCOGSYSTEM_H

#include <memory>
#include <vector>

namespace Tarcog {

  class CTarIGU;
  class CBaseIGUTarcogLayer;
  class CTarIGUSolidLayer;
  class CTarEnvironment;
  class CTarIndoorEnvironment;
  class CTarOutdoorEnvironment;
  class CTarNonLinearSolver;
  
  class CTarcogSystem {
  public:
    CTarcogSystem( std::shared_ptr< CTarIGU > t_IGU,
                   std::shared_ptr< CTarEnvironment > t_Indoor,
                   std::shared_ptr< CTarEnvironment > t_Outdoor );

    std::shared_ptr< CTarEnvironment > getIndoor() const;
    std::shared_ptr< CTarEnvironment > getOutdoor() const;
    std::vector< std::shared_ptr < CBaseIGUTarcogLayer > > getLayers() const;
    std::vector< std::shared_ptr< CTarIGUSolidLayer > > getSolidLayers();

    // Set solution tolerance
    void setTolerance( const double t_Tolerance );
    // Returns number of iterations
    size_t getNumberOfIterations() const;
    // Set intial guess for solution.
    void setInitialGuess( std::shared_ptr< std::vector< double > > t_Temperatures );

    void solve();

  private:
    std::shared_ptr< CTarIGU > m_IGU;
    std::shared_ptr< CTarEnvironment > m_Indoor;
    std::shared_ptr< CTarEnvironment > m_Outdoor;
    std::shared_ptr< CTarNonLinearSolver > m_NonLinearSolver;
    void initializeStartValues();
  };

}

#endif