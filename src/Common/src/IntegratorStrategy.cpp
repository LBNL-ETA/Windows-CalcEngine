#include <cassert>
#include "IntegratorStrategy.hpp"



namespace FenestrationCommon {

	double IIntegratorStrategy::dX( double const x1, double const x2 ) const {
		return x2 - x1;
	}

	double CIntegratorRectangular::integrate( double const x1, double const x2, double const y1, double const ) {
		const auto deltaX = dX( x1, x2 );
		return y1 * deltaX;
	}

	double CIntegratorRectangularCentroid::integrate( double const x1, double const x2, double const y1,
		double const y2 ) {
		const auto diffX = ( x2 - x1 ) / 2;
		const auto deltaX = dX( x1 - diffX, x2 - diffX );
		return y1 * deltaX;
	}

	double CIntegratorTrapezoidal::integrate( double const x1, double const x2, double const y1, double const y2 ) {
		const auto deltaX = dX( x1, x2 );
		const auto yCenter = ( y1 + y2 ) / 2;
		return yCenter * deltaX;
	}

	std::shared_ptr< IIntegratorStrategy > CIntegratorFactory::getIntegrator( IntegrationType t_IntegratorType ) {
		std::shared_ptr< IIntegratorStrategy > aStrategy = nullptr;
		switch ( t_IntegratorType ) {
		case IntegrationType::Rectangular:
			aStrategy = std::make_shared< CIntegratorRectangular >();
			break;
		case IntegrationType::Trapezoidal:
			aStrategy = std::make_shared< CIntegratorTrapezoidal >();
			break;
		default:
			assert("Irregular call of integration strategy.");
			break;
		}
		return aStrategy;
	}

}
