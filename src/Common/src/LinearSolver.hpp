#ifndef LINEARSOLVER_H
#define LINEARSOLVER_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CSquareMatrix;

  class CLinearSolver {
  public:
    CLinearSolver();

    std::vector< double > solveSystem(
      FenestrationCommon::CSquareMatrix& t_MatrixA,
      std::vector< double >& t_VectorB );

  private:
    std::vector< double > checkSingularity( FenestrationCommon::CSquareMatrix& t_MatrixA );
    std::vector< size_t > makeUpperTriangular( FenestrationCommon::CSquareMatrix& t_MatrixA );

  };
}

#endif