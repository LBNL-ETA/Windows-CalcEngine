#pragma once

#include <memory>
#include <WCEGases.hpp>

namespace Tarcog
{
    class CIGUSolidLayer;
    class CIGUGapLayer;

    class Layers
    {
    public:
        static std::shared_ptr<CIGUSolidLayer> solid(double thickness,
                                                     double conductivity,
                                                     double frontEmissivity = 0.84,
                                                     double frontIRTransmittance = 0.0,
                                                     double backEmissivity = 0.84,
                                                     double backIRTransmittance = 0.0);

        static std::shared_ptr<CIGUSolidLayer> makeDeflectable(
			const std::shared_ptr< CIGUSolidLayer > & layer, double youngsModulus,
			double poisonRatio );

        static std::shared_ptr<CIGUSolidLayer> shading(double thickness,
                                                       double conductivity,
                                                       double aTop,
                                                       double aBot,
                                                       double aLeft,
                                                       double aRight,
                                                       double aFront,
                                                       double frontEmissivity = 0.84,
                                                       double frontTransmittance = 0.0,
                                                       double backEmissivity = 0.84,
                                                       double backTransmittance = 0.0);

        static std::shared_ptr<CIGUGapLayer> gap(double thickness, double pressure);
        static std::shared_ptr<CIGUGapLayer>
          gap(double thickness, double pressure, const Gases::CGas & gas);

        static std::shared_ptr<CIGUGapLayer>
          addCircularPillar(const std::shared_ptr<CIGUGapLayer> & gap,
                            double conductivity,
                            double spacing,
                            double radius);
    };

}   // namespace Tarcog
