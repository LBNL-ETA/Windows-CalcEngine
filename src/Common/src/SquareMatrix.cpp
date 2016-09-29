#include <assert.h>
#include <stdexcept>
#include <thread>

#include "SquareMatrix.hpp"

using namespace std;

namespace FenestrationCommon {
  CSquareMatrix::CSquareMatrix( const size_t aSize ) {
    m_Size = aSize;
    m_Matrix.resize( m_Size );
    for( size_t i = 0; i < m_Size; ++i )
      m_Matrix[ i ].resize( m_Size );
  }

  size_t CSquareMatrix::getSize() const {
    return m_Size;
  }

  void CSquareMatrix::setZeros() {
    for( size_t i = 0; i < m_Size; ++i ) {
      for( size_t j = 0; j < m_Size; ++j ) {
        m_Matrix[ i ][ j ] = 0;
      }
    }
  }

  void CSquareMatrix::setIdentity() {
    setZeros();
    for( size_t i = 0; i < m_Size; ++i ) {
      m_Matrix[ i ][ i ] = 1;
    }
  }

  void CSquareMatrix::setDiagonal( const vector< double >& t_Values ) {
    if( t_Values.size() != m_Size ) {
      throw runtime_error("Supplied vector size mismatch matrix size");
    }
    setZeros();
    for( size_t i = 0; i < m_Size; ++i ) {
      m_Matrix[ i ][ i ] = t_Values[ i ];
    }
  }

  vector< double >& CSquareMatrix::operator[]( const size_t index ) {
    assert( index < m_Size );
    return m_Matrix[ index ];
  }

  shared_ptr< CSquareMatrix > CSquareMatrix::add( const CSquareMatrix& t_Matrix ) const {
    if( m_Size != t_Matrix.m_Size ) {
      throw runtime_error("Matrices must be identical in size.");
    }

    shared_ptr< CSquareMatrix > aMatrix = make_shared< CSquareMatrix >( m_Size );
    for( size_t i = 0; i < m_Size; ++i ) {
      for( size_t j = 0; j < t_Matrix.m_Size; ++j ) {
        ( *aMatrix )[ i ][ j ] = m_Matrix[ i ][ j ] + t_Matrix.m_Matrix[ i ][ j ];
      }
    }

    return aMatrix;
  }

  shared_ptr< CSquareMatrix > CSquareMatrix::sub( const CSquareMatrix& t_Matrix ) const {
    if( m_Size != t_Matrix.m_Size ) {
      throw runtime_error("Matrices must be identical in size.");
    }

    shared_ptr< CSquareMatrix > aMatrix = make_shared< CSquareMatrix >( m_Size );
    for( size_t i = 0; i < m_Size; ++i ) {
      for( size_t j = 0; j < t_Matrix.m_Size; ++j ) {
        ( *aMatrix )[ i ][ j ] = m_Matrix[ i ][ j ] - t_Matrix.m_Matrix[ i ][ j ];
      }
    }

    return aMatrix;
  }

  shared_ptr< CSquareMatrix > CSquareMatrix::mult( const CSquareMatrix& t_Matrix ) const {
    if( m_Size != t_Matrix.m_Size ) {
      throw runtime_error("Matrices must be identical in size.");
    }

    shared_ptr< CSquareMatrix > aMatrix = make_shared< CSquareMatrix >( m_Size );
    
    for( size_t i = 0; i < m_Size; ++i ) {
      for( size_t j = 0; j < t_Matrix.m_Size; ++j ) {
        for( size_t k = 0; k < m_Size; ++k ) {
          ( *aMatrix )[ i ][ j ] = ( *aMatrix )[ i ][ j ] + m_Matrix[ i ][ k ] * t_Matrix.m_Matrix[ k ][ j ];
        }
      }
    }

    return aMatrix;

  }

  shared_ptr< vector< double > > CSquareMatrix::multMxV( const vector< double >& t_Vector ) const {
    if( m_Size != t_Vector.size() ) {
      throw runtime_error("Matrix and vector does not have same number of rows and columns. It is not possible to perform multiplication.");
    }

    shared_ptr< vector< double > > aResult = make_shared< vector< double > >( m_Size );

    for( size_t i = 0; i < m_Size; ++i ) {
      for( size_t j = 0; j < m_Size; ++j ) {
        ( *aResult )[ i ] = ( *aResult )[ i ] + m_Matrix[ i ][ j ] * t_Vector[ j ];
      }
    }

    return aResult;
  }

  shared_ptr< vector< double > > CSquareMatrix::multVxM( const vector< double >& t_Vector ) const {
    if( m_Size != t_Vector.size() ) {
      throw runtime_error("Matrix and vector does not have same number of rows and columns. It is not possible to perform multiplication.");
    }

    shared_ptr< vector< double > > aResult = make_shared< vector< double > >( m_Size );

    for( size_t i = 0; i < m_Size; ++i ) {
      for( size_t j = 0; j < m_Size; ++j ) {
        ( *aResult )[ i ] = ( *aResult )[ i ] + m_Matrix[ j ][ i ] * t_Vector[ j ];
      }
    }

    return aResult;
  }

  void CSquareMatrix::copyFrom( const CSquareMatrix& t_Matrix ) {
    if( m_Size != t_Matrix.m_Size ) {
      throw runtime_error("Matrices must be identical in size");
    }
    for( size_t i = 0; i < m_Size; ++i ) {
      m_Matrix[ i ] = t_Matrix.m_Matrix[ i ];
    }
  }

  shared_ptr< CSquareMatrix > CSquareMatrix::inverse() {
    // return LU decomposed matrix of current matrix
    shared_ptr< CSquareMatrix > aLU = LU();

    // find the inverse
    shared_ptr< CSquareMatrix > inverse = make_shared< CSquareMatrix >( m_Size );
    vector< double > d( m_Size );
    vector< double > y( m_Size );

    int size = int( m_Size - 1 );

    for( int m = 0; m <= size; ++m ) {
      fill( d.begin(), d.end(), 0 );
      fill( y.begin(), y.end(), 0 );
      d[ m ] = 1;
      for( int i = 0; i <= size; ++i ) { 
        double x = 0;
        for( int j = 0; j <= i - 1; ++j ) {
          x = x + ( *aLU )[ i ][ j ] * y[ j ];
        }
         y[ i ] = ( d[ i ] - x );
      }
  
      for( int i = size; i >= 0; --i ) { 
        double x = 0.0;
        for( int j = i + 1; j <= size; ++j ) { 
          x = x + ( *aLU )[ i ][ j ] * ( *inverse )[ j ][ m ];
        }
         ( *inverse )[ i ][ m ] = ( y[ i ] - x ) / ( *aLU )[ i ][ i ];
      }
    }

    return inverse;
  }

  shared_ptr< CSquareMatrix > CSquareMatrix::LU() {
    shared_ptr< CSquareMatrix > D = make_shared< CSquareMatrix >( m_Size );
    D->copyFrom( *this );

    for( size_t k = 0; k <= m_Size - 2; ++k ) {
      for( size_t j = k + 1; j <= m_Size - 1; ++j) {
        double x = ( *D )[ j ][ k ] / ( *D )[ k ][ k ];
        for( size_t i = k; i <= m_Size - 1; ++i ) {  
          ( *D )[ j ][ i ] = ( *D )[ j ][ i ] - x * ( *D )[ k ][ i ];
        }
        ( *D )[ j ][ k ] = x;
      }
    }

    return D;
  }

}