#ifndef WINDOWS_CALCENGINE_ISCATTERINGLAYER_HPP
#define WINDOWS_CALCENGINE_ISCATTERINGLAYER_HPP

namespace FenestrationCommon {

	enum class PropertySimple;
	enum class Side;
	enum class Scattering;

}

namespace SingleLayerOptics {

	class IScatteringLayer {
	public:

		virtual double getPropertySimple(
				const FenestrationCommon::PropertySimple t_Property,
				const FenestrationCommon::Side t_Side,
				const FenestrationCommon::Scattering t_Scattering,
				const double t_Theta = 0,
				const double t_Phi = 0 ) = 0;

		virtual std::vector< double > getWavelengths() const = 0;

		virtual double getMinLambda() const = 0;
		virtual double getMaxLambda() const = 0;

	};

}


#endif //WINDOWS_CALCENGINE_ISCATTERINGLAYER_HPP
