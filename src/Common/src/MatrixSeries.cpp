#include <assert.h>

#include "MatrixSeries.hpp"
#include "SquareMatrix.hpp"
#include "Series.hpp"

using namespace std;

namespace FenestrationCommon {

	CMatrixSeries::CMatrixSeries( const size_t t_Size1, const size_t t_Size2 ) :
		m_Size1( t_Size1 ), m_Size2( t_Size2 ) {
		m_Matrix = vector< vector< shared_ptr< CSeries > > >( m_Size1 );
		for ( size_t i = 0; i < m_Size1; ++i ) {
			m_Matrix[ i ].resize( m_Size2 );
			for ( size_t j = 0; j < m_Size2; ++j ) {
				m_Matrix[ i ][ j ] = make_shared< CSeries >();
			}
		}
	}

	void CMatrixSeries::addProperty( const size_t i, const size_t j,
	                                 const double t_Wavelength, const double t_Value ) {
		m_Matrix[ i ][ j ]->addProperty( t_Wavelength, t_Value );
	}

	void CMatrixSeries::addProperties( const size_t i, const double t_Wavelength,
	                                   const vector< double >& t_Values ) {
		for ( size_t j = 0; j < t_Values.size(); ++j ) {
			m_Matrix[ i ][ j ]->addProperty( t_Wavelength, t_Values[ j ] );
		}
	}

	void CMatrixSeries::addProperties( const double t_Wavelength, CSquareMatrix& t_Matrix ) {
		for ( size_t i = 0; i < m_Matrix.size(); ++i ) {
			assert( m_Matrix.size() == t_Matrix.getSize() );
			for ( size_t j = 0; j < m_Matrix[ i ].size(); ++j ) {
				m_Matrix[ i ][ j ]->addProperty( t_Wavelength, t_Matrix[ i ][ j ] );
			}
		}
	}

	void CMatrixSeries::mMult( const CSeries& t_Series ) {
		for ( size_t i = 0; i < m_Matrix.size(); ++i ) {
			for ( size_t j = 0; j < m_Matrix[ i ].size(); ++j ) {
				assert( t_Series.size() == ( *m_Matrix[ i ][ j ] ).size() );
				m_Matrix[ i ][ j ] = m_Matrix[ i ][ j ]->mMult( t_Series );
			}
		}
	}

	void CMatrixSeries::mMult( const vector< shared_ptr< CSeries > >& t_Series ) {
		for ( size_t i = 0; i < m_Matrix.size(); ++i ) {
			for ( size_t j = 0; j < m_Matrix[ i ].size(); ++j ) {
				//assert( t_Series[ i ]->size() == ( *m_Matrix[ i ][ j ] ).size() );
				m_Matrix[ i ][ j ] = m_Matrix[ i ][ j ]->mMult( *t_Series[ i ] );
			}
		}
	}

	vector< shared_ptr< CSeries > >& CMatrixSeries::operator[]( const size_t index ) {
		return m_Matrix[ index ];
	}

	void CMatrixSeries::integrate( const IntegrationType t_Integration ) {
		for ( size_t i = 0; i < m_Matrix.size(); ++i ) {
			for ( size_t j = 0; j < m_Matrix[ i ].size(); ++j ) {
				m_Matrix[ i ][ j ] = m_Matrix[ i ][ j ]->integrate( t_Integration );
			}
		}
	}

	shared_ptr< vector< shared_ptr< vector< double > > > > CMatrixSeries::getSums(
		const double minLambda, const double maxLambda, const vector< double >& t_ScaleValue ) {
		shared_ptr< vector< shared_ptr< vector< double > > > > Result =
			make_shared< vector< shared_ptr< vector< double > > > >( m_Matrix.size() );
		for ( size_t i = 0; i < m_Matrix.size(); ++i ) {
			if ( m_Matrix[ i ].size() != t_ScaleValue.size() ) {
				throw runtime_error( "Size of vector for scaling must be same as size of the matrix." );
			}
			( *Result )[ i ] = make_shared< vector< double > >();
			for ( size_t j = 0; j < m_Matrix[ i ].size(); ++j ) {
				double value = m_Matrix[ i ][ j ]->sum( minLambda, maxLambda ) / t_ScaleValue[ i ];
				( *Result )[ i ]->push_back( value );
			}
		}
		return Result;
	}

	shared_ptr< CSquareMatrix > CMatrixSeries::getSquaredMatrixSums(
		const double minLambda, const double maxLambda, const vector< double >& t_ScaleValue ) {
		assert( m_Matrix.size() == m_Matrix[ 0 ].size() );
		shared_ptr< CSquareMatrix > Res = make_shared< CSquareMatrix >( m_Matrix.size() );
		for ( size_t i = 0; i < m_Matrix.size(); ++i ) {
			for ( size_t j = 0; j < m_Matrix[ i ].size(); ++j ) {
				double value = m_Matrix[ i ][ j ]->sum( minLambda, maxLambda ) / t_ScaleValue[ i ];
				( *Res )[ i ][ j ] = value;
			}
		}
		return Res;
	}

	size_t CMatrixSeries::size1() const {
		return m_Size1;
	}

	size_t CMatrixSeries::size2() const {
		return m_Size2;
	}

}
