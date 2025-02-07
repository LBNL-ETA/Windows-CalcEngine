#pragma once

#include <WCECommon.hpp>

namespace ThermalPermeability
{
    namespace Venetian
    {
        double maxAngle(double t_SlatSpacing, double t_MatThickness);

        double calculateRise(double t_Curvature, double t_SlatWidth);

        double calculateCurvature(double t_Rise, double t_SlatWidth);

        double permeabilityFactor(double materialThickness,
                                  const FenestrationCommon::Venetian::Geometry & geometry);
    }   // namespace Venetian

    namespace Perforated
    {

        double permeabilityFactor(const FenestrationCommon::Perforated::Geometry & geometry);
    }   // namespace Perforated

    namespace Woven
    {
        double permeabilityFactor(const FenestrationCommon::Woven::Geometry & geometry);
    }

    namespace LouveredShutter
    {
        double permeabilityFactor(const FenestrationCommon::LouveredShutter::Geometry & geometry);
    }
}   // namespace ThermalPermeability