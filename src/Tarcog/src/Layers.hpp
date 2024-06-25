#pragma once

#include <memory>
#include <WCEGases.hpp>

#include "EffectiveOpenness.hpp"
#include "LayerInterfaces.hpp"
#include "TarcogConstants.hpp"
#include "BaseShade.hpp"
#include "SupportPillar.hpp"
#include "SupportPillarMeasured.hpp"
#include "IGUSolidLayer.hpp"
#include "LayerTypes.hpp"

namespace Tarcog::ISO15099
{

    class CIGUGapLayer;

    namespace Layers
    {
        SolidLayer solid(double thickness,
                         double conductivity,
                         double frontEmissivity = 0.84,
                         double frontIRTransmittance = 0.0,
                         double backEmissivity = 0.84,
                         double backIRTransmittance = 0.0);

        SolidLayer updateMaterialData(const SolidLayer & layer,
                                      double density = MaterialConstants::GLASSDENSITY,
                                      double youngsModulus = DeflectionConstants::YOUNGSMODULUS);

        SolidLayer shading(double thickness,
                           double conductivity,
                           const EffectiveLayers::EffectiveOpenness & effectiveOpenness =
                             EffectiveLayers::EffectiveOpenness(0, 0, 0, 0, 0, 0),
                           double frontEmissivity = 0.84,
                           double frontIRTransmittance = 0.0,
                           double backEmissivity = 0.84,
                           double backIRTransmittance = 0.0);

        ShadeLayer sealedLayer(double thickness,
                               double conductivity,
                               double frontEmissivity = 0.84,
                               double frontIRTransmittance = 0.0,
                               double backEmissivity = 0.84,
                               double backIRTransmittance = 0.0);

        GapLayer gap(double thickness, double pressure = 101325);
        GapLayer gap(double thickness, const Gases::CGas & gas, double pressure = 101325);

        std::shared_ptr<CIGUVentilatedGapLayer>
          forcedVentilationGap(const GapLayer & gap,
                               double forcedVentilationAirSpeed,
                               double forcedVentilationAirTemperature);

        GapLayer createPillar(const CylindricalPillar & pillar, double pressure);

        GapLayer createPillar(const SphericalPillar & pillar, double pressure);

        GapLayer createPillar(const RectangularPillar & pillar, double pressure);

        GapLayer createPillar(const TriangularPillar & pillar, double pressure);

        GapLayer createPillar(const PentagonPillar & pillar, double pressure);

        GapLayer createPillar(const HexagonPillar & pillar, double pressure);

        GapLayer createPillar(const LinearBearingPillar & pillar, double pressure);

        GapLayer createPillar(const TruncatedConePillar & pillar, double pressure);

        GapLayer createPillar(const AnnulusCylinderPillar & pillar, double pressure);

        GapLayer createPillar(const CShapedCylinderPillar & pillar, double pressure);

        GapLayer createPillar(const PillarMeasurement & pillar);
    };   // namespace Layers

}   // namespace Tarcog::ISO15099
