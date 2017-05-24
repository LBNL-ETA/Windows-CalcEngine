#include <stdexcept>
#include <assert.h>

#include "LinearSolver.hpp"
#include "SquareMatrix.hpp"

#include <math.h>

using namespace std;

namespace FenestrationCommon {

  CLinearSolver::CLinearSolver() : m_MatrixA( nullptr ), m_VectorB( nullptr ) {
  
  }

  void CLinearSolver::assignMatrixA( const shared_ptr< CSquareMatrix >& t_MatrixA ) {
    m_MatrixA = t_MatrixA;
  }

  void CLinearSolver::assignVectorB( const shared_ptr< vector< double > >& t_VectorB ) {
    m_VectorB = t_VectorB;
  }

  shared_ptr< vector< double > > CLinearSolver::checkSingularity() {
    size_t size = m_MatrixA->getSize();
    shared_ptr< vector< double > > vv = make_shared< vector < double > >();

    for( size_t i = 0 ; i < size; ++i ) {
      double aamax = 0;
      for( size_t j = 0 ; j < size; ++j ) {
        double absCellValue = fabs( ( *m_MatrixA )[ i ][ j ] );
        if ( absCellValue > aamax ) {
          aamax = absCellValue;
        }
      }
      if( aamax == 0 ) {
        assert( aamax != 0 );
      }
      vv->push_back( 1 / aamax );
    }

    return vv;
  }

  shared_ptr< vector< size_t > > CLinearSolver::makeUpperTriangular() {
    double TINY = 1e-20;
    shared_ptr< vector< size_t > > index = make_shared< vector< size_t > >();

    int size = int( m_MatrixA->getSize() );
    index->resize( size );
    shared_ptr< vector< double > > vv = nullptr;

    vv = checkSingularity();

    int d = 1;

    for( int j = 0; j < size; ++j ) {

      for( int i = 0; i <= j - 1; ++i ) {
        double sum = ( *m_MatrixA )[ i ][ j ];
        for( int k = 0; k <= i - 1; ++k ) {
          sum = sum - ( *m_MatrixA )[ i ][ k ] * ( *m_MatrixA )[ k ][ j ];
        }
        ( *m_MatrixA )[ i ][ j ] = sum;
      }

      double aamax = 0;
      int imax = 0;

      for( int i = j; i < size; ++i ) {
        double sum = ( *m_MatrixA )[ i ][ j ];
        for( int k = 0; k <= j - 1; ++k ) {
          sum = sum - ( *m_MatrixA )[ i ][ k ] * ( *m_MatrixA )[ k ][ j ];
        }
        ( *m_MatrixA )[ i ][ j ] = sum;
        double dum = (*vv)[i] * fabs( sum );
        if ( dum >= aamax ) {
          imax = i;
          aamax = dum;
        }
      }

      if ( j != imax ) {
        for ( int k = 0; k < size; ++k ) {
          double dum = ( *m_MatrixA )[ imax ][ k ];
          ( *m_MatrixA )[ imax ][ k ] = ( *m_MatrixA )[ j ][ k ];
          ( *m_MatrixA )[ j ][ k ] = dum;
        } // k
        d = -d;
        (*vv)[ imax ] = (*vv)[ j ];
      }
      ( *index )[ j ] = imax;
      if ( ( *m_MatrixA )[ j ][ j ] == 0.0 ) {
        ( *m_MatrixA )[ j ][ j ] = TINY;
      }
      if ( j != ( size - 1 ) ) {
        double dum = 1.0 / ( *m_MatrixA )[ j ][ j ];
        for ( int i = j + 1; i < size; ++i ) {
          ( *m_MatrixA )[ i ][ j ] = ( *m_MatrixA )[ i ][ j ] * dum;
        } // i
      }

    }

    return index;
  }

  shared_ptr< vector< double > > CLinearSolver::solveSystem(
    const shared_ptr< CSquareMatrix >& t_MatrixA, 
    const shared_ptr< vector< double > >& t_VectorB ) {

    if ( t_MatrixA->getSize() != t_VectorB->size() ) {
      runtime_error( "Matrix and vector for system of linear equations are not same size." );
    }

    m_MatrixA = t_MatrixA;
    m_VectorB = t_VectorB;

    shared_ptr< vector< size_t > > index = nullptr;
    index = makeUpperTriangular();

    int size = int( m_MatrixA->getSize() );

    int ii = -1;
    for ( int i = 0; i < size; ++i ) {
      size_t ll = ( *index )[ i ];
      double sum = ( *m_VectorB )[ ll ];
      ( *m_VectorB )[ ll ] = ( *m_VectorB )[ i ];
      if ( ii != -1 ) {
        for ( int j = ii; j <= i - 1; ++j ) {
          sum -= ( *m_MatrixA )[ i ][ j ] * ( *m_VectorB )[ j ];
        } // j
      } else if ( sum != 0.0 ) {
        ii = int( i );
      }
      ( *m_VectorB )[ i ] = sum;
    } // i

    for ( int i = ( size - 1 ); i >= 0; --i ) {
      double sum = ( *m_VectorB )[ i ];
      for ( int j = i + 1; j < size; ++j ) {
        sum -= ( *m_MatrixA )[ i ][ j ] * ( *m_VectorB )[ j ];
      } // j
      ( *m_VectorB )[ i ] = sum / ( *m_MatrixA )[ i ][ i ];
    } // i

    return m_VectorB;

  }

}