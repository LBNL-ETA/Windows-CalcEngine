#ifndef TARCOGQBALANCE_H
#define TARCOGQBALANCE_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CSquareMatrix;
  class CLinearSolver;

}

namespace Tarcog {

  class CBaseTarcogLayer;
  class CTarIGU;
  class CTarLinearSolver;

  class CTarcogQBalance {
  public:
    explicit CTarcogQBalance( std::shared_ptr< CTarIGU > const t_IGU );

    std::shared_ptr< std::vector< double > > calcBalanceMatrix();
  private:
    void buildCell( std::shared_ptr< CBaseTarcogLayer > const t_Previous, 
      std::shared_ptr< CBaseTarcogLayer > const t_Current, 
      std::shared_ptr< CBaseTarcogLayer > const t_Next, int const t_Index );

    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_MatrixA;
    std::shared_ptr< std::vector< double > > m_VectorB;
    std::shared_ptr< FenestrationCommon::CLinearSolver > m_LinearSolver;

    std::shared_ptr< CTarIGU > m_IGU;
    
  };

}

#endif