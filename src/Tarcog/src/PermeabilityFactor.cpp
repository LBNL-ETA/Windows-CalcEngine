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

        double frontOpenness(const double t_TiltAngle,
                        const double t_SlatSpacing,
                        const double t_MatThickness,
                        const double t_SlatCurvature,
                        const double t_SlatWidth)
        {
            const auto aRise{calculateRise(t_SlatCurvature, t_SlatWidth)};
            const auto h{aRise > 1e-6 ? aRise : 1e-6};
            const auto temp{h + pow(t_SlatWidth, 2) / (4 * h)};
            const auto Ls{asin(t_SlatWidth / temp) * temp};
            const auto angleMax{maxAngle(t_SlatSpacing, t_MatThickness)};
            const auto slatAngle{std::fmin(std::abs(t_TiltAngle), angleMax)};

            double cosPhi = std::cos(slatAngle * ConstantsData::WCE_PI / 180);
            double sinPhi = std::sin(std::abs(slatAngle) * ConstantsData::WCE_PI / 180);
            if((slatAngle == 90) || (slatAngle == -90))
                cosPhi = 0;
            auto opennessFactor{
              1
              - (t_MatThickness * Ls)
                  / ((Ls * cosPhi + t_MatThickness * sinPhi) * (t_SlatSpacing + t_MatThickness))};
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
