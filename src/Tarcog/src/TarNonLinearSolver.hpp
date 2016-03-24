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
    explicit CTarNonLinearSolver( std::shared_ptr< CTarIGU > const t_IGU );

    void solve();

  private:
    double calculateTolerance( std::shared_ptr< std::vector< double > > t_Solution );
    void estimateNewState( std::shared_ptr< std::vector< double > > t_Solution );

    std::shared_ptr< CTarIGU > m_IGU;
    std::shared_ptr< FenestrationCommon::CLinearSolver > m_LinearSolver;
    std::shared_ptr< CTarcogQBalance > m_QBalance;
    std::shared_ptr< std::vector< double > > m_IGUState;
    double m_RelaxParam;
    double m_Tolerance;
    int m_Iterations;
  };

}


#endif