#ifndef TARNONLINEARSOLVER_H
#define TARNONLINEARSOLVER_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CLinearSolver;

}

namespace Tarcog {

  class CTarcogQBalance;
  class CTarIGU;

  class CTarNonLinearSolver {
  public:
    explicit CTarNonLinearSolver( std::shared_ptr< CTarIGU > t_IGU );

    // sets tolerance for solution
    void setTolerance( const double t_Tolerance );

    // returns number of iterations for current solution.
    size_t getNumOfIterations() const;

    void solve();

  private:
    double calculateTolerance( std::shared_ptr< std::vector< double > > t_Solution );
    void estimateNewState( std::shared_ptr< std::vector< double > > t_Solution );

    std::shared_ptr< CTarIGU > m_IGU;
    std::shared_ptr< FenestrationCommon::CLinearSolver > m_LinearSolver;
    std::shared_ptr< CTarcogQBalance > m_QBalance;
    std::shared_ptr< std::vector< double > > m_IGUState;
    double m_Tolerance;
    size_t m_Iterations;
    double m_RelaxParam;
  };

}


#endif