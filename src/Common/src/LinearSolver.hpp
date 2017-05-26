#ifndef LINEARSOLVER_H
#define LINEARSOLVER_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CSquareMatrix;

  class CLinearSolver {
  public:
    CLinearSolver();

    std::shared_ptr< std::vector< double > > solveSystem( 
      std::shared_ptr< FenestrationCommon::CSquareMatrix > const & t_MatrixA,
      std::shared_ptr< std::vector< double > > const & t_VectorB );

  private:
    std::shared_ptr< std::vector< double > > checkSingularity();
    std::shared_ptr< std::vector< size_t > > makeUpperTriangular();

    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_MatrixA;
    std::shared_ptr< std::vector< double > > m_VectorB;
  };
}

#endif