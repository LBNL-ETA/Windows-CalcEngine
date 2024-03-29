#pragma once

#include <memory>
#include <WCEGases.hpp>

#include "EffectiveOpenness.hpp"
#include "LayerInterfaces.hpp"
#include "TarcogConstants.hpp"
#include "BaseShade.hpp"


namespace Tarcog::ISO15099
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

        static std::shared_ptr<CIGUSolidLayer>
          updateMaterialData(const std::shared_ptr<CIGUSolidLayer> & layer,
                             double density = MaterialConstants::GLASSDENSITY,
                             double youngsModulus = DeflectionConstants::YOUNGSMODULUS);

        static std::shared_ptr<CIGUSolidLayer>
          shading(double thickness,
                  double conductivity,
                  EffectiveLayers::EffectiveOpenness effectiveOpenness =
                    EffectiveLayers::EffectiveOpenness(0, 0, 0, 0, 0, 0),
                  double frontEmissivity = 0.84,
                  double frontIRTransmittance = 0.0,
                  double backEmissivity = 0.84,
                  double backIRTransmittance = 0.0);

        static std::shared_ptr<CIGUShadeLayer> sealedLayer(double thickness,
                                                           double conductivity,
                                                           double frontEmissivity = 0.84,
                                                           double frontIRTransmittance = 0.0,
                                                           double backEmissivity = 0.84,
                                                           double backIRTransmittance = 0.0);

        static std::shared_ptr<CIGUGapLayer> gap(double thickness, double pressure = 101325);
        static std::shared_ptr<CIGUGapLayer>
          gap(double thickness, const Gases::CGas & gas, double pressure = 101325);

        static std::shared_ptr<CIGUVentilatedGapLayer>
          forcedVentilationGap(const std::shared_ptr<CIGUGapLayer> & gap,
                               double forcedVentilationAirSpeed,
                               double forcedVentilationAirTemperature);

        static std::shared_ptr<CIGUGapLayer>
          addCircularPillar(const std::shared_ptr<CIGUGapLayer> & gap,
                            double conductivity,
                            double spacing,
                            double radius);
    };

}   // namespace Tarcog::ISO15099
