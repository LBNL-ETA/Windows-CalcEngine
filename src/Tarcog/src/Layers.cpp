#include "Layers.hpp"
#include "IGUGapLayer.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUSolidDeflection.hpp"
#include "BaseShade.hpp"
#include "Surface.hpp"
#include "SupportPillar.hpp"
#include "SupportPillarMeasured.hpp"
#include "EffectiveOpenness.hpp"
#include "IGUVentilatedGapLayer.hpp"

namespace Tarcog::ISO15099::Layers
{
    SolidLayer solid(const double thickness,
                     const double conductivity,
                     const double frontEmissivity,
                     const double frontIRTransmittance,
                     const double backEmissivity,
                     const double backIRTransmittance)
    {
        return std::make_shared<CIGUSolidLayer>(
          thickness,
          conductivity,
          std::make_shared<Surface>(frontEmissivity, frontIRTransmittance),
          std::make_shared<Surface>(backEmissivity, backIRTransmittance));
    }

    GapLayer gap(const double thickness, const double pressure)
    {
        return std::make_shared<CIGUGapLayer>(thickness, pressure);
    }

    GapLayer gap(double thickness, const Gases::CGas & gas, double pressure)
    {
        return std::make_shared<CIGUGapLayer>(thickness, pressure, gas);
    }

    std::shared_ptr<CIGUVentilatedGapLayer>
      forcedVentilationGap(const GapLayer & gap,
                           double forcedVentilationAirSpeed,
                           double forcedVentilationAirTemperature)
    {
        return std::make_shared<CIGUVentilatedGapLayer>(
          gap, forcedVentilationAirTemperature, forcedVentilationAirSpeed);
    }

    SolidLayer updateMaterialData(const SolidLayer & layer, double density, double youngsModulus)
    {
        // Deflection cannot be applied to shading layers
        if(std::dynamic_pointer_cast<CIGUShadeLayer>(layer) == nullptr)
        {
            static const double poissonRatio{0.22};
            return std::make_shared<CIGUSolidLayerDeflection>(
              *layer, youngsModulus, poissonRatio, density);
        }
        else
        {
            return layer;
        }
    }

    SolidLayer shading(double thickness,
                       double conductivity,
                       const EffectiveLayers::EffectiveOpenness & effectiveOpenness,
                       double frontEmissivity,
                       double frontIRTransmittance,
                       double backEmissivity,
                       double backIRTransmittance)
    {
        if(isClosed(effectiveOpenness))
        {
            return solid(thickness,
                         conductivity,
                         frontEmissivity,
                         frontIRTransmittance,
                         backEmissivity,
                         backIRTransmittance);
        }

        return std::make_shared<CIGUShadeLayer>(
          thickness,
          conductivity,
          CShadeOpenings(effectiveOpenness.Atop,
                         effectiveOpenness.Abot,
                         effectiveOpenness.Al,
                         effectiveOpenness.Ar,
                         effectiveOpenness.Ah,
                         effectiveOpenness.FrontPorosity),
          std::make_shared<Surface>(frontEmissivity, frontIRTransmittance),
          std::make_shared<Surface>(backEmissivity, backIRTransmittance));
    }

    ShadeLayer sealedLayer(double thickness,
                           double conductivity,
                           double frontEmissivity,
                           double frontIRTransmittance,
                           double backEmissivity,
                           double backIRTransmittance)
    {
        return std::make_shared<CIGUShadeLayer>(
          thickness,
          conductivity,
          CShadeOpenings(),
          std::make_shared<Surface>(frontEmissivity, frontIRTransmittance),
          std::make_shared<Surface>(backEmissivity, backIRTransmittance));
    }

    namespace Helper
    {
        Gases::CGas defaultVacuumMixture()
        {
            return Gases::CGas{std::vector<Gases::CGasItem>{{1.0, Gases::GasDef::VacuumMixture}}};
        }
    }   // namespace Helper

    template<typename PillarType, typename PillarLayerType>
    GapLayer createPillar(const PillarType & pillar, double pressure)
    {
        auto pillarGap =
          Tarcog::ISO15099::Layers::gap(pillar.height, Helper::defaultVacuumMixture(), pressure);
        return std::make_shared<PillarLayerType>(*pillarGap, pillar);
    }

    GapLayer createPillar(const CylindricalPillar & pillar, double pressure)
    {
        return createPillar<CylindricalPillar, CylindricalPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const SphericalPillar & pillar, double pressure)
    {
        return createPillar<SphericalPillar, SphericalPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const RectangularPillar & pillar, double pressure)
    {
        return createPillar<RectangularPillar, RectangularPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const TriangularPillar & pillar, double pressure)
    {
        return createPillar<TriangularPillar, TriangularPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const PentagonPillar & pillar, double pressure)
    {
        return createPillar<PentagonPillar, PentagonPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const HexagonPillar & pillar, double pressure)
    {
        return createPillar<HexagonPillar, HexagonPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const LinearBearingPillar & pillar, double pressure)
    {
        return createPillar<LinearBearingPillar, LinearBearingPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const TruncatedConePillar & pillar, double pressure)
    {
        return createPillar<TruncatedConePillar, TruncatedConePillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const AnnulusCylinderPillar & pillar,
                                               double pressure)
    {
        return createPillar<AnnulusCylinderPillar, AnnulusCylinderPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const CShapedCylinderPillar & pillar,
                                               double pressure)
    {
        return createPillar<CShapedCylinderPillar, CShapedCylinderPillarLayer>(pillar, pressure);
    }

    GapLayer createPillar(const PillarMeasurement & pillar)
    {
        return std::make_shared<MeasuredPillarLayer>(pillar);
    }
}   // namespace Tarcog::ISO15099::Layers
