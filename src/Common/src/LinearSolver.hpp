#ifndef LINEARSOLVER_H
#define LINEARSOLVER_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  class CSquareMatrix;

  class CLinearSolver {
  public:
    CLinearSolver();

    void assignMatrixA( std::shared_ptr< CSquareMatrix > t_MatrixA );
    void assignVectorB( std::shared_ptr< std::vector< double > > t_VectorB );

    std::shared_ptr< std::vector< double > > solveSystem( 
      std::shared_ptr< FenestrationCommon::CSquareMatrix > t_MatrixA, 
      std::shared_ptr< std::vector< double > > t_VectorB );

  private:
    std::shared_ptr< std::vector< double > > checkSingularity();
    std::shared_ptr< std::vector< size_t > > makeUpperTriangular();

    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_MatrixA;
    std::shared_ptr< std::vector< double > > m_VectorB;
  };
}

#endif