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
    explicit CTarcogQBalance( const std::shared_ptr< CTarIGU >& t_IGU );

    std::shared_ptr< std::vector< double > > calcBalanceMatrix();
  private:
    void buildCell( const std::shared_ptr< CBaseTarcogLayer >& t_Previous, 
      const std::shared_ptr< CBaseTarcogLayer >& t_Current, 
      const std::shared_ptr< CBaseTarcogLayer >& t_Next, const int t_Index );

    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_MatrixA;
    std::shared_ptr< std::vector< double > > m_VectorB;
    std::shared_ptr< FenestrationCommon::CLinearSolver > m_LinearSolver;

    std::shared_ptr< CTarIGU > m_IGU;
    
  };

}

#endif