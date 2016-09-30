#ifndef SQUAREMATRIXSERIES_H
#define SQUAREMATRIXSERIES_H

#include <vector>
#include <memory>

namespace FenestrationCommon {

  class CSeries;
  class CSquareMatrix;
  enum class IntegrationType;
  
  class CMatrixSeries {
  public:
    CMatrixSeries( const size_t t_Size1, const size_t t_Size2 );

    // add property at specific series position
    void addProperty( const size_t i, const size_t j, const double t_Wavelength, const double t_Value );

    // Multiply all series in matrix with provided one
    void mMult( const CSeries& t_Series );

    void integrate( const IntegrationType t_Integration );

    std::shared_ptr< std::vector< std::shared_ptr< std::vector< double > > > > 
      getSums( const double minLambda, const double maxLambda, const double t_ScaleValue );

    std::shared_ptr< CSquareMatrix > getSquaredMatrixSums( const double minLambda, const double maxLambda, const double t_ScaleValue );

  private:
    std::vector< std::vector< std::shared_ptr< CSeries > > > m_Matrix;
  };
  
}

#endif