#ifndef BSDFLAYER_H
#define BSDFLAYER_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

	class CSquareMatrix;
  enum class Side;

}

namespace LayerOptics {

  class CBSDFDirections;

  // Layer results from BSDF directions.
  class CBSDFResults {
  public:
    CBSDFResults( std::shared_ptr< const CBSDFDirections > t_Directions );

    // Transmittance matrices for front and back sides
    std::shared_ptr< FenestrationCommon::CSquareMatrix > Tau( FenestrationCommon::Side t_Side ) const;
    // Reflectance matrices for front and back sides
    std::shared_ptr< FenestrationCommon::CSquareMatrix > Rho( FenestrationCommon::Side t_Side ) const;

    void setResultMatrices( std::shared_ptr< FenestrationCommon::CSquareMatrix > t_Tau,
      std::shared_ptr< FenestrationCommon::CSquareMatrix > t_Rho, FenestrationCommon::Side t_Side );

    std::shared_ptr< std::vector< double > > TauHem( FenestrationCommon::Side t_Side );
    std::shared_ptr< std::vector< double > > RhoHem( FenestrationCommon::Side t_Side );

    std::shared_ptr< std::vector< double > > Abs( FenestrationCommon::Side t_Side );

    std::shared_ptr< const CBSDFDirections > getDirections() const;

    // Diffuse transmittance for front and back sides
    double TauDiff( FenestrationCommon::Side t_Side ) const;

    // Diffuse reflectance for front and back sides
    double RhoDiff( FenestrationCommon::Side t_Side ) const;

    // Lambda values for the layer.
    std::shared_ptr< const std::vector< double > > lambdaVector() const;
    std::shared_ptr< const FenestrationCommon::CSquareMatrix > lambdaMatrix() const;

  protected:
    std::shared_ptr< const CBSDFDirections > m_Directions;
    size_t m_DimMatrices;

  private:
    // Hemispherical integration over m_Directions
    double integrate( std::shared_ptr< FenestrationCommon::CSquareMatrix > t_Matrix ) const;

    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_TauF;
    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_TauB;
    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_RhoF;
    std::shared_ptr< FenestrationCommon::CSquareMatrix > m_RhoB;

    std::shared_ptr< std::vector< double > > m_VTauHemF;
    std::shared_ptr< std::vector< double > > m_VTauHemB;
    std::shared_ptr< std::vector< double > > m_VRhoHemF;
    std::shared_ptr< std::vector< double > > m_VRhoHemB;

    std::shared_ptr< std::vector< double > > m_AbsF;
    std::shared_ptr< std::vector< double > > m_AbsB;

    void calcHemispherical();
    bool m_HemisphericalCalculated;
  };

}

#endif