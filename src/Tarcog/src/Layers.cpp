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


namespace Tarcog::ISO15099
{
    std::shared_ptr<CIGUSolidLayer> Layers::solid(const double thickness,
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

    std::shared_ptr<CIGUGapLayer> Layers::gap(const double thickness, const double pressure)
    {
        return std::make_shared<CIGUGapLayer>(thickness, pressure);
    }

    std::shared_ptr<CIGUGapLayer>
      Layers::gap(double thickness, const Gases::CGas & gas, double pressure)
    {
        return std::make_shared<CIGUGapLayer>(thickness, pressure, gas);
    }

    std::shared_ptr<CIGUVentilatedGapLayer>
      Layers::forcedVentilationGap(const std::shared_ptr<CIGUGapLayer> & gap,
                                   double forcedVentilationAirSpeed,
                                   double forcedVentilationAirTemperature)
    {
        return std::make_shared<CIGUVentilatedGapLayer>(
          gap, forcedVentilationAirTemperature, forcedVentilationAirSpeed);
    }

    std::shared_ptr<CIGUSolidLayer> Layers::updateMaterialData(
      const std::shared_ptr<CIGUSolidLayer> & layer, double density, double youngsModulus)
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

    std::shared_ptr<CIGUSolidLayer>
      Layers::shading(double thickness,
                      double conductivity,
                      const EffectiveLayers::EffectiveOpenness & effectiveOpenness,
                      double frontEmissivity,
                      double frontIRTransmittance,
                      double backEmissivity,
                      double backIRTransmittance)
    {
        if(effectiveOpenness.isClosed())
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

    std::shared_ptr<CIGUShadeLayer> Layers::sealedLayer(double thickness,
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

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const CylindricalPillar & pillar,
                                                       double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<CylindricalPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const SphericalPillar & pillar,
                                                       double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<SphericalPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const RectangularPillar & pillar,
                                                       double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<RectangularPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const TriangularPillar & pillar,
                                                       double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<TriangularPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const PentagonPillar &pillar, double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<PentagonPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const HexagonPillar &pillar, double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<HexagonPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const LinearBearingPillar & pillar,
                                                       double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<LinearBearingPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const TruncatedConePillar & pillar,
                                                       double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<TruncatedConePillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const AnnulusCylinderPillar & pillar,
                                                       double pressure)
    {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<AnnulusCylinderPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const CShapedCylinderPillar &pillar, double pressure) {
        auto pillarGap{Tarcog::ISO15099::Layers::gap(pillar.height, pressure)};
        return std::make_shared<CShapedCylinderPillarLayer>(*pillarGap, pillar);
    }

    std::shared_ptr<CIGUGapLayer> Layers::createPillar(const PillarMeasurement & pillar)
    {
        return std::make_shared<MeasuredPillarLayer>(pillar);
    }
}   // namespace Tarcog::ISO15099
