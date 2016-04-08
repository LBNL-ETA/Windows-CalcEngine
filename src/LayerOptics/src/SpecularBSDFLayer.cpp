#include <assert.h>

#include "SpecularBSDFLayer.hpp"
#include "SpecularCell.hpp"
#include "BSDFDirections.hpp"
#include "BSDFPatch.hpp"
#include "BSDFResults.hpp"
#include "FenestrationCommon.hpp"
#include "BeamDirection.hpp"
#include "SquareMatrix.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

  CSpecularBSDFLayer::CSpecularBSDFLayer( shared_ptr< CSpecularCell > t_Cell, 
    shared_ptr< const CBSDFHemisphere > t_Hemisphere ) : CBSDFLayer( t_Cell, t_Hemisphere ) {

  };


  shared_ptr< CSpecularCell > CSpecularBSDFLayer::cellAsSpecular() const {
    shared_ptr< CSpecularCell > aCell = dynamic_pointer_cast< CSpecularCell >( m_Cell );
    assert( aCell != nullptr );
    return aCell;
  };

  void CSpecularBSDFLayer::calcDiffuseDistribution( const Side , shared_ptr< const CBeamDirection > , const size_t ) {
    // No diffuse calculations are necessary for specular layer. To avoid this, further refactoring is needed 
  };

  void CSpecularBSDFLayer::calcDiffuseDistribution_wv( const Side , shared_ptr< const CBeamDirection > , const size_t ) {
    // No diffuse calculations are necessary for specular layer. To avoid this, further refactoring is needed 
  };

}