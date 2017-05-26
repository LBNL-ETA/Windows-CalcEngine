#ifndef TARCOGQBALANCE_H
#define TARCOGQBALANCE_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CSquareMatrix;
  class CLinearSolver;

}

namespace Tarcog {

  class CBaseLayer;
  class CIGU;
  class CTarLinearSolver;

  class CHeatFlowBalance {
  public:
    explicit CHeatFlowBalance( const std::shared_ptr< CIGU >& t_IGU );

    std::vector< double > calcBalanceMatrix();
  private:
    void buildCell( const std::shared_ptr< CBaseLayer >& t_Previous, 
      const std::shared_ptr< CBaseLayer >& t_Current, 
      const std::shared_ptr< CBaseLayer >& t_Next, const int t_Index );

    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_MatrixA;
    std::shared_ptr< std::vector< double > > m_VectorB;
    std::shared_ptr< FenestrationCommon::CLinearSolver > m_LinearSolver;

    std::shared_ptr< CIGU > m_IGU;
    
  };

}

#endif