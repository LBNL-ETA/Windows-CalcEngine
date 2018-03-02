#ifndef WINDOWS_CALCENGINE_COLORPROPERTIES_HPP
#define WINDOWS_CALCENGINE_COLORPROPERTIES_HPP

#include <memory>
#include <vector>

#include "WCECommon.hpp"

namespace SingleLayerOptics {

	class IScatteringLayer;

	class ColorProperties {

	public:
		/// For BSDF layers make additional constructor
		ColorProperties(
				const std::shared_ptr< IScatteringLayer > & layerX,
				const std::shared_ptr< IScatteringLayer > & layerY,
				const std::shared_ptr< IScatteringLayer > & layerZ,
				const FenestrationCommon::CSeries & t_Source,
				const FenestrationCommon::CSeries & t_DetectorX,
				const FenestrationCommon::CSeries & t_DetectorY,
				const FenestrationCommon::CSeries & t_DetectorZ,
				const std::shared_ptr< std::vector< double > > & t_wavelengths = nullptr
		);

		double getTrichromaticX(
				const FenestrationCommon::PropertySimple t_Property,
				const FenestrationCommon::Side t_Side,
				const FenestrationCommon::Scattering t_Scattering,
				const double t_Theta = 0,
				const double t_Phi = 0 );

		double getTrichromaticY(
				const FenestrationCommon::PropertySimple t_Property,
				const FenestrationCommon::Side t_Side,
				const FenestrationCommon::Scattering t_Scattering,
				const double t_Theta = 0,
				const double t_Phi = 0 );

		double getTrichromaticZ(
				const FenestrationCommon::PropertySimple t_Property,
				const FenestrationCommon::Side t_Side,
				const FenestrationCommon::Scattering t_Scattering,
				const double t_Theta = 0,
				const double t_Phi = 0 );

	private:
		std::shared_ptr< IScatteringLayer > m_LayerX;
		std::shared_ptr< IScatteringLayer > m_LayerY;
		std::shared_ptr< IScatteringLayer > m_LayerZ;
		double m_SDx;
		double m_SDy;
		double m_SDz;

	};

}


#endif //WINDOWS_CALCENGINE_COLORPROPERTIES_HPP
