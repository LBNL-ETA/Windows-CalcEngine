#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "SquareMatrix.hpp"
#include "LinearSolver.hpp"

using namespace std;
using namespace FenestrationCommon;

class TestLinearSolver1 : public testing::Test {

private:
  shared_ptr< CLinearSolver > m_Solver;

protected:
  virtual void SetUp() {
    m_Solver = std::make_shared< CLinearSolver >();
    ASSERT_TRUE( m_Solver != nullptr );
  }

public:
  std::shared_ptr< CLinearSolver > GetSolver() { return m_Solver; };

};

TEST_F( TestLinearSolver1, Test1 ) {
  try {
    SCOPED_TRACE( "Begin Test: Test Linear Solver (1) - Solving simple matrix." );

    std::shared_ptr< CLinearSolver > aSolver = nullptr;
    std::shared_ptr< std::vector< double > > aSolution = nullptr;

    int size = 3;
    shared_ptr< CSquareMatrix > aMatrix = make_shared< CSquareMatrix >( size );
    ASSERT_TRUE( aMatrix != nullptr );

    (*aMatrix)[0][0] = 2;
    (*aMatrix)[0][1] = 1;
    (*aMatrix)[0][2] = 3;

    (*aMatrix)[1][0] = 2;
    (*aMatrix)[1][1] = 6;
    (*aMatrix)[1][2] = 8;

    (*aMatrix)[2][0] = 6;
    (*aMatrix)[2][1] = 8;
    (*aMatrix)[2][2] = 18;

    shared_ptr< vector< double > > aVector = make_shared< vector< double > >();
    ASSERT_TRUE( aVector != nullptr );
    aVector->push_back( 1 );
    aVector->push_back( 3 );
    aVector->push_back( 5 );
    
    aSolver = GetSolver();
    ASSERT_TRUE( aSolver != nullptr );
    
    aSolution = aSolver->solveSystem( aMatrix, aVector );
    ASSERT_TRUE( aSolution != nullptr );

    EXPECT_NEAR( 3.0/10.0, (*aSolution)[0], 1e-6 );
    EXPECT_NEAR( 2.0/5.0, (*aSolution)[1], 1e-6 );
    EXPECT_NEAR( 0.0, (*aSolution)[2], 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}

TEST_F( TestLinearSolver1, Test2 ) {
  try {
    SCOPED_TRACE( "Begin Test: Test Linear Solver (2) - Solving simple matrix." );

    std::shared_ptr< CLinearSolver > aSolver = nullptr;
    std::shared_ptr< std::vector< double > > aSolution = nullptr;

    int size = 4;
    shared_ptr< CSquareMatrix > aMatrix = make_shared< CSquareMatrix >( size );
    ASSERT_TRUE( aMatrix != nullptr );

    // This set-up is from fortran tarcog example
    (*aMatrix)[0][0] = 32817.2867004354;
    (*aMatrix)[0][1] = 1;
    (*aMatrix)[0][3] = -32808.3972386696;

    (*aMatrix)[1][0] = 1.28054053432588;
    (*aMatrix)[1][1] = -1;

    (*aMatrix)[2][2] = -1;
    (*aMatrix)[2][3] = 1.26433319889839;

    (*aMatrix)[3][0] = 32808.3972386696;
    (*aMatrix)[3][2] = -1;
    (*aMatrix)[3][3] = -32810.4664383299;

    shared_ptr< std::vector< double > > aVector = make_shared< std::vector< double > >();
    ASSERT_TRUE( aVector != nullptr );
    aVector->push_back( 3163.24185319654 );
    aVector->push_back( -73.4793240441777 );
    aVector->push_back( -67.9134111109072 );
    aVector->push_back( -1070.27145340963 );
    
    aSolver = GetSolver();
    ASSERT_TRUE( aSolver != nullptr );
    
    aSolution = aSolver->solveSystem( aMatrix, aVector );
    ASSERT_TRUE( aSolution != nullptr );

    EXPECT_NEAR( 303.040745958961, (*aSolution)[0], 1e-6 );
    EXPECT_NEAR( 461.535282796979, (*aSolution)[1], 1e-6 );
    EXPECT_NEAR( 451.057584815066, (*aSolution)[2], 1e-6 );
    EXPECT_NEAR( 303.040507073603, (*aSolution)[3], 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}