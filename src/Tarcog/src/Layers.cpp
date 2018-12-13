#include "Layers.hpp"
#include "IGUGapLayer.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUSolidDeflection.hpp"
#include "BaseShade.hpp"
#include "Surface.hpp"
#include "SupportPillar.hpp"

namespace Tarcog
{
	namespace ISO15099 {
		std::shared_ptr< CIGUSolidLayer > Layers::solid( const double thickness,
														 const double conductivity,
														 const double frontEmissivity,
														 const double frontIRTransmittance,
														 const double backEmissivity,
														 const double backIRTransmittance ) {
			return std::make_shared< CIGUSolidLayer >(
				thickness,
				conductivity,
				std::make_shared< CSurface >( frontEmissivity, frontIRTransmittance ),
				std::make_shared< CSurface >( backEmissivity, backIRTransmittance ) );
		}

		std::shared_ptr< CIGUGapLayer >
		Layers::gap( const double thickness, const double pressure ) {
			return std::make_shared< CIGUGapLayer >( thickness, pressure );
		}

		std::shared_ptr< CIGUGapLayer >
		Layers::gap( double thickness, const Gases::CGas & gas, double pressure ) {
			return std::make_shared< CIGUGapLayer >( thickness, pressure, gas );
		}

		std::shared_ptr< CIGUSolidLayer >
		Layers::makeDeflectable( const std::shared_ptr< CIGUSolidLayer > & layer,
								 const double youngsModulus,
								 const double poissonRatio ) {
			return std::make_shared< CIGUSolidLayerDeflection >( *layer, youngsModulus,
																 poissonRatio );
		}

		std::shared_ptr< CIGUSolidLayer > Layers::shading( double thickness,
														   double conductivity,
														   double aTop,
														   double aBot,
														   double aLeft,
														   double aRight,
														   double aFront,
														   double frontEmissivity,
														   double frontTransmittance,
														   double backEmissivity,
														   double backTransmittance ) {
			return std::make_shared< CIGUShadeLayer >(
				thickness,
				conductivity,
				std::make_shared< CShadeOpenings >( aTop, aBot, aLeft, aRight, aFront ),
				std::make_shared< CSurface >( frontEmissivity, frontTransmittance ),
				std::make_shared< CSurface >( backEmissivity, backTransmittance ) );
		}

		std::shared_ptr< CIGUGapLayer > Layers::addCircularPillar(
			const std::shared_ptr< CIGUGapLayer > & gap, double conductivity, double spacing,
			double radius ) {
			return std::make_shared< CCircularPillar >( *gap, conductivity, spacing, radius );
		}

	}

}   // namespace Tarcog
