//
// Created by Simon Vidanovic on 3/1/18.
//

#include "ColorProperties.hpp"
#include "IScatteringLayer.hpp"

namespace SingleLayerOptics {

	ColorProperties::ColorProperties(
			const std::shared_ptr< IScatteringLayer > & layerX,
			const std::shared_ptr< IScatteringLayer > & layerY,
			const std::shared_ptr< IScatteringLayer > & layerZ,
			const FenestrationCommon::CSeries & t_Source, const FenestrationCommon::CSeries & t_DetectorX,
			const FenestrationCommon::CSeries & t_DetectorY,
			const FenestrationCommon::CSeries & t_DetectorZ,
			const std::shared_ptr< std::vector< double > > & t_wavelengths ) :
			m_LayerX( layerX ), m_LayerY( layerY ), m_LayerZ( layerZ ) {
		auto wavelengths = m_LayerX->getWavelengths();
		if( t_wavelengths != nullptr ) {
			wavelengths = *t_wavelengths;
		}
		auto aSolar = t_Source;
		auto DX = t_DetectorX;
		auto DY = t_DetectorY;
		auto DZ = t_DetectorZ;
		aSolar = *aSolar.interpolate( wavelengths );
		DX = *DX.interpolate( wavelengths );
		DY = *DY.interpolate( wavelengths );
		DZ = *DZ.interpolate( wavelengths );
		aSolar.mMult( DX );
		m_SDx = aSolar.mMult( DX )->sum( m_LayerX->getMinLambda(), m_LayerX->getMaxLambda() );
		m_SDy = aSolar.mMult( DY )->sum( m_LayerX->getMinLambda(), m_LayerX->getMaxLambda() );
		m_SDz = aSolar.mMult( DZ )->sum( m_LayerX->getMinLambda(), m_LayerX->getMaxLambda() );
	}

	double ColorProperties::getTrichromaticX( const FenestrationCommon::PropertySimple t_Property,
	                                          const FenestrationCommon::Side t_Side,
	                                          const FenestrationCommon::Scattering t_Scattering,
	                                          double const t_Theta, double const t_Phi ) {
		return m_SDx / m_SDy * 100 *
		       m_LayerX->getPropertySimple( t_Property, t_Side, t_Scattering, t_Theta, t_Phi );
	}

	double ColorProperties::getTrichromaticY( const FenestrationCommon::PropertySimple t_Property,
	                                          const FenestrationCommon::Side t_Side,
	                                          const FenestrationCommon::Scattering t_Scattering,
	                                          double const t_Theta, double const t_Phi ) {
		/// It is m_SDy / m_SDy so no point doing it
		return 100 * m_LayerY->getPropertySimple( t_Property, t_Side, t_Scattering, t_Theta, t_Phi );;
	}

	double ColorProperties::getTrichromaticZ( const FenestrationCommon::PropertySimple t_Property,
	                                          const FenestrationCommon::Side t_Side,
	                                          const FenestrationCommon::Scattering t_Scattering,
	                                          double const t_Theta, double const t_Phi ) {
		return m_SDz / m_SDy * 100 *
		       m_LayerZ->getPropertySimple( t_Property, t_Side, t_Scattering, t_Theta, t_Phi );;
	}
}