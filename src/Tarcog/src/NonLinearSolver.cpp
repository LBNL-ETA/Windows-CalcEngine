#include <assert.h>
#include <math.h>
#include <algorithm>

#include "NonLinearSolver.hpp"
#include "WCECommon.hpp"
#include "TarcogConstants.hpp"
#include "HeatFlowBalance.hpp"
#include "IGU.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  CNonLinearSolver::CNonLinearSolver( const shared_ptr< CIGU >& t_IGU ) : 
    m_IGU( t_IGU ), m_Tolerance( IterationConstants::CONVERGENCE_TOLERANCE ), m_Iterations( 0 ),
    m_RelaxParam( IterationConstants::RELAXATION_PARAMETER_MAX ){
    assert( t_IGU != nullptr );
    m_LinearSolver = make_shared< CLinearSolver >();
    assert( m_LinearSolver != nullptr );
    m_QBalance = make_shared< CHeatFlowBalance >( m_IGU );
    assert( m_QBalance != nullptr );
  }

  double CNonLinearSolver::calculateTolerance( const vector< double >& t_Solution ) {
    assert(t_Solution.size() == m_IGUState->size() );
    double aError = fabs( t_Solution[ 0 ] - ( *m_IGUState )[ 0 ] );
    for( size_t i = 1; i < m_IGUState->size(); ++i ) {
      aError = max( aError, fabs( t_Solution[ i ] - ( *m_IGUState )[ i ] ));
    }
    return aError;
  }

  void CNonLinearSolver::estimateNewState( const vector< double >& t_Solution ) {
    assert(t_Solution.size() == m_IGUState->size() );
    for( size_t i = 0; i < m_IGUState->size(); ++i ) {
      ( *m_IGUState )[ i ] = m_RelaxParam * t_Solution[ i ] + ( 1 - m_RelaxParam ) * ( *m_IGUState )[ i ];
    }
  }

  void CNonLinearSolver::setTolerance( const double t_Tolerance ) {
    m_Tolerance = t_Tolerance;
  }

  size_t CNonLinearSolver::getNumOfIterations() const {
    return m_Iterations;
  }

  void CNonLinearSolver::solve() {
    m_IGUState = m_IGU->getState();
    assert( m_IGUState != nullptr );
    shared_ptr< vector< double > > aSolution = nullptr;
    double achievedTolerance = 1;

    m_Iterations = 0;

    while ( achievedTolerance > m_Tolerance ) {
      ++m_Iterations;
      aSolution = m_QBalance->calcBalanceMatrix();
      assert( aSolution != nullptr );

      achievedTolerance = calculateTolerance( *aSolution );

      estimateNewState( *aSolution );

      m_IGU->setState( m_IGUState );
      
      if( m_Iterations > IterationConstants::NUMBER_OF_STEPS && 
        m_RelaxParam == IterationConstants::RELAXATION_PARAMETER_MIN ) {
        throw runtime_error("Failed to converge in tarcog.");
      }

      if( m_Iterations > IterationConstants::NUMBER_OF_STEPS ) {
        m_Iterations = 0;
        m_RelaxParam -= IterationConstants::RELAXATION_PARAMETER_STEP;
      }

    }
  }

}