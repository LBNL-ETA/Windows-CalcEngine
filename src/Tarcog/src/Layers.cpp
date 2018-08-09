#include "Layers.hpp"
#include "IGUGapLayer.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUSolidDeflection.hpp"
#include "BaseShade.hpp"
#include "Surface.hpp"
#include "SupportPillar.hpp"

namespace Tarcog
{
    std::shared_ptr<CIGUSolidLayer> Layers::solid(const double thickness,
                                                  const double conductance,
                                                  const double frontEmissivity,
                                                  const double frontIRTransmittance,
                                                  const double backEmissivity,
                                                  const double backIRTransmittance)
    {
        return std::make_shared<CIGUSolidLayer>(
          thickness,
          conductance,
          std::make_shared<CSurface>(frontEmissivity, frontIRTransmittance),
          std::make_shared<CSurface>(backEmissivity, backIRTransmittance));
    }

    std::shared_ptr<CIGUGapLayer> Layers::gap(const double thickness, const double pressure)
    {
        return std::make_shared<CIGUGapLayer>(thickness, pressure);
    }

	std::shared_ptr< CIGUGapLayer >
	Layers::gap( double thickness, double pressure, const Gases::CGas & gas ) {
		return std::make_shared< CIGUGapLayer >(thickness, pressure, gas);
	}

    std::shared_ptr<CIGUSolidLayer> Layers::updateDeflectionCoefficients(
      const std::shared_ptr<CIGUSolidLayer> & layer, double youngsModulus, double poisonRatio)
    {
        return std::make_shared<CIGUSolidLayerDeflection>(*layer, youngsModulus, poisonRatio);
    }

    std::shared_ptr<CIGUSolidLayer> Layers::shadeWithOpenness(const double thickness,
                                                              const double conductance,
                                                              const double aTop,
                                                              const double aBot,
                                                              const double aLeft,
                                                              const double aRight,
                                                              const double aFront,
                                                              const double frontEmissivity,
                                                              const double frontTransmittance,
                                                              const double backEmissivity,
                                                              const double backTransmittance)
    {
        return std::make_shared<CIGUShadeLayer>(
          thickness,
          conductance,
          std::make_shared<CShadeOpenings>(aTop, aBot, aLeft, aRight, aFront),
          std::make_shared<CSurface>(frontEmissivity, frontTransmittance),
          std::make_shared<CSurface>(backEmissivity, backTransmittance));
    }

    std::shared_ptr<CIGUGapLayer> Layers::addCircularPillar(
      const std::shared_ptr<CIGUGapLayer> & gap, double conductivity, double spacing, double radius)
    {
        return std::make_shared<CCircularPillar>(*gap, conductivity, spacing, radius);
    }

}   // namespace Tarcog
