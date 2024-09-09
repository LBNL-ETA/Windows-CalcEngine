#include <cmath>
#include <functional>

#include <WCECommon.hpp>

#include "PermeabilityFactor.hpp"

namespace ThermalPermeability
{
    namespace Venetian
    {
        double maxAngle(const double t_SlatSpacing, const double t_MatThickness)
        {
            return std::acos(t_MatThickness / (t_SlatSpacing + t_MatThickness)) * 180
                   / ConstantsData::WCE_PI;
        }

        double calculateRise(const double t_Curvature, const double t_SlatWidth)
        {
            double rise{0};

            if(t_Curvature > 0)
            {
                const auto val{t_Curvature * t_Curvature - t_SlatWidth * t_SlatWidth / 4};
                if(val < 0)
                {
                    rise = t_SlatWidth / 2;
                }
                else
                {
                    const auto Rprime{std::sqrt(val)};
                    rise = t_Curvature - Rprime;
                }
            }
            return rise;
        }

        double calculateCurvature(const double t_Rise, const double t_SlatWidth)
        {
            double curvature = 0;

            if(t_Rise > 0)
            {
                double aRise = t_Rise;
                if(t_Rise > t_SlatWidth / 2)
                {
                    aRise = t_SlatWidth / 2;
                }
                curvature = (aRise * aRise + t_SlatWidth * t_SlatWidth / 4) / (2 * aRise);
            }
            return curvature;
        }

        double frontOpenness(const double tiltAngle,
                             const double slatSpacing,
                             const double matThickness,
                             const double slatCurvature,
                             const double slatWidth)
        {
            // Calculate rise based on slat curvature and width
            const auto rise = calculateRise(slatCurvature, slatWidth);
            const auto effectiveRise = (rise > 1e-6) ? rise : 1e-6;

            // Calculate the length of slat arc (Ls)
            const auto temp = effectiveRise + pow(slatWidth, 2) / (4 * effectiveRise);
            const auto slatArcLength = asin(slatWidth / temp) * temp;

            // Determine the maximum allowable angle and the effective slat angle
            const auto effectiveSlatAngle =
              std::fmin(std::abs(tiltAngle), maxAngle(slatSpacing, matThickness));

            // Handle special cases where slat angle is 90 degrees
            const double adjustedCosPhi =
              (effectiveSlatAngle == 90 || effectiveSlatAngle == -90)
                ? 0
                : std::cos(FenestrationCommon::radians(effectiveSlatAngle));

            // Compute the openness factor
            auto opennessFactor =
              1.0
              - (matThickness * slatArcLength)
                  / ((slatArcLength * adjustedCosPhi
                      + matThickness * std::sin(FenestrationCommon::radians(effectiveSlatAngle)))
                     * (slatSpacing + matThickness));

            // Ensure openness factor is non-negative
            if(opennessFactor < 0)
                opennessFactor = 0;

            return opennessFactor;
        }

    }   // namespace Venetian

    namespace Perforated
    {
        double frontOpenness(Type t_Type,
                             double t_SpacingX,
                             double t_SpacingY,
                             double t_DimensionX,
                             double t_DimensionY)
        {
            const auto cellArea{t_SpacingX * t_SpacingY};
            std::map<Type, std::function<double(const double, const double)>> opennessFraction{
              {Type::Circular, {[&](const double x, const double y) {
                   return (x / 2) * (y / 2) * ConstantsData::WCE_PI / cellArea;
               }}},
              {Type::Square, {[&](const double x, const double y) { return x * y / cellArea; }}},
              {Type::Rectangular,
               {[&](const double x, const double y) { return x * y / cellArea; }}}};
            return opennessFraction.at(t_Type)(t_DimensionX, t_DimensionY);
        }
    }   // namespace Perforated

    namespace Woven
    {
        double frontOpenness(const double t_Diameter, const double t_Spacing)
        {
            auto opennessFraction{(t_Spacing - t_Diameter) * (t_Spacing - t_Diameter)
                                  / (t_Spacing * t_Spacing)};
            if(opennessFraction < 0)
                opennessFraction = 0;
            return opennessFraction;
        }
    }   // namespace Woven

}   // namespace ThermalPermeability
