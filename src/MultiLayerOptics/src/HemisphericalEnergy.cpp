#include <algorithm>

#include "HemisphericalEnergy.hpp"
#include "FenestrationCommon.hpp"
#include "BSDFLayer.hpp"
#include "EquivalentBSDFLayer.hpp"
#include "SquareMatrix.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace SingleLayerOptics;

namespace MultiLayerOptics {

  //////////////////////////////////////////////////////////////////////////////////////////
  //////  CHemisphericalEnergy
  //////////////////////////////////////////////////////////////////////////////////////////
  CHemisphericalEnergy::CHemisphericalEnergy( const shared_ptr< vector< double > >& t_CommonWavelengths,
    const shared_ptr< CSeries >& t_SolarRadiation,
    const shared_ptr< vector< double > >& t_SolarMatrix,
    const shared_ptr< CSeries >& t_SkyRadiation,
    const shared_ptr< vector< double > >& t_SkyMatrix,
    const shared_ptr< CBSDFLayer >& t_Layer ) {
    m_Energy[ Energy::Solar ] = CSingleEnergy( t_CommonWavelengths, t_SolarRadiation, t_SolarMatrix, t_Layer );
    m_Energy[ Energy::Sky ] = CSingleEnergy( t_CommonWavelengths, t_SkyRadiation, t_SkyMatrix, t_Layer );
  }

  void CHemisphericalEnergy::addLayer( const shared_ptr< CBSDFLayer >& t_Layer ) {
    m_Energy.at( Energy::Solar ).addLayer( t_Layer );
    m_Energy.at( Energy::Sky ).addLayer( t_Layer );
  }

  shared_ptr< vector< double > > CHemisphericalEnergy::get( const PropertySimple t_Property,
    const Energy t_Energy ) {
    return m_Energy.at( t_Energy ).getVector( t_Property );
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  //////  CSingleEnergy
  //////////////////////////////////////////////////////////////////////////////////////////
  CHemisphericalEnergy::CSingleEnergy::CSingleEnergy() {
    // Map creation in CHemisphericalEnergy will complain about not having
    // default constructor.
  }

  CHemisphericalEnergy::CSingleEnergy::CSingleEnergy( 
    const shared_ptr< vector< double > >& t_CommonWavelengths, 
    const shared_ptr< CSeries >& t_Radiation, 
    const shared_ptr< vector< double > >& t_Distribution, 
    const shared_ptr< CBSDFLayer >& t_Layer ) : 
    m_Layer( make_shared< CEquivalentBSDFLayer >( t_CommonWavelengths, t_Radiation, t_Layer ) ),
    m_Distribution( t_Distribution ) {

  }

  void CHemisphericalEnergy::CSingleEnergy::addLayer( const shared_ptr< CBSDFLayer >& t_Layer ) {
    m_Layer->addLayer( t_Layer );
  }

  shared_ptr< vector< double > > CHemisphericalEnergy::CSingleEnergy::getVector(
    const PropertySimple t_Property ) {
    shared_ptr< vector< double > > mult = make_shared< vector< double > >( m_Distribution->size() );
    shared_ptr< vector< double > > tauHem = m_Layer->DirHem( 0.3, 2.5, Side::Front, t_Property );
    transform( tauHem->begin(), tauHem->end(), m_Distribution->begin(), mult->begin(), 
      multiplies< double >() );
    return mult;
  }

}
