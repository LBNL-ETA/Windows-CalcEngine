#ifndef FENESTRATIONSQUAREMATRIX_H
#define FENESTRATIONSQUAREMATRIX_H

#include <vector>
#include <memory>

namespace FenestrationCommon {

  class CSquareMatrix {
  public:
    explicit CSquareMatrix( size_t const aSize );
    size_t getSize() const;
    void setZeros();
    // All diagonal items are one and all non diagonal are zero
    void setIdentity();
    // set diagonal values from vector
    void setDiagonal( const std::shared_ptr< const std::vector< double > >& t_Values );
    std::vector< double >& operator[]( const size_t index );
    std::shared_ptr< CSquareMatrix > add( const CSquareMatrix& t_Matrix ) const;
    std::shared_ptr< CSquareMatrix > sub( const CSquareMatrix& t_Matrix ) const;
    std::shared_ptr< CSquareMatrix > mult( const CSquareMatrix& t_Matrix ) const;
    // Matrix multiplication with vector
    std::shared_ptr< std::vector< double > > multMxV( const std::vector< double >& t_Vector );
    // Matrix multiplication with vector
    std::shared_ptr< std::vector< double > > multVxM( const std::vector< double >& t_Vector );
    void copyFrom( const CSquareMatrix& t_Matrix );
    // inverse matrix
    std::shared_ptr< CSquareMatrix > inverse();

  private:
    // LU decomposition of current matrix
    std::shared_ptr< CSquareMatrix > LU();

    size_t m_Size;
    std::vector< std::vector < double > > m_Matrix;

  };
}


#endif