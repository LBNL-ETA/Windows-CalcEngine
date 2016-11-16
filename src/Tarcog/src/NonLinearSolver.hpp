#ifndef TARNONLINEARSOLVER_H
#define TARNONLINEARSOLVER_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CLinearSolver;

}

namespace Tarcog {

  class CHeatFlowBalance;
  class CIGU;

  class CNonLinearSolver {
  public:
    explicit CNonLinearSolver( const std::shared_ptr< CIGU >& t_IGU );

    // sets tolerance for solution
    void setTolerance( const double t_Tolerance );

    // returns number of iterations for current solution.
    size_t getNumOfIterations() const;

    void solve();

  private:
    double calculateTolerance( const std::vector< double >& t_Solution );
    void estimateNewState( const std::vector< double >& t_Solution );

    std::shared_ptr< CIGU > m_IGU;
    std::shared_ptr< FenestrationCommon::CLinearSolver > m_LinearSolver;
    std::shared_ptr< CHeatFlowBalance > m_QBalance;
    std::shared_ptr< std::vector< double > > m_IGUState;
    double m_Tolerance;
    size_t m_Iterations;
    double m_RelaxParam;
  };

}


#endif