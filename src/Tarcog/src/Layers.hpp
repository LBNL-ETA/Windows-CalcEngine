#pragma once

#include <memory>
#include <WCEGases.hpp>

#include "EffectiveOpenness.hpp"
#include "LayerInterfaces.hpp"
#include "TarcogConstants.hpp"
#include "BaseShade.hpp"
#include "SupportPillar.hpp"


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

        //! All vacuum gaps will have Air by default. If needed, different gases can be added.
        static std::shared_ptr<CIGUGapLayer> cylindricalPillar(double radius,
                                                               double height,
                                                               double materialConductivity,
                                                               double spacingArea,
                                                               double pressure);

        static std::shared_ptr<CIGUGapLayer> sphericalPillar(double radiusOfContact,
                                                             double height,
                                                             double materialConductivity,
                                                             double spacingArea,
                                                             double pressure);

        static std::shared_ptr<CIGUGapLayer> rectangularPillar(double length,
                                                               double width,
                                                               double height,
                                                               double materialConductivity,
                                                               double spacingArea,
                                                               double pressure);

        static std::shared_ptr<CIGUGapLayer> triangularPillar(double length,
                                                              double height,
                                                              double materialConductivity,
                                                              double spacingArea,
                                                              double pressure);

        static std::shared_ptr<CIGUGapLayer> pentagonPillar(double length,
                                                            double height,
                                                            double materialConductivity,
                                                            double spacingArea,
                                                            double pressure);

        static std::shared_ptr<CIGUGapLayer> hexagonPillar(double length,
                                                           double height,
                                                           double materialConductivity,
                                                           double spacingArea,
                                                           double pressure);
    };

}   // namespace Tarcog::ISO15099
