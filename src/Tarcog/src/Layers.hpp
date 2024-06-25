#pragma once

#include <memory>
#include <WCEGases.hpp>

#include "EffectiveOpenness.hpp"
#include "LayerInterfaces.hpp"
#include "TarcogConstants.hpp"
#include "BaseShade.hpp"
#include "SupportPillar.hpp"
#include "SupportPillarMeasured.hpp"


namespace Tarcog::ISO15099
{
    class CIGUSolidLayer;

    class CIGUGapLayer;

    namespace Layers
    {
        std::shared_ptr<CIGUSolidLayer> solid(double thickness,
                                              double conductivity,
                                              double frontEmissivity = 0.84,
                                              double frontIRTransmittance = 0.0,
                                              double backEmissivity = 0.84,
                                              double backIRTransmittance = 0.0);

        std::shared_ptr<CIGUSolidLayer>
          updateMaterialData(const std::shared_ptr<CIGUSolidLayer> & layer,
                             double density = MaterialConstants::GLASSDENSITY,
                             double youngsModulus = DeflectionConstants::YOUNGSMODULUS);

        std::shared_ptr<CIGUSolidLayer>
          shading(double thickness,
                  double conductivity,
                  const EffectiveLayers::EffectiveOpenness & effectiveOpenness =
                    EffectiveLayers::EffectiveOpenness(0, 0, 0, 0, 0, 0),
                  double frontEmissivity = 0.84,
                  double frontIRTransmittance = 0.0,
                  double backEmissivity = 0.84,
                  double backIRTransmittance = 0.0);

        std::shared_ptr<CIGUShadeLayer> sealedLayer(double thickness,
                                                    double conductivity,
                                                    double frontEmissivity = 0.84,
                                                    double frontIRTransmittance = 0.0,
                                                    double backEmissivity = 0.84,
                                                    double backIRTransmittance = 0.0);

        std::shared_ptr<CIGUGapLayer> gap(double thickness, double pressure = 101325);
        std::shared_ptr<CIGUGapLayer>
          gap(double thickness, const Gases::CGas & gas, double pressure = 101325);

        std::shared_ptr<CIGUVentilatedGapLayer>
          forcedVentilationGap(const std::shared_ptr<CIGUGapLayer> & gap,
                               double forcedVentilationAirSpeed,
                               double forcedVentilationAirTemperature);

        std::shared_ptr<CIGUGapLayer> createPillar(const CylindricalPillar & pillar,
                                                   double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const SphericalPillar & pillar, double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const RectangularPillar & pillar,
                                                   double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const TriangularPillar & pillar,
                                                   double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const PentagonPillar & pillar, double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const HexagonPillar & pillar, double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const LinearBearingPillar & pillar,
                                                   double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const TruncatedConePillar & pillar,
                                                   double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const AnnulusCylinderPillar & pillar,
                                                   double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const CShapedCylinderPillar & pillar,
                                                   double pressure);

        std::shared_ptr<CIGUGapLayer> createPillar(const PillarMeasurement & pillar);
    };   // namespace Layers

}   // namespace Tarcog::ISO15099
