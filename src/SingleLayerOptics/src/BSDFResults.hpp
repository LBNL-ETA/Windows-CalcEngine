#ifndef BSDFLAYER_H
#define BSDFLAYER_H

#include <memory>
#include <vector>
#include <map>

namespace FenestrationCommon {

  class CSquareMatrix;
  enum class Side;
  enum class PropertySimple;

}

namespace SingleLayerOptics {

  class CBSDFDirections;

  typedef std::pair< FenestrationCommon::Side, FenestrationCommon::PropertySimple > pair_Side_PropertySimple;
  typedef std::shared_ptr< FenestrationCommon::CSquareMatrix > p_SquareMatrix;

  // Layer results from BSDF directions.
  class CBSDFResults {
  public:
    explicit CBSDFResults( const std::shared_ptr< const CBSDFDirections >& t_Directions );

    // Result matrices
    std::shared_ptr< FenestrationCommon::CSquareMatrix > getMatrix( const FenestrationCommon::Side t_Side,
      const FenestrationCommon::PropertySimple t_Property ) const;

    void setResultMatrices( const std::shared_ptr< FenestrationCommon::CSquareMatrix >& t_Tau,
      const std::shared_ptr< FenestrationCommon::CSquareMatrix >& t_Rho, FenestrationCommon::Side t_Side );

    // Direct-direct components
    double DirDir( const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property, 
      const double t_Theta, const double t_Phi );

    // Directional hemispherical results for every direction in BSDF definition
    std::shared_ptr< std::vector< double > > DirHem( const FenestrationCommon::Side t_Side, 
      const FenestrationCommon::PropertySimple t_Property );
    std::shared_ptr< std::vector< double > > Abs( const FenestrationCommon::Side t_Side );

    // Directional hemispherical results for given Theta and Phi direction
    double DirHem( const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property, 
      const double t_Theta, const double t_Phi );
    double Abs( const FenestrationCommon::Side t_Side, const double t_Theta, const double t_Phi );

    std::shared_ptr< const CBSDFDirections > getDirections() const;

    double DiffDiff( const FenestrationCommon::Side t_Side, 
      const FenestrationCommon::PropertySimple t_Property ) const;

    // Lambda values for the layer.
    std::shared_ptr< const std::vector< double > > lambdaVector() const;
    std::shared_ptr< const FenestrationCommon::CSquareMatrix > lambdaMatrix() const;

  protected:
    std::shared_ptr< const CBSDFDirections > m_Directions;
    size_t m_DimMatrices;

  private:
    // Hemispherical integration over m_Directions
    double integrate( FenestrationCommon::CSquareMatrix& t_Matrix ) const;

    std::map< pair_Side_PropertySimple, p_SquareMatrix > m_Matrix;
    //std::map< FenestrationCommon::Side, std::shared_ptr< FenestrationCommon::CSquareMatrix > > m_Tau;
    //std::map< FenestrationCommon::Side, std::shared_ptr< FenestrationCommon::CSquareMatrix > > m_Rho;

    std::map< pair_Side_PropertySimple, std::shared_ptr< std::vector< double > > > m_Hem;
    // std::map< FenestrationCommon::Side, std::shared_ptr< std::vector< double > > > m_VTauHem;
    // std::map< FenestrationCommon::Side, std::shared_ptr< std::vector< double > > > m_VRhoHem;

    std::map< FenestrationCommon::Side, std::shared_ptr< std::vector< double > > > m_Abs;

    void calcHemispherical();
    bool m_HemisphericalCalculated;
  };

}

#endif