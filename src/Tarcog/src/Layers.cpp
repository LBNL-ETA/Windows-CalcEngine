#include "Layers.hpp"
#include "IGUGapLayer.hpp"
#include "IGUSolidLayer.hpp"
#include "Surface.hpp"
#include "SupportPillar.hpp"
#include "SupportPillarMeasured.hpp"
#include "EffectiveMultipliers.hpp"

namespace Tarcog::ISO15099
{
    // Forward declarations of free functions from SupportPillar.cpp
    double universalPillarThermalResistance(double glassConductivityHotSide,
                                            double glassConductivityColdSide,
                                            double pillarConductivity,
                                            double pillarHeight,
                                            double areaOfContact);

    double linearBearingPillarThermalResistance(double glassConductivityHotSide,
                                                double glassConductivityColdSide,
                                                double pillarConductivity,
                                                double pillarHeight,
                                                double length,
                                                double width);

    double annulusCylinderPillarThermalResistance(double pillarConductivity,
                                                  double height,
                                                  double areaOfContact,
                                                  double rcGlass1,
                                                  double rcGlass2);

    double cShapedCylinderPillarThermalResistance(double coverageFraction,
                                                  double pillarConductivity,
                                                  double height,
                                                  double areaOfContact,
                                                  double rcGlass1,
                                                  double rcGlass2);

    namespace Helper
    {
        double rCS(double kGlass, double innerRadius, double outerRadius);
        double adjustInnerRadius(double innerRadius);
        double adjustOuterRadius(double innerRadius, double outerRadius);
    }   // namespace Helper
}   // namespace Tarcog::ISO15099


namespace Tarcog::ISO15099::Layers
{
    SolidLayer solid(double thickness, double conductivity)
    {
        return solid(thickness, conductivity, 0.84, 0.0, 0.84, 0.0);
    }

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

    GapLayer gap(double thickness,
                 double pressure,
                 const Gases::CGas & gas,
                 double accommodation1,
                 double accommodation2,
                 bool t_isDCenterMeasured)
    {
        return std::make_shared<CIGUGapLayer>(
          thickness, pressure, gas, accommodation1, accommodation2, t_isDCenterMeasured);
    }

    GapLayer gap(double thickness, bool t_isDCenterMeasured)
    {
        return gap(thickness,
                   ConstantsData::DEFAULT_GAP_PRESSURE,
                   Gases::CGas(),
                   ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
                   ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
                   t_isDCenterMeasured);
    }

    GapLayer gap(double thickness, double pressure, bool t_isDCenterMeasured)
    {
        return gap(thickness,
                   pressure,
                   Gases::CGas(),
                   ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
                   ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
                   t_isDCenterMeasured);
    }

    GapLayer gap(double thickness, const Gases::CGas & gas, bool t_isDCenterMeasured)
    {
        return gap(thickness,
                   ConstantsData::DEFAULT_GAP_PRESSURE,
                   gas,
                   ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
                   ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
                   t_isDCenterMeasured);
    }

    GapLayer
      gap(double thickness, double pressure, const Gases::CGas & gas, bool t_isDCenterMeasured)
    {
        return gap(thickness,
                   pressure,
                   gas,
                   ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
                   ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
                   t_isDCenterMeasured);
    }

    GapLayer gap(double thickness,
                 const Gases::CGas & gas,
                 double accommodation1,
                 double accommodation2,
                 bool t_isDCenterMeasured)
    {
        return gap(thickness,
                   ConstantsData::DEFAULT_GAP_PRESSURE,
                   gas,
                   accommodation1,
                   accommodation2,
                   t_isDCenterMeasured);
    }

    GapLayer forcedVentilationGap(const GapLayer & gap,
                                  double forcedVentilationAirSpeed,
                                  double forcedVentilationAirTemperature)
    {
        gap->activateVentilation(forcedVentilationAirTemperature, forcedVentilationAirSpeed);
        return gap;
    }

    SolidLayer updateMaterialData(const SolidLayer & layer, double density, double youngsModulus)
    {
        // Deflection cannot be applied to shading layers
        if(!layer->isPermeable())
        {
            layer->setDeflectionMaterial({youngsModulus, DeflectionConstants::POISONRATIO, density});
        }
        return layer;
    }

    SolidLayer shading(double thickness,
                       double conductivity,
                       const EffectiveLayers::EffectiveMultipliers & effectiveOpenness,
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

        auto layer = std::make_shared<CIGUSolidLayer>(
          thickness,
          conductivity,
          std::make_shared<Surface>(frontEmissivity, frontIRTransmittance),
          std::make_shared<Surface>(backEmissivity, backIRTransmittance));

        layer->assignEffectiveMultipliers(effectiveOpenness);

        return layer;
    }

    SolidLayer sealedLayer(double thickness,
                           double conductivity,
                           double frontEmissivity,
                           double frontIRTransmittance,
                           double backEmissivity,
                           double backIRTransmittance)
    {
        auto layer = std::make_shared<CIGUSolidLayer>(
          thickness,
          conductivity,
          std::make_shared<Surface>(frontEmissivity, frontIRTransmittance),
          std::make_shared<Surface>(backEmissivity, backIRTransmittance));
        layer->markAsShadeLayer();
        return layer;
    }

    Gases::CGas defaultVacuumMixture()
    {
        return Gases::CGas{std::vector<Gases::CGasItem>{{1.0, Gases::GasDef::VacuumMixture}}};
    }

    namespace
    {
        GapLayer makeComputedPillar(double height,
                                    double pressure,
                                    const Gases::CGas & gas,
                                    double accommodation1,
                                    double accommodation2,
                                    double materialConductivity,
                                    double cellArea,
                                    CIGUGapLayer::PillarAreaFn areaFn,
                                    CIGUGapLayer::PillarResistanceFn resistanceFn)
        {
            auto gapLayer = gap(height, pressure, gas, accommodation1, accommodation2);
            gapLayer->activatePillar(materialConductivity, cellArea,
                                     std::move(areaFn), std::move(resistanceFn));
            return gapLayer;
        }
    }   // anonymous namespace

    GapLayer createPillar(const CylindricalPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto rad = pillar.radius;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [rad]() { return ConstantsData::WCE_PI * rad * rad; },
          universalPillarThermalResistance);
    }

    GapLayer createPillar(const SphericalPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto rad = pillar.radiusOfContact;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [rad]() { return ConstantsData::WCE_PI * rad * rad; },
          universalPillarThermalResistance);
    }

    GapLayer createPillar(const RectangularPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto len = pillar.length;
        const auto wid = pillar.width;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [len, wid]() { return len * wid; },
          [len, wid](double kHot, double kCold, double kPillar,
                     double height, double area) -> double
          {
              if(std::max(wid, len) / std::min(wid, len) < 2)
              {
                  return universalPillarThermalResistance(kHot, kCold, kPillar, height, area);
              }
              return linearBearingPillarThermalResistance(
                kHot, kCold, kPillar, height, len, wid);
          });
    }

    GapLayer createPillar(const TriangularPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto len = pillar.length;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [len]() { return std::sqrt(3) / 4 * len * len; },
          universalPillarThermalResistance);
    }

    GapLayer createPillar(const PentagonPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto len = pillar.length;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [len]() { return 5 * std::sqrt(3) / 4 * len * len; },
          universalPillarThermalResistance);
    }

    GapLayer createPillar(const HexagonPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto len = pillar.length;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [len]() { return 3 * std::sqrt(3) / 2 * len * len; },
          universalPillarThermalResistance);
    }

    GapLayer createPillar(const LinearBearingPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto len = pillar.length;
        const auto wid = pillar.width;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [len, wid]() { return len * wid; },
          [len, wid](double kHot, double kCold, double kPillar,
                     double height, double /*area*/) -> double
          {
              return linearBearingPillarThermalResistance(
                kHot, kCold, kPillar, height, len, wid);
          });
    }

    GapLayer createPillar(const TruncatedConePillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto rad1 = pillar.radius1;
        const auto rad2 = pillar.radius2;
        const auto matCond = pillar.materialConductivity;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          []() { return 0.0; },
          [rad1, rad2, matCond](double kHot, double kCold, double /*kPillar*/,
                                double height, double /*area*/) -> double
          {
              auto rpGen = [](double glassCond, double radius) -> double {
                  return 1 / (4 * glassCond * radius);
              };
              using FenestrationCommon::WCE_PI;
              return rpGen(kHot, rad1) + rpGen(kCold, rad2)
                     + height / (matCond * WCE_PI * std::pow((rad1 + rad2) / 2, 2));
          });
    }

    GapLayer createPillar(const AnnulusCylinderPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto inner = Helper::adjustInnerRadius(pillar.innerRadius);
        const auto outer = Helper::adjustOuterRadius(pillar.innerRadius, pillar.outerRadius);
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [inner, outer]()
          {
              return FenestrationCommon::WCE_PI * (outer * outer - inner * inner);
          },
          [inner, outer](double kHot, double kCold, double kPillar,
                         double height, double area) -> double
          {
              return annulusCylinderPillarThermalResistance(
                kPillar, height, area,
                Helper::rCS(kHot, inner, outer),
                Helper::rCS(kCold, inner, outer));
          });
    }

    GapLayer createPillar(const CShapedCylinderPillar & pillar,
                          double pressure,
                          const Gases::CGas & gas,
                          const double accommodation1,
                          const double accommodation2)
    {
        const auto inner = Helper::adjustInnerRadius(pillar.innerRadius);
        const auto outer = Helper::adjustOuterRadius(pillar.innerRadius, pillar.outerRadius);
        const auto fraction = pillar.fractionCovered;
        return makeComputedPillar(
          pillar.height, pressure, gas, accommodation1, accommodation2,
          pillar.materialConductivity, pillar.cellArea,
          [inner, outer, fraction]()
          {
              return FenestrationCommon::WCE_PI * (outer * outer - inner * inner) * fraction;
          },
          [inner, outer, fraction](double kHot, double kCold, double kPillar,
                                   double height, double area) -> double
          {
              return cShapedCylinderPillarThermalResistance(
                fraction, kPillar, height, area,
                Helper::rCS(kHot, inner, outer),
                Helper::rCS(kCold, inner, outer));
          });
    }

    GapLayer createPillar(const PillarMeasurement & pillar)
    {
        auto gapLayer = std::make_shared<CIGUGapLayer>(
          pillar.totalThickness - pillar.glass1.thickness - pillar.glass2.thickness, 0.0);
        gapLayer->activatePillar(cStar(pillar));
        return gapLayer;
    }
}   // namespace Tarcog::ISO15099::Layers
