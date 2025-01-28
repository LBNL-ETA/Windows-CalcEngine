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
        SolidLayer solid(double thickness, double conductivity);

        SolidLayer solid(double thickness,
                         double conductivity,
                         double frontEmissivity,
                         double frontIRTransmittance,
                         double backEmissivity,
                         double backIRTransmittance);

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

        //! This is complete function to create all necessary data for a gap layer. All subsequent
        //! functions are using this function with the default values for the missing parameters.
        GapLayer gap(double thickness,
                     double pressure,
                     const Gases::CGas & gas,
                     double accommodation1,
                     double accommodation2,
                     bool t_isDCenterMeasured = false);

        GapLayer gap(double thickness, bool t_isDCenterMeasured = false);
        GapLayer gap(double thickness, double pressure, bool t_isDCenterMeasured = false);
        GapLayer gap(double thickness, const Gases::CGas & gas, bool t_isDCenterMeasured = false);
        GapLayer gap(double thickness,
                     double pressure,
                     const Gases::CGas & gas,
                     bool t_isDCenterMeasured = false);
        GapLayer gap(double thickness,
                     const Gases::CGas & gas,
                     double accommodation1,
                     double accommodation2,
                     bool t_isDCenterMeasured = false);


        GapLayer forcedVentilationGap(const GapLayer & gap,
                                      double forcedVentilationAirSpeed,
                                      double forcedVentilationAirTemperature);

        Gases::CGas defaultVacuumMixture();

        GapLayer createPillar(
          const CylindricalPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const SphericalPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const RectangularPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const TriangularPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const PentagonPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const HexagonPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const LinearBearingPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const TruncatedConePillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const AnnulusCylinderPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);
        GapLayer createPillar(
          const CShapedCylinderPillar & pillar,
          double pressure,
          const Gases::CGas & gas = defaultVacuumMixture(),
          double accommodation1 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT,
          double accommodation2 = ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT);

        GapLayer createPillar(const PillarMeasurement & pillar);
    }   // namespace Layers

}   // namespace Tarcog::ISO15099
